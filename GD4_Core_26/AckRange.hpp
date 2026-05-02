//this packs packets acknowledgment compression for the udp, it groups packets into ranges ##
//instead of sending packets like 12345678910 that would be a lot of bytes data
//instead we send and it says i got packets from 1-10 which would only be 6 bytes 

class AckRange
{
public:
	AckRange() : mStart(0), mCount(0) {}

	AckRange(PacketSequenceNumber inStart) : mStart(inStart), mCount(1) {}

	//if this is the next in sequence, just extend the range
	inline bool ExtendIfShould(PacketSequenceNumber inSequenceNumber);

	PacketSequenceNumber	GetStart() const { return mStart; }
	uint32_t		GetCount() const { return mCount; }

	void Write(OutputMemoryBitStream& inOutputStream) const;
	void Read(InputMemoryBitStream& inInputStream);

private:
	PacketSequenceNumber	mStart; //where range will beging 
	uint32_t		mCount; //how many consecutive packets are in this range 
};

inline bool AckRange::ExtendIfShould(PacketSequenceNumber inSequenceNumber)
{
	if (inSequenceNumber == mStart + mCount)
	{
		++mCount;
		return true;
	}
	else
	{
		return false;
	}
}

//EXAMPlE 
//Received packets : 5, 6, 7, 8, 9
//↓
//AckRange{
//  mStart: 5,
//  mCount : 5
//}
//↓
//Means : "Packets 5 through 9 all arrived successfully"
//
