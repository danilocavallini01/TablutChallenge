#pragma once

#ifndef TABLUT_H
#define TABLUT_H

#include "Heuristic.h"

#include <iostream>
#include <cstdint>
#include <array>
#include <map>
#include <stdio.h>
#include <cstring>
#include <chrono>
#include <vector>
#include <deque>

class Heuristic;

typedef uint64_t ZobristKey;

typedef u_int16_t CheckerCodex;
typedef CheckerCodex StructuresCodex;
typedef CheckerCodex WinCodex;

typedef int16_t Pos;
typedef u_int16_t CheckerCountCodex;

// Table dimensions -> always 9
const Pos DIM(9);
const Pos LAST_ROW(DIM - 1);
const Pos SEC_LAST_ROW(DIM - 2);
const Pos LAST_COL(DIM - 1);
const Pos SEC_LAST_COL(DIM - 2);
const Pos FIRST_ROW(1);
const Pos SECOND_ROW(2);
const Pos FIRST_COL(1);
const Pos SECOND_COL(2);

// Dead king position value
const Pos KDEADPOSITION(20);

const int MAX_DRAW_LOG(150);

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

enum WIN : WinCodex
{
    NONE = 0,
    WHITEWIN = 1,
    BLACKWIN = 2,
    DRAW = 3
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
    Tablut(const Tablut &startFrom);

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

    // Kills recap in this round
    int killFeedIndex;

    // Board game
    Board board;

    // Turn count
    int turn;

    // Current table hash
    ZobristKey hash;

    // Past turn hashes, used to check if same game state is reached twice
    std::array<ZobristKey, MAX_DRAW_LOG> pastHashes;
    int pastHashesIndex;

    WIN gameState;

    void print();

    // Update table by one checker
    Tablut next(const Pos from_x, const Pos from_y, const Pos to_x, const Pos to_y);

    // Constructor from json
    static Tablut fromJson(const std::string &json);

    // Tablut with starting position set
    static Tablut newGame();

    inline bool isGameOver() const
    {
        return gameState != WIN::NONE;
    }

    // Tell if someone win or not
    inline WIN checkWinState()
    {
        if (gameState != WIN::NONE)
        {
            return gameState;
        }

        if (checkDraw())
        {
            gameState = WIN::DRAW;
        }
        else if (kingX == KDEADPOSITION)
        {
            gameState = WIN::BLACKWIN;
        }
        else if (tablutStructure[kingX][kingY] == S::ESCAPE)
        {
            gameState = WIN::WHITEWIN;
        }

        return gameState;
    }

    inline bool checkDraw() const
    {
        for (int i = 0; i < pastHashesIndex - 1; i++)
        {
            if (hash == pastHashes[i])
            {
                return true;
            }
        }
        return false;
    }

    inline CHECKER getLeftChecker(Pos by = 1)
    {
        return board[x][y - by];
    }

    inline CHECKER getRightChecker(Pos by = 1)
    {
        return board[x][y + by];
    }

    inline CHECKER getUpChecker(Pos by = 1)
    {
        return board[x - by][y];
    }

    inline CHECKER getDownChecker(Pos by = 1)
    {
        return board[x + by][y];
    }

    inline bool kingIsInThrone()
    {
        return kingX == 4 && kingY == 4;
    }

    inline bool isKingSurrounded()
    {
        return board[4][3] == C::BLACK && board[4][5] == C::BLACK && board[3][4] == C::BLACK && board[5][4] == C::BLACK;
    }

    inline bool kingNearThrone()
    {
        return board[4][3] == C::KING || board[4][5] == C::KING || board[3][4] == C::KING || board[5][4] == C::KING;
    }

    inline void killChecker(Pos x, Pos y)
    {
        CHECKER &target = board[x][y];

        if (target == C::EMPTY)
        {
            return;
        }

        if (target == C::WHITE)
        {
            target = C::EMPTY;
            whiteCheckersCount = whiteCheckersCount - 1U;
            killFeedIndex++;
            return;
        }

        if (target == C::BLACK)
        {
            target = C::EMPTY;
            blackCheckersCount = blackCheckersCount - 1U;
            killFeedIndex++;
            return;
        }

        if (target == C::KING)
        {
            target = C::EMPTY;
            kingX = KDEADPOSITION;
            kingY = KDEADPOSITION;
            killFeedIndex++;
        }
    }

    inline bool checkIfKingDead()
    {
        // KING IN THRONE OR NEAR TRONE EAT

        // King in throne and black surounded throne -> WHITE LOSE
        if (kingIsInThrone() && isKingSurrounded())
            return true;

        if (kingNearThrone())
        {
            // King near throne so can be killed by 3 black checkers surounding him
            u_int8_t surroundCount = 0U; // Number of black defenders surrounding king
            bool castleCounted = false;  // Tell if castle as been counted or not

            // Left CHECK
            if (board[kingX][kingY - 1] == C::BLACK)
            {
                surroundCount++;
            }
            else if (tablutStructure[kingX][kingY - 1] == S::CASTLE)
            {
                castleCounted = true;
            }
            else
            {
                return false;
            }

            // Right CHECK
            if (board[kingX][kingY + 1] == C::BLACK)
            {
                surroundCount++;
            }
            else if (!castleCounted && tablutStructure[kingX][kingY + 1] == S::CASTLE)
            {
                castleCounted = true;
            }
            else
            {
                return false;
            }

            // Up CHECK
            if (board[kingX - 1][kingY] == C::BLACK)
            {
                surroundCount++;
            }
            else if (!castleCounted && tablutStructure[kingX - 1][kingY] == S::CASTLE)
            {
                castleCounted = true;
            }
            else
            {
                return false;
            }

            // Down CHECK
            if (board[kingX + 1][kingY] == C::BLACK)
            {
                surroundCount++;
            }
            else if (!castleCounted && tablutStructure[kingX + 1][kingY] == S::CASTLE)
            {
                castleCounted = true;
            }
            else
            {
                return false;
            }

            return surroundCount == 3 && castleCounted;
        }
        return false;
    }

    inline void killLeft()
    {
        killChecker(x, y - 1);
    }

    inline void killRight()
    {
        killChecker(x, y + 1);
    }

    inline void killUp()
    {
        killChecker(x - 1, y);
    }

    inline void killDown()
    {
        killChecker(x + 1, y);
    }

    inline void switchTurn()
    {
        isWhiteTurn = !isWhiteTurn;
    }
};

#endif