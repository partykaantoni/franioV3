static inline void addMove(moveList *moves, u_int move) {
	moves -> moves[moves -> count] = move;
	moves -> count++;
}
/*
     +========+
    |  Magic |
    +========+
*/
static inline U64 getBishopMoves(u_int square, U64 occupancy) {
	occupancy &= BLOCKER_BISHOP_MASK[square];
	occupancy *= BISHOP_MAGIC_NUMBERS[square];
	occupancy >>= 64 - BISHOP_RELEVANCY[square];
	
	return bishopMask[square][occupancy];
}
static inline U64 getRookMoves(u_int square, U64 occupancy) {
	occupancy &= BLOCKER_ROOK_MASK[square];
	occupancy *= ROOK_MAGIC_NUMBERS[square];
	occupancy >>= 64 - ROOK_RELEVANCY[square];
	
	return rookMask[square][occupancy];
}
static inline U64 getQueenMoves(u_int square, U64 occupancy) {
	U64 bishop = occupancy;
	U64 rook = occupancy;
	
	bishop &= BLOCKER_BISHOP_MASK[square];
	bishop *= BISHOP_MAGIC_NUMBERS[square];
	bishop >>= 64 - BISHOP_RELEVANCY[square];
	
	rook &= BLOCKER_ROOK_MASK[square];
	rook *= ROOK_MAGIC_NUMBERS[square];
	rook >>= 64 - ROOK_RELEVANCY[square];
	
	return rookMask[square][rook] | bishopMask[square][bishop];
}
/*
     +==================+
    |  Checks and shii |
    +==================+
*/
static inline _Bool isSquareAttacked(u_int square) {
	if (whiteToMove == 0) {
		if (BP_ATTACK_MASK[square] & pieces[WHITE_PAWN]) return 1;
		if (KING_ATTACK_MASK[square] & pieces[WHITE_KING]) return 1;
		if (KNIGHT_ATTACK_MASK[square] & pieces[WHITE_KNIGHT]) return 1;
		if (getQueenMoves(square, occupancies[OCCUPANCY]) & pieces[WHITE_QUEEN]) return 1;
		if (getBishopMoves(square, occupancies[OCCUPANCY]) & pieces[WHITE_BISHOP]) return 1;
		if (getRookMoves(square, occupancies[OCCUPANCY]) & pieces[WHITE_ROOK]) return 1;
		
	} else {
		if (WP_ATTACK_MASK[square] & pieces[BLACK_PAWN]) return 1;
		if (KING_ATTACK_MASK[square] & pieces[BLACK_KING]) return 1;
		if (KNIGHT_ATTACK_MASK[square] & pieces[BLACK_KNIGHT]) return 1;
		if (getQueenMoves(square, occupancies[OCCUPANCY]) & pieces[BLACK_QUEEN]) return 1;
		if (getBishopMoves(square, occupancies[OCCUPANCY]) & pieces[BLACK_BISHOP]) return 1;
		if (getRookMoves(square, occupancies[OCCUPANCY]) & pieces[BLACK_ROOK]) return 1;
	}
	return 0;
}

static inline _Bool isSquareProtectedBy(_Bool white, u_int square) {
	if (white) {
		if (BP_ATTACK_MASK[square] & pieces[WHITE_PAWN]) return 1;
		if (KING_ATTACK_MASK[square] & pieces[WHITE_KING]) return 1;
		if (KNIGHT_ATTACK_MASK[square] & pieces[WHITE_KNIGHT]) return 1;
		if (getQueenMoves(square, occupancies[OCCUPANCY]) & pieces[WHITE_QUEEN]) return 1;
		if (getBishopMoves(square, occupancies[OCCUPANCY]) & pieces[WHITE_BISHOP]) return 1;
		if (getRookMoves(square, occupancies[OCCUPANCY]) & pieces[WHITE_ROOK]) return 1;
		
	} else {
		if (WP_ATTACK_MASK[square] & pieces[BLACK_PAWN]) return 1;
		if (KING_ATTACK_MASK[square] & pieces[BLACK_KING]) return 1;
		if (KNIGHT_ATTACK_MASK[square] & pieces[BLACK_KNIGHT]) return 1;
		if (getQueenMoves(square, occupancies[OCCUPANCY]) & pieces[BLACK_QUEEN]) return 1;
		if (getBishopMoves(square, occupancies[OCCUPANCY]) & pieces[BLACK_BISHOP]) return 1;
		if (getRookMoves(square, occupancies[OCCUPANCY]) & pieces[BLACK_ROOK]) return 1;
	}
	return 0;
}

