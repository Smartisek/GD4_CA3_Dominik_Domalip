#pragma once

class Tank : public GameObject, public std::enable_shared_from_this<Tank>
{
public: 
	CLASS_IDENTIFICATION('TANK', GameObject)

		enum ETankReplicationState
	{
		//use bit flags to indicate what data has changed and needs to be replicated
		// 1 << 0 means in the first byte assign bit 0 and so on 
		// we are doing selective state replication, instead of sending the entire tank state every frame - we  only send what changed with help of these bit flags (which we were using in previous CAs for receiver categories 
		ETRS_Pos = 1 << 0, //bit 0 for position + body rotation 
		ETRS_TurretRotation = 1 << 1, //bit 1 for separate turret rotation
		ETRS_Color = 1 << 2, //bit 2 for color
		ETRS_PlayerId = 1 << 3, // bit 3 for player id
		ETRS_Health = 1 << 4, // fourth bit for heatlth
		ETRS_Ammo = 1 << 5, // bit 5 for ammo 
		ETRS_Name = 1 << 6, // bit 6 for name tag

		//this is all the bitstates combined, so we can easily send the entire state if we want to
		ETRS_AllState = ETRS_Pos | ETRS_TurretRotation | ETRS_Color | ETRS_PlayerId | ETRS_Health | ETRS_Ammo | ETRS_Name
	};

	
	static GameObject* StaticCreate() { return new Tank(); }

	//this will get the ETRS_AllState bit flag, which means we want to send the entire state of the tank
	virtual uint32_t GetAllStateMask() const override { return ETRS_AllState; }

	virtual Tank* GetAsTank() override { return this; }
	virtual void Update() override;

	//movement and inputs functions 
	void ProcessInput(float inDeltaTime, const InputState& inInputState);
	void SimulateMovement(float inDeltaTime);

	//collisions
	void ProcessCollisions();
	void ProcessCollisionsWithScreenWalls();

	//players id
	void SetPlayerId(uint32_t inPlayerId) { mPlayerId = inPlayerId; }
	uint32_t GetPlayerId() const { return mPlayerId; }

	//player tank speed
	void SetVelocity(const Vector3& inVelocity) { mVelocity = inVelocity; }
	const Vector3& GetVelocity() const { return mVelocity; }

	//independent rotatoin for the turret of tank 
	void SetTurretRotation(float inRotation) { mTurretRotation = inRotation; }
	float GetTurretRotation() const { return mTurretRotation; }
	void SetHealth(int inHealth) { mHealth = inHealth; }
	int	GetHealth() const { return mHealth; }

	//ammo and health
	void SetAmmo(int inAmmo) { mAmmo = inAmmo; }
	int	GetAmmo() const { return mAmmo; }

	//firing
	bool CanShoot() const { return mAmmo > 0 && mHealth > 0; }

	const string& GetPlayerName() const { return mPlayerName; }
	void          SetPlayerName(const string& inName) { mPlayerName = inName; }

	// replication 
	virtual uint32_t Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const override;

protected:
	Tank();

private:
	void AdjustVelocityByThrust(float inDeltaTime);

	//movement variables 
	Vector3 mVelocity;
	float mMaxLinearSpeed;
	float mMaxRotationSpeed;
	float mThrustDir;

	//turret rotation
	float mTurretRotation;		//independent from body rotation
	float mTurretRotationSpeed;

	//physic when hitting things 
	float mWallRestitution;
	float mTankRestitution;

	//state variables 
	uint32_t	mPlayerId;
	string mPlayerName;
protected:
	bool mIsShooting;
	float		mLastMoveTimestamp;
	int			mHealth;
	int			mAmmo;
};

typedef shared_ptr<Tank> TankPtr;

