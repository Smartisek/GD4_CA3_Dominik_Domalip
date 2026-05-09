typedef shared_ptr< sf::Texture > TexturePtr;
typedef shared_ptr<sf::Font> FontPtr;

class SpriteComponent
{
public:

	SpriteComponent(GameObject* inGameObject);
	~SpriteComponent();


	virtual void SetTexture(TexturePtr inTexture);
	virtual sf::Sprite& GetSprite();
	virtual void Draw(sf::RenderWindow& window) { window.draw(GetSprite()); }



protected:

	sf::Sprite m_sprite;

	//don't want circular reference...
	GameObject* mGameObject;
};

typedef shared_ptr< SpriteComponent >	SpriteComponentPtr;

