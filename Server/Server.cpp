#include "SocketWrapperServerPCH.hpp"


bool Server::StaticInit()
{
	s_instance.reset(new Server());

	return true;
}

Server::Server()
{

	GameObjectRegistry::sInstance->RegisterCreationFunction('TANK', TankServer::StaticCreate);
	GameObjectRegistry::sInstance->RegisterCreationFunction('BLLT', BulletServer::StaticCreate);
	GameObjectRegistry::sInstance->RegisterCreationFunction('AMMO', AmmoPickupServer::StaticCreate);

	InitNetworkManager();

	// Setup latency
	float latency = 0.0f;
	string latencyString = StringUtils::GetCommandLineArg(2);
	if (!latencyString.empty())
	{
		latency = stof(latencyString);
	}
	NetworkManagerServer::sInstance->SetSimulatedLatency(latency);
}


int Server::Run()
{
	SetupWorld();

	return Engine::Run();
}

bool Server::InitNetworkManager()
{
	string portString = StringUtils::GetCommandLineArg(1);
	uint16_t port = stoi(portString);

	return NetworkManagerServer::StaticInit(port);
}


void Server::SetupWorld()
{
	//spawn pickups logic for later 
	//spawn some random mice
	//CreateRandomMice(10);

	//spawn more random mice!
	//CreateRandomMice(10);
	SpawnAmmoPickups(3);
}

void Server::SpawnAmmoPickups(int inCount)
{
	Vector3 spawnMin(30.f, 30.f, 0.f);
	Vector3 spawnMax(static_cast<float>(kWindowWidth), static_cast<float>(kWindowHeight), 0.f);

	for (int i = 0; i < inCount; ++i)
	{
		GameObjectPtr go = GameObjectRegistry::sInstance->CreateGameObject('AMMO');
		go->SetLocation(Math::GetRandomVector(spawnMin, spawnMax));
	}
}

int Server::CountAmmoPickups()
{
	int count = 0;
	for (const auto& go : World::sInstance->GetGameObjects())
	{
		if (go->GetClassId() == AmmoPickup::kClassId)
			count++;
	}
	return count;
}

void Server::DoFrame()
{
	NetworkManagerServer::sInstance->ProcessIncomingPackets();

	NetworkManagerServer::sInstance->CheckForDisconnects();

	NetworkManagerServer::sInstance->RespawnPickups();

	mPickupSpawnTimer -= Timing::sInstance.GetDeltaTime();
	if (mPickupSpawnTimer <= 0.f)
	{
		mPickupSpawnTimer = 15.f;  //reset timer
		int current = CountAmmoPickups();
		if (current < kMaxAmmoPickups)
		{
			SpawnAmmoPickups(kMaxAmmoPickups - current);
		}
	}

	Engine::DoFrame();

	NetworkManagerServer::sInstance->SendOutgoingPackets();

}

void Server::HandleNewClient(ClientProxyPtr inClientProxy)
{

	int playerId = inClientProxy->GetPlayerId();

	ScoreBoardManager::sInstance->AddEntry(playerId, inClientProxy->GetName());
	SpawnTankForPlayer(playerId);
}

void Server::SpawnTankForPlayer(int inPlayerId)
{
	TankPtr tank = std::static_pointer_cast<Tank>(GameObjectRegistry::sInstance->CreateGameObject('TANK'));
	tank->SetColor(ScoreBoardManager::sInstance->GetEntry(inPlayerId)->GetColor());
	tank->SetPlayerId(inPlayerId);
	//gotta pick a better spawn location than this...
	tank->SetLocation(Vector3(600.f - static_cast<float>(inPlayerId), 400.f, 0.f));
	//set the players name he chose
	ClientProxyPtr clientProxy = NetworkManagerServer::sInstance->GetClientProxy(inPlayerId);
	if (clientProxy)
	{
		tank->SetPlayerName(clientProxy->GetName());
	}

	//make sure to send client its health and ammo info so it can update the HUD right away
	NetworkManagerServer::sInstance->SetStateDirty(tank->GetNetworkId(), Tank::ETRS_Health);
	NetworkManagerServer::sInstance->SetStateDirty(tank->GetNetworkId(), Tank::ETRS_Name);
}

void Server::HandleLostClient(ClientProxyPtr inClientProxy)
{
	//kill client's cat
	//remove client from scoreboard
	int playerId = inClientProxy->GetPlayerId();

	ScoreBoardManager::sInstance->RemoveEntry(playerId);
	TankPtr tank = GetTankForPlayer(playerId);
	if (tank)
	{
		tank->SetDoesWantToDie(true);
	}
}

TankPtr Server::GetTankForPlayer(int inPlayerId)
{
	//run through the objects till we find the cat...
	//it would be nice if we kept a pointer to the cat on the clientproxy
	//but then we'd have to clean it up when the cat died, etc.
	//this will work for now until it's a perf issue
	const auto& gameObjects = World::sInstance->GetGameObjects();
	for (int i = 0, c = gameObjects.size(); i < c; ++i)
	{
		GameObjectPtr go = gameObjects[i];
		Tank* tank = go->GetAsTank();
		if (tank && tank->GetPlayerId() == inPlayerId)
		{
			return std::static_pointer_cast<Tank>(go);
		}
	}

	return nullptr;

}
