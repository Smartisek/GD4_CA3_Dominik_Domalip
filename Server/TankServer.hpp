enum ETankControlType
{
	ETCT_Human,
	ETCT_AI
};

class TankServer : public Tank
{
public:

	static GameObjectPtr	StaticCreate() { return NetworkManagerServer::sInstance->RegisterAndReturn(new TankServer()); }

	virtual void HandleDying() override;
	virtual void Update() override;

	void SetCatControlType(ETankControlType inTankControlType) { mControlType = inTankControlType; }
	void TakeDamage(int inDamagingPlayerId, int inDamageAmount);

protected:
	TankServer();

private:
	void HandleShooting();
	void HandleInput();
	void HandleAI();

	ETankControlType mControlType;
	float mTimeOfNextShot;
	float mTimeBetweenShots;
	float mTurretDesiredRotation;  // Where turret should rotate towards
};

typedef shared_ptr<TankServer> TankServerPtr;