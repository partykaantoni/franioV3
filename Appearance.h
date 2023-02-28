// Cute pieces :)
const char *unicodePieces[12] = {"♟︎", "♞", "♝", "♜", "♛", "♚", "♙", "♘", "♗", "♖", "♕", "♔"};

void drawLine() {
	for (int i = 0; i < 8; i++) {
		printf("+---");
		}
	printf("+\n");
	return;
}

void drawBitboard(U64 board) {
	drawLine();
	for (int i = 0; i < 64; i++) {
		if (i != 0 && i % 8 == 0) {
			printf("| %d\n", 9 - i/8);
			drawLine();
			}
		if (((board >> i) & 1) == 1) {
			#ifdef WIN64
			printf("| O ");
			#else
			printf("| ✪ ");
			#endif
			}
		else printf("|   ");
	}
	printf("| 1\n");
	drawLine();
	printf(" ");
	for (int i = 0; i < 8; i++) {
		printf(" %c  ", i + 97);
	}
	printf("\n");
	return;
}

void drawPosition() {
	char names[12] = {"PNBRQKpnbrqk"};
	drawLine();
	for (int i = 0; i < 64; i++) {
		if (i != 0 && i % 8 == 0) {
			printf("| %d\n", 9 - i/8);
			drawLine();
			}
		for (int j = 0; j < 13; j++) {
			if (j == 12) {
				printf("|   ");
				break;
			}
			if (((pieces[j] >> i) & 1) == 1) {
				#ifdef WIN64
				printf("| %c ", names[j]);
				#else
				printf("| %s ", unicodePieces[j]);
				#endif
				break;
			}
		}
	}
	printf("| 1\n");
	drawLine();
	printf(" ");
	for (int i = 0; i < 8; i++) {
		printf(" %c  ", i + 97);
	}
	printf("\n\n");
	
	// DEBUG
	printf("  CWK: %d, CWQ: %d, CBK: %d, CBQ: %d\n", (castles & wk) ? 1 : 0, (castles & wq) ? 1 : 0, (castles & bk) ? 1 : 0, (castles & bq) ? 1 : 0);
	
	printf("  HALF MOVES: %d, FULL MOVES: %d\n", half, full);
	printf("  Hash Key : %llx\n", hashKey);
    
	if (whiteToMove == 1) printf("  White to move\n");
	else printf("  Black to move\n");
	
	if (enPassant != -1) {
		printf("  En Passant : %s\n\n", indexToNames[enPassant]);
	} else {
		printf("\n");
	}

	return;
}