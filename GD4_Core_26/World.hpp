//robocat example code says that it is not ideal to track all the things here that it is inefficient, might need to look at this later****

class World
{

public:

	static void StaticInit();

	static std::unique_ptr< World >		sInstance;

	void AddGameObject(GameObjectPtr inGameObject);
	void RemoveGameObject(GameObjectPtr inGameObject);

	void Update();

	const std::vector< GameObjectPtr >& GetGameObjects()	const { return mGameObjects; }

private:

	World();

	int	GetIndexOfGameObject(GameObjectPtr inGameObject);

	std::vector< GameObjectPtr >	mGameObjects;


};