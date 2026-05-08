//hud rendering responsibility 

class HUD
{
public:

	static void StaticInit();
	static std::unique_ptr< HUD >	sInstance;

	void Render();

	void SetPlayerHealth(int inHealth) { mHealth = inHealth; }
	void SetPlayerAmmo(int inAmmo) { mAmmo = inAmmo; }

private:

	HUD();

	void	RenderBandWidth();
	void	RenderRoundTripTime();
	void	RenderScoreBoard();
	void    RenderCursor();
	void	RenderHealth();
	void	RenderAmmo();
	void	RenderText(const string& inStr, const Vector3& origin, const Vector3& inColor);

	Vector3										mBandwidthOrigin;
	Vector3										mRoundTripTimeOrigin;
	Vector3										mScoreBoardOrigin;
	Vector3										mScoreOffset;
	Vector3										mHealthOffset;
	Vector3										mAmmoOffset;
	int											mHealth;
	int											mAmmo;
	sf::Sprite									mCursorSprite;
};



