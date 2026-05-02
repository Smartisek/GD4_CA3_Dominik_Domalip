//helps with anticheat 
//it is a queue of timestamped player inputs used for server side input processing 
//game flow: 
//CLIENT SIDE(sends moves with timestamps) :
//	t = 0.00s : Player presses RIGHT → Send Move(RIGHT, timestamp = 0.00s)
//	t = 0.05s : Player releases RIGHT → Send Move(IDLE, timestamp = 0.05s)
//	t = 0.10s : Player presses UP → Send Move(UP, timestamp = 0.10s)
//
//	Packet arrives at SERVER
//	↓
//	SERVER SIDE(processes in order) :
//	ClientProxy->GetUnprocessedMoveList() returns :
//	[Move(RIGHT, 0.00s), Move(IDLE, 0.05s), Move(UP, 0.10s)]
//
//	Server processes moves in order :
//At game time 0.00s : Apply RIGHT
//At game time 0.05s : Apply IDLE
//At game time 0.10s : Apply UP

class MoveList
{
public:

	typedef deque< Move >::const_iterator			const_iterator;
	typedef deque< Move >::const_reverse_iterator	const_reverse_iterator;

	MoveList() :
		mLastMoveTimestamp(-1.f)
	{
	}

	const	Move& AddMove(const InputState& inInputState, float inTimestamp);
	bool	AddMoveIfNew(const Move& inMove);

	void	RemovedProcessedMoves(float inLastMoveProcessedOnServerTimestamp);

	float			GetLastMoveTimestamp()	const { return mLastMoveTimestamp; }

	const Move& GetLatestMove()			const { return mMoves.back(); }

	void			Clear() { mMoves.clear(); }
	bool			HasMoves()				const { return !mMoves.empty(); }
	int				GetMoveCount()			const { return mMoves.size(); }

	//for for each, we have to match stl calling convention
	const_iterator	begin()					const { return mMoves.begin(); }
	const_iterator	end()					const { return mMoves.end(); }

	const Move& operator[](size_t i)	const { return mMoves[i]; }
private:

	float			mLastMoveTimestamp;
	deque< Move >	mMoves;




};

