#include "Tablut.h"

#include <stdio.h>
#include <iostream>
#include <array>

Tablut::Tablut() {}
Tablut::~Tablut() {}

Tablut::Tablut(const Tablut &copy)
{
    isWhiteTurn = copy.isWhiteTurn;

    whiteCheckersCount = copy.whiteCheckersCount;
    blackCheckersCount = copy.blackCheckersCount;

    x = copy.x;
    y = copy.y;

    old_x = copy.old_x;
    old_y = copy.old_y;

    kingX = copy.kingX;
    kingY = copy.kingY;

    for (Pos x = 0; x < 9; x++)
    {
        for (Pos y = 0; y < 9; y++)
        {
            board[x][y] = copy.board[x][y];
        }
    }
}

Tablut Tablut::newGame()
{
    Tablut t;

    for (Pos x = 0; x < 9; x++)
    {
        for (Pos y = 0; y < 9; y++)
        {
            t.board[x][y] = CHECKER::EMPTY;
        }
    }

    t.isWhiteTurn = true;

    t.whiteCheckersCount = 8U;
    t.blackCheckersCount = 16U;

    t.x = 0U;
    t.y = 0U;

    t.old_x = 0U;
    t.old_y = 0U;

    t.kingX = 4U;
    t.kingY = 4U;

    // Setting all black defenders
    t.board[0][3] = C::BLACK;
    t.board[0][4] = C::BLACK;
    t.board[0][5] = C::BLACK;
    t.board[1][4] = C::BLACK;
    t.board[3][0] = C::BLACK;
    t.board[4][0] = C::BLACK;
    t.board[5][0] = C::BLACK;
    t.board[4][1] = C::BLACK;
    t.board[8][3] = C::BLACK;
    t.board[8][4] = C::BLACK;
    t.board[8][5] = C::BLACK;
    t.board[7][4] = C::BLACK;
    t.board[3][8] = C::BLACK;
    t.board[4][8] = C::BLACK;
    t.board[5][8] = C::BLACK;
    t.board[4][7] = C::BLACK;

    // Setting up white attackers
    t.board[4][2] = C::WHITE;
    t.board[4][3] = C::WHITE;
    t.board[4][5] = C::WHITE;
    t.board[4][6] = C::WHITE;
    t.board[2][4] = C::WHITE;
    t.board[3][4] = C::WHITE;
    t.board[5][4] = C::WHITE;
    t.board[6][4] = C::WHITE;

    // Setting Throne
    t.board[4][4] = C::KING;

    return t;
}

void Tablut::print()
{
    std::cout << std::endl
              << "   ";
    for (int y = 0; y < 9; y++)
    {
        if (y == Tablut::y)
        {
            std::cout << " X ";
        }
        else
        {
            std::cout << " " << y << " ";
        }
    }
    std::cout << std::endl
              << "   ";

    for (int y = 0; y < 9; y++)
    {
        std::cout << "___";
    }
    std::cout << std::endl;

    for (int x = 0; x < 9; x++)
    {
        std::cout << " " << x << "|";
        for (int y = 0; y < 9; y++)
        {
            if (board[x][y] == C::KING)
            {
                if (x == Tablut::x && y == Tablut::y)
                {
                    std::cout << "\033[1;42m K \033[0m";
                } else {
                    std::cout << " B ";
                }
            }
            else if (board[x][y] == C::BLACK)
            {
                if (x == Tablut::x && y == Tablut::y)
                {
                    std::cout << "\033[1;42m B \033[0m";
                } else {
                    std::cout << " B ";
                }
            }
            else if (board[x][y] == C::WHITE)
            {
                if (x == Tablut::x && y == Tablut::y)
                {
                    std::cout << "\033[1;42m W \033[0m";
                }
                else
                {
                    std::cout << " W ";
                }
            }
            else
            {
                if ( x == Tablut::old_x && y == Tablut::old_y ) {
                    std::cout << "\033[1;41m - \033[0m";
                }
                else
                {
                    std::cout << " - ";
                }
            }
        }
        std::cout << std::endl;
    }
    std::cout << (isWhiteTurn ? "BLACK MOVED -> NOW WHITE MOVE" : "WHITE MOVED -> NOW BLACK MOVE") << std::endl;
    std::cout << "whiteCheckers: " << unsigned(whiteCheckersCount) << std::endl;
    std::cout << "blackPosition: " << unsigned(blackCheckersCount) << std::endl;
    std::cout << "kingPosition: " << unsigned(kingX) << "-" << unsigned(kingY) << std::endl;
    std::cout << "checkerMovedTo: " << unsigned(x) << "-" << unsigned(y) << std::endl;
    std::cout << "checkerMovedFrom: " << unsigned(old_x) << "-" << unsigned(old_y) << std::endl;
}

