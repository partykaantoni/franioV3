#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef WIN64
	#include <windows.h>
#else
	#include <sys/time.h>
#endif
/*
    +==========+
    |  Macros  |
    +=========+
*/
// Bit manipulation
#define getBit(bitboard, index) ((bitboard) & (1ULL << (index)))
#define setBit(bitboard, index) ((bitboard) |= (1ULL << (index)))
#define popWhile(bitboard) ((bitboard) &= (bitboard - 1))
#define popBit(bitboard, index) (getBit(bitboard, index) ? (bitboard) ^= (1ULL << (index)) : 0)

// Bit counting
#define bitCount(bitboard) (__builtin_popcountll(bitboard))
#define firstBit(bitboard) (__builtin_ffsll(bitboard) - 1)

// Debug
#define nl() printf("\n")

// Eval help
#define getRank(square) (int)(7 - (square / 8))
#define getFile(square) (int)(square % 8)
/*
    +===============+
    |  Data Types  |
   +===============+
*/
#define U64 unsigned long long // Bitboards
#define u_int unsigned int // No idea, I sometimes use it
/*
    +===============+
    |  Search Data  |
   +===============+
*/
#define INFINITY 70000 // Big ass number for alpha-beta type beat searches
#define MATE_VALUE 49000 // Also a big ass number used for evaluation
#define MATE_SCORE 48000 // To recognise if a move is a mate
#define PIECE_COUNT 12
#define MAX_PLY 64
/*
    +===============+
    |  Hash Tables |
   +===============+
*/
#define NO_ENTRY 100000 // Big ass number to indicate no match was found in the hash table
#define HASH_SIZE 800000

// Flags for the hash table
#define EXACT 0
#define ALPHA 1
#define BETA 2

/*
    +=================+
    |  Move Encoding |
   +=================+

0000 0000 0000 0000 0011 1111 Origin
0000 0000 0000 1111 1100 0000 Target	
0000 0000 1111 0000 0000 0000 Piece
0000 1111 0000 0000 0000 0000 Promoted piece 
0010 0000 0000 0000 0000 0000 Double pawn push flag 
0100 0000 0000 0000 0000 0000 Enpassant flag 
1000 0000 0000 0000 0000 0000 Castling flag
*/

#define encodeMove(origin, target, piece, promotion, capture, doublep, enpassant, castling) (origin) | (target << 6) | (piece << 12) | (promotion << 16) | (capture << 20) | (doublep << 21) | (enpassant << 22) | (castling << 23)
#define origin(move) (move & 0x3f)
#define target(move) ((move & 0xfc0) >> 6)
#define piece(move) ((move & 0xf000) >> 12)
#define promotion(move) ((move & 0xf0000) >> 16)
#define capture(move) (move & 0x100000)
#define doublep(move) (move & 0x200000)
#define enpassant(move) (move & 0x400000)
#define castling(move) (move & 0x800000)

// Debug
#define startPos "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define tricky "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"