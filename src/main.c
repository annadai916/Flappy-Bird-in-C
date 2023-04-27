#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <windows.h>
#include <stdio.h>

#define boolean int
#define false 0
#define true 1

// Screen size
const int screenX = 100, screenY = 100, screenWidth = 1000, screenHeight = 700;

// Position of Flappy Bird pipes and gaps between them
const int pipeStart = 500;
const int pipeWidth = 60;
const int pipeGap = 125;
const int pipeDistance = 300;
const int pipeSpeed = -2;

// Position of score display
const int scoreX = 870;
const int scoreY = 50;
int scoreW = 50;
int scoreH = 50;
const int scoreSize = 50;
// Score variable
int score = 0;

// Position of high score display
const int highScoreX = 50;
const int highScoreY = 50;
int highScoreW = 100;
int highScoreH = 30;
const int highScoreSize = 50;
// High score variable
int highScore = 0;

boolean updateScore = false;

// Position and size of bird
const int birdSize = 30;
const int startPos = (screenHeight - birdSize) / 2;
int birdX = 100, birdY = startPos;
// Physics of bird
float velocity = 0;
const float gravity = 0.3f;
const int jumpHeight = -6;

// Flappy Bird Pipes
struct pipe{
    SDL_Rect topPipe;
    SDL_Rect bottomPipe;
};
struct pipe pipes[4];

// Increases score during the game
void incrementScore(){
    if (updateScore) {
        score++;
        char str[4];
        sprintf(str, "%d", score);
        printf("%d %s\n", score, str);
        updateScore = false;

        if (score > highScore) highScore = score;
    }   
}

// Main function
int main(int argc, char** argv){

    TTF_Init();
    // Sets up window and renderer
    SDL_Window *window = SDL_CreateWindow("Clappy Bird", screenX, screenY, 
                                            screenWidth, screenHeight, 0);
    SDL_SetWindowResizable(window, SDL_TRUE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 
                                                SDL_RENDERER_ACCELERATED);

    // Sets font for score and high score displays
    TTF_Font *font = TTF_OpenFont("assets/VCR_OSD_MONO.ttf", scoreSize);
    // Sets colors for score and high score displays
    SDL_Color scoreColor = {0, 0, 255};
    SDL_Color highScoreColor = {255, 0, 0};
    // Sets positions for score and high score displays
    SDL_Rect scoreRect = {scoreX, scoreY, scoreW, scoreH};
    SDL_Rect highScoreRect = {highScoreX, highScoreY, highScoreW, highScoreH};

    // Creates bird and sets its positions
    SDL_Rect rect;
    rect.x = birdX;   
    rect.y = birdY;
    rect.w = rect.h = birdSize;

    // Sets size of pipes
    int pipesSize = sizeof(pipes)/sizeof(struct pipe);

    boolean pressed = false;
    boolean running = true;
    boolean freeze = true;
    boolean collision = false;
    boolean endGame = false;
    boolean restart = true;

    // Game loop
    while(running){
        SDL_Event event;

        // Allows quitting 
        // Sets mouse click or any key down for bird flapping
        while(SDL_PollEvent(&event)) {
            switch(event.type){

                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    pressed = true;
                    freeze = false;
                    velocity = jumpHeight;
                    break;
                
                case SDL_KEYDOWN:
                    pressed = true;
                    freeze = false;
                    velocity = jumpHeight;
                    break;

                default:
                    break;
            }
        }

        // On restart
        if (restart) {
            freeze = true;
            score = 0;

            for (unsigned int i = 0; i < pipesSize; i++){
                // Sets pipe positions
                pipes[i].topPipe.x = pipes[i].bottomPipe.x = pipeStart + 
                                     pipeDistance * i;
                pipes[i].topPipe.y = 0;
                pipes[i].topPipe.w = pipes[i].bottomPipe.w = pipeWidth;

                // Sets random pipe gaps
                pipes[i].topPipe.h = rand() % (screenHeight - pipeGap);
                pipes[i].bottomPipe.y = pipes[i].topPipe.h + pipeGap; 
                pipes[i].bottomPipe.h = screenHeight - pipes[i].bottomPipe.y;
            }

            restart = false;
        }

        // Unfreezes bird and pipe movement
        if (!freeze) {
            birdY += velocity;           
            velocity += gravity;

            for (unsigned int i = 0; i < pipesSize; i++) {
                pipes[i].topPipe.x = pipes[i].bottomPipe.x += pipeSpeed;

                // Brings pipe back to right side of screen when it goes off 
                // the left side of screen
                if (pipes[i].topPipe.x < -pipeWidth)
                    pipes[i].topPipe.x = pipes[i].bottomPipe.x = pipes[(i - 1 + 
                    pipesSize) % pipesSize].topPipe.x + pipeDistance;
            }
        }

        // Floor collision
        if (birdY > screenHeight) 
            collision = true;
        
        boolean needsUpdate = true;

        // While the bird is between pipes, check for collision or score update
        for (unsigned int i = 0; i < pipesSize; i++) {
            if ((pipes[i].topPipe.x - birdX) < birdSize && 
                (birdX - (pipes[i].topPipe.x + pipeWidth)) < 0) {
                if (birdY - pipes[i].topPipe.h < 0 || 
                    birdY + birdSize > pipes[i].bottomPipe.y)
                    collision = true;

                else incrementScore();

                needsUpdate = false;

                break;
            }
        }  

        if (needsUpdate) updateScore = true;

        // On pipe collision
        if (collision) {
            Sleep(1500);
            birdY = startPos;
            velocity = 0;
            restart = true;
            collision = false;
        }
        rect.y = birdY;
            
        // Clears everything rendered
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        // Draws rectangles
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

        // Draws the pipes
        for (unsigned int i = 0; i < pipesSize; i++){
            SDL_RenderFillRect(renderer, &pipes[i].topPipe);
            SDL_RenderFillRect(renderer, &pipes[i].bottomPipe);
        }

        // Displays score
        char str[12];
        sprintf(str, "%d", score);
        SDL_Surface *surface = TTF_RenderText_Solid(font, str, scoreColor);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(texture, NULL, NULL, &scoreW, &scoreH);
        SDL_FreeSurface(surface);
        SDL_RenderCopy(renderer, texture, NULL, &scoreRect);
        SDL_DestroyTexture(texture);

        // Displays highscore
        sprintf(str, "HIGH: %d", highScore);
        SDL_Surface *surface2 = TTF_RenderText_Solid(font, str, highScoreColor);
        SDL_Texture *texture2 = SDL_CreateTextureFromSurface(renderer, 
                                                            surface2);
        SDL_QueryTexture(texture2, NULL, NULL, &highScoreW, &highScoreH);
        SDL_FreeSurface(surface2);
        SDL_RenderCopy(renderer, texture2, NULL, &highScoreRect);
        SDL_DestroyTexture(texture2);

        // Shows what is drawn
        SDL_RenderPresent(renderer);    

        SDL_Delay(1000.0f/60.0f);
    }
    
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}