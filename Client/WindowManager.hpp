//this file is responsible for managing the window of the game, it will be used to create the window and manage it throughout the game


class WindowManager
{

public:

	static bool StaticInit();
	static std::unique_ptr< sf::RenderWindow >	sInstance;
};

