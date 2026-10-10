#include <raylib.h>
#include "c4.h"
#include <stdio.h>


#define LEFT_PAD_RATIO   0.2f
#define RIGHT_PAD_RATIO  0.2f
#define TOP_PAD_RATIO    0.3f
#define BOTTOM_PAD_RATIO 0.15f
#define DISC_RATIO       0.06f

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
    Color grid;
    Color column_selector;
} color_palette;

static color_palette themes[NUMBER_OF_THEMES] = {
    [LIGHT] = {
    .disc1 = RED,
    .disc2 = BLUE,
    .background = WHITE,
    .grid = BLACK,
    .column_selector = GRAY,
    },
    [DARK] = {
    .disc1 = RED,
    .disc2 = BLUE,
    .background = BLACK,
    .grid = WHITE,
    .column_selector = WHITE,
    },
    [LIGHT_COLOR] = {
    .disc1 = (Color){0xff, 0x69, 0x78, 0xff},
    .disc2 = (Color){0x34, 0x7f, 0xc4, 0xff},
    .background = (Color){0x6d, 0x43, 0x5a, 0xff},
    .grid = (Color){0xff, 0xfc, 0xf9, 0xff},
    .column_selector = (Color){0xff, 0xe8, 0xd1, 0xff},
    },
};

color_palette *current_theme;

void draw_background(int width, int height){
    ClearBackground(current_theme->background);
    int left_pad = width * LEFT_PAD_RATIO;
    int right_pad = width * RIGHT_PAD_RATIO;
    int top_pad = height * TOP_PAD_RATIO;
    int bottom_pad = height * BOTTOM_PAD_RATIO;
    const float radius = 3.0f;

    int grid_width  = width - left_pad - right_pad;
    int grid_height = height - top_pad - bottom_pad;

    float step_x = grid_width / COLUMNS;
    float step_y = grid_height / ROWS;

    for (int i = 0; i <= ROWS; i++)
    {
        for (int j = 0; j <= COLUMNS; j++)
        {
            Vector2 center = {.x = left_pad + step_x*j, .y = top_pad + i *step_y};
            DrawCircleV(center, radius, current_theme->grid);
        }
    }
}
void draw_discs(int width, int height){
    int left_pad = width * LEFT_PAD_RATIO;
    int right_pad = width * RIGHT_PAD_RATIO;
    int top_pad = height * TOP_PAD_RATIO;
    int bottom_pad = height * BOTTOM_PAD_RATIO;
    int grid_width  = width - left_pad - right_pad;
    int grid_height = height - top_pad - bottom_pad;

    float step_x = grid_width / COLUMNS;
    float step_y = grid_height / ROWS;
    for (int i = 0; i < ROWS; i++)
            {
                for (int j = 0; j < COLUMNS; j++)
                {
                    Vector2 center = {.x = left_pad+step_x/2 + step_x*j, .y = top_pad +step_y/2+ i *step_y};
                    if (get_color(i,j) == 1)
                    {
                        DrawCircleV(center, 40, current_theme->disc1);     
                    }
                    else if (get_color(i,j) == 2)
                    {
                        DrawCircleV(center, 40, current_theme->disc2);                        
                    }
                }
            }
}
void draw_column_selector(int index, int height, int left_pad, int bottom_pad, float step_x){
    //DrawCircle(index*200 + 70, 10, 10, GREEN);
    Vector2 center = {.x = left_pad + step_x*index + 0.1*step_x, .y = height - (bottom_pad * 0.85)};
    //DrawCircleV(center, 10, GREEN);
    DrawRectangleV(center, (Vector2){.x = 0.8*step_x, .y = 3}, current_theme->column_selector);
}

void animate_falling_disc(int *play_animation, falling_disc *the_disc, int top_pad, int step_y){
    if(*play_animation == 1){
        the_disc->position.y +=10;
        float height = top_pad +step_y/2+ (get_next_row_from_column(the_disc->column)-1) *step_y; 
        if(the_disc->position.y >= height) {
            *play_animation = 0;
            place_disc(the_disc->column);
        }
        if(the_disc->player == 1){
            DrawCircleV(the_disc->position, 40, current_theme->disc1);
        } else {
            DrawCircleV(the_disc->position, 40, current_theme->disc2);
        }
    }
}

int main(){

    const int screenWidth = 1080;
    const int screenHeight = 1080;
    int current_column_index = 0;

    int play_falling_disc_animation = 0;
    falling_disc the_falling_disk = {0};


    current_theme = &themes[LIGHT];
    init_c4();
    //SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_MSAA_4X_HINT);   
    InitWindow(screenWidth, screenHeight, "Connect Four");

    SetTargetFPS(120);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose()) 
    {
            int height = GetScreenHeight();
            int width = GetScreenWidth();

            int left_pad = width * LEFT_PAD_RATIO;
            int right_pad = width * RIGHT_PAD_RATIO;
            int top_pad = height * TOP_PAD_RATIO;
            int bottom_pad = height * BOTTOM_PAD_RATIO;

            int grid_width  = width - left_pad - right_pad;
            int grid_height = height - top_pad - bottom_pad;

            float step_x = grid_width / COLUMNS;
            float step_y = grid_height / ROWS;


        if(IsKeyPressed(KEY_U)){
            undo_last_move();
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
                the_falling_disk.position = (Vector2){.x = left_pad+step_x/2 + step_x*current_column_index, .y = top_pad +step_y/2};
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

        BeginDrawing();

            //draw_board();
            //ClearBackground(LIGHTGRAY);


            draw_background(width, height);
            animate_falling_disc(&play_falling_disc_animation, &the_falling_disk, top_pad, step_y);
            draw_discs(width, height);
            draw_column_selector(current_column_index, height, left_pad, bottom_pad, step_x);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}

