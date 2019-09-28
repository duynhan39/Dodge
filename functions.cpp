#include "functions.h"

//using namespace std;

int MODE;
vector<int> player;
int level;
int gameStat;

queue<int> objLoc;
string playerName;
int maxOfPlayer;

vector<queue<int>> bullets(2);
vector<int> avaiBullets(2);
vector<int> bulletTimer(2);

////////////////////////////////////////////////////////
//                  GAME OUTLINE
////////////////////////////////////////////////////////
void GAME()
{
    refresh();
    init();
    greet();
    
    bool cont=true;
    while(cont)
    {
        getMode();
        switch (MODE)
        {
            case PC_MODE:
                playPC();
                break;
            case HUM_MODE:
                playHUM();
                break;
            default:
                break;
        }
        cont=askCont();
        clear();
    }
    gameOver();
}
////////////////////////////////////////////////////////

void init()
{
    use_default_colors();
    init_pair(1, COLOR_YELLOW, -1);
    init_pair(2, COLOR_CYAN, -1);
    init_pair(3, COLOR_GREEN, -1);
    init_pair(4, COLOR_RED, -1);
    init_pair(5, COLOR_WHITE, -1);
    init_pair(6, COLOR_MAGENTA, -1);
    init_pair(7, COLOR_BLUE, -1);
    
    init_pair(8, COLOR_GREEN, COLOR_RED);
    init_pair(9, COLOR_CYAN, COLOR_RED);
    
    init_pair(10, COLOR_BLACK, COLOR_YELLOW);
    init_pair(11, COLOR_BLACK, COLOR_WHITE);
    init_pair(12, COLOR_CYAN, COLOR_BLUE);
    init_pair(13, COLOR_RED, COLOR_GREEN);
    
    MODE=1;
    gameStat=0;
}

void greet()
{
    char s[COLS];
    chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
    
    // Roll up the screen
    for(int rep=0; rep<=LINES+1; rep++)
    {
        clear();
        printMess(GAME_NAME, YELLOW);
        
        chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
        
        srand(time(NULL));
        int seed = rand()%100 + 1;
        usleep(seed*rep % 300);
        chrono::duration<double, milli> time_span = t2 - t1;
        
        int dens=1;
        for(int y=0; y<=(LINES-rep)/dens; y++)
        {
            for (int x = 0; x <= COLS/dens; x++)
            {
                srand(time_span.count()*1000);
                seed = rand()%100 + 1;
                
                int random = (x+1)*(y+1)*seed;
                while(random%7 == 0)
                {
                    srand(random);
                    seed = rand()%100 + 1;
                    random -= seed;
                }
                if(x*y % 7 == 1)
                    attrset(COLOR_PAIR(random%COLOR_RANGE + 1) | A_BOLD);
                else
                    attrset(COLOR_PAIR(random%COLOR_RANGE + 1));
                srand(random);
                sprintf(s, "%c", '0' + (rand() % 10) );
                
                mvprintw(y*dens, x*dens, s);
            }
        }
        refresh();
        move(0,0);
        usleep(SLIDE_SP*1/3);
    }
    
    // Waiting for user to read the title
    usleep(SLIDE_SP*10);
    
    // Moving the name down along with the borders
    for(int y=0; y<=LINES/2-1; y++)
    {
        printMess(GAME_NAME, y%7+1, LINES/2+y, y%2==0);
        printBorders(y, 0, y%7+1, y%2==0);
        
        usleep(SLIDE_SP*3/2);
        refresh();
        clear();
    }
    
    // Finishing the rest of the border
    for(int y=0; y<=LINES/2-1; y++)
    {
        printBorders(y+LINES/2, 0, y%7+1, y%2==0);
        usleep(SLIDE_SP*3/2);
        refresh();
        clear();
    }
}

