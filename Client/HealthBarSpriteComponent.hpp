#pragma once
class HealthBarSpriteComponent : public SpriteComponent
{
public:
	HealthBarSpriteComponent(GameObject* inGameObject);
	virtual sf::Sprite& GetSprite() override;

private:
	static TexturePtr sWhiteTexture;
};

