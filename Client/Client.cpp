#include "SocketWrapperClientPCH.hpp"
Client* Client::sInstance = nullptr;

//menu ui cinstants 
float cx = kWindowWidth / 2.f;   //horizontal centre
float cy = kWindowHeight / 2.f + 250.f;  //vertical centre
float boxWidth = 400.f;
float boxHeight = 45.f;
float buttonWidth = 200.f;

bool Client::StaticInit()
{
	// Create the Client pointer first because it initializes SDL
	Client* client = new Client();
	InputManager::StaticInit();
	WindowManager::StaticInit();
	FontManager::StaticInit();
	TextureManager::StaticInit();
	RenderManager::StaticInit();
	SoundPlayer::StaticInit();
	HUD::StaticInit();

	s_instance.reset(client);
	sInstance = client;

	return true;
}

Client::Client()
{
	GameObjectRegistry::sInstance->RegisterCreationFunction('TANK', TankClient::StaticCreate);
	GameObjectRegistry::sInstance->RegisterCreationFunction('BLLT', BulletClient::StaticCreate);
	GameObjectRegistry::sInstance->RegisterCreationFunction('AMMO', AmmoPickupClient::StaticCreate);
	//NetworkManagerClient::StaticInit(*serverAddress, name);

	//NetworkManagerClient::sInstance->SetSimulatedLatency(0.0f);
}

void Client::ConnectToServer(const std::string& address, const std::string& name)
{
	SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString(address);
	NetworkManagerClient::StaticInit(*serverAddress, name);
	mIsConnected = true;
}

void Client::OnGameOver(InputMemoryBitStream& inInputStream)
{
	ScoreBoardManager::sInstance->Read(inInputStream);
	mGameOver = true;
}

void Client::DoFrame()
{
	InputManager::sInstance->Update();

	if (!mIsConnected)
	{
		RenderMenu();
		return; //dont render anything else until were connected to server 
	}
	//game over call
	if (mGameOver)
	{
		RenderGameOver();
		return;
	}

	LOG("In game! mIsConnected=%d, mGameOver=%d, Timer=%.1f", mIsConnected, mGameOver, mGameTimer);
	//reduce timer
	mGameTimer -= Timing::sInstance.GetDeltaTime();
	if (mGameTimer < 0.f) mGameTimer = 0.f;

	HUD::sInstance->SetGameTimer(mGameTimer); //set the time to game time

	Engine::DoFrame();

	NetworkManagerClient::sInstance->ProcessIncomingPackets();

	SoundPlayer::sInstance->RemoveStoppedSounds();

	RenderManager::sInstance->Render();

	NetworkManagerClient::sInstance->SendOutgoingPackets();
}