void printBorders(int end, int from, int color, bool bold)
{
    if(end==-1)
        printBorders( (LINES+getHei())/2+1, (LINES-getHei())/2, MODE);
    
    bold && color!=-1 ? attrset(COLOR_PAIR(color) | A_BOLD) : attrset(COLOR_PAIR(color));
    
    for(int y=max(from, 0); y<min(end, LINES); y++)
    {
        move(y, (COLS-getWid())/2-2);
        printw("||");
        move(y, (COLS+getWid())/2+1);
        printw("||");
        move(0,0);
    }
    refresh();
}

void printScore()
{
    attrset(COLOR_PAIR(GREEN));
    string message = "SCORE: " + to_string(score());
    move(2, (COLS-getWid())/2 - 15);
    printw("%s", message.c_str());
    move(0,0);
    refresh();
}

void printPockets()
{
    for(int i=0; i<2; i++)
    {
        avaiBullets[i] == MAX_BULLETS ? attrset(COLOR_PAIR(i+3) | A_BOLD) : attrset(COLOR_PAIR(i+3));
        move((LINES + (1-2*i)*getHei())/2 - (1-2*i)*(MAX_BULLETS), (COLS + (1-2*i)*getWid())/2 + (1-2*i)*4 );
        printw("-");
        move(0,0);
        for(int bull=0; bull<avaiBullets[i]; bull++)
        {
            move((LINES + (1-2*i)*getHei())/2 - (1-2*i)*bull, (COLS + (1-2*i)*getWid())/2 + (1-2*i)*4 );
            i == 0 ? printw("A") : printw("V");
            move(0,0);
        }
    }
    refresh();
}

void printPlayer()
{
    switch (MODE) {
        case PC_MODE:
            attrset(COLOR_PAIR(CYAN));
            move(LINES-1, player[0]);
            printw("@");
            /////
            printScore();
            break;
        case HUM_MODE:
            // Player 1 (bottom)
            attrset(COLOR_PAIR(GREEN));
            move((LINES+getHei())/2, player[0]);
            printw("X");
            // Player 2 (top)
            attrset(COLOR_PAIR(RED));
            move((LINES-getHei())/2, player[1]);
            printw("Y");
            /////
            printPockets();
            break;
        default:
            break;
    }
    printBorders();
    refresh();
}

void printMess(string message, int color, int loc, bool bold)
{
    if(color != -1) bold ? attrset(COLOR_PAIR(color) | A_BOLD) : attrset(COLOR_PAIR(color));
    
    if (loc == -1) loc = LINES/2;
    
    move(loc, (COLS+1-message.size())/2);
    printw("%s", message.c_str());
    
    move(0,0);
    refresh();
}

void printWinner()
{
    string s;
    int color=CYAN;
    switch (gameStat) {
        case 1:
            s = "GREEN WINS!";
            color = GREEN;
            break;
        case 2:
            s = "RED WINS!";
            color = RED;
            break;
        case 3:
            s = "YOU FOOLS BOTH DIED!";
            break;
        default:
            s = "HA, ANOTHER QUITER";
            break;
    }
    string header = "++++++++++++++++++++";
    for(int i=0; i<21; i++)
    {
        printMess(header, color, LINES/2-1, i%2==0);
        printMess(s, color);
        printMess(header, color, LINES/2+1, i%2==0);
        usleep(100000);
    }
    sleep(1);
}

void getMode()
{
    vector<string> options;
    options.push_back("PC's mind");
    options.push_back("Another mortal");
    
    // Taking user input
    flushinp();
    nodelay(stdscr, FALSE);
    keypad(stdscr, TRUE);
    int key;
    do {
        clear();
        printBorders();
        printMess("Young man (or woman)", MODE);
        printMess("pick your opponent", MODE, LINES/2+1);
        
        for(int op=0; op<2; op++) {
            printMess(options[op], MODE, LINES/2+op+3, op+1 == MODE);
        }
        
        key = getch();
        if(key == KEY_UP)
            MODE = max(PC_MODE, MODE-1);
        else if(key == KEY_DOWN)
            MODE = min(HUM_MODE, MODE+1);
    } while(key != '\n');
}

