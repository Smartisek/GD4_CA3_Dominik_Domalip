#include "SocketWrapperClientPCH.hpp"

TurretSpriteComponent::TurretSpriteComponent(GameObject* inGameObject) :
	SpriteComponent(inGameObject)
{
}


void TurretSpriteComponent::SetTexture(TexturePtr inTexture)
{
	SpriteComponent::SetTexture(inTexture);

	auto tSize = inTexture->getSize();

	//make origin of the sprite lower for turret
	float originX = tSize.x / 2.0f;
	float originY = tSize.y * 0.75f; 
	m_sprite.setOrigin(originX, originY);
}


sf::Sprite& TurretSpriteComponent::GetSprite()
{
	auto pos = mGameObject->GetLocation();
	m_sprite.setPosition(pos.mX, pos.mY);

	if (Tank* tank = mGameObject->GetAsTank())
	{
		const float kTurretSpriteOffset = +90.f;
		m_sprite.setRotation(tank->GetTurretRotation() + kTurretSpriteOffset);

		//Vector3 color = tank->GetColor();
		//m_sprite.setColor(sf::Color(color.mX, color.mY, color.mZ, 255));
	}

	return m_sprite;
}