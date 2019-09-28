#include "functions.h"

//#include <unistd.h>   // sleep
//#include <string.h>
//#include <iostream>
//#include <stdlib.h>   // random
//#include <time.h>     // time

int main(int argc, char *argv[])
{
    initscr();
   
    if (!has_colors()) {
        endwin();
        fprintf(stderr, "Error - no color support on this terminal\n");
        exit(1);
    }
    if (start_color() != OK) {
        endwin();
        fprintf(stderr, "Error - could not initialize colors\n");
        exit(2);
    }

    Dodge game = Dodge();
    game.GAME();

    endwin();
    return 0;
}


