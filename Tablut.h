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

// Forward Declaration
class Heuristic;

typedef uint64_t ZobristKey;

typedef int16_t CheckerCodex;
typedef CheckerCodex StructuresCodex;
typedef CheckerCodex WinCodex;

typedef int16_t Pos;
typedef int16_t CheckerCountCodex;

// Table dimensions -> always 9
const Pos DIM(9);
const Pos LAST_ROW(DIM - 1);
const Pos SEC_LAST_ROW(DIM - 2);
const Pos LAST_COL(DIM - 1);
const Pos SEC_LAST_COL(DIM - 2);
const Pos FIRST_ROW(0);
const Pos SECOND_ROW(1);
const Pos FIRST_COL(0);
const Pos SECOND_COL(1);

// Dead king position value
const Pos KDEADPOSITION(20);

// Max _hash log extensions, used to check if position is draw(game state position reached twice)
const int MAX_DRAW_LOG(250);

// Enum for Checker values
enum CHECKER : CheckerCodex
{
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2,
    KING = 3
};

// ENUM FOR _board structures VALUES
enum STRUCTURE : StructuresCodex
{
    NOTHING = 0,
    ESCAPE = 1,
    CAMPS = 2,
    CASTLE = 3
};

// ENUM For _gameState win or lose
enum GAME_STATE : WinCodex
{
    NONE = 0,
    WHITEWIN = 1,
    BLACKWIN = 2,
    WHITEDRAW = 3,
    BLACKDRAW = 4
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
    Tablut(const Tablut &__startFrom);

    // Tells if is White or Black _turn
    bool _isWhiteTurn;

    // New position of the moved piece
    Pos _x;
    Pos _y;

    // Old position of the moved piece
    Pos _oldX;
    Pos _oldY;

    // Total number of white and black checkers alive on _board
    CheckerCountCodex _whiteCount;
    CheckerCountCodex _blackCount;

    // King position during game
    Pos _kingX;
    Pos _kingY;

    // Kills recap in this round
    int _kills;

    // Board game
    Board _board;

    // Turn count
    int _turn;

    // Current table _hash
    ZobristKey _hash;

    // Past turn hashes, used to check if same game state is reached twice
    std::array<ZobristKey, MAX_DRAW_LOG> _pastHashes;
    int _pastHashesIndex;

    // How Much Movements can the king make;
    int _kingMovements;

    // Tell if game is in win or draw state
    GAME_STATE _gameState;

    inline void print()
    {
        std::cout << *this << std::endl;
    }

    friend std::ostream &operator<<(std::ostream &out, const Tablut &__tablut)
    {
        out << std::endl
            << "   ";
        for (int y = 0; y < 9; y++)
        {
            out << " " << y << " ";
        }
        out << std::endl
            << "   ";

        for (int y = 0; y < 9; y++)
        {
            out << "___";
        }
        out << std::endl;

        for (int x = 0; x < 9; x++)
        {
            out << " " << x << "|";
            for (int y = 0; y < 9; y++)
            {
                if (__tablut._board[x][y] == C::KING)
                {
                    if (x == __tablut._x && y == __tablut._y)
                    {
                        out << "\033[1;42m K \033[0m";
                    }
                    else
                    {
                        out << "\033[1;93m K \033[0m";
                    }
                }
                else if (__tablut._board[x][y] == C::BLACK)
                {
                    if (x == __tablut._x && y == __tablut._y)
                    {
                        out << "\033[1;42m B \033[0m";
                    }
                    else if (tablutStructure[x][y] == S::CAMPS)
                    {
                        out << "\033[1;100m B \033[0m";
                    }
                    else if (tablutStructure[x][y] == S::ESCAPE)
                    {
                        out << "\033[1;106m B \033[0m";
                    }
                    else
                    {
                        out << "\033[1;97m B \033[0m";
                    }
                }
                else if (__tablut._board[x][y] == C::WHITE)
                {
                    if (x == __tablut._x && y == __tablut._y)
                    {
                        out << "\033[1;42m W \033[0m";
                    }
                    else if (tablutStructure[x][y] == S::CAMPS)
                    {
                        out << "\033[1;100m W \033[0m";
                    }
                    else if (tablutStructure[x][y] == S::ESCAPE)
                    {
                        out << "\033[1;106m W \033[0m";
                    }
                    else
                    {
                        out << "\033[1;97m W \033[0m";
                    }
                }
                else
                {
                    if (x == __tablut._oldX && y == __tablut._oldY)
                    {
                        out << "\033[1;41m   \033[0m";
                    }
                    else if (tablutStructure[x][y] == S::CAMPS)
                    {
                        out << "\033[1;100m   \033[0m";
                    }
                    else if (tablutStructure[x][y] == S::ESCAPE)
                    {
                        out << "\033[1;106m   \033[0m";
                    }
                    else if (tablutStructure[x][y] == S::CASTLE)
                    {
                        out << "\033[1;103m   \033[0m";
                    }
                    else
                    {
                        out << "   ";
                    }
                }
            }
            out << std::endl;
        }

        out << std::endl
            << "====================" << std::endl;
        out << (__tablut._isWhiteTurn ? "\033[1;47m| WHITE MOVE NOW (BLACK MOVED) |\033[0m" : "\033[1;40m| BLACK MOVE NOW (WHITE MOVED)|\033[0m") << std::endl;
        out << "====================" << std::endl;

        out << "TURN: " << __tablut._turn << std::endl;
        out << "whiteCheckers: " << unsigned(__tablut._whiteCount) << std::endl;
        out << "blackCheckers: " << unsigned(__tablut._blackCount) << std::endl;
        out << "kingPosition: " << int(__tablut._kingX) << "-" << int(__tablut._kingY) << std::endl;
        out << "kills: " << __tablut._kills << std::endl;
        out << "checkerMovedTo: " << int(__tablut._x) << "-" << int(__tablut._y) << std::endl;
        out << "checkerMovedFrom: " << int(__tablut._oldX) << "-" << int(__tablut._oldY) << std::endl;

        return out;
    }

