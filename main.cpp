#include <iostream>
#include "game.hpp"

using namespace std;

int main() {
    
    Game game;

    if(game.Initialize())
        game.RunLoop();

    game.Shutdown();

    return 0;
}