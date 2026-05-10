#include "SocketWrapperClientPCH.hpp"

std::unique_ptr<SoundPlayer> SoundPlayer::sInstance;

void SoundPlayer::StaticInit()
{
    sInstance.reset(new SoundPlayer());
}


SoundPlayer::SoundPlayer()
{
    //load
    mSoundBuffers[SoundEffect::kTankHit].loadFromFile("../Assets/Sound/bulletHit.wav");
    mSoundBuffers[SoundEffect::kBulletFire].loadFromFile("../Assets/Sound/tankBullet.flac");
    mSoundBuffers[SoundEffect::kExplosion].loadFromFile("../Assets/Sound/explosion.wav");

    sf::Listener::setDirection({ 0.f, 0.f, -1.f });
}

void SoundPlayer::Play(SoundEffect effect)
{
    sf::Vector3f pos = sf::Listener::getPosition();
    Play(effect, pos.x, -pos.y);
}

void SoundPlayer::Play(SoundEffect effect, float x, float y)
{
    float minDist3D = std::sqrt(kMinDistance2D * kMinDistance2D + kListenerZ * kListenerZ);

    mSounds.emplace_back(mSoundBuffers.at(effect));
    sf::Sound& sound = mSounds.back();
    sound.setPosition({ x, -y, 0.f });
    sound.setAttenuation(kAttenuation);
    sound.setMinDistance(minDist3D);
    sound.setVolume(100.f);
    sound.play();
}

void SoundPlayer::RemoveStoppedSounds()
{
    mSounds.remove_if([](const sf::Sound& s) {
        return s.getStatus() == sf::Sound::Stopped;
        });
}

void SoundPlayer::SetListenerPosition(float x, float y)
{
    sf::Listener::setPosition({ x, -y, kListenerZ });
}