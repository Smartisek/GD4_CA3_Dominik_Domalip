class Server : public Engine
{
public:

	static bool StaticInit();

	virtual void DoFrame() override;

	virtual int Run();

	void HandleNewClient(ClientProxyPtr inClientProxy);
	void HandleLostClient(ClientProxyPtr inClientProxy);

	TankPtr	GetTankForPlayer(int inPlayerId);
	void	SpawnTankForPlayer(int inPlayerId);

	float GetGameTimer() const { return mGameTimer; }


private:
	Server();

	bool	InitNetworkManager();
	void	SetupWorld();

	void SpawnAmmoPickups(int inCount);
	int  CountAmmoPickups();

	void HandleGameOver();


	static const int kMaxAmmoPickups = 5;
	float mPickupSpawnTimer = 15.f;

	float mGameTimer = 30.f;
	bool mGameOver = false; 
	bool mGameStarted = false;

};

