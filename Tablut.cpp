#include "Tablut.h"

Tablut::Tablut() {}
Tablut::~Tablut() {}

/*
    Constructor that copy only necessary elements to the new Tablut
*/
Tablut::Tablut(const Tablut &startFrom)
{
    std::memcpy(this, &startFrom, sizeof(Tablut));
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

    t.x = 0;
    t.y = 0;

    t.old_x = 0;
    t.old_y = 0;

    t.kingX = 4;
    t.kingY = 4;

    t.killFeedIndex = 0;

    t.gameState == WIN::NONE;

    t.pastHashesIndex = 0;

    t.turn = 0;

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
        std::cout << " " << y << " ";
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
                }
                else
                {
                    std::cout << "\033[1;93m K \033[0m";
                }
            }
            else if (board[x][y] == C::BLACK)
            {
                if (x == Tablut::x && y == Tablut::y)
                {
                    std::cout << "\033[1;42m B \033[0m";
                }
                else if (tablutStructure[x][y] == S::CAMPS)
                {
                    std::cout << "\033[1;100m B \033[0m";
                }
                else if (tablutStructure[x][y] == S::ESCAPE)
                {
                    std::cout << "\033[1;106m B \033[0m";
                }
                else
                {
                    std::cout << "\033[1;97m B \033[0m";
                }
            }
            else if (board[x][y] == C::WHITE)
            {
                if (x == Tablut::x && y == Tablut::y)
                {
                    std::cout << "\033[1;42m W \033[0m";
                }
                else if (tablutStructure[x][y] == S::CAMPS)
                {
                    std::cout << "\033[1;100m W \033[0m";
                }
                else if (tablutStructure[x][y] == S::ESCAPE)
                {
                    std::cout << "\033[1;106m W \033[0m";
                }
                else
                {
                    std::cout << "\033[1;97m W \033[0m";
                }
            }
            else
            {
                if (x == Tablut::old_x && y == Tablut::old_y)
                {
                    std::cout << "\033[1;41m - \033[0m";
                }
                else if (tablutStructure[x][y] == S::CAMPS)
                {
                    std::cout << "\033[1;100m - \033[0m";
                }
                else if (tablutStructure[x][y] == S::ESCAPE)
                {
                    std::cout << "\033[1;106m - \033[0m";
                }
                else
                {
                    std::cout << " - ";
                }
            }
        }
        std::cout << std::endl;
    }

    std::cout << std::endl << "====================" << std::endl;
    std::cout << (isWhiteTurn ? "\033[1;47m| WHITE MOVE NOW (BLACK MOVED) |\033[0m" : "\033[1;40m| BLACK MOVE NOW (WHITE MOVED)|\033[0m" ) << std::endl;
    std::cout << "====================" << std::endl;

    std::cout << "TURN: " << turn << std::endl;
    std::cout << "whiteCheckers: " << unsigned(whiteCheckersCount) << std::endl;
    std::cout << "blackCheckers: " << unsigned(blackCheckersCount) << std::endl;
    std::cout << "kingPosition: " << int(kingX) << "-" << int(kingY) << std::endl;
    std::cout << "kills: " << killFeedIndex << std::endl;
    std::cout << "checkerMovedTo: " << int(x) << "-" << int(y) << std::endl;
    std::cout << "checkerMovedFrom: " << int(old_x) << "-" << int(old_y) << std::endl;
}