bool askCont()
{
    vector<string> options;
    options.push_back("YES");
    options.push_back("NO!");
    int choice=0;
    
    // Taking user input
    flushinp();
    nodelay(stdscr, FALSE);
    keypad(stdscr, TRUE);
    int key;
    do {
        clear();
        printMess("--Another round?--", choice+3);
        for(int op=0; op<2; op++) {
            printMess(options[op], choice+3, LINES/2+op+1, op == choice);
        }
        key = getch();
        
        if(key == KEY_UP)
            choice = max(0, choice-1);
        else if(key == KEY_DOWN)
            choice = min(1, choice+1);
    } while(key != '\n');
    
    return choice == 0;
}

void getName()
{
    flushinp();
    nodelay(stdscr, FALSE);
    echo();
    
    char name[NAME_LEN+1];
    printMess("[Tell me your name, mortal]", BLUE); // it's actualy purple @@
    move(LINES/2+1, (COLS-NAME_LEN)/2);
    getnstr(name, NAME_LEN);
    
    string nameStr(name);
    
    playerName = nameStr == "" ? "A random f*k" : nameStr;
}

void getMax()
{
    GDBM_FILE f;
    datum pName, maxScore;
    
    char *nameR = new char[playerName.length() + 1];
    strcpy(nameR, playerName.c_str());
    
    pName.dptr = nameR;
    pName.dsize = strlen(nameR);
    
    f = gdbm_open(DATA_FILE, 512, GDBM_WRCREAT, 0666, 0);
    maxScore = gdbm_fetch(f, pName);
    
    maxScore.dptr == NULL ? maxOfPlayer = -1 : maxOfPlayer = atoi(maxScore.dptr);
    
    gdbm_close(f);
}

int score() { return max(0,level)/LEVEL_SCORE; }

int getWid()
{
    switch (MODE) {
        case PC_MODE:
            return PC_WID;
        case HUM_MODE:
            return HUM_WID;
        default:
            return 0;
    }
}

int getHei()
{
    switch (MODE) {
        case PC_MODE:
            return LINES;
        case HUM_MODE:
            return HUM_HEI;
        default:
            return 0;
    }
}

/////////////////////////
void playPC()
{
    // Init Game Data
    player.clear();
    player.push_back(COLS/2);
    level = -LINES+1;
    
    queue<int> empty;
    swap(objLoc, empty);
    //......................
    
    clear();
    printBorders();
    getName();
    getMax();
    
    clear();
    printMess("Only move", YELLOW);
    printMess("if you are truely prepared", YELLOW, LINES/2+1);
    printMess("LEFT || RIGHT", YELLOW, LINES/2+3);
    printMess("['p' to quit]", YELLOW, LINES/2+4);
    
    printPlayer();
    
    // Game On!
    RUN();
    
    // Game Over
    clear();
    printMess("YOUR SCORE: " + to_string(score()), YELLOW);
    sleep(1);
    
    clear();
    
    if(score() > maxOfPlayer)
        upDateDataBase();
    
    printLeaderBoard();
}

/////////////////////////
void playHUM()
{
    // Init game data
    player.clear();
    player.push_back(COLS/2);
    player.push_back(COLS/2);
    
    level = 0;
    
    vector<queue<int>> empty(2);
    swap(bullets, empty);
    
    vector<int> fullBullets(2, 10);
    swap(avaiBullets, fullBullets);
    
    vector<int> emptyTimer(2, 0);
    swap(bulletTimer, emptyTimer);
    
    clear();
    
    printMess("READY?", MODE);
    printMess("<=, => and ^", GREEN, LINES/2+1);
    printMess("a, d, and w", RED, LINES/2+2);
    printMess("['p' to quit]", MODE, LINES/2+3);
    printPlayer();
    
    // Game ON!
    RUN();
    
    clear();
    printWinner();
}