/*
     +==================+
    |  Move Generation |
    +==================+
*/
static inline void generateMoves(moveList* moves) {
	moves -> count = 0;
	int originSq, targetSq;
	U64 attacks, board;
	
	for (int i = WHITE_PAWN; i <= BLACK_KING; i++) {
		board = pieces[i];
		
		if (whiteToMove) {
			if (i == WHITE_PAWN) {
			
				while (board) {
					originSq = firstBit(board);
					targetSq = originSq - 8;
					// Pawn Moves
					if ((targetSq >= a8) && !getBit(occupancies[OCCUPANCY], targetSq)) {
						if (originSq >= a7 && originSq <= h7) {
							// Promotion single push
							addMove(moves, encodeMove(originSq, targetSq, i, WHITE_QUEEN, 0, 0, 0, 0));
							addMove(moves, encodeMove(originSq, targetSq, i, WHITE_ROOK, 0, 0, 0, 0));
							addMove(moves, encodeMove(originSq, targetSq, i, WHITE_BISHOP, 0, 0, 0, 0));
							addMove(moves, encodeMove(originSq, targetSq, i, WHITE_KNIGHT, 0, 0, 0, 0));
						} else {
							// Single push
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));

							if ((originSq >= a2 && originSq <= h2) && (!getBit(occupancies[OCCUPANCY], targetSq - 8))) {
								// Double push
								addMove(moves, encodeMove(originSq, targetSq - 8, i, 0, 0, 1, 0, 0));
							}
						}
					}
					attacks = WP_ATTACK_MASK[originSq] & occupancies[OCCUPANCYB];
					
					// Pawn Attacks
					while (attacks) {
						targetSq = firstBit(attacks);
						
						if (originSq >= a7 && originSq <= h7) {
							// Promotion Capture
							addMove(moves, encodeMove(originSq, targetSq, i, WHITE_QUEEN, 1, 0, 0, 0));
							addMove(moves, encodeMove(originSq, targetSq, i, WHITE_ROOK, 1, 0, 0, 0));
							addMove(moves, encodeMove(originSq, targetSq, i, WHITE_BISHOP, 1, 0, 0, 0));
							addMove(moves, encodeMove(originSq, targetSq, i, WHITE_KNIGHT, 1, 0, 0, 0));

						} else {
							// Capture
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
						}
						// popBit(attacks, targetSq);
						popWhile(attacks);
					}
					
					if (enPassant != -1) {
						U64 enAttack = WP_ATTACK_MASK[originSq] & (1ULL << enPassant);
						if (enAttack) {
							// En Passant Capture
							u_int targetEn = firstBit(enAttack);
							addMove(moves, encodeMove(originSq, targetEn, i, 0, 1, 0, 1, 0));
						}
					}
					popWhile(board);
				}
				continue;
			}
			
			
			
			else if (i == WHITE_KING) {
				if (castles & wk) {
					if(!(CASTLING_MASK[CASTLE_WHITE_KING] & occupancies[OCCUPANCY]) && !isSquareAttacked(f1)) {
						//addMoveCastle
						addMove(moves, encodeMove(e1, g1, i, 0, 0, 0, 0, 1));
					}
				}
				if (castles & wq) {
					if(!(CASTLING_MASK[CASTLE_WHITE_QUEEN] & occupancies[OCCUPANCY]) && !isSquareAttacked(d1)) {
						//addMoveCastle
						addMove(moves, encodeMove(e1, c1, i, 0, 0, 0, 0, 1));
					}
				}

				originSq = firstBit(board);
				attacks = KING_ATTACK_MASK[originSq] & ~occupancies[OCCUPANCYW];
				while(attacks) {
					targetSq = firstBit(attacks);
					if (getBit(occupancies[OCCUPANCYB], targetSq)) {
						// Add capture king
						addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
					} else {
						// Add quiet move king
						addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
					}
					popWhile(attacks);
				}
				continue;
			}
			
			
			
			else if (i == WHITE_KNIGHT) {
				while(board) {
					originSq = firstBit(board);
					attacks = KNIGHT_ATTACK_MASK[originSq] & ~occupancies[OCCUPANCYW];
					while(attacks) {
						targetSq = firstBit(attacks);
						if (getBit(occupancies[OCCUPANCYB], targetSq)) {
							// Add capture knight
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
						} else {
							// Add quiet move knight
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
						}
						popWhile(attacks);
					}
					popWhile(board);
				}
				continue;
			}
			
			
			
			else if (i == WHITE_BISHOP) {
				while(board) {
					originSq = firstBit(board);
					attacks = getBishopMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYW];
					while(attacks) {
						targetSq = firstBit(attacks);
						if (getBit(occupancies[OCCUPANCYB], targetSq)) {
							// Add capture bishop
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
						} else {
							// Add quiet move bishop
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
						}
						popWhile(attacks);
					}
					popWhile(board);
				}
				continue;
			}



			else if (i == WHITE_ROOK) {
				while(board) {
					originSq = firstBit(board);
					attacks = getRookMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYW];
					while(attacks) {
						targetSq = firstBit(attacks);
						if (getBit(occupancies[OCCUPANCYB], targetSq)) {
							// Add capture rook
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
						} else {
							// Add quiet move rook
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
						}
						popWhile(attacks);
					}
					popWhile(board);
				}
				continue;
			}



			else if (i == WHITE_QUEEN) {
				while(board) {
					originSq = firstBit(board);
					attacks = getQueenMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYW];
					while(attacks) {
						targetSq = firstBit(attacks);
						if (getBit(occupancies[OCCUPANCYB], targetSq)) {
							// Add capture queen
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
						} else {
							// Add quiet move queen
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
						}
						popWhile(attacks);
					}
					popWhile(board);
				}
				continue;
			}
			
		} else {
			if (i == BLACK_PAWN) {
			
				while (board) {
					originSq = firstBit(board);
					targetSq = originSq + 8;
					
					// Pawn Moves
					if ((targetSq <= h1) && !getBit(occupancies[OCCUPANCY], targetSq)) {
						if (originSq >= a2 && originSq <= h2) {
							addMove(moves, encodeMove(originSq, targetSq, i, BLACK_QUEEN, 0, 0, 0, 0));
							addMove(moves, encodeMove(originSq, targetSq, i, BLACK_ROOK, 0, 0, 0, 0));
							addMove(moves, encodeMove(originSq, targetSq, i, BLACK_BISHOP, 0, 0, 0, 0));
							addMove(moves, encodeMove(originSq, targetSq, i, BLACK_KNIGHT, 0, 0, 0, 0));
							
						} else {
							//addMoveSinglePush
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));

							if ((originSq >= a7 && originSq <= h7) && (!getBit(occupancies[OCCUPANCY], targetSq + 8))) {
								//addMoveDoublePush
								addMove(moves, encodeMove(originSq, targetSq + 8, i, 0, 0, 1, 0, 0));
							}
						}
					}
					attacks = BP_ATTACK_MASK[originSq] & occupancies[OCCUPANCYW];
					
					// Pawn Attacks
					while (attacks) {
						targetSq = firstBit(attacks);
						
						if (originSq >= a2 && originSq <= h2) {
							//addMovePromotionCapture
							addMove(moves, encodeMove(originSq, targetSq, i, BLACK_QUEEN, 1, 0, 0, 0));
							addMove(moves, encodeMove(originSq, targetSq, i, BLACK_ROOK, 1, 0, 0, 0));
							addMove(moves, encodeMove(originSq, targetSq, i, BLACK_BISHOP, 1, 0, 0, 0));
							addMove(moves, encodeMove(originSq, targetSq, i, BLACK_KNIGHT, 1, 0, 0, 0));
						} else {
							//addMoveCapture
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
						}
						popWhile(attacks);
					}
					
					if (enPassant != -1) {
						U64 enAttack = BP_ATTACK_MASK[originSq] & (1ULL << enPassant);
						if (enAttack) {
							//addMoveEnPassantCapture
							u_int targetEn = firstBit(enAttack);
							addMove(moves, encodeMove(originSq, targetEn, i, 0, 1, 0, 1, 0));
						}
					}
					popWhile(board);
				}
				continue;
			}
			
			
			
			else if (i == BLACK_KING) {
				if (castles & bk) {
					if(!(CASTLING_MASK[CASTLE_BLACK_KING] & occupancies[OCCUPANCY]) && !isSquareAttacked(f8)) {
						// AddCastlingMove
						addMove(moves, encodeMove(e8, g8, i, 0, 0, 0, 0, 1));
					}
				}
				if (castles & bq) {
					if(!(CASTLING_MASK[CASTLE_BLACK_QUEEN] & occupancies[OCCUPANCY]) && !isSquareAttacked(d8)) {
						// AddCastlingMove
						addMove(moves, encodeMove(e8, c8, i, 0, 0, 0, 0, 1));
					}
				}

				
				originSq = firstBit(board);
				attacks = KING_ATTACK_MASK[originSq] & ~occupancies[OCCUPANCYB];
				while(attacks) {
					targetSq = firstBit(attacks);
					if (getBit(occupancies[OCCUPANCYW], targetSq)) {
						// Add capture king
						addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
					} else {
						// Add quiet move king
						addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
					}
					popWhile(attacks);
				}
				continue;
			}

			
			else if (i == BLACK_KNIGHT) {
				while(board) {
					originSq = firstBit(board);
					attacks = KNIGHT_ATTACK_MASK[originSq] & ~occupancies[OCCUPANCYB];
					while(attacks) {
						targetSq = firstBit(attacks);
						if (getBit(occupancies[OCCUPANCYW], targetSq)) {
							// Add capture knight
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
						} else {
							// Add quiet move knight
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
						}
						popWhile(attacks);
					}
					popWhile(board);
				}
				continue;
			}


			else if (i == BLACK_BISHOP) {
				while(board) {
					originSq = firstBit(board);
					attacks = getBishopMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYB];
					while(attacks) {
						targetSq = firstBit(attacks);
						if (getBit(occupancies[OCCUPANCYW], targetSq)) {
							// Add capture bishop
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
						} else {
							// Add quiet move bishop
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
						}
						popWhile(attacks);
					}
					popWhile(board);
				}
				continue;
			}


			else if (i == BLACK_ROOK) {
				while(board) {
					originSq = firstBit(board);
					attacks = getRookMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYB];
					while(attacks) {
						targetSq = firstBit(attacks);
						if (getBit(occupancies[OCCUPANCYW], targetSq)) {
							// Add capture rook
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
						} else {
							// Add quiet move rook
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
						}
						popWhile(attacks);
					}
					popWhile(board);
				}
				continue;
			}


			else if (i == BLACK_QUEEN) {
				while(board) {
					originSq = firstBit(board);
					attacks = getQueenMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYB];
					while(attacks) {
						targetSq = firstBit(attacks);
						if (getBit(occupancies[OCCUPANCYW], targetSq)) {
							// Add capture queen
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
						} else {
							// Add quiet move queen
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
						}
						popWhile(attacks);
					}
					popWhile(board);
				}
				continue;
			}
		}
	}
	return;
}


















