#include "SocketWrapperClientPCH.hpp"

TankClient::TankClient() :
	mTimeLocationBecameOutOfSync(0.f),
	mTimeVelocityBecameOutOfSync(0.f),
	mTimeTurretBecameOutOfSync(0.f),
	mLastKnownHealth(kMaxTankHealth), 
	mHitFlashTimer(0.f)
{
	// Main tank body sprite
	SetScale(kTankSpriteScale);
	mSpriteComponent.reset(new PlayerSpriteComponent(this));
	mSpriteComponent->SetTexture(TextureManager::sInstance->GetTexture("tank"));

	mHealthBarComponent.reset(new HealthBarSpriteComponent(this));

	// Turret sprite (separate, rotates independently)
	SetScale(kTurretSpriteScale);
	mTurretSpriteComponent.reset(new TurretSpriteComponent(this));
	mTurretSpriteComponent->SetTexture(TextureManager::sInstance->GetTexture("turret"));

	auto font = FontManager::sInstance->GetFont("carlito");
	mNameTag.setFont(*font);
	mNameTag.setCharacterSize(16);
	mNameTag.setOutlineColor(sf::Color::Black);
	mNameTag.setOutlineThickness(1.5f);
}

void TankClient::HandleDying()
{
	Tank::HandleDying();

	SoundPlayer::sInstance->Play(SoundEffect::kExplosion,
		GetLocation().mX, GetLocation().mY);

	// If this is our local tank, update HUD
	if (GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId())
	{
		HUD::sInstance->SetPlayerHealth(0);
	}
}

void TankClient::Update()
{
	//pop up logic update
	float dt = Timing::sInstance.GetDeltaTime();
	//set red blick for the tank
	if (mHitFlashTimer > 0.f)
	{
		mHitFlashTimer -= dt;
		if (mSpriteComponent && mTurretSpriteComponent)
		{
			mSpriteComponent->GetSprite().setColor(sf::Color(255, 80, 80, 255));
			mTurretSpriteComponent->GetSprite().setColor(sf::Color(255, 80, 80, 255));
		}
	}
	else
	{
		mSpriteComponent->GetSprite().setColor(sf::Color::White);
		mTurretSpriteComponent->GetSprite().setColor(sf::Color::White);
	}

	for (int i = (int)mDamagePopups.size() - 1; i >= 0; i--)
	{
		mDamagePopups[i]->Update(dt);
		if (mDamagePopups[i]->IsExpired())
		{
			mDamagePopups.erase(mDamagePopups.begin() + i);
		}
	}

	// Is this the tank owned by us (local player)?
	if (GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId())
	{
		const Move* pendingMove = InputManager::sInstance->GetAndClearPendingMove();
		if (pendingMove)
		{
			//UpdateTurretAim();

			float deltaTime = pendingMove->GetDeltaTime();
			const InputState& inputState = pendingMove->GetInputState();

			// Apply input locally (includes body movement and turret rotation)
			ProcessInput(deltaTime, pendingMove->GetInputState());


			// Simulate movement
			SimulateMovement(deltaTime);

			// Update HUD with local tank state
			HUD::sInstance->SetPlayerHealth(GetHealth());
			HUD::sInstance->SetPlayerAmmo(GetAmmo());
		}
	}
	else
	{
		// Remote tank: just simulate movement based on last known velocity
		SimulateMovement(Timing::sInstance.GetDeltaTime());

		// Check if we're back in sync
		if (Math::Is2DVectorEqual(GetVelocity(), Vector3::Zero))
		{
			mTimeLocationBecameOutOfSync = 0.f;
		}
	}
}

void TankClient::DrawDamagePopups(sf::RenderWindow& window)
{
	for (const auto& popup : mDamagePopups)
	{
		popup.get()->Draw(*WindowManager::sInstance);
	}
}

