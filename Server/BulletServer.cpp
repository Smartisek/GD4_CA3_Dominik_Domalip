#include "SocketWrapperServerPCH.hpp"

BulletServer::BulletServer()
{
	mTimeToDie = Timing::sInstance.GetFrameStartTime() + 2.f; //bullet lives for 2 secs 
}

void BulletServer::HandleDying()
{
	NetworkManagerServer::sInstance->UnregisterGameObject(this);
}

void BulletServer::Update()
{
	Bullet::Update();

	if (Timing::sInstance.GetFrameStartTime() > mTimeToDie)
	{
		SetDoesWantToDie(true);
	}
}

bool BulletServer::HandleCollisionWithTank(Tank* inTank)
{
	if (inTank->GetPlayerId() != GetOwnerPlayerId())
	{
		//go away 
		SetDoesWantToDie(true);

		static_cast<TankServer*>(inTank)->TakeDamage(GetOwnerPlayerId(), 1); //damagage here do constanst.hpp later***
	}

	return false;
}