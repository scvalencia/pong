#include <SDL2/SDL.h>
#include <vector>

struct Vector2D {
    float x, y;
};

class Game {
    public:
        Game(int _currentBalls) : 
            window(nullptr), 
            renderer(nullptr), 
            ticksCount(0),
            currentBalls(_currentBalls),
            numberOfBalls(_currentBalls),
            isRunning(true), 
            paddleDirection1(0), 
            paddleDirection2(0) 
        {};

        bool Initialize();
        void RunLoop();
        void Shutdown();

    private:
        void ProcessInput();
        void UpdateGame();
        void GenerateOutput();

        SDL_Renderer* renderer;
        SDL_Window* window;
        bool isRunning;

        int currentBalls;
        int numberOfBalls;

        Uint32 ticksCount;

        Vector2D paddlePosition1;
        Vector2D paddlePosition2;

        int paddleDirection1;
        int paddleDirection2;

        std::vector<Vector2D> ballPositions;
        std::vector<Vector2D> ballVelocities;
        
};