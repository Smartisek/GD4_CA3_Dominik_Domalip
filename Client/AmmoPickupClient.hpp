#pragma once
class AmmoPickupClient : public AmmoPickup
{
public:
    static GameObjectPtr StaticCreate() { return GameObjectPtr(new AmmoPickupClient()); }

    AmmoPickupClient();

    virtual void Read(InputMemoryBitStream& inInputStream) override;

private:
    SpriteComponentPtr mSpriteComponent;
};