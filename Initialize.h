/*
     +=========+
    |  Magics |
    +=========+
*/
U64 bishopAttacks(u_int square, U64 blockers) {
	U64 attacks = 0ULL;
	
	int r, f;
	u_int tr = square / 8;
	u_int tf = square % 8;
	
	for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++) {
		attacks |= (1ULL << (r*8 + f));
		if ((1ULL << (r * 8 + f)) & blockers) break;
	}
	for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++) {
		attacks |= (1ULL << (r*8 +f));
		if ((1ULL << (r * 8 + f)) & blockers) break;
	}
	for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--) {
		attacks |= (1ULL << (r*8 +f));
		if ((1ULL << (r * 8 + f)) & blockers) break;
	}
	for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) { 
		attacks |= (1ULL << (r*8 +f));
		if ((1ULL << (r * 8 + f)) & blockers) break;
	}
	return attacks;
}
U64 rookAttacks(u_int square, U64 blockers) {
	U64 attacks = 0ULL;
	int r, f;
	u_int tr = square / 8;
	u_int tf = square % 8;
	for (r = tr + 1; r <= 7; r++) {
		attacks |= (1ULL << (r * 8 +tf));
		if (1ULL << (r * 8 + tf) & blockers) break;
	}
	for (r = tr - 1; r >= 0; r--) { 
		attacks |= (1ULL << (r * 8 +tf));
		if (1ULL << (r * 8 + tf) & blockers) break;
	}	
	for (f = tf + 1; f <= 7; f++) {
		attacks |= (1ULL << (tr * 8 +f));
		if (1ULL << (tr * 8 + f) & blockers) break;
	}
	for (f = tf - 1; f >= 0; f--) {
		attacks |= (1ULL << (tr * 8 +f));
		if (1ULL << (tr * 8 + f) & blockers) break;
	}
	
	return attacks;
}
U64 setOccupancy(int index, int bits, U64 attackMask) {
	U64 occupancy = 0ULL;
	
	for (int i = 0; i < bits; i++) {
		int square = firstBit(attackMask);
		popBit(attackMask, square);
		if (index & (1 << i)) {
			occupancy |= (1ULL << square);
		}
	}
	return occupancy;	
}
void initializeSliderPieces(_Bool bishop) {
	for (int i = 0; i < 64; i++) {
		U64 attackMask = bishop ? BLOCKER_BISHOP_MASK[i] : BLOCKER_ROOK_MASK[i];
		int maskBitCount = bitCount(attackMask);
		int occupancyIndex = (1 << maskBitCount);
		
		for (int j = 0; j < occupancyIndex; j++) {
			if(bishop) {
				U64 occupancy = setOccupancy(j, maskBitCount, attackMask);
				int magicIndex = (occupancy * BISHOP_MAGIC_NUMBERS[i]) >> (64 - BISHOP_RELEVANCY[i]);
				bishopMask[i][magicIndex] = bishopAttacks(i, occupancy);
			} else {
				U64 occupancy = setOccupancy(j, maskBitCount, attackMask);
				int magicIndex = (occupancy * ROOK_MAGIC_NUMBERS[i]) >> (64 - ROOK_RELEVANCY[i]);
				rookMask[i][magicIndex] = rookAttacks(i, occupancy);
			}
		}
	}
}
/*
     +============+
     |  Pawn shii |
     +============+
*/
void initializeEvaluationMasks() {

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {

			int square = i * 8 + j;
			ISOLATED_MASK[square] |= FILES[j-1];
			if (j + 1 != 8) ISOLATED_MASK[square] |= FILES[j+1];
		}
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {

			int square = i * 8 + j;
			WHITE_PASSED_MASK[square] |= ISOLATED_MASK[square];
			WHITE_PASSED_MASK[square] |= FILES[j];
			for (int k = 0; k < 8 - i; k++) {
				WHITE_PASSED_MASK[square] &= ~RANK[k];
			}
		}
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {

			int square = (7 - i) * 8 + j;
			BLACK_PASSED_MASK[square] |= ISOLATED_MASK[square];
			BLACK_PASSED_MASK[square] |= FILES[j];
			for (int k = 7; k  >= i; k--) {
				BLACK_PASSED_MASK[square] &= ~RANK[k];
			}
		}
	}
    return;
}