void TankClient::DrawNameTag(sf::RenderWindow& window)
{
	bool isLocal = NetworkManagerClient::sInstance &&
		GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId();

	std::string name = GetPlayerName();
	if (name.empty()) return;

	mNameTag.setString(name);
	mNameTag.setFillColor(isLocal ? sf::Color(0, 255, 100) : sf::Color::White);

	sf::FloatRect bounds = mNameTag.getLocalBounds();
	mNameTag.setOrigin(bounds.left + bounds.width / 2.f, bounds.top);

	//use same rotation math as HealthBarSpriteComponent to stay fixed under health bar
	Vector3 pos = GetLocation();
	float rot = GetRotation();
	float rotRad = Math::ToRadians(rot);
	float cosR = cosf(rotRad);
	float sinR = sinf(rotRad);

	float localX = 0.f;  //centred horizontally on tank
	float localY = kHealthBarOffset + kHealthBarHeight + 18.f;  //just below health bar

	float worldX = pos.mX + (localX * cosR - localY * sinR);
	float worldY = pos.mY + (localX * sinR + localY * cosR);

	mNameTag.setPosition(worldX, worldY);
	mNameTag.setRotation(rot);  //rotate the text with the tank

	window.draw(mNameTag);
}

void TankClient::UpdateTurretAim()
{
	if (GetPlayerId() != NetworkManagerClient::sInstance->GetPlayerId())
	{
		return;
	}

	sf::Vector2i mousePixel = sf::Mouse::getPosition();
	sf::Vector2f mouseWorld = WindowManager::sInstance->mapPixelToCoords(mousePixel);

	Vector3 tankPos = GetLocation();

	float dx = mouseWorld.x - tankPos.mX;
	float dy = mouseWorld.y - tankPos.mY;

	float angleDeg = Math::ToDegrees(atan2(dy, dx));
	SetTurretRotation(angleDeg);
}

void TankClient::Read(InputMemoryBitStream& inInputStream)
{
	bool stateBit;
	uint32_t readState = 0;

	// Read PlayerId
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		uint32_t playerId;
		inInputStream.Read(playerId);
		SetPlayerId(playerId);
		readState |= ETRS_PlayerId;
	}

	// Save old state for interpolation
	float oldRotation = GetRotation();
	float oldTurretRotation = GetTurretRotation();
	Vector3 oldLocation = GetLocation();
	Vector3 oldVelocity = GetVelocity();

	// Read Pose (position, body rotation, velocity)
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		Vector3 replicatedVelocity;
		inInputStream.Read(replicatedVelocity.mX);
		inInputStream.Read(replicatedVelocity.mY);
		SetVelocity(replicatedVelocity);

		Vector3 replicatedLocation;
		inInputStream.Read(replicatedLocation.mX);
		inInputStream.Read(replicatedLocation.mY);
		SetLocation(replicatedLocation);

		float replicatedRotation;
		inInputStream.Read(replicatedRotation);
		SetRotation(replicatedRotation);

		readState |= ETRS_Pos;
	}

	// Read thrust direction (compact format)
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		bool isThrustPositive;
		inInputStream.Read(isThrustPositive);
		// mThrustDir = isThrustPositive ? 1.f : -1.f;
	}

	// Read TurretRotation (separate from body rotation)
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		float replicatedTurretRotation;
		inInputStream.Read(replicatedTurretRotation);
		SetTurretRotation(replicatedTurretRotation);
		readState |= ETRS_TurretRotation;
	}

	// Read Color
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		Vector3 color;
		inInputStream.Read(color);
		SetColor(color);
		readState |= ETRS_Color;
	}

	// Read Health
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		uint8_t health;
		inInputStream.Read(health, 8);
		LOG("Client tank %d health=%d", GetPlayerId(), health);

		//**** UI pop up logic when getting hit 
		int newHealth = static_cast<int>(health);
		if (newHealth < mLastKnownHealth)
		{
			int damage = mLastKnownHealth - newHealth; //calculate the damage(could also just use the constant)

			SoundPlayer::sInstance->Play(SoundEffect::kTankHit,
				GetLocation().mX, GetLocation().mY);

			auto font = FontManager::sInstance->GetFont("carlito");
			if (font)
			{
				std::string msg = "-" + std::to_string(damage);
				mDamagePopups.push_back(make_shared<DamagePopupComponent>(
					this,
					msg,
					GetLocation().mX,
					GetLocation().mY
				));
			}
			//red blick timer
			mHitFlashTimer = kFlashDuration;
		}
		mLastKnownHealth = newHealth;
		// *****

		SetHealth(health);
		readState |= ETRS_Health;
	}

	// Read Ammo
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		uint8_t ammo;
		inInputStream.Read(ammo, 8);

		int newAmmo = static_cast<int>(ammo);
		if (newAmmo > static_cast<int>(GetAmmo()))
		{
			int gained = newAmmo - static_cast<int>(GetAmmo());
			auto font = FontManager::sInstance->GetFont("carlito");
			if (font)
			{
				std::string msg = "+" + std::to_string(gained) + " ammo";
				mDamagePopups.push_back(make_shared<DamagePopupComponent>(
					this,
					msg,
					GetLocation().mX,
					GetLocation().mY,
					sf::Color(0, 220, 100, 255)
				));
			}
		}

		SetAmmo(ammo);
		readState |= ETRS_Ammo;
	}

	//read name 
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		string name;
		inInputStream.Read(name);
		SetPlayerName(name);
		readState |= ETRS_Name;
	}

	// Apply client-side prediction
	if (GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId())
	{
		DoClientSidePredictionAfterReplicationForLocalTank(readState);
	}
	else
	{
		DoClientSidePredictionAfterReplicationForRemoteTank(readState);
	}
}

