
# Tablut Game Artificial Intelligence
This is a Game Engine written in C++ for the [Tafl Game](https://en.wikipedia.org/wiki/Tafl_games), used during the [Tablut Yearly Challange](https://github.com/AGalassi/TablutCompetition) while attending Fundamentals of Artificial Intelligence Masters course - Alma Mater Unibo.  

The Ai engine was created with the intention finding the next best move with a <b>decent speed</b> while implementing the code using the <b>clean architetcure paradigm</b> to get a modular system

The Ai was created by Danilo Cavallini that partecipated in the challange under the group name "Impostor"
## Architecture
### Search Engine Architecture

The engine is composed by the following components:  

+ **Search Engine**
	+ **Nega Scout ( Preferred )**
	+ **Alpha Beta Pruning** 
+ Heuristic Function
+ Transposition Table
+ Zobrist Function
+ Move Ordering Function
+ Stop Watch (for timeout)

Engine mainly use **NegaScout** algorithm as search algorithm to achieve the best performance, an implementation of the standard AlphaBeta algorithm is also given

The engine also implements  **KillerMoves** and **Quiescence Search** to achieve the best speed as the NegaScout algorithm is usually implemented with this two particular algorithm

Anyone can implement their game by using the interface and abstract classes present in the library

![Tablut Architecture](/assets/Architecture_crop.png "Tablut Architecture")

### Genetic Algorithm Architecture
This library also implements a genetic algorithm to try improve the player intelligence by updating the heuristic function weights as generation passes by

The Genetic Algorithm implemented uses:
+ **Fitness:** A Fitness Function to evaluate players score
+ **GA** 
    + **Tournament Selection**: used to select parents weights for breeding
    + **Uniform Crossover**: crossover function used to create children weights
    + **Mutation Function**
    + **Truncation Selection**

The genetic algorithm play the game by itself using the start() function and also backup the following informations:
+ **Weights**: every generation created by the GA backup itself under the */backup* directory
+ **Fitnesses**: every time the matches between generation are completed, the GA backup the fitnesses values under the */fitness* directory

## Usage
There are two ways of compiling this library code but both ways NEEDS 
+ a working C++ compiler, g++ usually (library were compiled using g++-v12 even newer vesion should be fine )
+ the version 1.84 of [BOOST C++ Library V1.84](https://www.boost.org/ "Boost C++ Library")

### Compile with bash
U can compile the whole library by launching the *./start.sh* bash file under the root directory, the bash script use g++ to compiler to compile the mainfile
so u can either
```./start.sh```
or do it manually
```g++ -Ofast -std=c++17 -I $HOME/boost_1_84_0/ -pthread -g ./main.cpp -o main```
then launch the program with
```./main```


### Compile with CMAKE
U can compile the library by using Cmake tool by launching
```cmake CMakeLists.txt```
```make -j 4```
then launch the program with
```./main```

Remember to have the enviroment variable *BOOST_DIR* set to the location of the boost library example BOOST_DIR="/usr/x/boost_1_84_0")
