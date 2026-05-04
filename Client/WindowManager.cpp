#include "SocketWrapperClientPCH.hpp"

std::unique_ptr<sf::RenderWindow>	WindowManager::sInstance;

bool WindowManager::StaticInit()
{
	sInstance.reset(new sf::RenderWindow(sf::VideoMode(1920, 1080), "ARMORED ASSAULT"));
	return true;
}