static inline void moveGen(moveList* moves) {
	moves -> count = 0;
	int originSq, targetSq;
	U64 attacks, board;

	if (whiteToMove) {

		for (int i = WHITE_PAWN; i <= WHITE_KING; i++) {
			board = pieces[i];

			switch (i) {
				case WHITE_PAWN:
			
					while (board) {
						originSq = firstBit(board);
						targetSq = originSq - 8;

						// Pawn Moves
						if ((targetSq >= a8) && !getBit(occupancies[OCCUPANCY], targetSq)) {
							if (originSq >= a7 && originSq <= h7) {
								// Promotion single push
								addMove(moves, encodeMove(originSq, targetSq, i, WHITE_QUEEN, 0, 0, 0, 0));
								addMove(moves, encodeMove(originSq, targetSq, i, WHITE_ROOK, 0, 0, 0, 0));
								addMove(moves, encodeMove(originSq, targetSq, i, WHITE_BISHOP, 0, 0, 0, 0));
								addMove(moves, encodeMove(originSq, targetSq, i, WHITE_KNIGHT, 0, 0, 0, 0));
							} else {
								// Single push
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));

								if ((originSq >= a2 && originSq <= h2) && (!getBit(occupancies[OCCUPANCY], targetSq - 8))) {
									// Double push
									addMove(moves, encodeMove(originSq, targetSq - 8, i, 0, 0, 1, 0, 0));
								}
							}
						}
						attacks = WP_ATTACK_MASK[originSq] & occupancies[OCCUPANCYB];
						
						// Pawn Attacks
						while (attacks) {
							targetSq = firstBit(attacks);
							
							if (originSq >= a7 && originSq <= h7) {
								// Promotion Capture
								addMove(moves, encodeMove(originSq, targetSq, i, WHITE_QUEEN, 1, 0, 0, 0));
								addMove(moves, encodeMove(originSq, targetSq, i, WHITE_ROOK, 1, 0, 0, 0));
								addMove(moves, encodeMove(originSq, targetSq, i, WHITE_BISHOP, 1, 0, 0, 0));
								addMove(moves, encodeMove(originSq, targetSq, i, WHITE_KNIGHT, 1, 0, 0, 0));

							} else {
								// Capture
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
							}
							// popBit(attacks, targetSq);
							popWhile(attacks);
						}
						
						if (enPassant != -1) {
							U64 enAttack = WP_ATTACK_MASK[originSq] & (1ULL << enPassant);
							if (enAttack) {
								// En Passant Capture
								u_int targetEn = firstBit(enAttack);
								addMove(moves, encodeMove(originSq, targetEn, i, 0, 1, 0, 1, 0));
							}
						}
						popWhile(board);
					}
					continue;

				break;
			


				case WHITE_KING:

					if (castles & wk) {
						if(!(CASTLING_MASK[CASTLE_WHITE_KING] & occupancies[OCCUPANCY]) && !isSquareAttacked(f1)) {
							//addMoveCastle
							addMove(moves, encodeMove(e1, g1, i, 0, 0, 0, 0, 1));
						}
					}
					if (castles & wq) {
						if(!(CASTLING_MASK[CASTLE_WHITE_QUEEN] & occupancies[OCCUPANCY]) && !isSquareAttacked(d1)) {
							//addMoveCastle
							addMove(moves, encodeMove(e1, c1, i, 0, 0, 0, 0, 1));
						}
					}

					originSq = firstBit(board);
					attacks = KING_ATTACK_MASK[originSq] & ~occupancies[OCCUPANCYW];

					while(attacks) {
						targetSq = firstBit(attacks);
						if (getBit(occupancies[OCCUPANCYB], targetSq)) {
							// Add capture king
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
						} else {
							// Add quiet move king
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
						}
						popWhile(attacks);
					}
					continue;
			
				break;
			
			
			
				case WHITE_KNIGHT:

					while(board) {

						originSq = firstBit(board);
						attacks = KNIGHT_ATTACK_MASK[originSq] & ~occupancies[OCCUPANCYW];

						while(attacks) {

							targetSq = firstBit(attacks);
							
							if (getBit(occupancies[OCCUPANCYB], targetSq)) {
								// Add capture knight
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
							} else {
								// Add quiet move knight
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;

				
				
				case WHITE_BISHOP:

					while(board) {
						originSq = firstBit(board);
						attacks = getBishopMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYW];
						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(occupancies[OCCUPANCYB], targetSq)) {
								// Add capture bishop
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
							} else {
								// Add quiet move bishop
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;



				case WHITE_ROOK:

					while(board) {
						originSq = firstBit(board);
						attacks = getRookMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYW];
						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(occupancies[OCCUPANCYB], targetSq)) {
								// Add capture rook
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
							} else {
								// Add quiet move rook
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;



				case WHITE_QUEEN:

					while(board) {
						originSq = firstBit(board);
						attacks = getQueenMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYW];
						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(occupancies[OCCUPANCYB], targetSq)) {
								// Add capture queen
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
							} else {
								// Add quiet move queen
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;
				


				default:
					break;
			}
		}
		return;
	} else {

		for (int i = BLACK_PAWN; i <= BLACK_KING; i++) {
			board = pieces[i];

			switch (i) {
				case BLACK_PAWN:
			
					while (board) {

						originSq = firstBit(board);
						targetSq = originSq + 8;
						
						// Pawn Moves
						if ((targetSq <= h1) && !getBit(occupancies[OCCUPANCY], targetSq)) {

							if (originSq >= a2 && originSq <= h2) {
								addMove(moves, encodeMove(originSq, targetSq, i, BLACK_QUEEN, 0, 0, 0, 0));
								addMove(moves, encodeMove(originSq, targetSq, i, BLACK_ROOK, 0, 0, 0, 0));
								addMove(moves, encodeMove(originSq, targetSq, i, BLACK_BISHOP, 0, 0, 0, 0));
								addMove(moves, encodeMove(originSq, targetSq, i, BLACK_KNIGHT, 0, 0, 0, 0));
								
							} else {
								//addMoveSinglePush
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));

								if ((originSq >= a7 && originSq <= h7) && (!getBit(occupancies[OCCUPANCY], targetSq + 8))) {
									//addMoveDoublePush
									addMove(moves, encodeMove(originSq, targetSq + 8, i, 0, 0, 1, 0, 0));
								}
							}
						}

						attacks = BP_ATTACK_MASK[originSq] & occupancies[OCCUPANCYW];
						
						// Pawn Attacks
						while (attacks) {

							targetSq = firstBit(attacks);
							
							if (originSq >= a2 && originSq <= h2) {
								//addMovePromotionCapture
								addMove(moves, encodeMove(originSq, targetSq, i, BLACK_QUEEN, 1, 0, 0, 0));
								addMove(moves, encodeMove(originSq, targetSq, i, BLACK_ROOK, 1, 0, 0, 0));
								addMove(moves, encodeMove(originSq, targetSq, i, BLACK_BISHOP, 1, 0, 0, 0));
								addMove(moves, encodeMove(originSq, targetSq, i, BLACK_KNIGHT, 1, 0, 0, 0));
							} else {
								//addMoveCapture
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
							}
							popWhile(attacks);
						}
						
						if (enPassant != -1) {

							U64 enAttack = BP_ATTACK_MASK[originSq] & (1ULL << enPassant);

							if (enAttack) {
								//addMoveEnPassantCapture
								u_int targetEn = firstBit(enAttack);
								addMove(moves, encodeMove(originSq, targetEn, i, 0, 1, 0, 1, 0));
							}
						}
						popWhile(board);
					}
					continue;

				break;
			
			
			
				case BLACK_KING:

					if (castles & bk) {

						if(!(CASTLING_MASK[CASTLE_BLACK_KING] & occupancies[OCCUPANCY]) && !isSquareAttacked(f8)) {
							// AddCastlingMove
							addMove(moves, encodeMove(e8, g8, i, 0, 0, 0, 0, 1));
						}
					}
					if (castles & bq) {

						if(!(CASTLING_MASK[CASTLE_BLACK_QUEEN] & occupancies[OCCUPANCY]) && !isSquareAttacked(d8)) {
							// AddCastlingMove
							addMove(moves, encodeMove(e8, c8, i, 0, 0, 0, 0, 1));
						}
					}
					
					originSq = firstBit(board);
					attacks = KING_ATTACK_MASK[originSq] & ~occupancies[OCCUPANCYB];

					while(attacks) {

						targetSq = firstBit(attacks);

						if (getBit(occupancies[OCCUPANCYW], targetSq)) {
							// Add capture king
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
						} else {
							// Add quiet move king
							addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
						}
						popWhile(attacks);
					}
					continue;

				break;

				
				case BLACK_KNIGHT:

					while(board) {
						originSq = firstBit(board);
						attacks = KNIGHT_ATTACK_MASK[originSq] & ~occupancies[OCCUPANCYB];

						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(occupancies[OCCUPANCYW], targetSq)) {
								// Add capture knight
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
							} else {
								// Add quiet move knight
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;


				case BLACK_BISHOP:

					while(board) {

						originSq = firstBit(board);
						attacks = getBishopMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYB];

						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(occupancies[OCCUPANCYW], targetSq)) {
								// Add capture bishop
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
							} else {
								// Add quiet move bishop
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;


				case BLACK_ROOK:

					while(board) {

						originSq = firstBit(board);
						attacks = getRookMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYB];

						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(occupancies[OCCUPANCYW], targetSq)) {
								// Add capture rook
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
							} else {
								// Add quiet move rook
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;


				case BLACK_QUEEN:

					while(board) {

						originSq = firstBit(board);
						attacks = getQueenMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYB];

						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(occupancies[OCCUPANCYW], targetSq)) {
								// Add capture queen
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0));
							} else {
								// Add quiet move queen
								addMove(moves, encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0));
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;
			}
		}
		return;
	}
}

