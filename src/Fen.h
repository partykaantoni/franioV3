void importFen(char* fen) {
	u_int fenLength = strlen(fen);

	// Reset
	memset(pieces, 0ULL, sizeof(pieces));
	memset(occupancies, 0ULL, sizeof(occupancies));
	whiteToMove = 1;
	enPassant = -1;
	castles = 0;

	repetitionIndex = 0;
	memset(repetitionTable, 0ULL, sizeof(repetitionTable));
	ply = 0;

	if (fen[fenLength - 1] == ' ') fenLength--;
	
	int charIndex = 0;
	int boardIndex = 0;
	char names[12] = {"PNBRQKpnbrqk"};
	
	// Piece positions
	while (fen[charIndex] != 32) {
		
		for (int i = 0; i < PIECE_COUNT; i++) {
			if (fen[charIndex] == names[i]) {
				pieces[i] |= (1L << boardIndex++);
				break;
			}
		}
		switch (fen[charIndex]) {
			case '/':
				break;
			case '1':
				boardIndex++;
				break;
			case '2':
				boardIndex += 2;
				break;
			case '3':
				boardIndex += 3;
				break;
			case '4':
				boardIndex += 4;
				break;
			case '5':
				boardIndex += 5;
				break;
			case '6':
				boardIndex += 6;
				break;
			case '7':
				boardIndex += 7;
				break;
			case '8':
				boardIndex += 8;
				break;
			default:
				break;
		}
		charIndex++;	
	}
	
	// White to move and possible castles
	charIndex++;
	if (fen[charIndex++] == 'w') whiteToMove = 1;
	else whiteToMove = 0;
	
	charIndex++;
	while (fen[charIndex] != 32) {

		switch (fen[charIndex++]) {
			case '-':
				break;
			case 'K':
				castles |= wk;
				break;
			case 'Q':
				castles |= wq;
				break;
			case 'k':
				castles |= bk;
				break;
			case 'q':
				castles |= bq;
				break;
			default:
				break;
		}
	}
	
	// En passant and moves
	if (fen[charIndex + 1] != '-') {
		enPassant = fen[charIndex + 1] - 'a' + (8 - (fen[charIndex + 2] - '0')) * 8;
		charIndex += 4;
		
	} else charIndex += 3;
	
	if (!(charIndex >= fenLength)) {
		if (fen[charIndex + 1] != 32) {
			half = (fen[charIndex] - '0') * 10 + fen[charIndex + 1] - '0';
			charIndex += 2;
			
		} else {
			half = fen[charIndex] - '0';
			charIndex++;
		}
		if (charIndex + 2 == fenLength) full = fen[charIndex + 1] - '0';
		else full = (fen[charIndex + 1] - '0') * 10 + fen[charIndex + 2] - '0';
		
	} else {
		half = 0;
		full = 0;

	}

	U64 OCUPwhite = 0ULL, OCUPblack = 0ULL;
		for (int i = WHITE_PAWN; i <= WHITE_KING; i++) {
			OCUPwhite |= pieces[i];
		}
		for (int i = BLACK_PAWN; i <= BLACK_KING; i++) {
			OCUPblack |= pieces[i];
		}
	occupancies[OCCUPANCYW] = OCUPwhite;
	occupancies[OCCUPANCYB] = OCUPblack;
	occupancies[OCCUPANCY]  = OCUPwhite | OCUPblack;

	//hashKey = generateHashKey();
}
