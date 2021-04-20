#include <SDL2/SDL.h>
#include <vector>

struct Vector2D {
    float x, y;
};

class Game {
    public:
        Game() : 
            window(nullptr), 
            renderer(nullptr), 
            ticksCount(0), 
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

        Vector2D paddlePosition1;
        Vector2D paddlePosition2;

        std::vector<Vector2D> ballPositions;
        std::vector<Vector2D> ballVelocities;

        Uint32 ticksCount;

        int paddleDirection1;
        int paddleDirection2;
};