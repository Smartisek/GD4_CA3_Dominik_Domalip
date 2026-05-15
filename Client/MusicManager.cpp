#include "SocketWrapperClientPCH.hpp"

std::unique_ptr<MusicManager> MusicManager::sInstance;

void MusicManager::StaticInit()
{
	sInstance.reset(new MusicManager());
	sInstance->mMusicPaths["gameMusic"] = "../Assets/Sound/game_beat.wav";
	sInstance->mMusicPaths["menuMusic"] = "../Assets/Sound/menu_beat.mp3";
}

void MusicManager::PlayMusic(const std::string& musicName, bool loop)
{
	auto it = mMusicPaths.find(musicName);
	if (it == mMusicPaths.end())
	{
		LOG("Music not found: %s", musicName.c_str());
		return;
	}

	mCurrentMusic = std::make_unique<sf::Music>();
	if (!mCurrentMusic->openFromFile(it->second))
	{
		LOG("Failed to load music: %s", it->second.c_str());
		mCurrentMusic = nullptr;
		return;
	}

	mCurrentMusic->setLoop(loop);
	mCurrentMusic->play();
	LOG("Playing music: %s", musicName.c_str());
}

void MusicManager::StopMusic()
{
	if (mCurrentMusic)
	{
		mCurrentMusic->stop();
	}
}

void MusicManager::PauseMusic()
{
	if (mCurrentMusic)
	{
		mCurrentMusic->pause();
	}
}

void MusicManager::ResumeMusic()
{
	if (mCurrentMusic)
	{
		mCurrentMusic->play();
	}
}

void MusicManager::SetVolume(float volume)
{
	if (mCurrentMusic)
	{
		mCurrentMusic->setVolume(volume);
	}
}