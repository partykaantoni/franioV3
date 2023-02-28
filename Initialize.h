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