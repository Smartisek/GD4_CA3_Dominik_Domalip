class AmmoPickup : public GameObject
{
public:
    CLASS_IDENTIFICATION('AMMO', GameObject)

        enum EAmmoPickupReplicationState
    {
        EARS_Pos = 1 << 0,
        EARS_Active = 1 << 1,
        EARS_AllState = EARS_Pos | EARS_Active
    };

    static GameObject* StaticCreate() { return new AmmoPickup(); }

    virtual uint32_t GetAllStateMask() const override { return EARS_AllState; }


    virtual void Update() override;

    int  GetAmmoAmount() const { return mAmmoAmount; }
    bool IsActive()      const { return mIsActive; }
    void SetActive(bool inActive) { mIsActive = inActive; }

    virtual uint32_t Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const override;

protected:
    AmmoPickup();

private:
    int  mAmmoAmount = 10;
    bool mIsActive = true;
};

typedef shared_ptr<AmmoPickup> AmmoPickupPtr;