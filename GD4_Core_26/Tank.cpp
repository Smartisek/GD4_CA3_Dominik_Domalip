#include "SocketWrapperPCH.hpp"

const float WORLD_HEIGHT = 720.f;
const float WORLD_WIDTH = 1280.f;

Tank::Tank() :
	GameObject(),
	mMaxRotationSpeed(100.f),
	mMaxLinearSpeed(5000.f),
	mVelocity(Vector3::Zero),
	mTurretRotation(0.f),
	mTurretRotationSpeed(80.f), //adjust for feeling 
	mWallRestitution(0.1f),
	mTankRestitution(0.1f),
	mThrustDir(0.f),
	mPlayerId(0),
	mIsShooting(false),
	mHealth(100),
	mAmmo(50)
{
	SetCollisionRadius(40.f); //adjust later***
}

void Tank::ProcessInput(float inDeltaTime, const InputState& inInputState)
{
	//left right movement
	float newRotation = GetRotation() + inInputState.GetDesiredHorizontalDelta() * mMaxRotationSpeed * inDeltaTime;
	SetRotation(newRotation);
	//up down movement
	float inputForwardDelta = inInputState.GetDesiredVerticalDelta();
	mThrustDir = inputForwardDelta;

	//turret speed turn using Math helpers
	float desired = Math::NormalizeAngleDegrees(inInputState.GetTurretRotation());
	float current = Math::NormalizeAngleDegrees(GetTurretRotation());

	float delta = Math::DeltaAngleDegrees(current, desired);

	float maxStep = mTurretRotationSpeed * inDeltaTime;
	if (delta > maxStep) delta = maxStep;
	else if (delta < -maxStep) delta = -maxStep;

	float newTurret = Math::NormalizeAngleDegrees(current + delta);

	SetTurretRotation(newTurret);


	//shooting 
	mIsShooting = inInputState.IsShooting();

}

void Tank::AdjustVelocityByThrust(float inDeltaTime)
{
	Vector3 forwardVector = GetForwardVector();
	mVelocity = forwardVector * (mThrustDir * inDeltaTime * mMaxLinearSpeed);
}

void Tank::SimulateMovement(float inDeltaTime)
{
	// Update velocity based on thrust
	AdjustVelocityByThrust(inDeltaTime);

	// Move the tank
	SetLocation(GetLocation() + mVelocity * inDeltaTime);

	// Handle collisions
	ProcessCollisions();
}

void Tank::Update()
{
	//for now nothing 
}

void Tank::ProcessCollisions()
{
	ProcessCollisionsWithScreenWalls();

	float sourceRadius = GetCollisionRadius();
	Vector3 sourceLocation = GetLocation();
	//itterator for all things living in the world
	for (auto goIt = World::sInstance->GetGameObjects().begin(), end = World::sInstance->GetGameObjects().end(); goIt != end; ++goIt)
	{
		GameObject* target = goIt->get();

		if (target != this && !target->DoesWantToDie())
		{
			Vector3 targetLocation = target->GetLocation();
			float targetRadius = target->GetCollisionRadius();

			Vector3 delta = targetLocation - sourceLocation;
			float distSq = delta.LengthSq2D();
			float collisionDist = (sourceRadius + targetRadius);
			//pythagorean collision checl 
			if (distSq < (collisionDist * collisionDist))
			{
				//check if it is another tank 
				Tank* targetTank = target->GetAsTank();

				if (target->HandleCollisionWithTank(this))
				{
					Vector3 dirToTarget = delta;
					dirToTarget.Normalize2D();
					Vector3 acceptableDeltaFromSourceToTarget = dirToTarget * collisionDist;
					SetLocation(targetLocation - acceptableDeltaFromSourceToTarget);

					Vector3 relVel = mVelocity;

					if (targetTank)
					{
						relVel -= targetTank->mVelocity;
					}

					float relVelDotDir = Dot2D(relVel, dirToTarget);

					if (relVelDotDir > 0.f)
					{
						Vector3 impulse = relVelDotDir * dirToTarget;

						if (targetTank)
						{
							mVelocity -= impulse;
							mVelocity *= mTankRestitution;
						}
						else
						{
							mVelocity -= impulse * 2.f;
							mVelocity *= mWallRestitution;
						}
					}
				}
			}
		}
	}
}

