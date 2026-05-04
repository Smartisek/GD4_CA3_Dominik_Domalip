#include "SocketWrapperServerPCH.hpp"

TankServer::TankServer() :
	mControlType(ETCT_Human),
	mTimeOfNextShot(0.f),
	mTimeBetweenShots(0.15f), 
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

	//track old state for dirty flag checking
	Vector3 oldLocation = GetLocation();
	Vector3 oldVelocity = GetVelocity();
	float oldRotation = GetRotation();
	float oldTurretRotation = GetTurretRotation();
	int oldAmmo = GetAmmo();

	if (mControlType == ETCT_Human)
	{
		HandleInput();
	}
	else
	{
		HandleAI();
	}

	HandleShooting();

	SimulateMovement(Timing::sInstance.GetDeltaTime());

	//mark state dirty if it changed since last update, so it will be replicated to clients
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

void TankServer::HandleInput()
{
	//get client that is controlling this tank 
	ClientProxyPtr client = NetworkManagerServer::sInstance->GetClientProxy(GetPlayerId());
	if (!client)
	{
		return;
	}

	//process all unprocessed moves from the client
	MoveList& moveList = client->GetUnprocessedMoveList();
	for (const Move& unprocessedMove : moveList)
	{
		const InputState& currentState = unprocessedMove.GetInputState();
		float deltaTime = unprocessedMove.GetDeltaTime();

		//process body movement and rotation
		ProcessInput(deltaTime, currentState);

		//fix the shooting here ******
		//mIsShooting = currentState.IsShooting();

		// Simulate the movement for this frame
		SimulateMovement(deltaTime);
	}
	//clear processed moves
	moveList.Clear();
}

void TankServer::HandleAI()
{
//do nothing for now 
}

void TankServer::HandleShooting()
{
	float currentTime = Timing::sInstance.GetFrameStartTime();

	//only shoot when we have ammo and time to shoot
	if (mIsShooting && currentTime > mTimeOfNextShot && CanShoot())
	{
		mTimeOfNextShot = currentTime + mTimeBetweenShots;

		//create a bullet
		BulletPtr bullet = std::static_pointer_cast<Bullet>(
			GameObjectRegistry::sInstance->CreateGameObject('BLLT')
		);

		//fire from turret tip in turret direction
		float fireOffsetDist = GetCollisionRadius() + 10.f;
		Vector3 fireDir(cos(GetTurretRotation()), sin(GetTurretRotation()), 0.f);
		Vector3 firePos = GetLocation() + (fireDir * fireOffsetDist);

		//initialize the bullet
		bullet->InitializeFromTank(
			std::static_pointer_cast<Tank>(shared_from_this()),
			firePos,
			fireDir
		);
		//consume the ammo and mark it dirty so it will be replicated to clients
		SetAmmo(GetAmmo() - 1);
		NetworkManagerServer::sInstance->SetStateDirty(GetNetworkId(), ETRS_Ammo);

		mIsShooting = false;

		LOG("TankServer %d fired! Ammo: %d", GetPlayerId(), GetAmmo());
	}

}

void TankServer::TakeDamage(int inDamagingPlayerId, int inDamageAmount)
{
	int newHealth = GetHealth() - inDamageAmount;
	SetHealth(newHealth);
	//mark dirty 
	NetworkManagerServer::sInstance->SetStateDirty(GetNetworkId(), ETRS_Health);

	if (newHealth <= 0)
	{
		//give point to killer
		ScoreBoardManager::sInstance->IncScore(inDamagingPlayerId, 1);

		//mark for death
		SetDoesWantToDie(true);

		//notify client proxy to handle respawn
		ClientProxyPtr clientProxy = NetworkManagerServer::sInstance->GetClientProxy(GetPlayerId());
		if (clientProxy)
		{
			clientProxy->HandleTankDied();
		}

		LOG("Tank %d destroyed by player %d", GetPlayerId(), inDamagingPlayerId);
	}
}