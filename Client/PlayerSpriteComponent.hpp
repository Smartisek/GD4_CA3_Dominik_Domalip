#pragma once

//this file is responsible for managing the sprite of player 

class PlayerSpriteComponent : public SpriteComponent
{
public:
	PlayerSpriteComponent(GameObject* inGameObject);
	virtual sf::Sprite& GetSprite() override;
};
typedef shared_ptr<PlayerSpriteComponent >	PlayerSpriteComponentPtr;

