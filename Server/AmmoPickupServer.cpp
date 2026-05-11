#include "SocketWrapperServerPCH.hpp"

void AmmoPickupServer::Update()
{
	//dont need to do anything if the pickup is not active 
	if (!IsActive())
	{
		return;
	}

	const auto& gameObjects = World::sInstance->GetGameObjects();
	for (const auto& go : gameObjects)
	{
		Tank* tank = go->GetAsTank();
		//only need tank so skip anything else 
		if (!tank)
		{
			continue;
		}

		Vector3 diff = tank->GetLocation() - GetLocation();
		float distSq = diff.mX * diff.mX + diff.mY * diff.mY;
		float pickupRadius = 40.f;

		if (distSq < pickupRadius * pickupRadius)
		{
			int newAmmo = std::min(tank->GetAmmo() + GetAmmoAmount(), kMaxTankAmmo);  //clamp the ammo 
			tank->SetAmmo(newAmmo);
			NetworkManagerServer::sInstance->SetStateDirty(tank->GetNetworkId(), Tank::ETRS_Ammo);

			//deactivate the pickup and remove it from the world
			SetActive(false);
			SetDoesWantToDie(true);
			NetworkManagerServer::sInstance->UnregisterGameObject(this);
			return;
		}
	}
}