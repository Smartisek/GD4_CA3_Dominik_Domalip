#include "SocketWrapperPCH.hpp"

//constatns for bullet, might move to precomplied header later ? ****
const float BULLET_SPEED = 3000.f;
const float BULLET_LIFETIME = 10.f;
const float BULLET_RADIUS = 5.f;

Bullet::Bullet() :
	GameObject(),
	mVelocity(Vector3::Zero),
	mOwnerPlayerId(0),
	mLifeTime(0.f)
{
	SetCollisionRadius(BULLET_RADIUS);
}

void Bullet::InitializeFromTank(Tank* inShooter)
{
	SetColor(inShooter->GetColor());
	SetPlayerId(inShooter->GetPlayerId());

	float turretRad = Math::ToRadians(inShooter->GetTurretRotation());
	Vector3 forward(cosf(turretRad), sinf(turretRad), 0.f);

	SetVelocity(inShooter->GetVelocity() + forward * 50);

	float fireOffsetDist = inShooter->GetCollisionRadius() + 10.f;
	Vector3 firePos = inShooter->GetLocation() + (forward * fireOffsetDist);
	SetLocation(firePos);

	SetRotation(inShooter->GetTurretRotation());
}

void Bullet::Update()
{
	float deltaTime = Timing::sInstance.GetDeltaTime();

	SetLocation(GetLocation() + mVelocity * deltaTime);

	//let tanks handle the collision
}

uint32_t Bullet::Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const
{
	uint32_t writtenState = 0;

	//write ownerId
	if (inDirtyState & EBRS_OwnerPlayerId)
	{
		inOutputStream.Write((bool)true);
		inOutputStream.Write(GetOwnerPlayerId());
		writtenState |= EBRS_OwnerPlayerId;
	}
	else
	{
		inOutputStream.Write((bool)false);
	}

	//write position + rotation for direction
	if (inDirtyState & EBRS_Pos)
	{
		inOutputStream.Write((bool)true);

		Vector3 location = GetLocation();
		inOutputStream.Write(location.mX);
		inOutputStream.Write(location.mY);

		inOutputStream.Write(GetRotation());

		writtenState |= EBRS_Pos;
	}
	else
	{
		inOutputStream.Write((bool)false);
	}

	//write the velocity 
	if (inDirtyState & EBRS_Velocity)
	{
		inOutputStream.Write((bool)true);

		inOutputStream.Write(mVelocity.mX);
		inOutputStream.Write(mVelocity.mY);

		writtenState |= EBRS_Velocity;
	}
	else
	{
		inOutputStream.Write((bool)false);
	}

	return writtenState;
}