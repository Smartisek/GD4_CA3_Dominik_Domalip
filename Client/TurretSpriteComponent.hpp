#pragma once
class TurretSpriteComponent : public SpriteComponent
{
public:
	TurretSpriteComponent(GameObject* inGameObject);
	virtual sf::Sprite& GetSprite() override;
};

