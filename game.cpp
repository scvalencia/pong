#include <stdlib.h> 

#include "game.hpp"



const int THICKNESS = 15;
const float PADDLE_HEIGHT = 100.0f;
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

Vector2D randomVelocity() {

	int minBound = rand() % 300 + 1;
	int maxBound = rand() % 300 + 1;

	minBound *= rand() & 1 ? 1 : -1;
	maxBound *= rand() & 1 ? 1 : -1;

	return {
		(float) minBound,
		(float) 0
	};

}

/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
bool Game::Initialize() {

	int sdlInitializationResult = SDL_Init(SDL_INIT_VIDEO);

    if(sdlInitializationResult != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    this->window = SDL_CreateWindow(
        "Willy's and Sebas's Pong", // Title
        100,                        // Top left x-coordinate of window
        100,                        // Top left y-coordinate of window
        WINDOW_WIDTH,               // Width of window
        WINDOW_HEIGHT,              // Height of window
        0                           // Window creation flags, ex SDL_WINDOW_FULLSCREEN, etc
    );

    if(!this->window) {
        SDL_Log("Failed to create Window: %s", SDL_GetError());
        return false;
    }

    this->renderer = SDL_CreateRenderer(
        this->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC // imwiwiim, you should read what vsync is
    );

    if(!this->renderer) {
        SDL_Log("Failed to create Renderer: %s", SDL_GetError());
        return false;
    }
	
	this->paddlePosition1.x = 10.0f;
	this->paddlePosition1.y = (float) WINDOW_HEIGHT / 2.0f;

	this->paddlePosition2.x = (float) WINDOW_WIDTH - 10.0f;
	this->paddlePosition2.y = (float) WINDOW_HEIGHT / 2.0f;

	
	int spaceBetweenBalls = (int) (WINDOW_HEIGHT / (this->numberOfBalls + 1));
	int yBallPosition = spaceBetweenBalls;
	
	for(int i = 0; i < this->numberOfBalls; i++, yBallPosition += spaceBetweenBalls) {
		
		Vector2D ballPosition = {
			(float) WINDOW_WIDTH / 2.0f,
			(float) yBallPosition
		};

		this->ballPositions.push_back(ballPosition);
		this->ballVelocities.push_back(randomVelocity());

	}


	return true;

}

/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
void Game::Shutdown() {
    SDL_DestroyWindow(this->window);
    SDL_DestroyRenderer(this->renderer);
    SDL_Quit();
}

/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
void Game::RunLoop() {
    
    while(this->isRunning) {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }

}

/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
void Game::ProcessInput() {

    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                this->isRunning = false;
                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);

    if(state[SDL_SCANCODE_ESCAPE])
        this->isRunning = false;

    this->paddleDirection1 = 0;

    if(state[SDL_SCANCODE_W])
        this->paddleDirection1 -= 1;

    if(state[SDL_SCANCODE_S])
        this->paddleDirection1 += 1;

	this->paddleDirection2 = 0;

    if(state[SDL_SCANCODE_I])
        this->paddleDirection2 -= 1;

    if(state[SDL_SCANCODE_K])
        this->paddleDirection2 += 1;

}

/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
void updatePaddlePosition(int paddleDirection, float deltaTime, Vector2D& paddlePosition) {

	// Update paddle position based on direction
	if (paddleDirection != 0) {
		paddlePosition.y += paddleDirection * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (paddlePosition.y < (PADDLE_HEIGHT/2.0f + THICKNESS))
			paddlePosition.y = PADDLE_HEIGHT/2.0f + THICKNESS;
		else if (paddlePosition.y > ((float) WINDOW_HEIGHT - PADDLE_HEIGHT/2.0f - THICKNESS))
			paddlePosition.y = (float) WINDOW_HEIGHT - PADDLE_HEIGHT/2.0f - THICKNESS;
	}

}

/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
void handleBallBounce(int ballIndex, Vector2D& ballVelocity, Vector2D& ballPosition, Vector2D& paddlePosition1, Vector2D& paddlePosition2, std::vector<Vector2D>& ballPositions, int numberOfBalls) {
    int ADJUSTED_THICKNESS = THICKNESS*3/2;

    // Top collition
    if (ballPosition.y <= ADJUSTED_THICKNESS && ballVelocity.y < 0) {
        ballVelocity.y = -ballVelocity.y;
    }

    // Bottom collition
    if (ballPosition.y >= WINDOW_HEIGHT - ADJUSTED_THICKNESS && ballVelocity.y > 0) {
        ballVelocity.y = -ballVelocity.y;
    }

    // Left paddle collition
    if (ballPosition.x < ADJUSTED_THICKNESS
        && ballPosition.y > (paddlePosition1.y - PADDLE_HEIGHT/2 - THICKNESS/2) 
        && ballPosition.y < (paddlePosition1.y + PADDLE_HEIGHT/2 + THICKNESS/2)
        && ballVelocity.x < 0) {

        ballVelocity.x = -ballVelocity.x;
    }

    // Right paddle collition
    if (ballPosition.x > WINDOW_WIDTH - ADJUSTED_THICKNESS
        && ballPosition.y > (paddlePosition2.y - PADDLE_HEIGHT/2 - THICKNESS/2) 
        && ballPosition.y < (paddlePosition2.y + PADDLE_HEIGHT/2 + THICKNESS/2)
        && ballVelocity.x > 0) {

        ballVelocity.x = -ballVelocity.x;
    }

    // Balls collition
    for (int i = 0; i < numberOfBalls; i++) {

        auto& otherBallPosition = ballPositions[i];

        bool hasYCollition = ballPosition.y >= otherBallPosition.y - THICKNESS && ballPosition.y <= otherBallPosition.y + THICKNESS;

        if (ballIndex != i
            && ballPosition.x >= otherBallPosition.x - THICKNESS && ballPosition.x <= otherBallPosition.x
            && hasYCollition
            && ballVelocity.x > 0) {

            ballVelocity.x = -ballVelocity.x;

        }

        if (ballIndex != i
            && ballPosition.x >= otherBallPosition.x && ballPosition.x <= otherBallPosition.x + THICKNESS
            && hasYCollition
            && ballVelocity.x < 0) {

            ballVelocity.x = -ballVelocity.x;

        }
    }



}