Tablut Tablut::next(const Pos from_x, const Pos from_y, const Pos to_x, const Pos to_y)
{
    Tablut next = *this;

    next.board[to_x][to_y] = next.board[from_x][from_y]; // Update checker position
    next.board[from_x][from_y] = C::EMPTY;               // Remove checker from its past position

    // Update old and new positions of the moved pieces
    next.old_x = from_x;
    next.old_y = from_y;

    next.x = to_x;
    next.y = to_y;

    CHECKER *leftChecker = next.getLeftChecker();
    CHECKER *rightChecker = next.getRightChecker();
    CHECKER *upChecker = next.getUpChecker();
    CHECKER *downChecker = next.getDownChecker();

    // WHITE TURN
    if (next.isWhiteTurn)
    {
        // UPDATE KING POSITION
        if (next.board[x][y] == C::KING)
        {
            next.kingX = x;
            next.kingY = y;
        }

        // LEFT eat
        if (*leftChecker == C::BLACK && *next.getLeftChecker(2U) == C::WHITE)
            killChecker(*leftChecker);

        // RIGHT eat
        if (*rightChecker == C::BLACK && *next.getRightChecker(2U) == C::WHITE)
            killChecker(*rightChecker);

        // UP eat
        if (*upChecker == C::BLACK && *next.getUpChecker(2U) == C::WHITE)
            killChecker(*upChecker);

        // DOWN eat
        if (*downChecker == C::BLACK && *next.getDownChecker(2U) == C::WHITE)
            killChecker(*downChecker);
    }

    // BLACK TURN
    else
    {
        // KING IN THRONE OR NEAR TRONE EAT

        // King in throne and black surounded throne -> WHITE LOSE
        if (next.kingIsInThrone() && next.isKingSurrounded())
        {
            next.killChecker(next.board[4][4]);
        }

        // King near throne so can be killed by 3 black checkers surounding him
        if (next.kingNearThrone())
        {

            u_int8_t surroundCount = 0U; // Number of black defenders surrounding king
            bool castleCounted = false;  // Tell if castle as been counted or not
            bool kill = true;            // Tell if kill requirement hasn't been satisfied

            // Left CHECK
            if (next.board[next.kingX][next.kingY - 1U] == C::BLACK)
            {
                surroundCount++;
            }
            else if (tablutStructure[next.kingX][next.kingY - 1U] == S::CASTLE)
            {
                castleCounted = true;
            }
            else
            {
                kill = false;
            }

            // Right CHECK
            if (kill && next.board[next.kingX][next.kingY + 1U] == C::BLACK)
            {
                surroundCount++;
            }
            else if (!castleCounted && tablutStructure[next.kingX][next.kingY + 1U] == S::CASTLE)
            {
                castleCounted = true;
            }
            else
            {
                kill = false;
            }

            // Up CHECK
            if (kill && next.board[next.kingX - 1U][next.kingY] == C::BLACK)
            {
                surroundCount++;
            }
            else if (!castleCounted && tablutStructure[next.kingX - 1U][next.kingY] == S::CASTLE)
            {
                castleCounted = true;
            }
            else
            {
                kill = false;
            }

            // Down CHECK
            if (kill && next.board[next.kingX + 1U][next.kingY] == C::BLACK)
            {
                surroundCount++;
            }
            else if (!castleCounted && tablutStructure[next.kingX + 1U][next.kingY] == S::CASTLE)
            {
                castleCounted = true;
            }
            else
            {
                kill = false;
            }

            if (kill && surroundCount == 3 && castleCounted)
            {
                next.killChecker(next.board[next.kingX][next.kingY]);
            }
        }

        // NORMAL EATS

        // LEFT eat ( KING OR SOLDIER ( > 1))
        if (*leftChecker > 1)
        {
            STRUCTURE oppositeStructure = tablutStructure[to_x][to_y - 2];

            // eat if nor   l eat (black on opposite side) or theres a camp on the opposite side  @see STRUCTURE
            if (*next.getLeftChecker(2U) == C::BLACK || oppositeStructure == S::CAMPS)
            {
                next.killChecker(*leftChecker);
            }
            // eat if white checker and there's a camp on the oppositeSide
            else if (*leftChecker == C::WHITE && oppositeStructure == S::CAMPS)
            {
                next.killChecker(*leftChecker);
            }
        }

        // RIGHT eat
        if (*rightChecker > 1)
        {
            STRUCTURE oppositeStructure = tablutStructure[to_x][to_y + 2];

            // eat if normal eat (black on opposite side) or theres a camp on the opposite side  @see STRUCTURE
            if (*next.getRightChecker(2U) == C::BLACK || oppositeStructure == S::CAMPS)
            {
                next.killChecker(*rightChecker);
            }
            // eat if white checker and there's a camp on the oppositeSide
            else if (*rightChecker == C::WHITE && oppositeStructure == S::CAMPS)
            {
                next.killChecker(*rightChecker);
            }
        }

        // UP eat
        if (*upChecker > 1)
        {
            STRUCTURE oppositeStructure = tablutStructure[to_x - 2][to_y];

            // eat if normal eat (black on opposite side) or theres a camp on the opposite side  @see STRUCTURE
            if (*next.getUpChecker(2U) == C::BLACK || oppositeStructure == S::CAMPS)
            {
                next.killChecker(*upChecker);
            }
            // eat if white checker and there's a camp on the oppositeSide
            else if (*upChecker == C::WHITE && oppositeStructure == S::CAMPS)
            {
                next.killChecker(*upChecker);
            }
        }

        // DOWN eat
        if (*downChecker > 1)
        {
            STRUCTURE oppositeStructure = tablutStructure[to_x + 2][to_y];

            // eat if normal eat (black on opposite side) or theres a camp on the opposite side  @see STRUCTURE
            if (*next.getUpChecker(2) == C::BLACK || oppositeStructure == S::CAMPS)
            {
                next.killChecker(*downChecker);
            }
            // eat if white checker and there's a camp on the oppositeSide
            else if (*upChecker == C::WHITE && oppositeStructure == S::CAMPS)
            {
                next.killChecker(*downChecker);
            }
        }
    }

    next.switchTurn();

    return next;
}