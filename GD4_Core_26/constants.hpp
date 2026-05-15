#pragma once
constexpr int kMaxPacketSize = 1300;

//resolution constants
constexpr unsigned int kWindowWidth = 1920;
constexpr unsigned int kWindowHeight = 1080;

//collision radiuses
constexpr float kTankCollisionRadius = 40.f;
constexpr float kBulletCollisionRadius = 5.f;

//bullet constants
constexpr float BULLET_SPEED = 450.f;
constexpr float BULLET_LIFETIME = 10.f;
constexpr unsigned int kBulletDamage = 10;

//visuals scale
constexpr float kTankSpriteScale = 0.6f;
constexpr float kTurretSpriteScale = 0.6f;
constexpr float kBulletSpriteScale = 0.4f;

//Health bar constants
constexpr float kHealthBarWidth = 85.f;
constexpr float kHealthBarHeight = 6.f;
constexpr float kHealthBarOffset = 85.f;

//tank game constants
constexpr int kMaxTankHealth = 100;
constexpr int kMaxTankAmmo = 10;
constexpr float kFlashTime = 10.f;

constexpr float kGameTimer = 30.f;
