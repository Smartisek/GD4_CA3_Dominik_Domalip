#include "SocketWrapperClientPCH.hpp"

TankClient::TankClient() :
	mTimeLocationBecameOutOfSync(0.f),
	mTimeVelocityBecameOutOfSync(0.f),
	mTimeTurretBecameOutOfSync(0.f)
{
	// Main tank body sprite
	mSpriteComponent.reset(new PlayerSpriteComponent(this));
	mSpriteComponent->SetTexture(TextureManager::sInstance->GetTexture("tank"));

	// Turret sprite (separate, rotates independently)
	mTurretSpriteComponent.reset(new SpriteComponent(this));
	mTurretSpriteComponent->SetTexture(TextureManager::sInstance->GetTexture("turret"));
}

void TankClient::HandleDying()
{
	Tank::HandleDying();

	// If this is our local tank, update HUD
	if (GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId())
	{
		HUD::sInstance->SetPlayerHealth(0);
	}
}

void TankClient::Update()
{
	// Is this the tank owned by us (local player)?
	if (GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId())
	{
		const Move* pendingMove = InputManager::sInstance->GetAndClearPendingMove();
		if (pendingMove)
		{
			float deltaTime = pendingMove->GetDeltaTime();

			// Apply input locally (includes body movement and turret rotation)
			ProcessInput(deltaTime, pendingMove->GetInputState());

			// Simulate movement
			SimulateMovement(deltaTime);

			// Update HUD with local tank state
			HUD::sInstance->SetPlayerHealth(GetHealth());
			HUD::sInstance->SetPlayerAmmo(GetAmmo());
		}
	}
	else
	{
		// Remote tank: just simulate movement based on last known velocity
		SimulateMovement(Timing::sInstance.GetDeltaTime());

		// Check if we're back in sync
		if (Math::Is2DVectorEqual(GetVelocity(), Vector3::Zero))
		{
			mTimeLocationBecameOutOfSync = 0.f;
		}
	}
}

void TankClient::Read(InputMemoryBitStream& inInputStream)
{
	bool stateBit;
	uint32_t readState = 0;

	// Read PlayerId
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		uint32_t playerId;
		inInputStream.Read(playerId);
		SetPlayerId(playerId);
		readState |= ETRS_PlayerId;
	}

	// Save old state for interpolation
	float oldRotation = GetRotation();
	float oldTurretRotation = GetTurretRotation();
	Vector3 oldLocation = GetLocation();
	Vector3 oldVelocity = GetVelocity();

	// Read Pose (position, body rotation, velocity)
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		Vector3 replicatedVelocity;
		inInputStream.Read(replicatedVelocity.mX);
		inInputStream.Read(replicatedVelocity.mY);
		SetVelocity(replicatedVelocity);

		Vector3 replicatedLocation;
		inInputStream.Read(replicatedLocation.mX);
		inInputStream.Read(replicatedLocation.mY);
		SetLocation(replicatedLocation);

		float replicatedRotation;
		inInputStream.Read(replicatedRotation);
		SetRotation(replicatedRotation);

		readState |= ETRS_Pos;
	}

	// Read thrust direction (compact format)
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		bool isThrustPositive;
		inInputStream.Read(isThrustPositive);
		// mThrustDir = isThrustPositive ? 1.f : -1.f;
	}

	// Read TurretRotation (separate from body rotation)
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		float replicatedTurretRotation;
		inInputStream.Read(replicatedTurretRotation);
		SetTurretRotation(replicatedTurretRotation);
		readState |= ETRS_TurretRotation;
	}

	// Read Color
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		Vector3 color;
		inInputStream.Read(color);
		SetColor(color);
		readState |= ETRS_Color;
	}

	// Read Health
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		int health;
		inInputStream.Read(health, 8);
		SetHealth(health);
		readState |= ETRS_Health;
	}

	// Read Ammo
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		int ammo;
		inInputStream.Read(ammo, 8);
		SetAmmo(ammo);
		readState |= ETRS_Ammo;
	}

	// Apply client-side prediction
	if (GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId())
	{
		DoClientSidePredictionAfterReplicationForLocalTank(readState);
	}
	else
	{
		DoClientSidePredictionAfterReplicationForRemoteTank(readState);
	}
}

void TankClient::DoClientSidePredictionAfterReplicationForLocalTank(uint32_t inReadState)
{
	// Local tank prediction - keep movement smooth
	// The server will correct if needed
}

void TankClient::DoClientSidePredictionAfterReplicationForRemoteTank(uint32_t inReadState)
{
	// Remote tank prediction - interpolate smoothly to new position
	float oldRotation = GetRotation();
	float oldTurretRotation = GetTurretRotation();
	Vector3 oldLocation = GetLocation();
	Vector3 oldVelocity = GetVelocity();

	InterpolateClientSidePrediction(oldRotation, oldTurretRotation, oldLocation, oldVelocity, true);
}

void TankClient::InterpolateClientSidePrediction(float inOldRotation, float inOldTurretRotation, const Vector3& inOldLocation, const Vector3& inOldVelocity, bool inIsForRemoteTank)
{
	if (inOldRotation != GetRotation() && !inIsForRemoteTank)
	{
		LOG("ERROR! Move replay ended with incorrect rotation!", 0);
	}

	float roundTripTime = NetworkManagerClient::sInstance->GetRoundTripTime();

	if (!Math::Is2DVectorEqual(inOldLocation, GetLocation()))
	{
		//LOG( "ERROR! Move replay ended with incorrect location!", 0 );

		//have we been out of sync, or did we just become out of sync?
		float time = Timing::sInstance.GetFrameStartTime();
		if (mTimeLocationBecameOutOfSync == 0.f)
		{
			mTimeLocationBecameOutOfSync = time;
		}

		float durationOutOfSync = time - mTimeLocationBecameOutOfSync;
		if (durationOutOfSync < roundTripTime)
		{
			SetLocation(Lerp(inOldLocation, GetLocation(), inIsForRemoteTank ? (durationOutOfSync / roundTripTime) : 0.1f));
		}
	}
	else
	{
		//we're in sync
		mTimeLocationBecameOutOfSync = 0.f;
	}


	if (!Math::Is2DVectorEqual(inOldVelocity, GetVelocity()))
	{
		//LOG( "ERROR! Move replay ended with incorrect velocity!", 0 );

		//have we been out of sync, or did we just become out of sync?
		float time = Timing::sInstance.GetFrameStartTime();
		if (mTimeVelocityBecameOutOfSync == 0.f)
		{
			mTimeVelocityBecameOutOfSync = time;
		}

		//now interpolate to the correct value...
		float durationOutOfSync = time - mTimeVelocityBecameOutOfSync;
		if (durationOutOfSync < roundTripTime)
		{
			SetVelocity(Lerp(inOldVelocity, GetVelocity(), inIsForRemoteTank ? (durationOutOfSync / roundTripTime) : 0.1f));
		}
		//otherwise, fine...

	}
	else
	{
		//we're in sync
		mTimeVelocityBecameOutOfSync = 0.f;
	}

}