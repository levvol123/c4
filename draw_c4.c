#include <raylib.h>
#include "c4.h"
#include <stdio.h>
#include <math.h>

#define LOGICAL_HEIGHT 1000
#define LOGICAL_WIDTH  1000

#define GRID_OFFSET_X 40
#define GRID_OFFSET_Y 40
#define GRID_DOT_SIZE 3.0f
#define CELL_WALL_LENGTH 90

#define DISC_SIZE 40

typedef struct 
{
    Vector2 position;
    int player;
    int column;
} falling_disc;


typedef enum {LIGHT, DARK, LIGHT_COLOR, NUMBER_OF_THEMES} theme;

typedef struct {
    Color disc1;
    Color disc2;
    Color background;
    Color board_grid;
    Color background_grid;
    Color column_selector;
} color_palette;

static color_palette themes[NUMBER_OF_THEMES] = {
    [LIGHT] = {
    .disc1 = RED,
    .disc2 = BLUE,
    .background = WHITE,
    .board_grid = BLACK,
    .background_grid = LIGHTGRAY,
    .column_selector = GRAY,
    },
    [DARK] = {
    .disc1 = RED,
    .disc2 = BLUE,
    .background = BLACK,
    .board_grid = WHITE,
    .background_grid = LIGHTGRAY,
    .column_selector = WHITE,
    },
    [LIGHT_COLOR] = {
    .disc1 = (Color){0xff, 0x69, 0x78, 0xff},
    .disc2 = (Color){0x34, 0x7f, 0xc4, 0xff},
    .background = (Color){0x6d, 0x43, 0x5a, 0xff},
    .board_grid = (Color){0xff, 0xfc, 0xf9, 0xff},
    .background_grid = LIGHTGRAY,
    .column_selector = (Color){0xff, 0xe8, 0xd1, 0xff},
    },
};

color_palette *current_theme;

void draw_discs(){
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            Vector2 center = {.x = (j * CELL_WALL_LENGTH) + GRID_OFFSET_X + (2.5 * CELL_WALL_LENGTH), .y = (i * CELL_WALL_LENGTH) + GRID_OFFSET_Y + (4.5 * CELL_WALL_LENGTH)};
           
            if (get_color(i,j) == 1) {
                DrawCircleV(center, DISC_SIZE, current_theme->disc1);     
            } else if (get_color(i,j) == 2) {                             
                DrawCircleV(center, DISC_SIZE, current_theme->disc2); 
            }
        }
    }
}

void draw_grid(){
    static int steps_x = LOGICAL_WIDTH  / 50;
    static int steps_y = LOGICAL_HEIGHT / 50;  
    for (int i = 0; i <= steps_x; i++)
        {
            for (int j = 0; j <= steps_y; j++)
            {
                if(j > 1 && j < 10 && i > 3 && i < 11){
                    Vector2 center = {.x = (j * CELL_WALL_LENGTH) + GRID_OFFSET_X, .y = (i * CELL_WALL_LENGTH) + GRID_OFFSET_Y};
                    DrawCircleV(center, GRID_DOT_SIZE, current_theme->board_grid); //playing grid
                } else {
                    Vector2 center = {.x = (j * CELL_WALL_LENGTH) + GRID_OFFSET_X, .y = (i * CELL_WALL_LENGTH) + GRID_OFFSET_Y};
                    DrawCircleV(center, GRID_DOT_SIZE, current_theme->background_grid); //background grid
                }
            }
        }
}
void draw_column_selector(int index){
    Vector2 center = {.x = (index * CELL_WALL_LENGTH) + GRID_OFFSET_X + (2.1 * CELL_WALL_LENGTH), .y = (10.5 * CELL_WALL_LENGTH) + GRID_OFFSET_Y};
    DrawRectangleV(center, (Vector2){.x = 0.8*CELL_WALL_LENGTH, .y = 3}, current_theme->column_selector);
}

void animate_falling_disc(int *play_animation, falling_disc *the_disc){
    if(*play_animation == 1){
        the_disc->position.y += 10;
        int height = ((get_next_row_from_column(the_disc->column)-1) * CELL_WALL_LENGTH) + GRID_OFFSET_Y + ( 4.5 * CELL_WALL_LENGTH);
        if(the_disc->position.y >= height){
            *play_animation = 0;
            place_disc(the_disc->column);
        }
        if(the_disc->player == 1){
            DrawCircleV(the_disc->position, DISC_SIZE, current_theme->disc1);
        } else {
            DrawCircleV(the_disc->position, DISC_SIZE, current_theme->disc2);
        }
    }
}
static Camera2D fit_camera(){
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float scale = fminf(sw / 1080.f, sh / 1080.f);
    return (Camera2D){
        .offset   = { (sw - 1080.f*scale) / 2, (sh - 1080.f*scale) / 2 },
        .target   = { 0, 0 },
        .rotation = 0,
        .zoom     = scale,
    };
}

int main(){

    const int screenWidth = 1080;
    const int screenHeight = 1080;
    int current_column_index = 0;

    int play_falling_disc_animation = 0;
    falling_disc the_falling_disk = {0};

    Camera2D camera = { 0 };

    current_theme = &themes[LIGHT];
    init_c4();
    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);   
    InitWindow(screenWidth, screenHeight, "Connect Four");

    SetTargetFPS(120);
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose()) 
    {

        if(IsKeyPressed(KEY_U)){
            undo_last_move();
        } else if(IsKeyPressed(KEY_R)){
            while (undo_last_move() == 1)
            {
                ;
            }
        } else if(IsKeyPressed(KEY_RIGHT)){
            if(current_column_index < COLUMNS-1){
                current_column_index +=1;
            }
        } else if(IsKeyPressed(KEY_LEFT)){
            if(current_column_index > 0){
                current_column_index -=1;
            }
        } else if(IsKeyPressed(KEY_DOWN)){
            if(play_falling_disc_animation == 0 && get_next_row_from_column(current_column_index) != 0){
                the_falling_disk.column = current_column_index;
                the_falling_disk.position = (Vector2){.x = (current_column_index * CELL_WALL_LENGTH) + GRID_OFFSET_X + (2.5 * CELL_WALL_LENGTH), .y = GRID_OFFSET_Y + (4.5 * CELL_WALL_LENGTH)};
                the_falling_disk.player = get_current_player();
                play_falling_disc_animation = 1;
            }

        } else if(IsKeyPressed(KEY_ONE)){
            current_theme = &themes[LIGHT];
        } else if(IsKeyPressed(KEY_TWO)){
            current_theme = &themes[DARK];
        } else if(IsKeyPressed(KEY_THREE)){
            current_theme = &themes[LIGHT_COLOR];
        }
        camera = fit_camera();
        BeginDrawing();
            ClearBackground(current_theme->background);
            BeginMode2D(camera);
            //----------
            draw_grid();
            animate_falling_disc(&play_falling_disc_animation, &the_falling_disk);
            draw_discs();
            draw_column_selector(current_column_index);
            //---------
            EndMode2D();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}
