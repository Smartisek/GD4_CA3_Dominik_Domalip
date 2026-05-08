#include "SocketWrapperClientPCH.hpp"

TexturePtr HealthBarSpriteComponent::sWhiteTexture;

//constuct
HealthBarSpriteComponent::HealthBarSpriteComponent(GameObject* inGameObject) :
	SpriteComponent(inGameObject)
{
	//make white texture so no import is needed for halth bar
	if (!sWhiteTexture)
	{
		sWhiteTexture = std::make_shared<sf::Texture>();
		sf::Image img;
		img.create(1, 1, sf::Color::White);
		sWhiteTexture->loadFromImage(img);
	}

	m_sprite.setTexture(*sWhiteTexture);
	m_sprite.setOrigin(0.f, 0.5f);
}

sf::Sprite& HealthBarSpriteComponent::GetSprite()
{
	auto pos = mGameObject->GetLocation();
	auto rot = mGameObject->GetRotation();

	Tank* tank = mGameObject->GetAsTank();
	if (!tank)
	{
		return m_sprite;
	}

	//progress bar for health
	float currentHealth = static_cast<float>(tank->GetHealth()) / static_cast<float>(kMaxTankHealth);
	if (currentHealth < 0.f) currentHealth = 0.f;
	if (currentHealth > 1.f) currentHealth = 1.f;

	float barWidth = kHealthBarWidth * currentHealth;
	if (barWidth < 1.f) barWidth = 1.f;

	// local offset under tank, rotate with body
	float rotRad = Math::ToRadians(rot);
	float cosR = cosf(rotRad);
	float sinR = sinf(rotRad);

	float localX = -kHealthBarWidth * 0.5f;
	float localY = kHealthBarOffset;

	float worldX = pos.mX + (localX * cosR - localY * sinR);
	float worldY = pos.mY + (localX * sinR + localY * cosR);

	m_sprite.setPosition(worldX, worldY);
	m_sprite.setRotation(rot);

	m_sprite.setScale(barWidth, kHealthBarHeight);

	//color from green to red based on health
	sf::Uint8 r = static_cast<sf::Uint8>((1.f - currentHealth) * 255.f);
	sf::Uint8 g = static_cast<sf::Uint8>(currentHealth * 255.f);
	m_sprite.setColor(sf::Color(r, g, 0, 255));

	return m_sprite;
}