/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
void Game::UpdateGame() {

	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), this->ticksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - this->ticksCount) / 1000.0f;
	
	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
		deltaTime = 0.05f;

	// Update tick counts (for next frame)
	this->ticksCount = SDL_GetTicks();

	updatePaddlePosition(this->paddleDirection1, deltaTime, this->paddlePosition1);
	updatePaddlePosition(this->paddleDirection2, deltaTime, this->paddlePosition2);
	
	// Update ball position based on ball velocity

	for(int i = 0; i < this->numberOfBalls; i++) {
		auto& ballPosition = this->ballPositions[i];
		auto ballVelocity = this->ballVelocities[i];

		ballPosition.x += ballVelocity.x * deltaTime;
		ballPosition.y += ballVelocity.y * deltaTime;

        if(ballPosition.x <= 0) {
            this->currentBalls--;
        }

        if(ballPosition.x >= WINDOW_WIDTH) {
            this->currentBalls--;
        }

	}

	// How does the ball bounce with the walls, paddles and other balls

    for (int i = 0; i < this->numberOfBalls; i++) {
        auto& ballPosition = this->ballPositions[i];
        auto& ballVelocity = this->ballVelocities[i];

        handleBallBounce(i, ballVelocity, ballPosition, this->paddlePosition1, this->paddlePosition2, this->ballPositions, this->numberOfBalls);
    }

    if(this->currentBalls == 0)
        isRunning = false;

	
}
/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
void renderWall(SDL_Renderer* renderer, int x, int y, int w, int h) {

	SDL_Rect wall;

    wall.x = x;        // Top left x
    wall.y = y;        // Top left y
    wall.w = w;        // Width
    wall.h = h;        // Height

    
    SDL_RenderFillRect(renderer, &wall);

}

/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
void renderPaddle(SDL_Renderer* renderer, int r, int g, int b, int a, Vector2D& paddlePosition) {

	SDL_SetRenderDrawColor(renderer, r, g, b, a);

    SDL_Rect paddle;

	// Member variables, express positions as center points, while SDL
    // processes positions as top-left point, to convert them,
    // x - (width / 2) and y - (height / 2)

    paddle.x = paddlePosition.x - (THICKNESS / 2);      // Top left x
    paddle.y = paddlePosition.y - (PADDLE_HEIGHT / 2);            	// Top left y
    paddle.w = THICKNESS;                               // Width
    paddle.h = PADDLE_HEIGHT; 

	SDL_RenderFillRect(renderer, &paddle);

}

/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
void renderBall(SDL_Renderer* renderer, int r, int g, int b, int a, Vector2D& ballPosition) {

	SDL_SetRenderDrawColor(renderer, r, g, b, a);

    // Draw ball
	SDL_Rect ball;

    ball.x = ballPosition.x - (THICKNESS / 2);        // Top left x
    ball.y = ballPosition.y - (THICKNESS / 2);       	// Top left y
    ball.w = THICKNESS;                             // Width
    ball.h = THICKNESS;                             // Height

    
    SDL_RenderFillRect(renderer, &ball);

}

/*
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
*/
void Game::GenerateOutput() {

    /*
     * 1. Clear the back buffer to a color
     * 2. Draw the entire game scene
     * 3. Swap the front buffer and back buffer
    **/

    // Clear the back buffer to a color
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0);

    SDL_RenderClear(this->renderer);

    // Draw the entire game scene

	// Draw walls, bottom and top
	SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);
	renderWall(this->renderer, 0, 0, WINDOW_WIDTH, THICKNESS);
	renderWall(this->renderer, 0, WINDOW_HEIGHT - THICKNESS, WINDOW_WIDTH, THICKNESS);

	// Draw paddles
	renderPaddle(this->renderer, 0, 0, 255, 255, this->paddlePosition1);
	renderPaddle(this->renderer, 255, 182, 0, 0, this->paddlePosition2);

	// Draw one ball

	for(auto ballPosition : this->ballPositions)
		renderBall(this->renderer, 255, 255, 255, 255, ballPosition);

    // Swap the front buffer and back buffer
    SDL_RenderPresent(this->renderer);

}