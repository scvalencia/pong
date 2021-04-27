#include <map>
#include <string>
#include <iostream>
#include <iterator>

#include "game.hpp"

using namespace std;

int main(int argc, char **argv) {
    
    Game game(std::stoi(argv[1]));

    if(game.Initialize())
        game.RunLoop();

    game.Shutdown();

    return 0;
}