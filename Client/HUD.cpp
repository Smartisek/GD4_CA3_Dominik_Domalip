#include "SocketWrapperClientPCH.hpp"

std::unique_ptr< HUD >	HUD::sInstance;


HUD::HUD() :
	mScoreBoardOrigin(50.f, 60.f, 0.0f),
	mBandwidthOrigin(50.f, 10.f, 0.0f),
	mRoundTripTimeOrigin(580.f, 10.f, 0.0f),
	mAmmoOffset(kWindowWidth - 300.f, 70.f, 0.0f),
	mScoreOffset(0.f, 50.f, 0.0f),
	mHealthOffset(kWindowWidth - 300.f, 10.f, 0.0f),
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
	RenderTimer();
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
	WindowManager::sInstance->setMouseCursorVisible(false);
	sf::Vector2i mousePixel = sf::Mouse::getPosition(*WindowManager::sInstance);
	sf::Vector2f mouseWorld = WindowManager::sInstance->mapPixelToCoords(mousePixel);
	mCursorSprite.setPosition(mouseWorld.x, mouseWorld.y);
	WindowManager::sInstance->draw(mCursorSprite);
}

void HUD::RenderText(const string& inStr, const Vector3& origin, const Vector3& inColor)
{
	sf::Text text;
	text.setString(inStr);
	text.setFillColor(sf::Color::White);
	text.setOutlineColor(sf::Color::Black);    
	text.setOutlineThickness(2.f);            
	text.setStyle(sf::Text::Bold);             
	text.setCharacterSize(50);
	text.setPosition(origin.mX, origin.mY);
	text.setFont(*FontManager::sInstance->GetFont("carlito"));
	WindowManager::sInstance->draw(text);
}



void HUD::RenderTimer()
{
	int seconds = (int)mGameTimer;
	int mins = seconds / 60;
	int secs = seconds % 60;
	char buf[16];
	snprintf(buf, sizeof(buf), "%d:%02d", mins, secs);

	Vector3 timerPos(kWindowWidth / 2.f - 40.f, 10.f, 0.f);  // Adjust position as needed
	Vector3 timerColor = (mins == 0 && secs <= 30) ? Colors::Red : Colors::White;
	RenderText(buf, timerPos, timerColor);
}