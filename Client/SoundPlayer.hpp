class SoundPlayer
{
public:
    static void StaticInit();
    static std::unique_ptr<SoundPlayer> sInstance;

    void Play(SoundEffect effect);
    void Play(SoundEffect effect, float x, float y);
    void SetListenerPosition(float x, float y);
    void RemoveStoppedSounds();

private:
    SoundPlayer();

    std::map<SoundEffect, sf::SoundBuffer> mSoundBuffers;
    std::list<sf::Sound>                   mSounds;

    static constexpr float kListenerZ = 300.f;
    static constexpr float kAttenuation = 1.f;
    static constexpr float kMinDistance2D = 200.f;
};