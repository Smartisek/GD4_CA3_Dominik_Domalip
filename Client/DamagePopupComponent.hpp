class DamagePopupComponent : public SpriteComponent
{
public:
    DamagePopupComponent(GameObject* inGameObject, const std::string& inText, float inX, float inY);

    virtual sf::Sprite& GetSprite() override;

    bool IsExpired() const { return mElapsed >= mLifetime; }
    void Update(float dt);
    void Draw(sf::RenderWindow& window) override { window.draw(mText); }

private:
    sf::Text        mText;
    float           mLifetime = 0.75f;
    float           mElapsed = 0.f;
    sf::Vector2f    mPosition;
    sf::Vector2f    mVelocity = { 0.f, -80.f };
};