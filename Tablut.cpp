#include "Tablut.h"

Tablut::Tablut() {}
Tablut::~Tablut() {}

/*
    Constructor that copy only necessary elements to the new Tablut
*/
Tablut::Tablut(const Tablut &__startFrom)
{
    std::memcpy(this, &__startFrom, sizeof(Tablut));
}

Tablut Tablut::getStartingPosition()
{
    Tablut t = Tablut();

    for (Pos x = 0; x < 9; x++)
    {
        for (Pos y = 0; y < 9; y++)
        {
            t._board[x][y] = CHECKER::EMPTY;
        }
    }

    t._isWhiteTurn = true;

    t._whiteCount = 8U;
    t._blackCount = 16U;

    t._x = 0;
    t._y = 0;

    t._oldX = 0;
    t._oldY = 0;

    t._kingX = 4;
    t._kingY = 4;

    t._kills = 0;

    t._gameState = GAME_STATE::NONE;

    t._hash = 0;
    t._pastHashesIndex = 0;

    t._kingMovements = 0;
    t._turn = 0;

    t._score = 0;

    // Setting all black defenders
    t._board[0][3] = C::BLACK;
    t._board[0][4] = C::BLACK;
    t._board[0][5] = C::BLACK;
    t._board[1][4] = C::BLACK;
    t._board[3][0] = C::BLACK;
    t._board[4][0] = C::BLACK;
    t._board[5][0] = C::BLACK;
    t._board[4][1] = C::BLACK;
    t._board[8][3] = C::BLACK;
    t._board[8][4] = C::BLACK;
    t._board[8][5] = C::BLACK;
    t._board[7][4] = C::BLACK;
    t._board[3][8] = C::BLACK;
    t._board[4][8] = C::BLACK;
    t._board[5][8] = C::BLACK;
    t._board[4][7] = C::BLACK;

    // Setting up white attackers
    t._board[4][2] = C::WHITE;
    t._board[4][3] = C::WHITE;
    t._board[4][5] = C::WHITE;
    t._board[4][6] = C::WHITE;
    t._board[2][4] = C::WHITE;
    t._board[3][4] = C::WHITE;
    t._board[5][4] = C::WHITE;
    t._board[6][4] = C::WHITE;

    // Setting Throne
    t._board[4][4] = C::KING;

    t._checkerPositionIndex = 0;
    for (int i = 0; i < DIM; i++)
    {
        for (int j = 0; j < DIM; j++)
        {
            if (t._board[i][j] == C::BLACK || t._board[i][j] == C::WHITE)
            {
                t._checkerPositions[t._checkerPositionIndex++] = {i, j};
            }
        }
    }

    return t;
}

