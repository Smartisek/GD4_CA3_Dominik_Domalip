//habdkes rendering input prediction and network synchronization for tanks in the game 
//EXAMPLE
//Server sends tank update
//↓
//TankClient.Read() receives new state
//↓
//Apply server state to tank
//↓
//Predict next movement based on velocity
//↓
//Interpolate smoothly between old and predicted position
//↓
//Render on screen
class TankClient : public Tank
{
public:
	static GameObjectPtr StaticCreate() { return GameObjectPtr(new TankClient()); }

	virtual void Update() override;
	virtual void HandleDying() override;
	void DrawDamagePopups(sf::RenderWindow& window);
	void DrawNameTag(sf::RenderWindow& window);

	virtual void Read(InputMemoryBitStream& inInputStream) override;

	void DoClientSidePredictionAfterReplicationForLocalTank(uint32_t inReadState);
	void DoClientSidePredictionAfterReplicationForRemoteTank(uint32_t inReadState);

protected:
	TankClient();

private:
	void InterpolateClientSidePrediction(float inOldRotation, float inOldTurretRotation, const Vector3& inOldLocation, const Vector3& inOldVelocity, bool inIsForRemoteTank);
	//void RenderTurret();
	void UpdateTurretAim();

	float mTimeLocationBecameOutOfSync;
	float mTimeVelocityBecameOutOfSync;
	float mTimeTurretBecameOutOfSync;

	SpriteComponentPtr mSpriteComponent;
	SpriteComponentPtr mTurretSpriteComponent;  //separate turret sprite
	SpriteComponentPtr mHealthBarComponent;

	int   mLastKnownHealth;           // track previous health to detect hits
	float mHitFlashTimer;             // red flash duration remaining
	static constexpr float kFlashDuration = 0.18f;
	std::vector<shared_ptr<DamagePopupComponent>> mDamagePopups;

	sf::Text mNameTag;

};