void Tank::ProcessCollisionsWithScreenWalls()
{
	Vector3 location = GetLocation();

	float x = location.mX;
	float y = location.mY;

	float vx = mVelocity.mX;
	float vy = mVelocity.mY;

	float radius = GetCollisionRadius();

	//top and bottom walls based on top variables in this file 
	if ((y + radius) >= WORLD_HEIGHT && vy > 0)
	{
		mVelocity.mY = -vy * mWallRestitution;
		location.mY = WORLD_HEIGHT - radius;
		SetLocation(location);
	}
	else if (y - radius <= 0 && vy < 0)
	{
		mVelocity.mY = -vy * mWallRestitution;
		location.mY = radius;
		SetLocation(location);
	}

	//left and right walls 
	if ((x + radius) >= WORLD_WIDTH && vx > 0)
	{
		mVelocity.mX = -vx * mWallRestitution;
		location.mX = WORLD_WIDTH - radius;
		SetLocation(location);
	}
	else if (x - radius <= 0 && vx < 0)
	{
		mVelocity.mX = -vx * mWallRestitution;
		location.mX = radius;
		SetLocation(location);
	}
}

uint32_t Tank::Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const
{
	//this takes track of what state changed 
	uint32_t writtenState = 0;

	//this is for writting playerID
	if (inDirtyState & ETRS_PlayerId)
	{
		inOutputStream.Write((bool)true);
		inOutputStream.Write(GetPlayerId());
		writtenState |= ETRS_PlayerId;
	}
	else
	{
		inOutputStream.Write((bool)false);
	}

	//this is for position, body rotation and velocity 
	if (inDirtyState & ETRS_Pos)
	{
		inOutputStream.Write((bool)true);

		Vector3 velocity = mVelocity;
		inOutputStream.Write(velocity.mX);
		inOutputStream.Write(velocity.mY);

		Vector3 location = GetLocation();
		inOutputStream.Write(location.mX);
		inOutputStream.Write(location.mY);

		inOutputStream.Write(GetRotation());

		writtenState |= ETRS_Pos;
	}
	else
	{
		inOutputStream.Write((bool)false);
	}
	//thrust direction for movement 
	if (mThrustDir != 0.f)
	{
		inOutputStream.Write(true);
		inOutputStream.Write(mThrustDir > 0.f);
	}
	else
	{
		inOutputStream.Write(false);
	}

	//write turret rotation
	if (inDirtyState & ETRS_TurretRotation)
	{
		inOutputStream.Write((bool)true);
		inOutputStream.Write(mTurretRotation);
		writtenState |= ETRS_TurretRotation;
	}
	else
	{
		inOutputStream.Write((bool)false);
	}

	//write color of the tank
	if (inDirtyState & ETRS_Color)
	{
		inOutputStream.Write((bool)true);
		inOutputStream.Write(GetColor());
		writtenState |= ETRS_Color;
	}
	else
	{
		inOutputStream.Write((bool)false);
	}

	if (inDirtyState & ETRS_Health)
	{
		inOutputStream.Write((bool)true);
		inOutputStream.Write(mHealth, 8);  // 8 bits for health (0-255)
		writtenState |= ETRS_Health;
	}
	else
	{
		inOutputStream.Write((bool)false);
	}
	
	//write ammo
	if (inDirtyState & ETRS_Ammo)
	{
		inOutputStream.Write((bool)true);
		inOutputStream.Write(mAmmo, 8);  // 8 bits for ammo (0-255)
		writtenState |= ETRS_Ammo;
	}
	else
	{
		inOutputStream.Write((bool)false);
	}

	//return the bit flag of what we actually wrote, so the client can update its state based on that
	return writtenState;
}