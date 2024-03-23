#ifndef TABLUT_H
#define TABLUT_H

#include <iostream>

typedef u_int8_t CheckerCodex;
typedef u_int8_t StructuresCodex;
typedef u_int8_t MoveCodex;
typedef u_int8_t KingPosCodex;
typedef u_int8_t CheckerCountCodex;

// Table dimensions -> always 9
const int DIM(9);

// Dead king position value
const KingPosCodex KDEADPOSITION(255);


// ENUM FOR CHECKERS VALUES
enum CHECKER : CheckerCodex
{
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2,
    KING = 3
};

// ENUM FOR board structures VALUES
enum STRUCTURE : StructuresCodex {
    NOTHING = 0,
    ESCAPE = 1,
    CAMPS = 2,
    CASTLE = 3
};

// ALIASES for structure and checker enum
typedef STRUCTURE S;
typedef CHECKER C;



// Matrix representing all tiles types
/*--0 1 2 3 4 5 6 7 8
 |
 0  - E E C C C E E -
 1  E - - - C - - - E
 2  E - - - - - - - E
 3  C - - - - - - - C
 4  C C - - T - - C C
 5  C - - - - - - - C
 6  E - - - - - - - E
 7  E - - - C - - - E
 8  - E E C C C E E -

*/
const S tablutStructure[DIM][DIM]
{
    {S::NOTHING,S::ESCAPE,S::ESCAPE,S::CAMPS,S::CAMPS,S::CAMPS,S::ESCAPE,S::ESCAPE,S::NOTHING},
    {S::ESCAPE,S::NOTHING,S::NOTHING,S::NOTHING,S::CAMPS,S::NOTHING,S::NOTHING,S::NOTHING,S::ESCAPE},
    {S::ESCAPE,S::NOTHING,S::NOTHING,S::NOTHING,S::NOTHING,S::NOTHING,S::NOTHING,S::NOTHING,S::ESCAPE},
    {S::CAMPS,S::NOTHING,S::NOTHING,S::NOTHING,S::NOTHING,S::NOTHING,S::NOTHING,S::NOTHING,S::CAMPS},
    {S::CAMPS,S::NOTHING,S::NOTHING,S::NOTHING,S::CASTLE,S::NOTHING,S::NOTHING,S::NOTHING,S::CAMPS},
    {S::CAMPS,S::NOTHING,S::NOTHING,S::NOTHING,S::NOTHING,S::NOTHING,S::NOTHING,S::NOTHING,S::CAMPS},
    {S::ESCAPE,S::NOTHING,S::NOTHING,S::NOTHING,S::NOTHING,S::NOTHING,S::NOTHING,S::NOTHING,S::ESCAPE},
    {S::ESCAPE,S::NOTHING,S::NOTHING,S::NOTHING,S::CAMPS,S::NOTHING,S::NOTHING,S::NOTHING,S::ESCAPE},
    {S::NOTHING,S::ESCAPE,S::ESCAPE,S::CAMPS,S::CAMPS,S::CAMPS,S::ESCAPE,S::ESCAPE,S::NOTHING},
};

class Tablut
{
    public:
        Tablut();
        ~Tablut();

        bool isWhiteTurn;
        
        MoveCodex x;
        MoveCodex y;

        CheckerCountCodex whiteCheckersCount;
        CheckerCountCodex blackCheckersCount;

        KingPosCodex kingX;
        KingPosCodex kingY; 

        CHECKER board[DIM][DIM]; // Board game

        void print();

        Tablut next(const MoveCodex from_x, const MoveCodex from_y, 
                            const MoveCodex to_x, const MoveCodex to_y);       // Update table by one checker
        static Tablut fromJson(const std::string &json);                       // Constructor from json
        static Tablut newGame();                                               // Tablut with starting position set


        inline CHECKER * getLeftChecker(MoveCodex by = 1U) {
            return &board[x][y - by];
        }

        inline CHECKER * getRightChecker(MoveCodex by = 1U) {
            return &board[x][y + by];
        }

        inline CHECKER * getUpChecker(MoveCodex by = 1U) {
            return &board[x - by][y];
        }

        inline CHECKER * getDownChecker(MoveCodex by = 1U) {
            return &board[x + by][y];
        }

        inline bool kingIsInThrone() {
            return kingX == 4 && kingY == 4;
        }

        inline bool isKingSurrounded() {
            return board[4][3] == C::BLACK || board[4][5] == C::BLACK || board[3][4] == C::BLACK || board[5][4] == C::BLACK;
        }

        inline bool kingNearThrone() {
            return board[4][3] == C::KING || board[4][5] == C::KING || board[3][4] == C::KING || board[5][4] == C::KING;
        }

        inline void killChecker(CHECKER& c) {
            if (c == C::WHITE) {
                c = C::EMPTY;
                whiteCheckersCount--;
                return;
            }
            if (c == C::BLACK) {
                blackCheckersCount--;
                return;
            }
            if (c == C::KING) {
                kingX = KDEADPOSITION;
                kingY = KDEADPOSITION;
            }
        }

        inline void switchTurn() {
            isWhiteTurn = !isWhiteTurn;
        }
};


#endif