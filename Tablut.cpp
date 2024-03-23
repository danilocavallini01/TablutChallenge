
#include "Tablut.h"

#include <stdio.h>
#include <iostream>

Tablut::Tablut()
{
    // Setting all empty spaces on gameBoard
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 9; y++)
        {
            board[x][y] = CHECKER::EMPTY;
        }
    }
}

Tablut::~Tablut() {}

Tablut Tablut::newGame()
{
    Tablut t;

    t.isWhiteTurn = true;

    t.whiteCheckersCount = 8U;
    t.blackCheckersCount = 16U;

    t.kingX = 4U;
    t.kingY = 4U;

    // Setting all black defenders
    t.board[0][1] = C::BLACK;
    t.board[0][2] = C::BLACK;
    t.board[0][6] = C::BLACK;
    t.board[0][7] = C::BLACK;
    t.board[1][0] = C::BLACK;
    t.board[2][0] = C::BLACK;
    t.board[6][0] = C::BLACK;
    t.board[7][0] = C::BLACK;
    t.board[1][8] = C::BLACK;
    t.board[2][8] = C::BLACK;
    t.board[6][8] = C::BLACK;
    t.board[7][8] = C::BLACK;
    t.board[8][1] = C::BLACK;
    t.board[8][2] = C::BLACK;
    t.board[8][6] = C::BLACK;
    t.board[8][7] = C::BLACK;

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
    std::cout << std::endl << "   ";
    for (int y = 0; y < 9; y++)
    {
        std::cout << " " << y << " ";
    }
    std::cout << std::endl << "   ";
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
                std::cout << " K ";
            }
            else if (board[x][y] == C::BLACK)
            {
                std::cout << " B ";
            }
            else if (board[x][y] == C::WHITE)
            {
                std::cout << " W ";
            }
            else
            {
                std::cout << " - ";
            }
        }
        std::cout << std::endl;
    }

    std::cout << "whiteCheckers: " << whiteCheckersCount << std::endl;
    std::cout << "blackPosition: " << blackCheckersCount << std::endl;
    std::cout << "kingPosition: " << kingX << "-" << kingY << std::endl;
    std::cout << "checkerMovedTo: " << x << "-" << y << std::endl;
}

Tablut Tablut::next(const MoveCodex from_x, const MoveCodex from_y, const MoveCodex to_x, const MoveCodex to_y)
{
    Tablut next = *this;

    next.board[to_x][to_y] = next.board[from_x][from_y]; // Update checker position
    next.board[from_x][from_y] = C::EMPTY;               // Remove checker from its past position

    x = to_x;
    y = to_y;

    CHECKER *leftChecker = next.getLeftChecker();
    CHECKER *rightChecker = next.getRightChecker();
    CHECKER *upChecker = next.getUpChecker();
    CHECKER *downChecker = next.getDownChecker();

    // WHITE TURN
    if (next.isWhiteTurn)
    {
        // UPDATE KING POSITION
        if (next.board[x][y] == C::KING) {
            kingX = x;
            kingY = y;
        }

        // LEFT eat
        if (*leftChecker == C::BLACK && *next.getLeftChecker(2U) == C::WHITE)
            *leftChecker = C::EMPTY;

        // RIGHT eat
        if (*rightChecker == C::BLACK && *next.getRightChecker(2U) == C::WHITE)
            *rightChecker = C::EMPTY;

        // UP eat
        if (*upChecker == C::BLACK && *next.getUpChecker(2U) == C::WHITE)
            *upChecker = C::EMPTY;

        // DOWN eat
        if (*downChecker == C::BLACK && *next.getDownChecker(2U) == C::WHITE)
            *downChecker = C::EMPTY;
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
        if (*leftChecker > 1U)
        {
            STRUCTURE oppositeStructure = tablutStructure[to_x][to_y - 2U];

            // eat if normal eat (black on opposite side) or theres a camp on the opposite side  @see STRUCTURE
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
        if (*rightChecker > 1U)
        {
            STRUCTURE oppositeStructure = tablutStructure[to_x][to_y + 2U];

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
        if (*upChecker > 1U)
        {
            STRUCTURE oppositeStructure = tablutStructure[to_x - 2U][to_y];

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
        if (*downChecker > 1U)
        {
            STRUCTURE oppositeStructure = tablutStructure[to_x + 2U][to_y];

            // eat if normal eat (black on opposite side) or theres a camp on the opposite side  @see STRUCTURE
            if (*next.getUpChecker(2U) == C::BLACK || oppositeStructure == S::CAMPS)
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

int main(int argc, char *argv[])
{
    Tablut t(Tablut::newGame());
    t.print();
    Tablut t2 = t.next(4, 3, 5, 3);
    t2.print();

    return EXIT_SUCCESS;
}