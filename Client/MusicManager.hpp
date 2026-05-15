class MusicManager
{
public:
	static void StaticInit();
	static std::unique_ptr<MusicManager> sInstance;

	void PlayMusic(const std::string& musicName, bool loop = true);
	void StopMusic();
	void PauseMusic();
	void ResumeMusic();
	void SetVolume(float volume);

private:
	MusicManager() = default;

	std::unordered_map<std::string, std::string> mMusicPaths;
	std::unique_ptr<sf::Music> mCurrentMusic;
};