Tablut Tablut::next(const Pos __fromX, const Pos __fromY, const Pos __toX, const Pos __toY)
{

    Tablut _next = Tablut(*this);

    _next._board[__toX][__toY] = _next._board[__fromX][__fromY]; // Update checker position
    _next._board[__fromX][__fromY] = C::EMPTY;                   // Remove checker from its past position

    // Moving past checker

    auto mustFind = std::make_pair(__fromX, __fromY);
    for (int i = 0; i < _next._checkerPositionIndex; i++)
    {
        if (_next._checkerPositions[i] == mustFind)
        {
            _next._checkerPositions[i] = std::make_pair(__toX, __toY);
            break;
        }
    }

    // Update old and new positions of the moved pieces
    _next._oldX = __fromX;
    _next._oldY = __fromY;

    _next._x = __toX;
    _next._y = __toY;

    // Reset kill feed of previous round
    _next._kills = 0;

    CHECKER leftChecker = _next.getLeftChecker();
    CHECKER rightChecker = _next.getRightChecker();
    CHECKER upChecker = _next.getUpChecker();
    CHECKER downChecker = _next.getDownChecker();

    // WHITE TURN
    if (_next._isWhiteTurn)
    {
        // UPDATE KING POSITION
        if (_next._board[__toX][__toY] == C::KING)
        {
            _next._kingX = __toX;
            _next._kingY = __toY;
        }

        // NORMAL WHITE EATS

        // Opposite structure to check if black checker are surrounded by a structure in any direction
        STRUCTURE oppositeStructure;

        // LEFT eat
        if (_next._y > FIRST_COL && leftChecker == C::BLACK)
        {
            oppositeStructure = tablutStructure[_next._x][_next._y - 2];

            if (tablutStructure[_next._x][_next._y - 1] != S::CAMPS && (_next.getLeftChecker(2) == C::WHITE || oppositeStructure > 1))
            {
                _next.killLeft();
            }
        }

        // RIGHT eat
        if (_next._y < LAST_COL && rightChecker == C::BLACK)
        {
            oppositeStructure = tablutStructure[_next._x][_next._y + 2];

            if (tablutStructure[_next._x][_next._y + 1] != S::CAMPS && (_next.getRightChecker(2) == C::WHITE || oppositeStructure > 1))
            {
                _next.killRight();
            }
        }

        // UP eat
        if (_next._x > FIRST_ROW && upChecker == C::BLACK)
        {
            oppositeStructure = tablutStructure[_next._x - 2][_next._y];

            if (tablutStructure[_next._x - 1][_next._y] != S::CAMPS && (_next.getUpChecker(2) == C::WHITE || oppositeStructure > 1))
            {
                _next.killUp();
            }
        }

        // DOWN eat
        if (_next._x < LAST_ROW && downChecker == C::BLACK)
        {
            oppositeStructure = tablutStructure[_next._x + 2][_next._y];

            if (tablutStructure[_next._x + 1][_next._y] != S::CAMPS && (_next.getDownChecker(2) == C::WHITE || oppositeStructure > 1))
            {
                _next.killDown();
            }
        }
    }

    // BLACK TURN
    else
    {
        // NORMAL EATS

        // Opposite structure to check if white checker are surrounded by a structure in any direction
        STRUCTURE oppositeStructure;

        // LEFT eat ( KING OR SOLDIER ( > 1))
        if (_next._y > FIRST_COL && (leftChecker == C::WHITE || leftChecker == C::KING))
        {
            // Special eat, if king is in throne or near throne kills
            if (leftChecker == C::KING && _next.checkIfKingThreatened())
            {
                if (_next.checkIfKingDead())
                {
                    _next.killLeft();
                }
            }
            else
            {
                oppositeStructure = tablutStructure[_next._x][_next._y - 2];

                // eat if normall eat (black on opposite side) or theres a camp or castle on the opposite side  @see STRUCTURE
                if (tablutStructure[_next._x][_next._y - 1] != S::CASTLE && _next.getLeftChecker(2) == C::BLACK || oppositeStructure > 1)
                {
                    _next.killLeft();
                }
            }
        }

        // RIGHT eat
        if (_next._y < LAST_COL && (rightChecker == C::WHITE || rightChecker == C::KING))
        {
            // Special eat, if king is in throne or near throne kills
            if (rightChecker == C::KING && _next.checkIfKingThreatened())
            {
                if (_next.checkIfKingDead())
                {
                    _next.killRight();
                }
            }
            else
            {
                oppositeStructure = tablutStructure[_next._x][_next._y + 2];

                // eat if normal eat (black on opposite side) or theres a camp or castle on the opposite side  @see STRUCTURE
                if (tablutStructure[_next._x][_next._y + 1] != S::CASTLE && _next.getRightChecker(2) == C::BLACK || oppositeStructure > 1)
                {
                    _next.killRight();
                }
            }
        }

        // UP eat
        if (_next._x > FIRST_ROW && (upChecker == C::WHITE || upChecker == C::KING))
        {
            // Special eat, if king is in throne or near throne kills
            if (upChecker == C::KING && _next.checkIfKingThreatened())
            {
                if (_next.checkIfKingDead())
                {
                    _next.killUp();
                }
            }
            else
            {
                oppositeStructure = tablutStructure[_next._x - 2][_next._y];

                // eat if normal eat (black on opposite side) or theres a camp or castle on the opposite side  @see STRUCTURE
                if (tablutStructure[_next._x - 1][_next._y] != S::CASTLE && _next.getUpChecker(2) == C::BLACK || oppositeStructure > 1)
                {
                    _next.killUp();
                }
            }
        }

        // DOWN eat
        if (_next._x < LAST_ROW && (downChecker == C::WHITE || downChecker == C::KING))
        {
            // Special eat, if king is in throne or near throne kills
            if (downChecker == C::KING && _next.checkIfKingThreatened())
            {
                if (_next.checkIfKingDead())
                {
                    _next.killDown();
                }
            }
            else
            {
                oppositeStructure = tablutStructure[_next._x + 2][_next._y];

                // eat if normal eat (black on opposite side) or theres a camp or castle on the opposite side  @see STRUCTURE
                if (tablutStructure[_next._x + 1][_next._y] != S::CASTLE && _next.getDownChecker(2) == C::BLACK || oppositeStructure > 1)
                {
                    _next.killDown();
                }
            }
        }
    }

    _next._turn++;
    _next.checkWinState();

    _next.switchTurn();

    return _next;
}