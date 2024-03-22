
#include "Tablut.h"
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <string>
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

    // Setting all black defenders
    for (TablutValue pos : campsPosition)
    {
        t.board[pos / FACTOR][pos % FACTOR] = CHECKER::BLACK;
    }

    // Setting up white attackers
    t.board[4][2] = CHECKER::WHITE;
    t.board[4][3] = CHECKER::WHITE;
    t.board[4][5] = CHECKER::WHITE; 
    t.board[4][6] = CHECKER::WHITE;
    t.board[2][4] = CHECKER::WHITE;
    t.board[3][4] = CHECKER::WHITE;
    t.board[5][4] = CHECKER::WHITE;
    t.board[6][4] = CHECKER::WHITE;

    // Setting Throne
    t.board[4][4] = CHECKER::KING;

    return t;
}

void Tablut::print()
{
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 9; y++)
        {
            if (board[x][y] == CHECKER::KING) {
                std::cout << " 8 ";
            } else if ( board[x][y] == CHECKER::BLACK) {
                std::cout << "-1 ";
            } else if ( board[x][y] == CHECKER::WHITE) {
                std::cout << " 1 ";
            } else {
                std::cout << " 0 ";
            }
            
        }
        std::cout << std::endl;
    }
}

int main(int argc, char *argv[])
{
    Tablut t(Tablut::newGame());
    t.print();

    return EXIT_SUCCESS;
}