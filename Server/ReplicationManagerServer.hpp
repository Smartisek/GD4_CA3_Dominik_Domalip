//this is the heart of multiplayer game state synchronization, it tells clients what game objects exist, how they changed and when they are destroyed 
//Server: "Create Tank #5 at position (100, 200)"
//↓ ReplicateCreate(5, ALL_STATE)
//Client : Sees Tank #5 appear on the map
//Server: Tank #5 moved to(150, 250)
//↓ SetStateDirty(5, POSITION_BIT)
//Client: Tank #5 smoothly moves to new position
//it is using the ETankReplicationState from tank.hpp 
class ReplicationManagerServer
{


public:
	void ReplicateCreate(int inNetworkId, uint32_t inInitialDirtyState);
	void ReplicateDestroy(int inNetworkId);
	void SetStateDirty(int inNetworkId, uint32_t inDirtyState);
	void HandleCreateAckd(int inNetworkId);
	void RemoveFromReplication(int inNetworkId);

	void Write(OutputMemoryBitStream& inOutputStream, ReplicationManagerTransmissionData* ioTransmissionData);

private:

	uint32_t WriteCreateAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint32_t inDirtyState);
	uint32_t WriteUpdateAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint32_t inDirtyState);
	uint32_t WriteDestroyAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint32_t inDirtyState);

	unordered_map< int, ReplicationCommand >	mNetworkIdToReplicationCommand;
};

