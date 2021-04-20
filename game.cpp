#include "game.hpp"

const int THICKNESS = 15;
const float PADDLE_HEIGHT = 100.0f;
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

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

	this->ballPosition.x = (float) WINDOW_WIDTH / 2.0f;
	this->ballPosition.y = (float) WINDOW_HEIGHT / 2.0f;
	
	this->ballVelocity.x = -200.0f;
	this->ballVelocity.y = 235.0f;

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
	this->ballPosition.x += this->ballVelocity.x * deltaTime;
	this->ballPosition.y += this->ballVelocity.y * deltaTime;
	
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
    paddle.y = paddlePosition.y - (70 / 2);            	// Top left y
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

    SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);

    // Draw top wall
	SDL_Rect wall;

    wall.x = 0;
	wall.y = 0;
	wall.w = WINDOW_WIDTH;
	wall.h = THICKNESS;
	SDL_RenderFillRect(renderer, &wall);
	
	// Draw bottom wall
	wall.y = WINDOW_HEIGHT - THICKNESS;
	SDL_RenderFillRect(renderer, &wall);

	// Draw paddles
	renderPaddle(this->renderer, 0, 0, 255, 255, this->paddlePosition1);
	renderPaddle(this->renderer, 255, 182, 0, 0, this->paddlePosition2);

    SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);

    // Draw ball
	SDL_Rect ball;

    ball.x = this->ballPosition.x - (THICKNESS / 2);        // Top left x
    ball.y = this->ballPosition.y - (THICKNESS / 2);         // Top left y
    ball.w = THICKNESS;                                 // Width
    ball.h = THICKNESS;                                  // Height

    
    SDL_RenderFillRect(this->renderer, &ball);

    // Swap the front buffer and back buffer
    SDL_RenderPresent(this->renderer);

}