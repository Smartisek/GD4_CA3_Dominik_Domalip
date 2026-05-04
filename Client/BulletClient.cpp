#include "SocketWrapperClientPCH.hpp"

BulletClient::BulletClient()
{
	mSpriteComponent.reset(new SpriteComponent(this));
	mSpriteComponent->SetTexture(TextureManager::sInstance->GetTexture("bullet"));
}

void BulletClient::Read(InputMemoryBitStream& inInputStream)
{
	bool stateBit;

	// Read OwnerPlayerId
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		uint32_t ownerPlayerId;
		inInputStream.Read(ownerPlayerId);
		// Set owner (you may need a setter in Bullet class)
	}

	// Read Pose (position + rotation)
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		Vector3 location;
		inInputStream.Read(location.mX);
		inInputStream.Read(location.mY);
		SetLocation(location);

		float rotation;
		inInputStream.Read(rotation);
		SetRotation(rotation);
	}

	// Read Velocity
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		Vector3 velocity;
		inInputStream.Read(velocity.mX);
		inInputStream.Read(velocity.mY);
		SetVelocity(velocity);
	}
}