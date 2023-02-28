#include "Files.h"
void initializeAll() {
    initializeEvaluationMasks();
    initializeSliderPieces(1);
    initializeSliderPieces(0);
}

void debug() {
    importFen(startPos);
    drawPosition();

    moveList list[1];
    generateMoves(list);
    return;
}

int main(void) {
    initializeAll();
    debug();
    return 0;
}