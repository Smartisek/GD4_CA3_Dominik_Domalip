//tracks waht was sent in a packet so it can handle deliveery failure intelligently, it is a bridge between packet delivery and game state resynchronization

//Logic:
//Server sends packet with changes :
//"Create Tank #5 at (100, 200)"
//"Update Tank #2 health to 50"
//"Destroy Bullet #3"
//Packet is LOST during network transmission
//↓
//How does server know WHAT to resend ?
//Without TransmissionData : Entire packet resent(wasteful)
//With TransmissionData : Only failed items resent(smart)

//Player fires 3 bullets → Packet with 3 CREATE actions
//Packet lost → Client doesn't see bullets
//↓
//ReplicationManagerTransmissionData remembers :
//"Bullet #100, #101, #102 were CREATEs"
//↓
//HandleDeliveryFailure() resends all 3 CREATEs
//↓
//Client now sees all 3 bullets

class ReplicationManagerServer;

class ReplicationManagerTransmissionData : public TransmissionData
{
public:

	ReplicationManagerTransmissionData(ReplicationManagerServer* inReplicationManagerServer) :
		mReplicationManagerServer(inReplicationManagerServer)
	{
	}

	class ReplicationTransmission //what was in the packet 
	{
	public:
		ReplicationTransmission(int inNetworkId, ReplicationAction inAction, uint32_t inState) :
			mNetworkId(inNetworkId), //which object 
			mAction(inAction), //what happened 
			mState(inState) //what data 
		{
		}

		int							GetNetworkId()		const { return mNetworkId; }
		ReplicationAction		GetAction()			const { return mAction; }
		uint32_t					GetState()			const { return mState; }

	private:
		int							mNetworkId;
		ReplicationAction		mAction;
		uint32_t					mState;
	};

	void AddTransmission(int inNetworkId, ReplicationAction inAction, uint32_t inState);

	virtual void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const override;
	virtual void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const override;


private:

	void HandleCreateDeliveryFailure(int inNetworkId) const;
	void HandleUpdateStateDeliveryFailure(int inNetworkId, uint32_t inState, DeliveryNotificationManager* inDeliveryNotificationManager) const;
	void HandleDestroyDeliveryFailure(int inNetworkId) const;
	void HandleCreateDeliverySuccess(int inNetworkId) const;
	void HandleDestroyDeliverySuccess(int inNetworkId) const;

	ReplicationManagerServer* mReplicationManagerServer;

	vector< ReplicationTransmission >		mTransmissions; //all changes in this packet 

};

typedef shared_ptr< ReplicationManagerTransmissionData > ReplicationManagerTransmissionDataPtr;
