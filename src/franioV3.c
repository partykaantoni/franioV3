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
    long start = getTimeMS();
    int gens[7] =    {100,      1000,        10000,          100000,             1000000,  10000000,     100000000};
    char* names[7] = {"Hundred", "Thousand", "Ten Thousand", "Hundred Thousand", "Milion", "Ten Milion", "Hundred Milion"};

    for (int j = 0; j < 7; j++) {
        printf("\n\nGENERATIONS %d %s\n", gens[j], names[j]);
        for (int i = 0; i < gens[j]; i++) {
            generateMoves(list);
        }
        printf("\nMove Gen no optimization \ntime %ldms\n", getTimeMS()-start);

        start = getTimeMS();
        for (int i = 0; i < gens[j]; i++) {
            moveGen(list);
        }
        printf("\nMove Gen switch and splitting loops \ntime %ldms\n", getTimeMS()-start);

        start = getTimeMS();
        for (int i = 0; i < gens[j]; i++) {
            moveGen2(list);
        }
        printf("\nMove Gen switch and splitting loops \nallocating moves manually \ntime %ldms\n", getTimeMS()-start);

        start = getTimeMS();
        for (int i = 0; i < gens[j]; i++) {
            moveGen3(list);
        }
        printf("\nMove Gen switch and splitting loops \nallocating moves manually\noccupancies in variables\ntime %ldms\n", getTimeMS()-start);
        nl();

    }

    return;
}

int main(void) {
    initializeAll();
    debug();
    return 0;
}