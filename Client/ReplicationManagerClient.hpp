//read network updates, handle create actions, handle update actions and handle destroy actions, update game objects based on server state

//EXAMPLE
//Server sends packet
//↓
//NetworkManagerClient receives it
//↓
//ReplicationManagerClient.Read() processes it
//↓
//Calls one of three actions :
//├─ Create new object
//├─ Update existing object
//└─ Destroy object
//↓
//Client game world stays in sync with server

class ReplicationManagerClient
{
public:
	void Read(InputMemoryBitStream& inInputStream);

private:

	void ReadAndDoCreateAction(InputMemoryBitStream& inInputStream, int inNetworkId);
	void ReadAndDoUpdateAction(InputMemoryBitStream& inInputStream, int inNetworkId);
	void ReadAndDoDestroyAction(InputMemoryBitStream& inInputStream, int inNetworkId);

};
