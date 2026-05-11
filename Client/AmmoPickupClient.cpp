#include "SocketWrapperClientPCH.hpp"

AmmoPickupClient::AmmoPickupClient()
{
    mSpriteComponent.reset(new SpriteComponent(this));
    mSpriteComponent->SetTexture(TextureManager::sInstance->GetTexture("ammoPickup"));
}

void AmmoPickupClient::Read(InputMemoryBitStream& inInputStream)
{
    bool stateBit;

    inInputStream.Read(stateBit);
    if (stateBit)
    {
        Vector3 location;
        inInputStream.Read(location.mX);
        inInputStream.Read(location.mY);
        SetLocation(location);
    }

    inInputStream.Read(stateBit);
    if (stateBit)
    {
        bool active;
        inInputStream.Read(active);
        SetActive(active);
        //hide sprite when picked up
        if (!active && mSpriteComponent)
            mSpriteComponent->GetSprite().setColor(sf::Color::Transparent);
    }
}