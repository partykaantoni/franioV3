/*
     +=======================+
    |  Board Representation |
    +=======================+
*/
U64 pieces[12];
U64 occupancies[3];

// Half and full moves
u_int half = 0;
u_int full = 0;
int enPassant = -1;
int castles; // Representing castling rights whith an int
_Bool whiteToMove;

// May dinomically allocate later
typedef struct {
	u_int moves[220];
	u_int count;
} moveList;

// Hash tables
typedef struct {
	U64 key;
	int depth;
	int flag;
	int score;
	int best;
} HASHt;

// TO DO array of two ints for piece count
// int pieceCount[white / black ?][count encoded in an int]

// TO DO material hashing or smthing
// // Material hashing to speed up the evaluation
// typedef struct {
// 	U64 key;
// 	int phase;
// 	int matScore;
// 	int posScore;
// 	int mobScore;
// } HASHMt;