void RUN()
{
    flushinp();
    noecho();
    keypad(stdscr, TRUE);
    int key;
    gameStat=0;
    
    key = getch();
    while(key != 'p' && gameStat==0)
    {
        nodelay(stdscr, TRUE);
        clear();
        printPlayer();
        
        if(key == KEY_RIGHT)
            player[0] = min(player[0]+1, (COLS+getWid())/2);
        else if(key == KEY_LEFT)
            player[0] = max(player[0]-1, (COLS-getWid())/2);
        
        if(MODE==PC_MODE)
        {
            //............
            dropObjects() ? gameStat=0 : gameStat=1;
        } else if(MODE==HUM_MODE)
        {
            switch (key) {
                case 'd':
                    player[1] = min(player[1]+1, (COLS+getWid())/2);
                    break;
                case 'a':
                    player[1] = max(player[1]-1, (COLS-getWid())/2);
                    break;
                case KEY_UP:
                    if(avaiBullets[0]>0) {
                        bullets[0].push(player[0]);
                        avaiBullets[0]--;
                    } else {
                        bullets[0].push(-1);
                    }
                    break;
                case 'w':
                    if(avaiBullets[1]>0) {
                        bullets[1].push(player[1]);
                        avaiBullets[1]--;
                    } else {
                        bullets[1].push(-1);
                    }
                    break;
                default:
                    break;
            }
            if(key != KEY_UP)
                bullets[0].push(-1);
            if(key != 'w')
                bullets[1].push(-1);
        
            renderBullets();
            recoverBullets();
        }
        key = getch();
        level++;
    }
    sleep(1);
}

bool dropObjects()
{
    bool gameOn=true;
    srand( time(NULL)*(level/5) );
    objLoc.push(rand() % (getWid()+1));
    attrset(COLOR_PAIR(CYAN_RED));
    
    while(objLoc.size() >= getHei())
        objLoc.pop();
    
    queue<int> temQue = objLoc;
    if(temQue.size() == getHei()-1 && (COLS-getWid())/2 + temQue.front() == player[0])
    {
        move(temQue.size(), player[0]);
        printw("@");
        gameOn=false;
        temQue.pop();
        move(0,0);
    }
    while(!temQue.empty())
    {
        move(temQue.size(), (COLS-getWid())/2 + temQue.front());
        printw(" ");
        temQue.pop();
        move(0,0);
    }
    refresh();
    usleep(sleepTime());
    
    return gameOn;
}

void renderBullets()
{
    gameStat=0;
    for(int i=0; i<2; i++)
    {
        while(bullets[i].size() > getHei())
            bullets[i].pop();
        
        // Marking bullet
        queue<int> temQue = bullets[i];
        
        if(temQue.front()!=-1 && temQue.size()==getHei() && temQue.front()==player[1-i])
        {
            move( (LINES + (1-2*i)*getHei())/2 - (1-2*i)*temQue.size() , temQue.front() );
            i==0 ? attrset(COLOR_PAIR(RED_GREEN) | A_BOLD) : attrset(COLOR_PAIR(GREEN_RED) | A_BOLD);
            i==0 ? printw("Y") : printw("X");
            temQue.pop();
            move(0,0);
            gameStat+=i+1;
        }
        
        while(!temQue.empty())
        {
            if(temQue.front()!=-1)
            {
                move( (LINES + (1-2*i)*getHei())/2 - (1-2*i)*temQue.size() , temQue.front() );
                i == 0 ? attrset(COLOR_PAIR(RED_GREEN)) : attrset(COLOR_PAIR(GREEN_RED));
                printw(" ");
                move(0,0);
            }
            temQue.pop();
        }
    }
    refresh();
    usleep(sleepTime());
}

void recoverBullets(){
    for(int i=0; i<2; i++)
    {
        if( (level-bulletTimer[i])*sleepTime()/1000 > A_BULLET)
        {
            bulletTimer[i]=level;
            avaiBullets[i] = min(MAX_BULLETS, avaiBullets[i]+1);
        }
    }
}

int sleepTime() { return level >= AC_GAP*(LIMIT-1) ? GAP/LIMIT : AC_GAP*GAP/(AC_GAP+max(0, level)); }

