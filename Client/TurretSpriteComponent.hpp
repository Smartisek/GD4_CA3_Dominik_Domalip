#pragma once
class TurretSpriteComponent : public SpriteComponent
{
public:
	TurretSpriteComponent(GameObject* inGameObject);
	void SetTexture(TexturePtr inTexture) override;
	virtual sf::Sprite& GetSprite() override;
};

