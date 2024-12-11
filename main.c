#include <stdio.h>
#include <SDL2/SDL.h>
#include "constants.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    direction_types
} Direction; // improves code readability, yaha o se start karke line wise saare variables ko serial wise 0,1,2 etc de dega

typedef struct
{
    float x; //  x , y are the coordiantes using the numbers in the maze
    float y;
    int grid_x; // grid_x , grid_y , are the coordinates of the grid of the ghost at the moment
    int grid_y;
    Direction dir;
    Direction prev_dir;
    float speed;
    SDL_Texture *texture;
    SDL_Rect sprite_rect; // sprite_rect yaha par difine kar diya as a part of the ghost structure, baad mein you can call on this to define the graohics of the ghost
    SDL_Color color;
} Ghost;

struct ball
{
    float x;
    float y;
    float width;
    float height;
    float vel_x;
    float vel_y;
}; // pacman ke variables define kar diye hai. includes position coordinates , dimensions and speed.Speed movement ke liye use le rhe wo, rspective x ya y me sign ke hisaab se add hogi and pixel wise me convert karke movemet kara rhe hai

int score = 0;
int game_is_running = FALSE;   // game start, out hone pe per etc control karan eme help karega
SDL_Window *window = NULL;     // library function to call a window. yaha pe t iis initialized to null to preventfalse openenigs of hhte window beforehand
SDL_Renderer *renderer = NULL; // renderer saari elements print karega on teh screeen
int up = 0;
int down = 0;
int right = 0;
int left = 0;
int last_frame_time = 0; // time elapsed after the laster last frame
struct ball ball;
Ghost *ghost;
Ghost *ghost1; /// defining 4 Ghost struct pointer foor initializing 4 diffrent ghost entities using the following functions
Ghost *ghost2;
Ghost *ghost3;
Ghost *ghost4;
float SPEED2 = 2.7;
// Maze layout
int maze[ROWS][COLUMNS] =
    {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
     {1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1},
     {1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1},
     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
     {1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1},
     {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
     {1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1},
     {1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1},
     {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
     {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
     {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
     {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
     {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
     {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
     {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
     {1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1},
     {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
     {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
     {1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1},
     {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
     {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
     {1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1},
     {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
     {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1},
     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

typedef struct
{
    int x;
    int y;
    int is_active;
} Pellet; // structure defining the pellets

Pellet pellets[ROWS * COLUMNS]; // create a 1-d matrix of Pellet structues

int num_pellets = 0;

void initialize_pellets()
{
    num_pellets = 0;
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            if (maze[i][j] == 0)
            {
                pellets[num_pellets].x = j * TILE_SIZE + TILE_SIZE / 2 - 2; // placing the pellets at the points in teh maze where the integer == 0
                pellets[num_pellets].y = i * TILE_SIZE + TILE_SIZE / 2 - 2; // this aligns the pellet at the centre of the file
                pellets[num_pellets].is_active = 1;                         // indicates wether a pellet is alive or eaten by the pacman, 1--alive , 0--eaten
                num_pellets++;                                              // at the end of the loop counts the total no. of pellets
            }
        }
    }
}

void update_pellets()
{
    SDL_Rect pacman_rect = {(int)ball.x, (int)ball.y, (int)ball.width, (int)ball.height}; // defining basic info for the rendering of pacman rectangle

    for (int i = 0; i < num_pellets; i++)
    {
        if (pellets[i].is_active)
        {
            SDL_Rect pellet_rect = {pellets[i].x, pellets[i].y, 4, 4};
            if (SDL_HasIntersection(&pacman_rect, &pellet_rect))
            {                             // checks the interaction between the entities pacman ka rectangle and pelleect ka recatngle and increases the score
                pellets[i].is_active = 0; // after interaction the pellet is considered dead so active == 0
                score++;
            }
        }
    }
}

void render_pellets()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < num_pellets; i++)
    {
        if (pellets[i].is_active)
        {
            SDL_Rect pellet = {pellets[i].x, pellets[i].y, 4, 4}; // in the game loop , only renders the pellet if the pellet is active
            SDL_RenderFillRect(renderer, &pellet);
        }
    }
}

// major function to create the ghosts
Ghost *create_ghost(SDL_Renderer *renderer, int start_x, int start_y, float speed, SDL_Color color)
{
    Ghost *ghost = (Ghost *)malloc(sizeof(Ghost)); // create memeory for the ghost entity

    if (!ghost)
    {
        return NULL;
    }

    ghost->grid_x = start_x;
    ghost->grid_y = start_y;
    ghost->x = start_x * TILE_SIZE; // starting point of the ghost within the maze
    ghost->y = start_y * TILE_SIZE;
    ghost->speed = speed;
    ghost->dir = rand() % direction_types; // rand() generates random values and modulus operater make it so that the overall value generated is within the the scope of the values of valid direction
    ghost->prev_dir = ghost->dir;          // actively updates the previous direction to current direction so as to backtrack if therre are no valid direction left
    ghost->color = color;

    ghost->sprite_rect.w = TILE_SIZE; // defining the graphics of the ghost sprite
    ghost->sprite_rect.h = TILE_SIZE;
    ghost->sprite_rect.x = (int)ghost->x;
    ghost->sprite_rect.y = (int)ghost->y; // spawing the ghost to the tile corresponding to the x and y within the window

    // The texture is rendered as a colored square, and the steps involve creating a temporary surface, filling it with a color, and converting it into a texture
    //  4 values -- red ,green,blue,alpha      0,0,0,0 - setting flags for the colors
    SDL_Surface *surface = SDL_CreateRGBSurface(0, TILE_SIZE, TILE_SIZE, 32, 0, 0, 0, 0);
    if (surface)
    {
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, color.r, color.g, color.b));
        ghost->texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
    return ghost;
}

// deterimins if the  input coordiantes represent an tile which is a movable position
int is_valid_position(int maze[ROWS][COLUMNS], int x, int y)
{
    if (x < 0 || x >= COLUMNS || y < 0 || y >= ROWS)
    {
        return FALSE;
    }
    return (maze[y][x] != 1);
}

// this function basiccally counts up all teh possible movements the ghost is allowed to make and make an array of Direction enum which will store the corresponding valid directions left
int get_available_directions(int maze[ROWS][COLUMNS], int x, int y, Direction available[], Direction prev_dir)
{
    int count = 0;

    if (is_valid_position(maze, x, y - 1))
    {
        available[count++] = UP;
    }
    if (is_valid_position(maze, x, y + 1))
    {
        available[count++] = DOWN;
    }
    if (is_valid_position(maze, x - 1, y))
    {
        available[count++] = LEFT;
    }
    if (is_valid_position(maze, x + 1, y))
    {
        available[count++] = RIGHT;
    }
    return count;
}

// function to update the  state of the ghost after each game loop
void update_ghost(Ghost *ghost, int maze[ROWS][COLUMNS])
{
    ghost->grid_x = (int)(ghost->x / TILE_SIZE);
    ghost->grid_y = (int)(ghost->y / TILE_SIZE); // maze ki grid mein x aur y position calulate kar di

    Direction available[direction_types]; // an arry which cointain no. of valid directions jaha ghost move kar sakta hai
    int available_count = get_available_directions(maze, ghost->grid_x, ghost->grid_y, available, ghost->prev_dir);
    // iske baad available direction ka count nikala upar function se and count check kiya ki kitni directions mein ghoost jaa sakta hai

    bool can_move = false;
    switch (ghost->dir)
    { // switch decides konsi diretion mein final movement hoga
    case UP:
        can_move = is_valid_position(maze, ghost->grid_x, ghost->grid_y - 1); // is_valid_postion() either false return karega ya phir valid position ka cooordinates return karega
        break;
    case DOWN:
        can_move = is_valid_position(maze, ghost->grid_x, ghost->grid_y + 1);
        break;
    case LEFT:
        can_move = is_valid_position(maze, ghost->grid_x - 1, ghost->grid_y);
        break;
    case RIGHT:
        can_move = is_valid_position(maze, ghost->grid_x + 1, ghost->grid_y);
        break;
    }

    // if can move return false then chose a new direction
    if (!can_move)
    {
        ghost->prev_dir = ghost->dir;                     // Save the previous direction
        ghost->dir = available[rand() % available_count]; // Choose a new random direction from the array of available directions
    }

    // Move the ghost in the current direction if valid
    switch (ghost->dir)
    {
    case UP:
        if (is_valid_position(maze, ghost->grid_x, ghost->grid_y - 1))
            ghost->y -= ghost->speed; // repeated addition and subtraction for changing the coordinates of the entity
        break;
    case DOWN:
        if (is_valid_position(maze, ghost->grid_x, ghost->grid_y + 1))
            ghost->y += ghost->speed;
        break;
    case LEFT:
        if (is_valid_position(maze, ghost->grid_x - 1, ghost->grid_y))
            ghost->x -= ghost->speed;
        break;
    case RIGHT:
        if (is_valid_position(maze, ghost->grid_x + 1, ghost->grid_y))
            ghost->x += ghost->speed;
        break;
    }

    ghost->sprite_rect.x = (int)ghost->x; // change the position of the sprite entity to the new ghost x and y
    ghost->sprite_rect.y = (int)ghost->y;
}

void render_ghost(Ghost *ghost, SDL_Renderer *renderer)
{
    SDL_RenderCopy(renderer, ghost->texture, NULL, &ghost->sprite_rect); // this library funtions copies the texture directly onto the renderer
}

void destroy_ghost(Ghost *ghost)
{
    if (ghost)
    {
        if (ghost->texture)
        {
            SDL_DestroyTexture(ghost->texture);
        }
        free(ghost);
    }
}

int initialize_window(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Error initializing SDL.\n");
        return FALSE; // sdl ke saare library function related to window input kiye
    }

    window = SDL_CreateWindow(
        "Pac-Man",              // title
        SDL_WINDOWPOS_CENTERED, // window ka x
        SDL_WINDOWPOS_CENTERED, // window ka y
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN); // window generated with given characteristics

    if (!window)
    {
        printf("Error creating SDL Window.\n");
        return FALSE;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); // should use hardware acceleration if available.
    if (!renderer)
    {
        printf("Error creating SDL Renderer.\n");
        return FALSE;
    }

    return TRUE;
}

void setup(void)
{
    ball.x = 30;
    ball.y = 240;
    ball.width = 14;
    ball.height = 14;
    ball.vel_x = 0;
    ball.vel_y = 0;

    srand((unsigned int)time(NULL));

    SDL_Color ghost_color1 = {255, 0, 0, 255}; // Red
    ghost1 = create_ghost(renderer, 20, 20, SPEED2, ghost_color1);

    SDL_Color ghost_color2 = {0, 255, 255, 255}; // Cyan
    ghost2 = create_ghost(renderer, 25, 25, SPEED2, ghost_color2);

    SDL_Color ghost_color3 = {0, 255, 0, 255}; // Green
    ghost3 = create_ghost(renderer, 28, 25, SPEED2, ghost_color3);

    SDL_Color ghost_color4 = {255, 184, 255, 255}; // Pink
    ghost4 = create_ghost(renderer, 28, 25, SPEED2, ghost_color4);

    initialize_pellets();
}

void process_input(void)
{
    SDL_Event event; // defines an input which can be a keybaord or mouse input or it can be any other activty that will be occuring
    SDL_PollEvent(&event);

    switch (event.type)
    { // event.type basically predefined set of inputs h jinme keydown and keyup etc etc cheeze aati hai.
    case SDL_QUIT:
        game_is_running = FALSE;
        break;

    case SDL_KEYDOWN: // res.. key press hone pe uss direction ke movement ko 1 kar dega and isliye vo move kata rahega jab tk doosri key press nhi kar rhe
        if (event.key.keysym.sym == SDLK_ESCAPE)
        {
            game_is_running = FALSE;
            break;
        }
        else if (event.key.keysym.sym == SDLK_UP)
        {
            up = 1;
            down = left = right = 0;
        }
        else if (event.key.keysym.sym == SDLK_DOWN)
        {
            down = 2;
            up = left = right = 0;
        }
        else if (event.key.keysym.sym == SDLK_RIGHT)
        {
            right = 1;
            up = left = down = 0;
        }
        else if (event.key.keysym.sym == SDLK_LEFT)
        {
            left = 1;
            up = down = right = 0;
        }
        break;
    }
}

void update(void)
{

    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    {
        SDL_Delay(time_to_wait);
    }

    last_frame_time = SDL_GetTicks();

    if (up)
    {
        ball.vel_x = 0;
        ball.vel_y = -SPEED;
    }
    else if (down)
    {
        ball.vel_x = 0;
        ball.vel_y = SPEED;
    }
    else if (right)
    {
        ball.vel_x = SPEED;
        ball.vel_y = 0;
    }
    else if (left)
    {
        ball.vel_x = -SPEED;
        ball.vel_y = 0;
    }

    float new_x = ball.x + ball.vel_x;
    float new_y = ball.y + ball.vel_y;

    int top_left_x = (int)floor(new_x / TILE_SIZE);
    int top_left_y = (int)floor(new_y / TILE_SIZE);
    int top_right_x = (int)floor((new_x + ball.width) / TILE_SIZE);
    int bottom_left_y = (int)floor((new_y + ball.height) / TILE_SIZE);

    // Check collision with walls
    if (maze[top_left_y][top_left_x] == 0 &&
        maze[bottom_left_y][top_right_x] == 0 &&
        maze[top_left_y][top_right_x] == 0 &&
        maze[bottom_left_y][top_left_x] == 0)
    {
        ball.x = new_x;
        ball.y = new_y;
    }

    // Handle teleport
    if (maze[top_left_y][top_left_x] == 2 &&
        maze[bottom_left_y][top_left_x] == 2 &&
        left == 1)
    {
        ball.x = 420;
    }
    else if (maze[bottom_left_y][top_right_x] == 2 &&
             maze[top_left_y][top_right_x] == 2 &&
             right == 1)
    {
        ball.x = 30;
    }

    update_ghost(ghost1, maze);
    update_ghost(ghost2, maze);
    update_ghost(ghost3, maze);
    update_ghost(ghost4, maze);

    // Check collision with ghost
    if (fabs(ball.x - ghost1->x) < TILE_SIZE && fabs(ball.y - ghost1->y) < TILE_SIZE)
    {
        game_is_running = FALSE;
    }
    if (fabs(ball.x - ghost2->x) < TILE_SIZE && fabs(ball.y - ghost2->y) < TILE_SIZE)
    {
        game_is_running = FALSE;
    }
    if (fabs(ball.x - ghost3->x) < TILE_SIZE && fabs(ball.y - ghost3->y) < TILE_SIZE)
    {
        game_is_running = FALSE;
    }
    if (fabs(ball.x - ghost4->x) < TILE_SIZE && fabs(ball.y - ghost4->y) < TILE_SIZE)
    {
        game_is_running = FALSE;
    }
    if (score > 50)
    {
        SPEED2 = 3.0;
    }
    if (score > 100)
    {
        SPEED2 = 4.0;
    }
    if (score > 150)
    {
        SPEED2 = 5.0;
    }

    update_pellets();
}

void maze_render(void)
{ // generation of graphics of the walls
    SDL_Rect wall = {0, 0, TILE_SIZE, TILE_SIZE};

    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            if (maze[i][j] == 1)
            {
                wall.x = j * TILE_SIZE;
                wall.y = i * TILE_SIZE;
                SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);
                SDL_RenderDrawRect(renderer, &wall);
            }
        }
    }
}

void render(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    maze_render();

    SDL_Rect ball_rect = {
        (int)ball.x,
        (int)ball.y,
        (int)ball.width,
        (int)ball.height};

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

    render_pellets();

    render_ghost(ghost1, renderer);
    render_ghost(ghost2, renderer);
    render_ghost(ghost3, renderer);
    render_ghost(ghost4, renderer);

    SDL_RenderPresent(renderer);
}

void destroy_window(void)
{
    destroy_ghost(ghost1);
    destroy_ghost(ghost2);
    destroy_ghost(ghost3);
    destroy_ghost(ghost4);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// main function ke andar main game loop run karega game loop
// name input ke baad loop will start
// 1--initialize , in loop--(2--take input , 3--render , 4--update)
int main(void)
{

    game_is_running = initialize_window();
    char name[50];
    setup();

    printf("Press'ESC' to end the game\n ");
    printf("Press'UP KEY' to move upwards\n ");
    printf("Press'DOWN KEY' to move downwards\n ");
    printf("Press'RIGHT KEY' to move right\n ");
    printf("Press'LEFT KEY' to move left\n ");
    printf("Please enter your name to start the game[single word] : ");
    fgets(name, 100, stdin);
    name[strcspn(name, "\n")] = 0;
    FILE *fptr = NULL;
    fptr = fopen("score.txt", "a");
    if (fptr == NULL)
    {
        printf("Error! in opening file");
    }
    fputs(name, fptr);

    while (game_is_running)
    {

        process_input();
        update();
        render();
        printf("\033[8;1HScore: %d", score);
    }

    fprintf(fptr, " %d", score);
    fputs("\n", fptr);

    /*if (score > high_current) {
        high_current = score;
       }*/

    destroy_window();

    printf("\n");
    printf("YOUR FINAL SCORE IS -> %d", score);
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    fclose(fptr);
    return FALSE;
}