// moves -> count += 4;
// 								moves -> moves[moves -> count - 4] = encodeMove(originSq, targetSq, i, WHITE_QUEEN, 0, 0, 0, 0);
// 								moves -> moves[moves -> count - 3] = encodeMove(originSq, targetSq, i, WHITE_ROOK, 0, 0, 0, 0);
// 								moves -> moves[moves -> count - 2] = encodeMove(originSq, targetSq, i, WHITE_BISHOP, 0, 0, 0, 0);
// 								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, WHITE_KNIGHT, 0, 0, 0, 0);























static inline void moveGen2(moveList* moves) {
	moves -> count = 0;
	int originSq, targetSq;
	U64 attacks, board;

	if (whiteToMove) {

		for (int i = WHITE_PAWN; i <= WHITE_KING; i++) {
			board = pieces[i];

			switch (i) {
				case WHITE_PAWN:
			
					while (board) {
						originSq = firstBit(board);
						targetSq = originSq - 8;

						// Pawn Moves
						if ((targetSq >= a8) && !getBit(occupancies[OCCUPANCY], targetSq)) {
							if (originSq >= a7 && originSq <= h7) {
								// Promotion single push
								moves -> count += 4;
								moves -> moves[moves -> count - 4] = encodeMove(originSq, targetSq, i, WHITE_QUEEN, 0, 0, 0, 0);
								moves -> moves[moves -> count - 3] = encodeMove(originSq, targetSq, i, WHITE_ROOK, 0, 0, 0, 0);
								moves -> moves[moves -> count - 2] = encodeMove(originSq, targetSq, i, WHITE_BISHOP, 0, 0, 0, 0);
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, WHITE_KNIGHT, 0, 0, 0, 0);
							} else {
								// Single push
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);


								if ((originSq >= a2 && originSq <= h2) && (!getBit(occupancies[OCCUPANCY], targetSq - 8))) {
									// Double push
									moves -> count++;
									moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq - 8, i, 0, 0, 1, 0, 0);
								}
							}
						}
						attacks = WP_ATTACK_MASK[originSq] & occupancies[OCCUPANCYB];
						
						// Pawn Attacks
						while (attacks) {
							targetSq = firstBit(attacks);
							
							if (originSq >= a7 && originSq <= h7) {
								// Promotion Capture
								moves -> count += 4;
								moves -> moves[moves -> count - 4] = encodeMove(originSq, targetSq, i, WHITE_QUEEN, 1, 0, 0, 0);
								moves -> moves[moves -> count - 3] = encodeMove(originSq, targetSq, i, WHITE_ROOK, 1, 0, 0, 0);
								moves -> moves[moves -> count - 2] = encodeMove(originSq, targetSq, i, WHITE_BISHOP, 1, 0, 0, 0);
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, WHITE_KNIGHT, 1, 0, 0, 0);

							} else {
								// Capture
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							}
							// popBit(attacks, targetSq);
							popWhile(attacks);
						}
						
						if (enPassant != -1) {
							U64 enAttack = BP_ATTACK_MASK[originSq] & (1ULL << enPassant);
							if (enAttack) {
								// En Passant Capture
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, firstBit(enAttack), i, 0, 1, 0, 1, 0);
							}
						}
						popWhile(board);
					}
					continue;

				break;
			


				case WHITE_KING:

					if (castles & wk) {
						if(!(CASTLING_MASK[CASTLE_WHITE_KING] & occupancies[OCCUPANCY]) && !isSquareAttacked(f1)) {
							//addMoveCastle
							moves -> count++;
							moves -> moves[moves -> count - 1] = encodeMove(e1, g1, i, 0, 0, 0, 0, 1);
						}
					}
					if (castles & wq) {
						if(!(CASTLING_MASK[CASTLE_WHITE_QUEEN] & occupancies[OCCUPANCY]) && !isSquareAttacked(d1)) {
							//addMoveCastle
							moves -> count++;
							moves -> moves[moves -> count - 1] = encodeMove(e1, c1, i, 0, 0, 0, 0, 1);
						}
					}

					originSq = firstBit(board);
					attacks = KING_ATTACK_MASK[originSq] & ~occupancies[OCCUPANCYW];

					while(attacks) {
						targetSq = firstBit(attacks);
						if (getBit(occupancies[OCCUPANCYB], targetSq)) {
							// Add capture king
							moves -> count++;
							moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
						} else {
							// Add quiet move king
							moves -> count++;
							moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
						}
						popWhile(attacks);
					}
					continue;
			
				break;
			
			
			
				case WHITE_KNIGHT:

					while(board) {

						originSq = firstBit(board);
						attacks = KNIGHT_ATTACK_MASK[originSq] & ~occupancies[OCCUPANCYW];

						while(attacks) {

							targetSq = firstBit(attacks);
							
							if (getBit(occupancies[OCCUPANCYB], targetSq)) {
								// Add capture knight
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							} else {
								// Add quiet move knight
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;

				
				
				case WHITE_BISHOP:

					while(board) {
						originSq = firstBit(board);
						attacks = getBishopMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYW];
						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(occupancies[OCCUPANCYB], targetSq)) {
								// Add capture bishop
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							} else {
								// Add quiet move bishop
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;



				case WHITE_ROOK:

					while(board) {
						originSq = firstBit(board);
						attacks = getRookMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYW];
						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(occupancies[OCCUPANCYB], targetSq)) {
								// Add capture rook
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							} else {
								// Add quiet move rook
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;



				case WHITE_QUEEN:

					while(board) {
						originSq = firstBit(board);
						attacks = getQueenMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYW];
						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(occupancies[OCCUPANCYB], targetSq)) {
								// Add capture queen
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							} else {
								// Add quiet move queen
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;
				


				default:
					break;
			}
		}
		return;
	} else {

		for (int i = BLACK_PAWN; i <= BLACK_KING; i++) {
			board = pieces[i];

			switch (i) {
				case BLACK_PAWN:
			
					while (board) {

						originSq = firstBit(board);
						targetSq = originSq + 8;
						
						// Pawn Moves
						if ((targetSq <= h1) && !getBit(occupancies[OCCUPANCY], targetSq)) {

							if (originSq >= a2 && originSq <= h2) {
								moves -> count += 4;
								moves -> moves[moves -> count - 4] = encodeMove(originSq, targetSq, i, BLACK_QUEEN, 0, 0, 0, 0);
								moves -> moves[moves -> count - 3] = encodeMove(originSq, targetSq, i, BLACK_ROOK, 0, 0, 0, 0);
								moves -> moves[moves -> count - 2] = encodeMove(originSq, targetSq, i, BLACK_BISHOP, 0, 0, 0, 0);
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, BLACK_KNIGHT, 0, 0, 0, 0);
								
							} else {
								//addMoveSinglePush
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);

								if ((originSq >= a7 && originSq <= h7) && (!getBit(occupancies[OCCUPANCY], targetSq + 8))) {
									//addMoveDoublePush
									moves -> count++;
									moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq + 8, i, 0, 0, 1, 0, 0);
								}
							}
						}

						attacks = BP_ATTACK_MASK[originSq] & occupancies[OCCUPANCYW];
						
						// Pawn Attacks
						while (attacks) {

							targetSq = firstBit(attacks);
							
							if (originSq >= a2 && originSq <= h2) {
								//addMovePromotionCapture
								moves -> count += 4;
								moves -> moves[moves -> count - 4] = encodeMove(originSq, targetSq, i, BLACK_QUEEN, 1, 0, 0, 0);
								moves -> moves[moves -> count - 3] = encodeMove(originSq, targetSq, i, BLACK_ROOK, 1, 0, 0, 0);
								moves -> moves[moves -> count - 2] = encodeMove(originSq, targetSq, i, BLACK_BISHOP, 1, 0, 0, 0);
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, BLACK_KNIGHT, 1, 0, 0, 0);
							} else {
								//addMoveCapture
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							}
							popWhile(attacks);
						}
						
						if (enPassant != -1) {

							//u_int enAttack = firstBit(BP_ATTACK_MASK[originSq] & (1ULL << enPassant));
							U64 enAttack = BP_ATTACK_MASK[originSq] & (1ULL << enPassant);
							if (enAttack) {
								//addMoveEnPassantCapture
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, firstBit(enAttack), i, 0, 1, 0, 1, 0);
							}
						}
						popWhile(board);
					}
					continue;

				break;
			
			
			
				case BLACK_KING:

					if (castles & bk) {

						if(!(CASTLING_MASK[CASTLE_BLACK_KING] & occupancies[OCCUPANCY]) && !isSquareAttacked(f8)) {
							// AddCastlingMove
							moves -> count++;
							moves -> moves[moves -> count - 1] = encodeMove(e8, g8, i, 0, 0, 0, 0, 1);
						}
					}
					if (castles & bq) {

						if(!(CASTLING_MASK[CASTLE_BLACK_QUEEN] & occupancies[OCCUPANCY]) && !isSquareAttacked(d8)) {
							// AddCastlingMove
							moves -> count++;
							moves -> moves[moves -> count - 1] = encodeMove(e8, c8, i, 0, 0, 0, 0, 1);
						}
					}
					
					originSq = firstBit(board);
					attacks = KING_ATTACK_MASK[originSq] & ~occupancies[OCCUPANCYB];

					while(attacks) {

						targetSq = firstBit(attacks);

						if (getBit(occupancies[OCCUPANCYW], targetSq)) {
							// Add capture king
							moves -> count++;
							moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
						} else {
							// Add quiet move king
							moves -> count++;
							moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
						}
						popWhile(attacks);
					}
					continue;

				break;

				
				case BLACK_KNIGHT:

					while(board) {
						originSq = firstBit(board);
						attacks = KNIGHT_ATTACK_MASK[originSq] & ~occupancies[OCCUPANCYB];

						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(occupancies[OCCUPANCYW], targetSq)) {
								// Add capture knight
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							} else {
								// Add quiet move knight
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;


				case BLACK_BISHOP:

					while(board) {

						originSq = firstBit(board);
						attacks = getBishopMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYB];

						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(occupancies[OCCUPANCYW], targetSq)) {
								// Add capture bishop
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							} else {
								// Add quiet move bishop
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;


				case BLACK_ROOK:

					while(board) {

						originSq = firstBit(board);
						attacks = getRookMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYB];

						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(occupancies[OCCUPANCYW], targetSq)) {
								// Add capture rook
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							} else {
								// Add quiet move rook
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;


				case BLACK_QUEEN:

					while(board) {

						originSq = firstBit(board);
						attacks = getQueenMoves(originSq, occupancies[OCCUPANCY]) & ~occupancies[OCCUPANCYB];

						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(occupancies[OCCUPANCYW], targetSq)) {
								// Add capture queen
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							} else {
								// Add quiet move queen
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;
			}
		}
		return;
	}
}
