// again, not ideal to have menu logic inside the client class but for a simple menu it works, for a more complex menu system I would probably make a separate class for it
void Client::RenderMenu()
{
	auto font = FontManager::sInstance->GetFont("carlito");

	WindowManager::sInstance->clear(sf::Color::Black);

	//ddraw background
	auto bgTexture = TextureManager::sInstance->GetTexture("menuBackground");
	sf::Sprite bgSprite;
	bgSprite.setTexture(*bgTexture);
	//scale to fit window size
	sf::Vector2u texSize = bgTexture->getSize();
	bgSprite.setScale(
		(float)kWindowWidth / texSize.x,
		(float)kWindowHeight / texSize.y
	);
	WindowManager::sInstance->draw(bgSprite);

	//address box
	sf::RectangleShape addressBox(sf::Vector2f(400.f, 45.f));
	addressBox.setPosition(cx - boxWidth / 2.f, cy + 20.f);
	//change color if active or not
	addressBox.setFillColor(mAddressActive ? sf::Color(60, 60, 80) : sf::Color(40, 40, 60));
	addressBox.setOutlineThickness(2.f);
	addressBox.setOutlineColor(mAddressActive ? sf::Color(255, 200, 0) : sf::Color(120, 120, 140));
	WindowManager::sInstance->draw(addressBox);

	//what to display above the address box
	sf::Text addressLabel;
	addressLabel.setFont(*font);
	addressLabel.setString("Server (ip:port)");
	addressLabel.setCharacterSize(18);
	addressLabel.setFillColor(sf::Color(255, 170, 51));
	addressLabel.setPosition(cx - boxWidth / 2.f, cy - 10.f);
	WindowManager::sInstance->draw(addressLabel);

	//what the user has typed in for the address
	sf::Text addressText;
	addressText.setFont(*font);
	addressText.setString(mAddressInput + (mAddressActive ? "|" : ""));
	addressText.setCharacterSize(22);
	addressText.setFillColor(sf::Color::White);
	addressText.setPosition(cx - boxWidth / 2.f + 10.f, cy + 20.f + 10.f);
	WindowManager::sInstance->draw(addressText);

	//name box
	sf::RectangleShape nameBox(sf::Vector2f(400.f, 45.f));
	nameBox.setPosition(cx - boxWidth / 2.f, cy + 110.f);
	nameBox.setFillColor(mNameActive ? sf::Color(60, 60, 80) : sf::Color(40, 40, 60));
	nameBox.setOutlineThickness(2.f);
	nameBox.setOutlineColor(mNameActive ? sf::Color(255, 200, 0) : sf::Color(120, 120, 140));
	WindowManager::sInstance->draw(nameBox);

	//what to display above the name box
	sf::Text nameLabel;
	nameLabel.setFont(*font);
	nameLabel.setString("Player Name");
	nameLabel.setCharacterSize(18);
	nameLabel.setFillColor(sf::Color(255, 170, 51));
	nameLabel.setPosition(cx - boxWidth / 2.f, cy + 80.f);
	WindowManager::sInstance->draw(nameLabel);

	//what the user has typed in for the name
	sf::Text nameText;
	nameText.setFont(*font);
	nameText.setString(mNameInput + (mNameActive ? "|" : ""));
	nameText.setCharacterSize(22);
	nameText.setFillColor(sf::Color::White);
	nameText.setPosition(cx - boxWidth / 2.f + 10.f, cy + 110.f + 10.f);
	WindowManager::sInstance->draw(nameText);

	//connect button
	bool canConnect = !mAddressInput.empty() && !mNameInput.empty(); //can connect if filled fields
	sf::RectangleShape button(sf::Vector2f(200.f, 50.f));
	button.setPosition(cx - buttonWidth / 2.f, cy + 185.f);
	button.setFillColor(canConnect ? sf::Color(0, 160, 80) : sf::Color(80, 80, 80));
	button.setOutlineThickness(2.f);
	button.setOutlineColor(canConnect ? sf::Color(0, 220, 100) : sf::Color(100, 100, 100));
	WindowManager::sInstance->draw(button);

	sf::Text buttonText;
	buttonText.setFont(*font);
	buttonText.setString("CONNECT");
	buttonText.setCharacterSize(24);
	buttonText.setStyle(sf::Text::Bold);
	sf::FloatRect textBounds = buttonText.getLocalBounds();
	buttonText.setOrigin(textBounds.left + textBounds.width / 2.f,
		textBounds.top + textBounds.height / 2.f);
	buttonText.setPosition(cx, cy + 185.f + 25.f); //centre of button
	WindowManager::sInstance->draw(buttonText);

	//error message if they try to connect without filling in both fields
	if (!canConnect)
	{
		sf::Text hint;
		hint.setFont(*font);
		hint.setString("Fill in both fields to connect");
		hint.setCharacterSize(16);
		hint.setFillColor(sf::Color(180, 80, 80));
		hint.setPosition(cx - boxWidth / 2.f, cy + 245.f);
		WindowManager::sInstance->draw(hint);
	}

	WindowManager::sInstance->setMouseCursorVisible(true);
	WindowManager::sInstance->display();
}

