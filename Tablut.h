#ifndef TABLUT_H
#define TABLUT_H

#include <iostream>
#include <array>
#include <map>

typedef u_int8_t CheckerCodex;
typedef CheckerCodex StructuresCodex;

typedef int8_t Pos;
typedef u_int8_t CheckerCountCodex;

// Table dimensions -> always 9
const Pos DIM(9);

// Dead king position value
const Pos KDEADPOSITION(20);

// ENUM FOR CHECKERS VALUES
enum CHECKER : CheckerCodex
{
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2,
    KING = 3
};

// ENUM FOR board structures VALUES
enum STRUCTURE : StructuresCodex
{
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
typedef std::array<std::array<STRUCTURE, DIM>, DIM> TablutStructure;
typedef std::array<std::array<CHECKER, DIM>, DIM> Board;

const TablutStructure tablutStructure{{
    {S::NOTHING, S::ESCAPE, S::ESCAPE, S::CAMPS, S::CAMPS, S::CAMPS, S::ESCAPE, S::ESCAPE, S::NOTHING},
    {S::ESCAPE, S::NOTHING, S::NOTHING, S::NOTHING, S::CAMPS, S::NOTHING, S::NOTHING, S::NOTHING, S::ESCAPE},
    {S::ESCAPE, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::ESCAPE},
    {S::CAMPS, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::CAMPS},
    {S::CAMPS, S::CAMPS, S::NOTHING, S::NOTHING, S::CASTLE, S::NOTHING, S::NOTHING, S::CAMPS, S::CAMPS},
    {S::CAMPS, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::CAMPS},
    {S::ESCAPE, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::ESCAPE},
    {S::ESCAPE, S::NOTHING, S::NOTHING, S::NOTHING, S::CAMPS, S::NOTHING, S::NOTHING, S::NOTHING, S::ESCAPE},
    {S::NOTHING, S::ESCAPE, S::ESCAPE, S::CAMPS, S::CAMPS, S::CAMPS, S::ESCAPE, S::ESCAPE, S::NOTHING},
}};

class Tablut
{
public:
    Tablut();
    ~Tablut();
    Tablut(const Tablut& copy);

    // Tells if is White or Black turn
    bool isWhiteTurn;

    // New position of the moved piece
    Pos x;
    Pos y;

    // Old position of the moved piece
    Pos old_x;
    Pos old_y;

    // Total number of white and black checkers alive on board
    CheckerCountCodex whiteCheckersCount;
    CheckerCountCodex blackCheckersCount;

    // King position during game
    Pos kingX;
    Pos kingY;

    // Board game
    Board board;

    void print();

    // Update table by one checker
    Tablut next(const Pos from_x, const Pos from_y, const Pos to_x, const Pos to_y);

    static Tablut fromJson(const std::string &json); // Constructor from json
    static Tablut newGame();                         // Tablut with starting position set

    inline CHECKER *getLeftChecker(Pos by = 1U)
    {
        return &board[x][y - by];
    }

    inline CHECKER *getRightChecker(Pos by = 1U)
    {
        return &board[x][y + by];
    }

    inline CHECKER *getUpChecker(Pos by = 1U)
    {
        return &board[x - by][y];
    }

    inline CHECKER *getDownChecker(Pos by = 1U)
    {
        return &board[x + by][y];
    }

    inline bool kingIsInThrone()
    {
        return kingX == 4 && kingY == 4;
    }

    inline bool isKingSurrounded()
    {
        return board[4][3] == C::BLACK || board[4][5] == C::BLACK || board[3][4] == C::BLACK || board[5][4] == C::BLACK;
    }

    inline bool kingNearThrone()
    {
        return board[4][3] == C::KING || board[4][5] == C::KING || board[3][4] == C::KING || board[5][4] == C::KING;
    }

    inline void killChecker(CHECKER &c)
    {
        if (c == C::WHITE)
        {
            c = C::EMPTY;
            whiteCheckersCount--;
            return;
        }
        if (c == C::BLACK)
        {
            blackCheckersCount--;
            return;
        }
        if (c == C::KING)
        {
            kingX = KDEADPOSITION;
            kingY = KDEADPOSITION;
        }
    }

    inline void switchTurn()
    {
        isWhiteTurn = !isWhiteTurn;
    }
};

#endif