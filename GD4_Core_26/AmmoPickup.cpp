#include "SocketWrapperPCH.hpp"

AmmoPickup::AmmoPickup() : mIsActive(true) 
{
    SetCollisionRadius(0.f); //we use overlap instead 
}

void AmmoPickup::Update()
{
    // none needed because server will handle the collision and all logic 
}

//write active and position for the pickup
uint32_t AmmoPickup::Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const
{
    uint32_t writtenState = 0;

    if (inDirtyState & EARS_Pos)
    {
        inOutputStream.Write((bool)true);
        Vector3 location = GetLocation();
        inOutputStream.Write(location.mX);
        inOutputStream.Write(location.mY);
        writtenState |= EARS_Pos;
    }
    else { inOutputStream.Write((bool)false); }

    if (inDirtyState & EARS_Active)
    {
        inOutputStream.Write((bool)true);
        inOutputStream.Write(mIsActive);
        writtenState |= EARS_Active;
    }
    else { inOutputStream.Write((bool)false); }

    return writtenState;
}