Tablut Tablut::next(const Pos from_x, const Pos from_y, const Pos to_x, const Pos to_y)
{

    Tablut next = Tablut(*this);

    next.board[to_x][to_y] = next.board[from_x][from_y]; // Update checker position
    next.board[from_x][from_y] = C::EMPTY;               // Remove checker from its past position

    // Update old and new positions of the moved pieces
    next.old_x = from_x;
    next.old_y = from_y;

    next.x = to_x;
    next.y = to_y;

    // Reset kill feed of previous round
    next.killFeedIndex = 0;

    CHECKER leftChecker = next.getLeftChecker();
    CHECKER rightChecker = next.getRightChecker();
    CHECKER upChecker = next.getUpChecker();
    CHECKER downChecker = next.getDownChecker();

    // WHITE TURN
    if (next.isWhiteTurn)
    {
        // UPDATE KING POSITION
        if (next.board[to_x][to_y] == C::KING)
        {
            next.kingX = to_x;
            next.kingY = to_y;
        }

        // NORMAL WHITE EATS

        // Opposite structure to check if black checker are surrounded by a structure in any direction
        STRUCTURE oppositeStructure;

        // LEFT eat
        if (next.y > SECOND_COL && leftChecker == C::BLACK)
        {
            oppositeStructure = tablutStructure[next.x][next.y - 2];

            if (tablutStructure[next.x][next.y - 1] != S::CAMPS && (next.getLeftChecker(2) == C::WHITE || oppositeStructure > 1U))
            {
                next.killLeft();
            }
        }

        // RIGHT eat
        if (next.y < SEC_LAST_COL && rightChecker == C::BLACK)
        {
            oppositeStructure = tablutStructure[next.x][next.y + 2];

            if (tablutStructure[next.x][next.y + 1] != S::CAMPS && (next.getRightChecker(2) == C::WHITE || oppositeStructure > 1U))
            {
                next.killRight();
            }
        }

        // UP eat
        if (next.x > SECOND_ROW && upChecker == C::BLACK)
        {
            oppositeStructure = tablutStructure[next.x - 2][next.y];

            if (tablutStructure[next.x - 1][next.y] != S::CAMPS && (next.getUpChecker(2) == C::WHITE || oppositeStructure > 1U))
            {
                next.killUp();
            }
        }

        // DOWN eat
        if (next.x < SEC_LAST_ROW && downChecker == C::BLACK)
        {
            oppositeStructure = tablutStructure[next.x + 2][next.y];

            if (tablutStructure[next.x + 1][next.y] != S::CAMPS && (next.getDownChecker(2) == C::WHITE || oppositeStructure > 1U))
            {
                next.killDown();
            }
        }
    }

    // BLACK TURN
    else
    {
        if (checkIfKingDead())
        {
            killChecker(next.kingX, next.kingY);

            next.turn++;
            next.switchTurn();

            return next;
        }

        // NORMAL EATS

        // Opposite structure to check if white checker are surrounded by a structure in any direction
        STRUCTURE oppositeStructure;

        // LEFT eat ( KING OR SOLDIER ( > 1))
        if (next.y > SECOND_COL && (leftChecker == C::WHITE || leftChecker == C::KING))
        {
            oppositeStructure = tablutStructure[next.x][next.y - 2];

            // eat if normall eat (black on opposite side) or theres a camp on the opposite side  @see STRUCTURE
            if (next.getLeftChecker(2) == C::BLACK || oppositeStructure == S::CAMPS)
            {
                next.killLeft();
            }
        }

        // RIGHT eat
        if (next.y < SEC_LAST_COL && (rightChecker == C::WHITE || rightChecker == C::KING))
        {
            oppositeStructure = tablutStructure[next.x][next.y + 2];

            // eat if normal eat (black on opposite side) or theres a camp on the opposite side  @see STRUCTURE
            if (next.getRightChecker(2) == C::BLACK || oppositeStructure == S::CAMPS)
            {
                next.killRight();
            }
        }

        // UP eat
        if (next.x > SECOND_ROW && (upChecker == C::WHITE || upChecker == C::KING))
        {
            oppositeStructure = tablutStructure[next.x - 2][next.y];

            // eat if normal eat (black on opposite side) or theres a camp on the opposite side  @see STRUCTURE
            if (next.getUpChecker(2) == C::BLACK || oppositeStructure == S::CAMPS)
            {
                next.killUp();
            }
        }

        // DOWN eat
        if (next.x < SEC_LAST_ROW && (downChecker == C::WHITE || downChecker == C::KING))
        {
            oppositeStructure = tablutStructure[next.x + 2][next.y];

            // eat if normal eat (black on opposite side) or theres a camp on the opposite side  @see STRUCTURE
            if (next.getDownChecker(2) == C::BLACK || oppositeStructure == S::CAMPS)
            {
                next.killDown();
            }
        }
    }

    next.turn++;
    next.switchTurn();

    return next;
}