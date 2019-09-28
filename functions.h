#ifndef NHAN
#define NHAN

#include <curses.h>
#include <unistd.h>     // sleep
#include <string.h>
#include <iostream>
#include <stdlib.h>     // random
#include <time.h>       // time
#include <ctime>
#include <ratio>
#include <chrono>       // millisecond
#include <algorithm>    // min/max
#include <queue>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <gdbm.h>       // Database

using namespace std;

// Constants for Intro
#define GAME_NAME "[DODGE]"
#define SLIDE_SP 100000
#define COLOR_RANGE 3

// Common parameters
#define GAP 100000              // (Milliseconds) Init gap between bullets/obstacles
#define AC_GAP 200              // How fast the GAP shrinks
#define LIMIT 10                // Max level of speed = INIT_SPEED / LIMIT = (1 / GAP) / LIMIT

// Constants for PC Mode
#define PC_WID 30
#define LEVEL_SCORE 15
#define NAME_LEN 12
#define NOONE "=============="
#define LB_SIZE 5
#define DATA_FILE "highscores.dat"

// Constants for Human Mode
#define HUM_HEI 15
#define HUM_WID 50
#define BULL_REC 10
#define A_BULLET 500
#define MAX_BULLETS 10

// Modes
#define PC_MODE 1
#define HUM_MODE 2

// COLOR SECTION
#define YELLOW 1
#define CYAN 2
#define GREEN 3
#define RED 4
#define WHITE 5
#define MAGENTA 6
#define BLUE 7

#define GREEN_RED 8
#define CYAN_RED 9

#define BLACK_YELLOW 10
#define BLACK_WHITE 11
#define CYAN_BLUE 12
#define RED_GREEN 13

struct hsRec { string name; int score; };

// MAIN FUNCTION
void GAME();

// Init Functions
void init();

// Printing function
void greet();
void printBorders(int end=-1,  int from=0, int color=YELLOW, bool bold=true);
void printMess(std::string message, int color=-1, int loc=LINES/2, bool bold=false);
void printPlayer();

void printScore();
void printLeaderBoard();

void printWinner();
void printPockets();

// Getting function
void getMode();
int sleepTime();
int getWid();
int getHei();
bool askCont();

void getName();
void getMax();
int score();
struct hsRec getMinPlayer();

// Games
void playPC();
void playHUM();

// Functional functions
void RUN();
void gameOver();

bool dropObjects();

void renderBullets();
void recoverBullets();

// Data base
void upDateDataBase();
void add_to_record(GDBM_FILE db, std::string player, std::string score, struct hsRec worst);
void printLeaderBoard();

#endif
