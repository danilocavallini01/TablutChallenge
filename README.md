
# Tablut Game Artificial Intelligence
This is a Game Engine written in C++ for the [Tafl Game](https://en.wikipedia.org/wiki/Tafl_games), used during the [Tablut Yearly Challange](https://github.com/AGalassi/TablutCompetition) hosted during Fundamentals of Artificial Intelligence master course - Alma Mater Unibo.  

The AI engine was created with the intention of finding the best move for a given game with a <b>hifh speed</b> whilst implementing the code structure using the <b>clean architecture paradigm</b> to get a fully modular and reusable system

The AI was created by Danilo Cavallini who partecipated in the challenge under the group name "Impostor"
## Architecture
### Search Engine Architecture

The engine library is composed by the following components:  

+ **Search Engine**
	+ **Nega Scout ( Preferred )**
	+ **Alpha Beta Pruning** 
+ Heuristic Function
+ Transposition Table
+ Zobrist Function
+ Move Ordering Function
+ Stop Watch (for timeout)

Library mainly uses **NegaScout** algorithm as the search algorithm to achieve the best performance, when move ordering is done correctly NegaScout can roughly be 15% faster than regular AlphaBeta, an implementation of the standard AlphaBeta algorithm is also given

The engine also implements  **KillerMoves** and **Quiescence Search** to achieve the best speed and fairness as the NegaScout algorithm is usually paired with these two particular algorithm

Anyone can implement their game by using the interface and abstract classes present in the library

![Tablut Architecture](/assets/Architecture_crop.png "Tablut Architecture")

### Genetic Algorithm Architecture
This library also implements a genetic algorithm to try to improve the player's intelligence and accuracy by updating the heuristic function weights as generations passes on

The Genetic Algorithm implemented uses:
+ **Fitness:** A Fitness Function to evaluate players score
+ **GA** 
    + **Tournament Selection**: used to select parents weights for breeding
    + **Uniform Crossover**: crossover function used to create children's weights
    + **Mutation Function**
    + **Truncation Selection**

The genetic algorithm plays the game by itself using the start() function and also backup, when needed, the following informations:
+ **Weights**: every generation created by the GA backup itself under the */backup* directory
+ **Fitnesses**: every time the matches between generations are completed, the GA backup the fitnesses values under the */fitness* directory

## Usage
There are two ways to compile this library code but both need:
+ a working C++ compiler, g++ usually (library were compiled using g++-v12 even newer version should be fine )
+ the version 1.84 of [BOOST C++ Library V1.84](https://www.boost.org/ "Boost C++ Library")

### Compile with bash (recommended)
U can compile the whole library by launching the *./start.sh* bash file under the root directory, the bash script uses g++ to compiler to compile the mainfile

so u can either use the bash script
```
./start.sh
```
or compile it manually
```
g++ -Ofast -std=c++17 -I $HOME/boost_1_84_0/ -pthread -g ./main.cpp -o main
```
then launch the program with
```
./main
```

### Compile with CMAKE
U can compile the library by using Cmake tool by launching
```
cmake CMakeLists.txt
make -j 4
```
then launch the program with
```
./main
```

Remember to have the enviroment variable *BOOST_DIR* set to the location of the boost library example BOOST_DIR="/usr/x/boost_1_84_0")
