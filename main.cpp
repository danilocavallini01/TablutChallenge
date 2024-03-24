#include "Tablut.h"
#include "MoveGenerator.h"
#include <vector>

int main(int argc, char *argv[])
{
    Tablut t = Tablut::newGame();
    std::vector<Tablut> res{};
    
    for ( int i = 0; i < 10; i++ ) {
        MoveGenerator::generateLegalMoves(t,res);

        int selectedMove = rand() % res.size();

        t = res[selectedMove];
        res[selectedMove].print();

        res = {};
    }

    
    

   

    

    return EXIT_SUCCESS;
}