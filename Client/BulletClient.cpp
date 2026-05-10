#include "SocketWrapperClientPCH.hpp"

BulletClient::BulletClient()
{
	SetScale(kBulletSpriteScale);
	mSpriteComponent.reset(new SpriteComponent(this));
	mSpriteComponent->SetTexture(TextureManager::sInstance->GetTexture("bullet"));

	//play shoot sound when created - not ideal but we can only call sound player from client project 
	SoundPlayer::sInstance->Play(SoundEffect::kBulletFire,
		GetLocation().mX, GetLocation().mY);
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