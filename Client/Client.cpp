#include "SocketWrapperClientPCH.hpp"

bool Client::StaticInit()
{
	// Create the Client pointer first because it initializes SDL
	Client* client = new Client();
	InputManager::StaticInit();

	WindowManager::StaticInit();
	FontManager::StaticInit();
	TextureManager::StaticInit();
	RenderManager::StaticInit();


	HUD::StaticInit();

	s_instance.reset(client);

	return true;
}

Client::Client()
{
	std::cout << "DEBUG: Registering TNK creation function\n";
	GameObjectRegistry::sInstance->RegisterCreationFunction('TANK', TankClient::StaticCreate);
	std::cout << "DEBUG: Registering BLLT creation function\n";
	GameObjectRegistry::sInstance->RegisterCreationFunction('BLLT', BulletClient::StaticCreate);

	std::cout << "DEBUG: Getting command line args\n";
	string destination = StringUtils::GetCommandLineArg(1);
	string name = StringUtils::GetCommandLineArg(2);
	std::cout << "DEBUG: destination=" << destination << " name=" << name << "\n";

	std::cout << "DEBUG: Creating socket address\n";
	SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString(destination);
	std::cout << "DEBUG: Initializing network manager\n";
	NetworkManagerClient::StaticInit(*serverAddress, name);

	//NetworkManagerClient::sInstance->SetSimulatedLatency(0.0f);
}



void Client::DoFrame()
{
	InputManager::sInstance->Update();

	Engine::DoFrame();

	NetworkManagerClient::sInstance->ProcessIncomingPackets();

	RenderManager::sInstance->Render();

	NetworkManagerClient::sInstance->SendOutgoingPackets();
}

void Client::HandleEvent(sf::Event& p_event)
{
	switch (p_event.type)
	{
	case sf::Event::KeyPressed:
		InputManager::sInstance->HandleInput(EIA_Pressed, p_event.key.code);
		break;
	case sf::Event::KeyReleased:
		InputManager::sInstance->HandleInput(EIA_Released, p_event.key.code);
		break;
	default:
		break;
	}
}

bool Client::PollEvent(sf::Event& p_event)
{
	return WindowManager::sInstance->pollEvent(p_event);
}


