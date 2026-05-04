#pragma once

class Bullet : public GameObject
{
public:
	CLASS_IDENTIFICATION('BLLT', GameObject)

	enum EBulletReplicationState
	{
		EBRS_Pos = 1 << 0, //position and rotations 
		EBRS_Velocity = 1 << 1, //direction and speed
		EBRS_OwnerPlayerId = 1 << 2, //who shot this bullet

		EBRS_AllState = EBRS_Pos | EBRS_Velocity | EBRS_OwnerPlayerId
	};

	static GameObject* StaticCreate() { return new Bullet(); }

	virtual uint32_t GetAllStateMask() const override { return EBRS_AllState; }

	virtual Bullet* GetAsBullet() override { return this; }

	virtual void Update() override;

	//spawn at the tank that is firing 
	void InitializeFromTank(Tank* inShooter);

	void SetVelocity(const Vector3& inVelocity) { mVelocity = inVelocity; }

	const Vector3& GetVelocity() const { return mVelocity; }

	void SetPlayerId(int inPlayerId) { mOwnerPlayerId = inPlayerId; }

	int GetOwnerPlayerId() const { return mOwnerPlayerId; }

	virtual uint32_t Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const override;

protected:
	Bullet();

private:
	Vector3 mVelocity;
	int mOwnerPlayerId;
	float mLifeTime;
	const float mMaxLifeTime = 10.f; //when to despawn
};

typedef shared_ptr<Bullet> BulletPtr;
