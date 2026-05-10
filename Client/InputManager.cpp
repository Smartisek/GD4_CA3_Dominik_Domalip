#include "SocketWrapperClientPCH.hpp"

unique_ptr< InputManager >	InputManager::sInstance;

namespace
{
	float kTimeBetweenInputSamples = 0.03f;
	const float kTurretDeadZone = 20.f; //pixels around the center of the tank where turret won't rotate
}

void InputManager::StaticInit()
{
	sInstance.reset(new InputManager());
}


namespace
{
	inline void UpdateDesireVariableFromKey(EInputAction inInputAction, bool& ioVariable)
	{
		if (inInputAction == EIA_Pressed)
		{
			ioVariable = true;
		}
		else if (inInputAction == EIA_Released)
		{
			ioVariable = false;
		}
	}

	inline void UpdateDesireFloatFromKey(EInputAction inInputAction, float& ioVariable)
	{
		if (inInputAction == EIA_Pressed)
		{
			ioVariable = 1.f;
		}
		else if (inInputAction == EIA_Released)
		{
			ioVariable = 0.f;
		}
	}
}

// ******************* assigning keys to action here 
void InputManager::HandleInput(EInputAction inInputAction, int inKeyCode)
{
	switch (inKeyCode)
	{
	case sf::Keyboard::A:
		UpdateDesireFloatFromKey(inInputAction, mCurrentState.mDesiredLeftAmount);
		break;
	case sf::Keyboard::D:
		UpdateDesireFloatFromKey(inInputAction, mCurrentState.mDesiredRightAmount);
		break;
	case sf::Keyboard::W:
		UpdateDesireFloatFromKey(inInputAction, mCurrentState.mDesiredUpAmount);
		break;
	case sf::Keyboard::S:
		UpdateDesireFloatFromKey(inInputAction, mCurrentState.mDesiredDownAmount);
		break;
	case sf::Keyboard::Space:
		UpdateDesireVariableFromKey(inInputAction, mCurrentState.mIsShooting);
		break;
	case sf::Keyboard::Add:
	case sf::Keyboard::Equal:
	{
		if (!NetworkManagerClient::sInstance) return;
		float latency = NetworkManagerClient::sInstance->GetSimulatedLatency();
		latency += 0.1f;
		if (latency > 0.5f)
		{
			latency = 0.5f;
		}
		NetworkManagerClient::sInstance->SetSimulatedLatency(latency);
		break;
	}
	case sf::Keyboard::Subtract:
	{
		if (!NetworkManagerClient::sInstance) return;
		float latency = NetworkManagerClient::sInstance->GetSimulatedLatency();
		latency -= 0.1f;
		if (latency < 0.0f)
		{
			latency = 0.0f;
		}
		NetworkManagerClient::sInstance->SetSimulatedLatency(latency);
		break;
	}
	}

}


InputManager::InputManager() :
	mNextTimeToSampleInput(0.f),
	mPendingMove(nullptr)
{

}

const Move& InputManager::SampleInputAsMove()
{
	return mMoveList.AddMove(GetState(), Timing::sInstance.GetFrameStartTime());
}

bool InputManager::IsTimeToSampleInput()
{
	float time = Timing::sInstance.GetFrameStartTime();
	if (time > mNextTimeToSampleInput)
	{
		mNextTimeToSampleInput = mNextTimeToSampleInput + kTimeBetweenInputSamples;
		return true;
	}

	return false;
}

void InputManager::Update()
{
	if (IsTimeToSampleInput())
	{
		mPendingMove = &SampleInputAsMove();
	}

	//get mouse position in worlds space
	sf::Vector2f mousePos = WindowManager::sInstance->mapPixelToCoords(
		sf::Mouse::getPosition(*WindowManager::sInstance));

	if (!NetworkManagerClient::sInstance) return; //not connected to server yet 

	//get local tank positoin
	Vector3 tankPos = Vector3::Zero;
	uint32_t localPlayerId = NetworkManagerClient::sInstance->GetPlayerId();
	bool foundTank = false;

	const auto& gameObjects = World::sInstance->GetGameObjects();
	for (const auto& go : gameObjects)
	{
		Tank* tank = go->GetAsTank();
		if (tank && tank->GetPlayerId() == localPlayerId)
		{
			tankPos = tank->GetLocation();
			foundTank = true;
			break;
		}
	}

	if (foundTank)
	{
		float dx = mousePos.x - tankPos.mX;
		float dy = mousePos.y - tankPos.mY;
		float distSq = dx * dx + dy * dy;

		if (distSq > (kTurretDeadZone * kTurretDeadZone))
		{
			float angleDeg = Math::ToDegrees(atan2(dy, dx));

			// normalize to [0, 360)
			while (angleDeg < 0.f) angleDeg += 360.f;
			while (angleDeg >= 360.f) angleDeg -= 360.f;

			mCurrentState.mTurretRotation = angleDeg;
		}
	}
}