void gameOver()
{
    clear();
    printMess("[Goodbye]", YELLOW);
    sleep(1);
    
    clear();
    endwin();
}

void upDateDataBase()
{
    struct hsRec worst = getMinPlayer();
    
    GDBM_FILE f;
    f = gdbm_open(DATA_FILE, 512, GDBM_WRCREAT, 0666, 0);
    add_to_record(f, playerName, to_string(score()), worst);
    
    gdbm_close(f);
}

void add_to_record(GDBM_FILE db, string player, string curScore, struct hsRec worst)
{
    if(worst.score < score())
    {
        /////
        if(maxOfPlayer == -1) {
            datum worstPl;
            char *nameR = new char[worst.name.length() + 1];
            strcpy(nameR, worst.name.c_str());
            worstPl.dptr = nameR;
            worstPl.dsize = worst.name.length()+1;
            gdbm_delete(db, worstPl);
        }
        /////
        
        datum name, data; // Key and value (name and score).
        
        char *nameR = new char[player.length() + 1];
        strcpy(nameR, player.c_str());
        
        char *scoreR = new char[curScore.length() +1];
        strcpy(scoreR, curScore.c_str());
        
        // Adding the player name and his max score
        name.dptr = nameR;
        name.dsize = strlen(nameR);
        
        data.dptr = scoreR;
        data.dsize = strlen(scoreR);
        
        // Storing
        gdbm_store(db, name, data, GDBM_REPLACE);
        
        delete [] nameR;
        delete [] scoreR;
    }
}

bool compareRec(const hsRec &a, const hsRec &b) { return a.score > b.score; }

struct hsRec getMinPlayer()
{
    GDBM_FILE f;
    datum current_key, current_data;
    f = gdbm_open(DATA_FILE, 512, GDBM_WRCREAT, 0666, 0);
    
    vector<hsRec> db;
    current_key = gdbm_firstkey(f);
    while(current_key.dptr != NULL)
    {
        // Fetching data out of database
        current_data = gdbm_fetch(f, current_key);
        
        if(current_data.dptr != NULL)
        {
            struct hsRec curr;
            curr.name = current_key.dptr;
            curr.score = atoi(current_data.dptr);
            
            db.push_back(curr);
        }
        current_key = gdbm_nextkey(f, current_key);
    }
    
    // Taking top scores
    sort(db.begin(), db.end(), compareRec);
    
    gdbm_close(f);
    struct hsRec noone;
    noone.name = NOONE;
    noone.score = -1;
    return db.size() > LB_SIZE ? db[db.size()-1] : noone;
}

void printLeaderBoard()
{
    GDBM_FILE f;
    datum current_key, current_data;
    f = gdbm_open(DATA_FILE, 512, GDBM_WRCREAT, 0666, 0);
    
    vector<hsRec> db;
    current_key = gdbm_firstkey(f);
    while(current_key.dptr != NULL)
    {
        // Fetching data out of database
        current_data = gdbm_fetch(f, current_key);
        
        if(current_data.dptr != NULL)
        {
            struct hsRec curr;
            curr.name = current_key.dptr;
            curr.score = atoi(current_data.dptr);
            
            db.push_back(curr);
        }
        current_key = gdbm_nextkey(f, current_key);
    }
    
    // Taking top scores
    sort(db.begin(), db.end(), compareRec);
    
    printMess("===Leader Board===");
    
    int listSize = min(LB_SIZE, (int)db.size() );
    for(int i=listSize-1; i>=0; i--)
    {
        string line(NAME_LEN-db[i].name.size()+6-to_string(db[i].score).size(), ' ');
        line = db[i].name + line + to_string(db[i].score);
        
        printMess(line, YELLOW, LINES/2+i+1);
        sleep(1);
    }
    
    refresh();
    
    noecho();
    keypad(stdscr, TRUE);
    int key = 1;
    while(key != '\n')
    {
        key = getch();
    }
    
    gdbm_close(f);
}
