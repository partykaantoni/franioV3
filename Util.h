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

// Debug
#define startPos "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"