#include "SocketWrapperClientPCH.hpp"

std::unique_ptr< HUD >	HUD::sInstance;


HUD::HUD() :
	mScoreBoardOrigin(50.f, 60.f, 0.0f),
	mBandwidthOrigin(50.f, 10.f, 0.0f),
	mRoundTripTimeOrigin(580.f, 10.f, 0.0f),
	mScoreOffset(0.f, 50.f, 0.0f),
	mHealthOffset(1000, 10.f, 0.0f),
	mHealth(0),
	mAmmo(0)
{
	auto tex = TextureManager::sInstance->GetTexture("cursor");
	mCursorSprite.setTexture(*tex);
	mCursorSprite.setScale(0.5f, 0.5f);
	auto size = tex->getSize();
	mCursorSprite.setOrigin(size.x / 2.0f, size.y / 2.0f);
}


void HUD::StaticInit()
{
	sInstance.reset(new HUD());
}

void HUD::Render()
{
	RenderBandWidth();
	RenderCursor();
	RenderRoundTripTime();
	RenderScoreBoard();
	RenderHealth();
	RenderAmmo();
}

void HUD::RenderHealth()
{
	if (mHealth > 0)
	{
		string healthString = StringUtils::Sprintf("Health %d", mHealth);
		RenderText(healthString, mHealthOffset, Colors::Red);
	}
}

void HUD::RenderAmmo()
{
	if (mAmmo >= 0)
	{
		string ammoString = StringUtils::Sprintf("Ammo %d", mAmmo);
		RenderText(ammoString, mAmmoOffset, Colors::LightYellow);
	}
}

void HUD::RenderBandWidth()
{
	if (!NetworkManagerClient::sInstance) return;

	string bandwidth = StringUtils::Sprintf("In %d  Bps, Out %d Bps",
		static_cast<int>(NetworkManagerClient::sInstance->GetBytesReceivedPerSecond().GetValue()),
		static_cast<int>(NetworkManagerClient::sInstance->GetBytesSentPerSecond().GetValue()));
	RenderText(bandwidth, mBandwidthOrigin, Colors::White);
}

void HUD::RenderRoundTripTime()
{
	if (!NetworkManagerClient::sInstance) return;

	float rttMS = NetworkManagerClient::sInstance->GetAvgRoundTripTime().GetValue() * 1000.f;

	string roundTripTime = StringUtils::Sprintf("RTT %d ms", (int)rttMS);
	RenderText(roundTripTime, mRoundTripTimeOrigin, Colors::White);
}

void HUD::RenderScoreBoard()
{
	const vector< ScoreBoardManager::Entry >& entries = ScoreBoardManager::sInstance->GetEntries();
	Vector3 offset = mScoreBoardOrigin;

	for (const auto& entry : entries)
	{
		RenderText(entry.GetFormattedNameScore(), offset, entry.GetColor());
		offset.mX += mScoreOffset.mX;
		offset.mY += mScoreOffset.mY;
	}

}

void HUD::RenderCursor()
{
	sf::Vector2i mousePixel = sf::Mouse::getPosition(*WindowManager::sInstance);
	sf::Vector2f mouseWorld = WindowManager::sInstance->mapPixelToCoords(mousePixel);
	mCursorSprite.setPosition(mouseWorld.x, mouseWorld.y);
	WindowManager::sInstance->draw(mCursorSprite);
}

void HUD::RenderText(const string& inStr, const Vector3& origin, const Vector3& inColor)
{
	sf::Text text;
	text.setString(inStr);
	text.setFillColor(sf::Color(inColor.mX, inColor.mY, inColor.mZ, 255));
	text.setCharacterSize(50);
	text.setPosition(origin.mX, origin.mY);
	text.setFont(*FontManager::sInstance->GetFont("carlito"));
	WindowManager::sInstance->draw(text);
}

