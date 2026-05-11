class AmmoPickupServer : public AmmoPickup
{
public:
    static GameObjectPtr StaticCreate() { return NetworkManagerServer::sInstance->RegisterAndReturn(new AmmoPickupServer()); }

    virtual void Update() override;
};