void TankClient::DoClientSidePredictionAfterReplicationForLocalTank(uint32_t inReadState)
{
	// Local tank prediction - keep movement smooth
	// The server will correct if needed
}

void TankClient::DoClientSidePredictionAfterReplicationForRemoteTank(uint32_t inReadState)
{
	// Remote tank prediction - interpolate smoothly to new position
	float oldRotation = GetRotation();
	float oldTurretRotation = GetTurretRotation();
	Vector3 oldLocation = GetLocation();
	Vector3 oldVelocity = GetVelocity();

	InterpolateClientSidePrediction(oldRotation, oldTurretRotation, oldLocation, oldVelocity, true);
}

void TankClient::InterpolateClientSidePrediction(float inOldRotation, float inOldTurretRotation, const Vector3& inOldLocation, const Vector3& inOldVelocity, bool inIsForRemoteTank)
{
	if (inOldRotation != GetRotation() && !inIsForRemoteTank)
	{
		LOG("ERROR! Move replay ended with incorrect rotation!", 0);
	}

	float roundTripTime = NetworkManagerClient::sInstance->GetRoundTripTime();

	if (!Math::Is2DVectorEqual(inOldLocation, GetLocation()))
	{
		//LOG( "ERROR! Move replay ended with incorrect location!", 0 );

		//have we been out of sync, or did we just become out of sync?
		float time = Timing::sInstance.GetFrameStartTime();
		if (mTimeLocationBecameOutOfSync == 0.f)
		{
			mTimeLocationBecameOutOfSync = time;
		}

		float durationOutOfSync = time - mTimeLocationBecameOutOfSync;
		if (durationOutOfSync < roundTripTime)
		{
			SetLocation(Lerp(inOldLocation, GetLocation(), inIsForRemoteTank ? (durationOutOfSync / roundTripTime) : 0.1f));
		}
	}
	else
	{
		//we're in sync
		mTimeLocationBecameOutOfSync = 0.f;
	}


	if (!Math::Is2DVectorEqual(inOldVelocity, GetVelocity()))
	{
		//LOG( "ERROR! Move replay ended with incorrect velocity!", 0 );

		//have we been out of sync, or did we just become out of sync?
		float time = Timing::sInstance.GetFrameStartTime();
		if (mTimeVelocityBecameOutOfSync == 0.f)
		{
			mTimeVelocityBecameOutOfSync = time;
		}

		//now interpolate to the correct value...
		float durationOutOfSync = time - mTimeVelocityBecameOutOfSync;
		if (durationOutOfSync < roundTripTime)
		{
			SetVelocity(Lerp(inOldVelocity, GetVelocity(), inIsForRemoteTank ? (durationOutOfSync / roundTripTime) : 0.1f));
		}
		//otherwise, fine...

	}
	else
	{
		//we're in sync
		mTimeVelocityBecameOutOfSync = 0.f;
	}

}