void Client::RenderGameOver()
{
	WindowManager::sInstance->clear(sf::Color(10, 10, 30));
	auto font = FontManager::sInstance->GetFont("carlito");

	// Title
	sf::Text title;
	title.setFont(*font);
	title.setString("GAME OVER");
	title.setCharacterSize(72);
	title.setStyle(sf::Text::Bold);
	title.setFillColor(sf::Color(255, 200, 0));
	title.setOutlineColor(sf::Color::Black);
	title.setOutlineThickness(3.f);
	sf::FloatRect tb = title.getLocalBounds();
	title.setOrigin(tb.left + tb.width / 2.f, tb.top);
	title.setPosition(kWindowWidth / 2.f, 80.f);
	WindowManager::sInstance->draw(title);

	//sort score descending
	auto entries = ScoreBoardManager::sInstance->GetEntries();
	std::sort(entries.begin(), entries.end(),
		[](const ScoreBoardManager::Entry& a, const ScoreBoardManager::Entry& b) {
			return a.GetScore() > b.GetScore();
		});

	float y = 220.f;
	for (int i = 0; i < (int)entries.size(); i++)
	{
		const auto& entry = entries[i];
		char buf[64];
		snprintf(buf, sizeof(buf), "#%d  %s  -  %d kills",
			i + 1, entry.GetPlayerName().c_str(), entry.GetScore());

		sf::Text line;
		line.setFont(*font);
		line.setString(buf);
		line.setCharacterSize(36);
		line.setFillColor(i == 0 ? sf::Color(255, 215, 0) : sf::Color::White);
		line.setOutlineColor(sf::Color::Black);
		line.setOutlineThickness(1.5f);
		sf::FloatRect lb = line.getLocalBounds();
		line.setOrigin(lb.left + lb.width / 2.f, lb.top);
		line.setPosition(kWindowWidth / 2.f, y);
		WindowManager::sInstance->draw(line);
		y += 60.f;
	}

	WindowManager::sInstance->display();
}

void Client::HandleMenuEvent(sf::Event& p_event)
{
	if (p_event.type == sf::Event::MouseButtonPressed)
	{
		float mx = (float)p_event.mouseButton.x;
		float my = (float)p_event.mouseButton.y;

		//typing in address box 
		mAddressActive = (mx >= cx - boxWidth / 2.f && mx <= cx + boxWidth / 2.f &&
			my >= cy + 20.f && my <= cy + 65.f);
		//typing in name box 
		mNameActive = (mx >= cx - boxWidth / 2.f && mx <= cx + boxWidth / 2.f &&
			my >= cy + 110.f && my <= cy + 155.f);

		//button cliked 
		bool canConnect = !mAddressInput.empty() && !mNameInput.empty();
		if(canConnect && mx >= cx - 100.f && mx <= cx + 100.f &&
			my >= cy + 185.f && my <= cy + 235.f)
		{
			ConnectToServer(mAddressInput, mNameInput);
		}
	}

	if (p_event.type == sf::Event::TextEntered)
	{
		std::string* target = nullptr;
		if (mAddressActive) target = &mAddressInput; //if address box active, type there
		else if (mNameActive) target = &mNameInput; //if name box active, type there

		if (target)
		{
			if (p_event.text.unicode == '\b')
			{
				if (!target->empty()) target->pop_back(); 
			}
			else if (p_event.text.unicode < 128) 
			{
				*target += static_cast<char>(p_event.text.unicode);
			}
		}
	}

	//tab swithing fields
	if (p_event.type == sf::Event::KeyPressed &&
		p_event.key.code == sf::Keyboard::Tab)
	{
		if (mAddressActive) { mAddressActive = false; mNameActive = true; }
		else { mAddressActive = true;  mNameActive = false; }
	}

	//connect with enter as well
	if (p_event.type == sf::Event::KeyPressed &&
		p_event.key.code == sf::Keyboard::Enter)
	{
		if (!mAddressInput.empty() && !mNameInput.empty())
			ConnectToServer(mAddressInput, mNameInput);
	}
}

void Client::HandleEvent(sf::Event& p_event)
{
	if (!mIsConnected)
	{
		HandleMenuEvent(p_event);
		return;
	}

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
