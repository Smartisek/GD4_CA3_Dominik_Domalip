#include "SocketWrapperServerPCH.hpp"

BulletServer::BulletServer()
{
	mTimeToDie = Timing::sInstance.GetFrameStartTime() + BULLET_LIFETIME; //bullet lives for 2 secs 
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
		LOG("Bullet hit tank: shooter=%d target=%d", GetOwnerPlayerId(), inTank->GetPlayerId());
		//go away 
		SetDoesWantToDie(true);

		static_cast<TankServer*>(inTank)->TakeDamage(GetOwnerPlayerId(), kBulletDamage); //damagage here do constanst.hpp later***
	}

	return false;
}