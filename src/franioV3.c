#include "Files.h"

void initializeAll() {
    initializeEvaluationMasks();
    initializeSliderPieces(1);
    initializeSliderPieces(0);
}

void debug() {
    importFen(tricky);
    //drawPosition();

    moveList list[1];
    int yo = 100000000;
    long start = getTimeMS();

    for (int i = 0; i < yo; i++) {
        generateMoves(list);
    }
    printf("\nMove Gen no optimization time %ldms generations : %ld\n\n", getTimeMS()-start, yo);
    printMoveList(list);

    start = getTimeMS();
    for (int i = 0; i < yo; i++) {
        moveGen(list);
    }
    printf("\nMove Gen switch and splitting loops time %ldms generations : %ld\n\n", getTimeMS()-start, yo);
    printMoveList(list);

    start = getTimeMS();
    for (int i = 0; i < yo; i++) {
        moveGen2(list);
    }
    printf("\nMove Gen switch and splitting loops \nallocating moves manually time %ldms generations : %ld\n\n", getTimeMS()-start, yo);
    printMoveList(list);


    return;
}

int main(void) {
    initializeAll();
    debug();
    return 0;
}