#include "SocketWrapperPCH.hpp"

namespace
{
	//these two will compress the movement input into minimal bits
	//Input value -> Bits Written -> total size
	// 0.f (no move) -> [0]       -> 1 bit
	// 1.f(right)    -> [1,1]     -> 2 bits (so first bit is 1 so positive
	// -1.f(left)    - [1, 0]     -> 2 bits (1 right , 0 left, same for up down)
	// basically 1 bit says are we moving and other is it positive or not
	void WriteSignedBinaryValue(OutputMemoryBitStream& inOutputStream, float inValue)
	{
		bool isNonZero = (inValue != 0.f);
		inOutputStream.Write(isNonZero);
		if (isNonZero)
		{
			inOutputStream.Write(inValue > 0.f);
		}
	}

	void ReadSignedBinaryValue(InputMemoryBitStream& inInputStream, float& outValue)
	{
		bool isNonZero;
		inInputStream.Read(isNonZero);
		if (isNonZero)
		{
			bool isPositive;
			inInputStream.Read(isPositive);
			outValue = isPositive ? 1.f : -1.f;
		}
		else
		{
			outValue = 0.f;
		}
	}
}

//for the current game i am making the tank will move either up/down/right/left and we will be shooting so we will be sending 5 bits per frame
// 2 bits or up down movement, 2 bits for right left movement and 1 bite if we are shooting 
bool InputState::Write(OutputMemoryBitStream& inOutputStream) const
{
	WriteSignedBinaryValue(inOutputStream, GetDesiredHorizontalDelta());
	WriteSignedBinaryValue(inOutputStream, GetDesiredVerticalDelta());
	inOutputStream.Write(mIsShooting);

	return false;
}

//this will read those 5 bits and put them back into players input state
//Client -> player presses arrow key -> InputState captures movement
//InputState::Write() -> compress and send those 5 its over UDP to server

//Served -> receives those 5 bits -> InputState::Read() -> reconstruct the input and updates tanks position 
// replicates tank state to all other clients

//Remote clients -> receive tank state and update remote tanks position on their screen
bool InputState::Read(InputMemoryBitStream& inInputStream)
{

	ReadSignedBinaryValue(inInputStream, mDesiredRightAmount);
	ReadSignedBinaryValue(inInputStream, mDesiredUpAmount);
	inInputStream.Read(mIsShooting);

	return true;
}