static inline void moveGen3(moveList* moves) {
	moves -> count = 0;

	if (whiteToMove) {
		int originSq, targetSq;
		U64 attacks, board;
		U64 ocupB = occupancies[OCCUPANCYB];
		U64 ocupW = occupancies[OCCUPANCYW];

		for (int i = WHITE_PAWN; i <= WHITE_KING; i++) {
			board = pieces[i];

			switch (i) {
				case WHITE_PAWN:
			
					while (board) {
						originSq = firstBit(board);
						targetSq = originSq - 8;

						// Pawn Moves
						if ((targetSq >= a8) && !getBit(occupancies[OCCUPANCY], targetSq)) {
							if (originSq >= a7 && originSq <= h7) {
								// Promotion single push
								moves -> count += 4;
								moves -> moves[moves -> count - 4] = encodeMove(originSq, targetSq, i, WHITE_QUEEN, 0, 0, 0, 0);
								moves -> moves[moves -> count - 3] = encodeMove(originSq, targetSq, i, WHITE_ROOK, 0, 0, 0, 0);
								moves -> moves[moves -> count - 2] = encodeMove(originSq, targetSq, i, WHITE_BISHOP, 0, 0, 0, 0);
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, WHITE_KNIGHT, 0, 0, 0, 0);
							} else {
								// Single push
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);


								if ((originSq >= a2 && originSq <= h2) && (!getBit(occupancies[OCCUPANCY], targetSq - 8))) {
									// Double push
									moves -> count++;
									moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq - 8, i, 0, 0, 1, 0, 0);
								}
							}
						}
						attacks = WP_ATTACK_MASK[originSq] & ocupB;
						
						// Pawn Attacks
						while (attacks) {
							targetSq = firstBit(attacks);
							
							if (originSq >= a7 && originSq <= h7) {
								// Promotion Capture
								moves -> count += 4;
								moves -> moves[moves -> count - 4] = encodeMove(originSq, targetSq, i, WHITE_QUEEN, 1, 0, 0, 0);
								moves -> moves[moves -> count - 3] = encodeMove(originSq, targetSq, i, WHITE_ROOK, 1, 0, 0, 0);
								moves -> moves[moves -> count - 2] = encodeMove(originSq, targetSq, i, WHITE_BISHOP, 1, 0, 0, 0);
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, WHITE_KNIGHT, 1, 0, 0, 0);

							} else {
								// Capture
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							}
							// popBit(attacks, targetSq);
							popWhile(attacks);
						}
						
						if (enPassant != -1) {
							U64 enAttack = BP_ATTACK_MASK[originSq] & (1ULL << enPassant);
							if (enAttack) {
								// En Passant Capture
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, firstBit(enAttack), i, 0, 1, 0, 1, 0);
							}
						}
						popWhile(board);
					}
					continue;

				break;
			


				case WHITE_KING:

					if (castles & wk) {
						if(!(CASTLING_MASK[CASTLE_WHITE_KING] & occupancies[OCCUPANCY]) && !isSquareAttacked(f1)) {
							//addMoveCastle
							moves -> count++;
							moves -> moves[moves -> count - 1] = encodeMove(e1, g1, i, 0, 0, 0, 0, 1);
						}
					}
					if (castles & wq) {
						if(!(CASTLING_MASK[CASTLE_WHITE_QUEEN] & occupancies[OCCUPANCY]) && !isSquareAttacked(d1)) {
							//addMoveCastle
							moves -> count++;
							moves -> moves[moves -> count - 1] = encodeMove(e1, c1, i, 0, 0, 0, 0, 1);
						}
					}

					originSq = firstBit(board);
					attacks = KING_ATTACK_MASK[originSq] & ~ocupW;

					while(attacks) {
						targetSq = firstBit(attacks);
						if (getBit(ocupB, targetSq)) {
							// Add capture king
							moves -> count++;
							moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
						} else {
							// Add quiet move king
							moves -> count++;
							moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
						}
						popWhile(attacks);
					}
					continue;
			
				break;
			
			
			
				case WHITE_KNIGHT:

					while(board) {

						originSq = firstBit(board);
						attacks = KNIGHT_ATTACK_MASK[originSq] & ~ocupW;

						while(attacks) {

							targetSq = firstBit(attacks);
							
							if (getBit(ocupB, targetSq)) {
								// Add capture knight
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							} else {
								// Add quiet move knight
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;

				
				
				case WHITE_BISHOP:

					while(board) {
						originSq = firstBit(board);
						attacks = getBishopMoves(originSq, occupancies[OCCUPANCY]) & ~ocupW;
						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(ocupB, targetSq)) {
								// Add capture bishop
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							} else {
								// Add quiet move bishop
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;



				case WHITE_ROOK:

					while(board) {
						originSq = firstBit(board);
						attacks = getRookMoves(originSq, occupancies[OCCUPANCY]) & ~ocupW;
						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(ocupB, targetSq)) {
								// Add capture rook
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							} else {
								// Add quiet move rook
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;



				case WHITE_QUEEN:

					while(board) {
						originSq = firstBit(board);
						attacks = getQueenMoves(originSq, occupancies[OCCUPANCY]) & ~ocupW;
						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(ocupB, targetSq)) {
								// Add capture queen
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							} else {
								// Add quiet move queen
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;
				


				default:
					break;
			}
		}
		return;
		
	} else {
		int originSq, targetSq;
		U64 attacks, board;
		U64 ocupB = occupancies[OCCUPANCYB];
		U64 ocupW = occupancies[OCCUPANCYW];

		for (int i = BLACK_PAWN; i <= BLACK_KING; i++) {
			board = pieces[i];

			switch (i) {
				case BLACK_PAWN:
			
					while (board) {

						originSq = firstBit(board);
						targetSq = originSq + 8;
						
						// Pawn Moves
						if ((targetSq <= h1) && !getBit(occupancies[OCCUPANCY], targetSq)) {

							if (originSq >= a2 && originSq <= h2) {
								moves -> count += 4;
								moves -> moves[moves -> count - 4] = encodeMove(originSq, targetSq, i, BLACK_QUEEN, 0, 0, 0, 0);
								moves -> moves[moves -> count - 3] = encodeMove(originSq, targetSq, i, BLACK_ROOK, 0, 0, 0, 0);
								moves -> moves[moves -> count - 2] = encodeMove(originSq, targetSq, i, BLACK_BISHOP, 0, 0, 0, 0);
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, BLACK_KNIGHT, 0, 0, 0, 0);
								
							} else {
								//addMoveSinglePush
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);

								if ((originSq >= a7 && originSq <= h7) && (!getBit(occupancies[OCCUPANCY], targetSq + 8))) {
									//addMoveDoublePush
									moves -> count++;
									moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq + 8, i, 0, 0, 1, 0, 0);
								}
							}
						}

						attacks = BP_ATTACK_MASK[originSq] & ocupW;
						
						// Pawn Attacks
						while (attacks) {

							targetSq = firstBit(attacks);
							
							if (originSq >= a2 && originSq <= h2) {
								//addMovePromotionCapture
								moves -> count += 4;
								moves -> moves[moves -> count - 4] = encodeMove(originSq, targetSq, i, BLACK_QUEEN, 1, 0, 0, 0);
								moves -> moves[moves -> count - 3] = encodeMove(originSq, targetSq, i, BLACK_ROOK, 1, 0, 0, 0);
								moves -> moves[moves -> count - 2] = encodeMove(originSq, targetSq, i, BLACK_BISHOP, 1, 0, 0, 0);
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, BLACK_KNIGHT, 1, 0, 0, 0);
							} else {
								//addMoveCapture
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							}
							popWhile(attacks);
						}
						
						if (enPassant != -1) {

							//u_int enAttack = firstBit(BP_ATTACK_MASK[originSq] & (1ULL << enPassant));
							U64 enAttack = BP_ATTACK_MASK[originSq] & (1ULL << enPassant);
							if (enAttack) {
								//addMoveEnPassantCapture
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, firstBit(enAttack), i, 0, 1, 0, 1, 0);
							}
						}
						popWhile(board);
					}
					continue;

				break;
			
			
			
				case BLACK_KING:

					if (castles & bk) {

						if(!(CASTLING_MASK[CASTLE_BLACK_KING] & occupancies[OCCUPANCY]) && !isSquareAttacked(f8)) {
							// AddCastlingMove
							moves -> count++;
							moves -> moves[moves -> count - 1] = encodeMove(e8, g8, i, 0, 0, 0, 0, 1);
						}
					}
					if (castles & bq) {

						if(!(CASTLING_MASK[CASTLE_BLACK_QUEEN] & occupancies[OCCUPANCY]) && !isSquareAttacked(d8)) {
							// AddCastlingMove
							moves -> count++;
							moves -> moves[moves -> count - 1] = encodeMove(e8, c8, i, 0, 0, 0, 0, 1);
						}
					}
					
					originSq = firstBit(board);
					attacks = KING_ATTACK_MASK[originSq] & ~ocupB;

					while(attacks) {

						targetSq = firstBit(attacks);

						if (getBit(ocupW, targetSq)) {
							// Add capture king
							moves -> count++;
							moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
						} else {
							// Add quiet move king
							moves -> count++;
							moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
						}
						popWhile(attacks);
					}
					continue;

				break;

				
				case BLACK_KNIGHT:

					while(board) {
						originSq = firstBit(board);
						attacks = KNIGHT_ATTACK_MASK[originSq] & ~ocupB;

						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(ocupW, targetSq)) {
								// Add capture knight
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							} else {
								// Add quiet move knight
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;


				case BLACK_BISHOP:

					while(board) {

						originSq = firstBit(board);
						attacks = getBishopMoves(originSq, occupancies[OCCUPANCY]) & ~ocupB;

						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(ocupW, targetSq)) {
								// Add capture bishop
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							} else {
								// Add quiet move bishop
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;


				case BLACK_ROOK:

					while(board) {

						originSq = firstBit(board);
						attacks = getRookMoves(originSq, occupancies[OCCUPANCY]) & ~ocupB;

						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(ocupW, targetSq)) {
								// Add capture rook
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							} else {
								// Add quiet move rook
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;


				case BLACK_QUEEN:

					while(board) {

						originSq = firstBit(board);
						attacks = getQueenMoves(originSq, occupancies[OCCUPANCY]) & ~ocupB;

						while(attacks) {
							targetSq = firstBit(attacks);
							if (getBit(ocupW, targetSq)) {
								// Add capture queen
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 1, 0, 0, 0);
							} else {
								// Add quiet move queen
								moves -> count++;
								moves -> moves[moves -> count - 1] = encodeMove(originSq, targetSq, i, 0, 0, 0, 0, 0);
							}
							popWhile(attacks);
						}
						popWhile(board);
					}
					continue;

				break;
			}
		}
		return;
	}
}
