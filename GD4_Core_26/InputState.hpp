#pragma once
class InputState
{
public:
	InputState() :
		mDesiredRightAmount(0),
		mDesiredLeftAmount(0),
		mDesiredUpAmount(0),
		mDesiredDownAmount(0),
		mIsShooting(false),
		mTurretRotation(0.f)
	{}

	float GetDesiredHorizontalDelta()	const { return mDesiredRightAmount - mDesiredLeftAmount; }
	float GetDesiredVerticalDelta()		const { return mDesiredUpAmount - mDesiredDownAmount; }
	float GetTurretRotation()			const { return mTurretRotation; }
	bool  IsShooting()					const { return mIsShooting; }

	bool Write(OutputMemoryBitStream& inOutputStream) const;
	bool Read(InputMemoryBitStream& inInputStream);

private:
	friend class InputManager;

	float mDesiredRightAmount, mDesiredLeftAmount;
	float mDesiredUpAmount, mDesiredDownAmount;
	bool mIsShooting;
	float mTurretRotation;
};

