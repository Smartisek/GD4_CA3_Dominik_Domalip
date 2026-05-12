#include "SocketWrapperServerPCH.hpp"

TankServer::TankServer() :
	mControlType(ETCT_Human),
	mTimeOfNextShot(0.f),
	mTimeBetweenShots(1.2f), 
	mTurretDesiredRotation(0.f)
{
}

void TankServer::HandleDying()
{
	//unregister from network manager
	NetworkManagerServer::sInstance->UnregisterGameObject(this);
}

void TankServer::Update()
{
	Tank::Update();

	Vector3 oldLocation = GetLocation();
	Vector3 oldVelocity = GetVelocity();
	float oldRotation = GetRotation();
	float oldTurretRotation = GetTurretRotation();
	int oldAmmo = GetAmmo();

	// Process input from client
	if (mControlType == ETCT_Human)
	{
		ClientProxyPtr client = NetworkManagerServer::sInstance->GetClientProxy(GetPlayerId());
		if (client)
		{
			MoveList& moveList = client->GetUnprocessedMoveList();
			for (const Move& unprocessedMove : moveList)
			{
				const InputState& currentState = unprocessedMove.GetInputState();
				float deltaTime = unprocessedMove.GetDeltaTime();

				ProcessInput(deltaTime, currentState);
				SimulateMovement(deltaTime);
			}

			moveList.Clear();
		}
	}
	else
	{
		SimulateMovement(Timing::sInstance.GetDeltaTime());
	}

	HandleShooting();  // ← Check if we should fire

	// Mark dirty state for replication
	if (!Math::Is2DVectorEqual(oldLocation, GetLocation()) ||
		!Math::Is2DVectorEqual(oldVelocity, GetVelocity()) ||
		oldRotation != GetRotation())
	{
		NetworkManagerServer::sInstance->SetStateDirty(GetNetworkId(), ETRS_Pos);
	}

	if (oldTurretRotation != GetTurretRotation())
	{
		NetworkManagerServer::sInstance->SetStateDirty(GetNetworkId(), ETRS_TurretRotation);
	}

	if (oldAmmo != GetAmmo())
	{
		NetworkManagerServer::sInstance->SetStateDirty(GetNetworkId(), ETRS_Ammo);
	}
}


void TankServer::HandleShooting()
{
	float time = Timing::sInstance.GetFrameStartTime();
	if (mIsShooting && mAmmo > 0 && Timing::sInstance.GetFrameStartTime() > mTimeOfNextShot)
	{
		//not exact, but okay
		mTimeOfNextShot = time + mTimeBetweenShots;

		//fire!
		BulletPtr bullet = std::static_pointer_cast<Bullet>(GameObjectRegistry::sInstance->CreateGameObject('BLLT'));
		if (bullet)
		{
			// Fire from turret tip using TURRET rotation, not body rotation
			float fireOffsetDist = GetCollisionRadius() + 20.f;
			float turretRad = Math::ToRadians(GetTurretRotation());
			Vector3 fireDir(cosf(turretRad), sinf(turretRad), 0.f);
			Vector3 firePos = GetLocation() + (fireDir * fireOffsetDist);

			bullet->InitializeFromTank(this);
			bullet->SetLocation(firePos);
			const float kBulletSpriteOffset = 90.f;
			bullet->SetRotation(GetTurretRotation() + kBulletSpriteOffset);
			bullet->SetVelocity(fireDir * BULLET_SPEED); //bullet speed here 
			mAmmo--;
		}
	}
}

void TankServer::TakeDamage(int inDamagingPlayerId, int inDamageAmount)
{
	mHealth -= inDamageAmount;
	LOG("Tank %d took %d damage, health now %d", GetPlayerId(), inDamageAmount, mHealth);
	if (mHealth <= 0.f)
	{
		//score one for damaging player...
		ScoreBoardManager::sInstance->IncScore(inDamagingPlayerId, 1);

		//and you want to die
		SetDoesWantToDie(true);

		//tell the client proxy to make you a new cat
		ClientProxyPtr clientProxy = NetworkManagerServer::sInstance->GetClientProxy(GetPlayerId());
		if (clientProxy)
		{
			clientProxy->HandleTankDied();
		}
	}

	//tell the world our health dropped
	NetworkManagerServer::sInstance->SetStateDirty(GetNetworkId(), ETRS_Health);
}