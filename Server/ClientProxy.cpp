#include "SocketWrapperServerPCH.hpp"

namespace
{
	const float kRespawnDelay = 3.f;
}

ClientProxy::ClientProxy(const SocketAddress& inSocketAddress, const string& inName, int inPlayerId) :
	mSocketAddress(inSocketAddress),
	mName(inName),
	mPlayerId(inPlayerId),
	mDeliveryNotificationManager(false, true),
	mIsLastMoveTimestampDirty(false),
	mTimeToRespawn(0.f)
{
	UpdateLastPacketTime();
}


void ClientProxy::UpdateLastPacketTime()
{
	mLastPacketFromClientTime = Timing::sInstance.GetTimef();
}

void	ClientProxy::HandleTankDied()
{
	mTimeToRespawn = Timing::sInstance.GetFrameStartTime() + kRespawnDelay;
}

void	ClientProxy::RespawnTankIfNecessary()
{
	if (mTimeToRespawn != 0.f && Timing::sInstance.GetFrameStartTime() > mTimeToRespawn)
	{
		static_cast<Server*> (Engine::s_instance.get())->SpawnTankForPlayer(mPlayerId);
		mTimeToRespawn = 0.f;
	}
}


