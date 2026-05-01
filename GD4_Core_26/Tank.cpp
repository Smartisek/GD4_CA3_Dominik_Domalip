#include "SocketWrapperPCH.hpp"

const float WORLD_HEIGHT = 720.f;
const float WORLD_WIDTH = 1280.f;

Tank::Tank() :
	GameObject(),
	mMaxRotationSpeed(100.f),
	mMaxLinearSpeed(5000.f),
	mVelocity(Vector3::Zero),
	mTurretRotation(0.f),
	mTurretRotationSpeed(120.f),
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

	//Implement turret rotation later ********************

	//shooting 
	mIsShooting = inInputState.IsShooting();

	if (mIsShooting)
	{
		Fire();
	}
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