class Client : public Engine
{
public:

	static bool StaticInit();
	static Client* sInstance;
	void OnGameOver(InputMemoryBitStream& inInputStream);
	void SetGameTimer(float inTimer) { mGameTimer = inTimer; }

protected:

	Client();

	virtual void	DoFrame() override;
	virtual void	HandleEvent(sf::Event& p_event) override;
	virtual bool	PollEvent(sf::Event& p_event) override;


private:
	void ConnectToServer(const std::string& address, const std::string& name);
	void RenderMenu();
	void HandleMenuEvent(sf::Event& p_event);
	void RenderGameOver();

	bool            mIsConnected = false;
	std::string     mAddressInput = "127.0.0.1:50000";  //default vaule 
	std::string     mNameInput    = "Player";
	bool            mAddressActive = false;
	bool            mNameActive = false;

	//ui for the menu, not really ideal approach to make here but works for my simple menu, for a more complex menu system I would probably make a separate class for it
	sf::RectangleShape  mAddressBox;
	sf::RectangleShape  mNameBox;
	sf::RectangleShape  mConnectButton;
	sf::Text            mAddressText;
	sf::Text            mNameText;
	sf::Text            mButtonText;
	sf::Text            mTitleText;

	bool  mGameOver = false;
	float mGameTimer = 0.f; //server will send this info in the welcome pack 
};
