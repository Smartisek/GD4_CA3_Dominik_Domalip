#include "SocketWrapperClientPCH.hpp"

std::unique_ptr<sf::RenderWindow>	WindowManager::sInstance;

bool WindowManager::StaticInit()
{
	sInstance.reset(new sf::RenderWindow(sf::VideoMode(kWindowWidth, kWindowHeight), "ARMORED ASSAULT"));
	return true;
}


