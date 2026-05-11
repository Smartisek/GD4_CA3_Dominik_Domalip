#include "SocketWrapperClientPCH.hpp"

DamagePopupComponent::DamagePopupComponent(GameObject* inGameObject, const std::string& inText, float inX, float inY, sf::Color inColor)
    : SpriteComponent(inGameObject),
    mPosition(inX, inY - 50.f)
{
    auto font = FontManager::sInstance->GetFont("carlito");
    mText.setFont(*font);
    mText.setString(inText);
    mText.setCharacterSize(32);
    mText.setStyle(sf::Text::Bold);
    mText.setFillColor(inColor);
    mText.setOutlineColor(sf::Color::Black);
    mText.setOutlineThickness(2.f);

    sf::FloatRect bounds = mText.getLocalBounds();
    mText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    mText.setPosition(mPosition);
}

void DamagePopupComponent::Update(float dt)
{
    mElapsed += dt;
    mPosition += mVelocity * dt;
    mText.setPosition(mPosition);

    float alpha = 255.f * (1.f - mElapsed / mLifetime);
    auto c = mText.getFillColor();
    mText.setFillColor({ c.r, c.g, c.b, static_cast<sf::Uint8>(std::max(0.f, alpha)) });
}

sf::Sprite& DamagePopupComponent::GetSprite()
{
    return m_sprite;
}