    // Update table by one checker
    Tablut next(const Pos __fromX, const Pos __fromY, const Pos __toX, const Pos __toY);

    // Constructor from json
    static Tablut fromJson(const std::string &__json);

    // Tablut with starting position set
    static Tablut getStartingPosition();

    // Tell if game is Over
    inline bool isGameOver() const
    {
        return _gameState != GAME_STATE::NONE;
    }

    // Tell if someone won, lost or drawed
    inline GAME_STATE checkWinState()
    {
        if (_gameState != GAME_STATE::NONE)
        {
            return _gameState;
        }

        if (checkDraw())
        {
            _gameState = _isWhiteTurn ? GAME_STATE::WHITEDRAW : GAME_STATE::BLACKDRAW;
        }
        else if (_kingX == KDEADPOSITION)
        {
            _gameState = GAME_STATE::BLACKWIN;
        }
        else if (tablutStructure[_kingX][_kingY] == S::ESCAPE)
        {
            _gameState = GAME_STATE::WHITEWIN;
        }

        return _gameState;
    }

    inline bool checkDraw() const
    {
        for (int i = 0; i < _pastHashesIndex - 1; i++)
        {
            if (_hash == _pastHashes[i])
            {
                return true;
            }
        }
        return false;
    }

    inline CHECKER getLeftChecker(Pos __by = 1) const
    {
        return _board[_x][_y - __by];
    }

    inline CHECKER getRightChecker(Pos __by = 1) const
    {
        return _board[_x][_y + __by];
    }

    inline CHECKER getUpChecker(Pos __by = 1) const
    {
        return _board[_x - __by][_y];
    }

    inline CHECKER getDownChecker(Pos __by = 1) const
    {
        return _board[_x + __by][_y];
    }

    inline bool kingIsInThrone() const
    {
        return _kingX == 4 && _kingY == 4;
    }

    inline bool isKingSurrounded() const
    {
        return _board[4][3] == C::BLACK && _board[4][5] == C::BLACK && _board[3][4] == C::BLACK && _board[5][4] == C::BLACK;
    }

    inline bool kingNearThrone() const
    {
        return _board[4][3] == C::KING || _board[4][5] == C::KING || _board[3][4] == C::KING || _board[5][4] == C::KING;
    }

    // Kill checker by checking which type of checker is on the specified point, update checkersCount and killFeed variables
    inline void killChecker(Pos __x, Pos __y)
    {
        CHECKER &target = _board[__x][__y];

        if (target == C::EMPTY)
        {
            return;
        }

        if (target == C::WHITE)
        {
            target = C::EMPTY;
            _whiteCount = _whiteCount - 1U;
            _kills++;
            return;
        }

        if (target == C::BLACK)
        {
            target = C::EMPTY;
            _blackCount = _blackCount - 1U;
            _kills++;
            return;
        }

        if (target == C::KING)
        {
            target = C::EMPTY;
            _kingX = KDEADPOSITION;
            _kingY = KDEADPOSITION;
            _kills++;
        }
    }

    inline bool checkIfKingDead() const
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
            if (_board[_kingX][_kingY - 1] == C::BLACK)
            {
                surroundCount++;
            }
            else if (tablutStructure[_kingX][_kingY - 1] == S::CASTLE)
            {
                castleCounted = true;
            }
            else
            {
                return false;
            }

            // Right CHECK
            if (_board[_kingX][_kingY + 1] == C::BLACK)
            {
                surroundCount++;
            }
            else if (!castleCounted && tablutStructure[_kingX][_kingY + 1] == S::CASTLE)
            {
                castleCounted = true;
            }
            else
            {
                return false;
            }

            // Up CHECK
            if (_board[_kingX - 1][_kingY] == C::BLACK)
            {
                surroundCount++;
            }
            else if (!castleCounted && tablutStructure[_kingX - 1][_kingY] == S::CASTLE)
            {
                castleCounted = true;
            }
            else
            {
                return false;
            }

            // Down CHECK
            if (_board[_kingX + 1][_kingY] == C::BLACK)
            {
                surroundCount++;
            }
            else if (!castleCounted && tablutStructure[_kingX + 1][_kingY] == S::CASTLE)
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
        killChecker(_x, _y - 1);
    }

    inline void killRight()
    {
        killChecker(_x, _y + 1);
    }

    inline void killUp()
    {
        killChecker(_x - 1, _y);
    }

    inline void killDown()
    {
        killChecker(_x + 1, _y);
    }

    inline void switchTurn()
    {
        _isWhiteTurn = !_isWhiteTurn;
    }
};

#endif