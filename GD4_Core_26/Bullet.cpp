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

void Bullet::InitializeFromTank(TankPtr inFiringTank, const Vector3& inStartPos, const Vector3& inDirection)
{
	//where to spawn the bullet which shoult be the turret tiip
	SetLocation(inStartPos);

	//normalize vector for direction and multiply by speed to get velocity
	Vector3 normalizedDir = inDirection;
	normalizedDir.Normalize2D();
	mVelocity = normalizedDir * BULLET_SPEED;

	//store the id of tank firing this bullet 
	mOwnerPlayerId = inFiringTank->GetPlayerId();

	//rotation matching direciton 
	float rotationAngle = atan2f(normalizedDir.mY, normalizedDir.mX);
	SetRotation(rotationAngle);

	mLifeTime = 0.f;

	LOG("Bullet created at (%.1f, %.1f) by player %d with velocity (%.1f, %.1f)",
		inStartPos.mX, inStartPos.mY, mOwnerPlayerId, mVelocity.mX, mVelocity.mY);
}

void Bullet::Update()
{
	//update the lifetime
	mLifeTime += Timing::sInstance.GetDeltaTime();
	// move the bullet based on its velocity and the time since the last update which is in timing class and decides the fps of the game
	SetLocation(GetLocation() + mVelocity * Timing::sInstance.GetDeltaTime());

	//check if out of bounds
	// **** should not be hardcoded values i shoult move it to constant file that i create later ****
	Vector3 location = GetLocation();
	if (location.mX < -100.f || location.mX > 1380.f ||
		location.mY < -100.f || location.mY > 820.f ||
		mLifeTime > BULLET_LIFETIME)
	{
		// Despawn bullet
		SetDoesWantToDie(true);
	}

	//check collision with tanks logic 
	float bulletRadius = GetCollisionRadius();
	Vector3 bulletLocation = GetLocation();

	for (auto goIt = World::sInstance->GetGameObjects().begin(), end = World::sInstance->GetGameObjects().end(); goIt != end; ++goIt)
	{
		GameObject* target = goIt->get();
		//dont collide with ourself or tank that fired 
		if (target == this || target->DoesWantToDie())
		{
			continue;
		}

		Tank* targetTank = target->GetAsTank();
		if (targetTank && targetTank->GetPlayerId() != mOwnerPlayerId)
		{
			//get the target tank location and its radius 
			Vector3 targetLocation = targetTank->GetLocation();
			float targetRadius = targetTank->GetCollisionRadius();
			//check for collision using pythagorean 
			Vector3 delta = targetLocation - bulletLocation;
			float distSq = delta.LengthSq2D();
			float collisionDist = (bulletRadius + targetRadius);

			if (distSq < (collisionDist * collisionDist))
			{
				//tank hit with bullet
				int damageAmount = 25;   //damage would move into constant file as well ******
				targetTank->SetHealth(targetTank->GetHealth() - damageAmount);

				LOG("Bullet hit tank %d! Health: %d",
					targetTank->GetPlayerId(), targetTank->GetHealth());

				//despawn this bullet
				SetDoesWantToDie(true);
				break;
			}
		}
	}
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