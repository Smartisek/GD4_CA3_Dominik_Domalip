#include "SocketWrapperClientPCH.hpp"

std::unique_ptr< RenderManager >	RenderManager::sInstance;

RenderManager::RenderManager()
{
	view.reset(sf::FloatRect(0, 0, kWindowWidth, kWindowHeight));
	WindowManager::sInstance->setView(view);
}


void RenderManager::StaticInit()
{
	sInstance.reset(new RenderManager());
}


void RenderManager::AddComponent(SpriteComponent* inComponent)
{
	mComponents.emplace_back(inComponent);
}

void RenderManager::RemoveComponent(SpriteComponent* inComponent)
{
	int index = GetComponentIndex(inComponent);

	if (index != -1)
	{
		int lastIndex = mComponents.size() - 1;
		if (index != lastIndex)
		{
			mComponents[index] = mComponents[lastIndex];
		}
		mComponents.pop_back();
	}
}

int RenderManager::GetComponentIndex(SpriteComponent* inComponent) const
{
	for (int i = 0, c = mComponents.size(); i < c; ++i)
	{
		if (mComponents[i] == inComponent)
		{
			return i;
		}
	}

	return -1;
}


//this part that renders the world is really a camera-
//in a more detailed engine, we'd have a list of cameras, and then render manager would
//render the cameras in order
void RenderManager::RenderComponents()
{
	//draw body of tank first or any non turret/health bar 
	for (SpriteComponent* c : mComponents)
	{
		if (dynamic_cast<TurretSpriteComponent*>(c) == nullptr &&
			dynamic_cast<HealthBarSpriteComponent*>(c) == nullptr)
		{
			WindowManager::sInstance->draw(c->GetSprite());
		}
	}

	//draw healthbar second
	for (SpriteComponent* c : mComponents)
	{
		if (dynamic_cast<HealthBarSpriteComponent*>(c) != nullptr)
		{
			WindowManager::sInstance->draw(c->GetSprite());
		}
	}

	//draw turret last 
	for (SpriteComponent* c : mComponents)
	{
		if (dynamic_cast<TurretSpriteComponent*>(c) != nullptr)
		{
			WindowManager::sInstance->draw(c->GetSprite());
		}
	}
}

void RenderManager::Render()
{
	//
	// Clear the back buffer
	//
	WindowManager::sInstance->clear(sf::Color(100, 149, 237, 255));

	RenderManager::sInstance->RenderComponents();
	//draw damage popups for every tank 
	for (auto& go : World::sInstance->GetGameObjects())
	{
		TankClient* tank = static_cast<TankClient*>(go->GetAsTank());
		if (tank)
			tank->DrawDamagePopups(*WindowManager::sInstance);
	}

	HUD::sInstance->Render();

	//
	// Present our back buffer to our front buffer
	//
	WindowManager::sInstance->display();

}
