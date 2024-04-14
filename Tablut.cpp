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

    return t;
}

Tablut Tablut::next(const Pos __fromX, const Pos __fromY, const Pos __toX, const Pos __toY)
{

    Tablut _next = Tablut(*this);

    _next._board[__toX][__toY] = _next._board[__fromX][__fromY]; // Update checker position
    _next._board[__fromX][__fromY] = C::EMPTY;                   // Remove checker from its past position

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
        if (checkIfKingDead())
        {
            killChecker(_next._kingX, _next._kingY);

            _next._turn++;
            _next.switchTurn();

            return _next;
        }

        // NORMAL EATS

        // Opposite structure to check if white checker are surrounded by a structure in any direction
        STRUCTURE oppositeStructure;

        // LEFT eat ( KING OR SOLDIER ( > 1))
        if (_next._y > FIRST_COL && (leftChecker == C::WHITE || leftChecker == C::KING))
        {
            oppositeStructure = tablutStructure[_next._x][_next._y - 2];

            // eat if normall eat (black on opposite side) or theres a camp or castle on the opposite side  @see STRUCTURE
            if (_next.getLeftChecker(2) == C::BLACK || oppositeStructure > 1)
            {
                _next.killLeft();
            }
        }

        // RIGHT eat
        if (_next._y < LAST_COL && (rightChecker == C::WHITE || rightChecker == C::KING))
        {
            oppositeStructure = tablutStructure[_next._x][_next._y + 2];

            // eat if normal eat (black on opposite side) or theres a camp or castle on the opposite side  @see STRUCTURE
            if (_next.getRightChecker(2) == C::BLACK || oppositeStructure > 1)
            {
                _next.killRight();
            }
        }

        // UP eat
        if (_next._x > FIRST_ROW && (upChecker == C::WHITE || upChecker == C::KING))
        {
            oppositeStructure = tablutStructure[_next._x - 2][_next._y];

            // eat if normal eat (black on opposite side) or theres a camp or castle on the opposite side  @see STRUCTURE
            if (_next.getUpChecker(2) == C::BLACK || oppositeStructure > 1)
            {
                _next.killUp();
            }
        }

        // DOWN eat
        if (_next._x < LAST_ROW && (downChecker == C::WHITE || downChecker == C::KING))
        {
            oppositeStructure = tablutStructure[_next._x + 2][_next._y];

            // eat if normal eat (black on opposite side) or theres a camp or castle on the opposite side  @see STRUCTURE
            if (_next.getDownChecker(2) == C::BLACK || oppositeStructure > 1)
            {
                _next.killDown();
            }
        }
    }

    _next._turn++;
    _next.switchTurn();

    return _next;
}