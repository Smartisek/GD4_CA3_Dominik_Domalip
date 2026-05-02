//Thi file works to confirm and track the udp packet delivery since udp is not reliable and can be lost 
// client may send packets: 1, 2, 3, 4
// server can only receive: 1, 3, 4
//packet sequence number is used to track and give packets unique numbers like id and time when it was sent 
// if we loose packet we can resend it thanks to this because we also store the data that was in the packet by unordered_map< int, TransmissionDataPtr > mTransmissionDataMap;
//HandleDeliverySuccess();   // Got ACK → packet arrived, clean up
//HandleDeliveryFailure();   // Timeout → packet lost, resend it
//EXAMPLES
//
//CLIENT                           SERVER
//|                                |
//|------Packet #5 (pos data) --->|
//|[InFlightPacket #5]      |
//|       TimeDispatched: 0.0s     |
//|                                |
//|   (waiting for ACK...)         |
//|                                |
//| (0.1s timeout, no ACK)         |
//|                                |
//|------Packet #5 (resent) -----> | Got it!
//| (same data)              |
//| < ----ACK for packet #5 --------|
//	|[HandleDeliverySuccess]  |
//	|       Remove from InFlight |
// **BASICALLY InFlightPacket is UDP error correction system to not loose data ****

class DeliveryNotificationManager;

//in case we decide to change the type of the sequence number to use fewer or more bits
typedef uint16_t	PacketSequenceNumber;

class InFlightPacket
{
public:

	InFlightPacket(PacketSequenceNumber inSequenceNumber);

	PacketSequenceNumber GetSequenceNumber() const { return mSequenceNumber; }
	float				 GetTimeDispatched() const { return mTimeDispatched; }

	void 				 SetTransmissionData(int inKey, TransmissionDataPtr	inTransmissionData)
	{
		mTransmissionDataMap[inKey] = inTransmissionData;
	}
	const TransmissionDataPtr GetTransmissionData(int inKey) const
	{
		auto it = mTransmissionDataMap.find(inKey);
		return (it != mTransmissionDataMap.end()) ? it->second : nullptr;
	}

	void			HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const;
	void			HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const;

private:
	PacketSequenceNumber	mSequenceNumber;
	float			mTimeDispatched;

	unordered_map< int, TransmissionDataPtr >	mTransmissionDataMap;
};
