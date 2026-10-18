#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include "c4.h"

#define LOGICAL_HEIGHT 1000
#define LOGICAL_WIDTH 1000

#define GRID_OFFSET_X 27
#define GRID_OFFSET_Y 27
#define GRID_DOT_SIZE 3.0f
#define CELL_WALL_LENGTH 86

#define MENU_GRID_LENGTH 60
#define MENU_GRID_OFFSET 20
#define MENU_GRID_DOT_RADIUS 2

#define DISC_SIZE 40

typedef struct {
    Vector2 position;
    int player;
    int column;
} falling_disc;

typedef enum { LIGHT, DARK, LIGHT_COLOR, NUMBER_OF_THEMES } theme;

typedef enum { MAIN_MENU, LOCAL_MULTIPLAYER, ONLINE_MULTIPLAYER } state;
static state current_scene;

typedef struct {
    Color disc1;
    Color disc2;
    Color background;
    Color board_grid;
    Color background_grid;
    Color column_selector;
    Color button_color;
    Color button_color_on_hover;
} color_palette;

static color_palette themes[NUMBER_OF_THEMES] = {
    [LIGHT] =
        {
            .disc1 = RED,
            .disc2 = BLUE,
            .background = WHITE,
            .board_grid = BLACK,
            .background_grid = LIGHTGRAY,
            .column_selector = GRAY,
            .button_color = RED,
            .button_color_on_hover = BLUE,
        },
    [DARK] =
        {
            .disc1 = RED,
            .disc2 = BLUE,
            .background = BLACK,
            .board_grid = WHITE,
            .background_grid = LIGHTGRAY,
            .column_selector = WHITE,
            .button_color = RED,
            .button_color_on_hover = BLUE,
        },
    [LIGHT_COLOR] =
        {
            .disc1 = (Color){0xff, 0x69, 0x78, 0xff},
            .disc2 = (Color){0x34, 0x7f, 0xc4, 0xff},
            .background = (Color){0x6d, 0x43, 0x5a, 0xff},
            .board_grid = (Color){0xff, 0xfc, 0xf9, 0xff},
            .background_grid = LIGHTGRAY,
            .column_selector = (Color){0xff, 0xe8, 0xd1, 0xff},
            .button_color = (Color){0xff, 0xe8, 0xd1, 0xff},
            .button_color_on_hover = BLUE,
        },
};

color_palette* current_theme;

void draw_discs() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            Vector2 center = {.x = (j * CELL_WALL_LENGTH) + GRID_OFFSET_X +
                                   (2.5 * CELL_WALL_LENGTH),
                              .y = (i * CELL_WALL_LENGTH) + GRID_OFFSET_Y +
                                   (4.5 * CELL_WALL_LENGTH)};

            if (get_color(i, j) == 1) {
                DrawCircleV(center, DISC_SIZE, current_theme->disc1);
            } else if (get_color(i, j) == 2) {
                DrawCircleV(center, DISC_SIZE, current_theme->disc2);
            }
        }
    }
}

void draw_grid() {
    static int steps_x = LOGICAL_WIDTH / 50;
    static int steps_y = LOGICAL_HEIGHT / 50;
    for (int i = 0; i <= steps_x; i++) {
        for (int j = 0; j <= steps_y; j++) {
            if (j > 1 && j < 10 && i > 3 && i < 11) {
                Vector2 center = {.x = (j * CELL_WALL_LENGTH) + GRID_OFFSET_X,
                                  .y = (i * CELL_WALL_LENGTH) + GRID_OFFSET_Y};
                DrawCircleV(center, GRID_DOT_SIZE,
                            current_theme->board_grid);  // playing grid
            } else {
                Vector2 center = {.x = (j * CELL_WALL_LENGTH) + GRID_OFFSET_X,
                                  .y = (i * CELL_WALL_LENGTH) + GRID_OFFSET_Y};
                DrawCircleV(center, GRID_DOT_SIZE,
                            current_theme->background_grid);  // background grid
            }
        }
    }
}
void draw_column_selector(int index) {
    Vector2 center = {.x = (index * CELL_WALL_LENGTH) + GRID_OFFSET_X +
                           (2.1 * CELL_WALL_LENGTH),
                      .y = (10.5 * CELL_WALL_LENGTH) + GRID_OFFSET_Y};
    DrawRectangleV(center, (Vector2){.x = 0.8 * CELL_WALL_LENGTH, .y = 3},
                   current_theme->column_selector);
}

void animate_falling_disc(int* play_animation, falling_disc* the_disc) {
    if (*play_animation == 1) {
        the_disc->position.y += 1200.0f * GetFrameTime();
        int height = ((get_next_row_from_column(the_disc->column) - 1) *
                      CELL_WALL_LENGTH) +
                     GRID_OFFSET_Y + (4.5 * CELL_WALL_LENGTH);
        if (the_disc->position.y >= height) {
            *play_animation = 0;
            place_disc(the_disc->column);
            printf("%d\n", evaluate_position_on_last_move(the_disc->column));
        }
        if (the_disc->player == 1) {
            DrawCircleV(the_disc->position, DISC_SIZE, current_theme->disc1);
        } else {
            DrawCircleV(the_disc->position, DISC_SIZE, current_theme->disc2);
        }
    }
}
void draw_a_grid(Camera2D cam,
                 float spacing,
                 float offset,
                 float radius,
                 Color color) {
    Vector2 top_left = GetScreenToWorld2D((Vector2){.x = 0, .y = 0}, cam);
    Vector2 bottom_right = GetScreenToWorld2D(
        (Vector2){.x = (float)GetScreenWidth(), .y = (float)GetScreenHeight()},
        cam);

    int first_column = (int)floorf((top_left.x - offset) / spacing);
    int last_column = (int)ceilf((bottom_right.x - offset) / spacing);
    int first_row = (int)floorf((top_left.y - offset) / spacing);
    int last_row = (int)ceilf((bottom_right.y - offset) / spacing);

    for (int row = first_row; row <= last_row; row++) {
        for (int column = first_column; column <= last_column; column++) {
            DrawCircleV(
                (Vector2){column * spacing + offset, row * spacing + offset},
                radius, color);
        }
    }
}
void draw_main_menu(int* current_button, Camera2D camera) {
    // const int steps = 30;
    // for (int i = 0; i < steps; i++)
    // {
    //     for (int j = 0; j < steps; j++)
    //     {
    //         Vector2 center = {.x = (j * MENU_GRID_LENGTH) + MENU_GRID_OFFSET,
    //         .y = (i * MENU_GRID_LENGTH) + MENU_GRID_OFFSET};
    //         DrawCircleV(center, MENU_GRID_DOT_RADIUS,
    //         current_theme->background_grid);
    //     }
    // }
    draw_a_grid(camera, MENU_GRID_LENGTH, MENU_GRID_OFFSET,
                MENU_GRID_DOT_RADIUS, current_theme->background_grid);
    const Vector2 button1_position = {.x = MENU_GRID_LENGTH * 2.5,
                                      .y = MENU_GRID_LENGTH * 4.5};
    const Vector2 button1_size = {.x = 300, .y = 100};
    const Vector2 button2_position = {.x = MENU_GRID_LENGTH * 2.5,
                                      .y = MENU_GRID_LENGTH * 7};
    const Vector2 button2_size = {.x = 300, .y = 100};
    const Vector2 button3_position = {.x = MENU_GRID_LENGTH * 2.5,
                                      .y = MENU_GRID_LENGTH * 9.5};
    const Vector2 button3_size = {.x = 300, .y = 100};

    switch (*current_button) {
        case 1:
            DrawRectangleV(button1_position, button1_size,
                           current_theme->button_color_on_hover);
            DrawRectangleV(button2_position, button2_size,
                           current_theme->button_color);
            DrawRectangleV(button3_position, button3_size,
                           current_theme->button_color);
            break;
        case 2:
            DrawRectangleV(button1_position, button1_size,
                           current_theme->button_color);
            DrawRectangleV(button2_position, button2_size,
                           current_theme->button_color_on_hover);
            DrawRectangleV(button3_position, button3_size,
                           current_theme->button_color);
            break;
        case 3:
            DrawRectangleV(button1_position, button1_size,
                           current_theme->button_color);
            DrawRectangleV(button2_position, button2_size,
                           current_theme->button_color);
            DrawRectangleV(button3_position, button3_size,
                           current_theme->button_color_on_hover);
            break;

        default:
            DrawRectangleV(button1_position, button1_size,
                           current_theme->button_color);
            DrawRectangleV(button2_position, button2_size,
                           current_theme->button_color);
            DrawRectangleV(button3_position, button3_size,
                           current_theme->button_color);
            break;
    }
}
void update_main_menu(int* current_button) {
    if (IsKeyPressed(KEY_N)) {
        current_scene = LOCAL_MULTIPLAYER;
    }
    if (IsKeyPressed(KEY_DOWN)) {
        *current_button += 1;
        if (*current_button == 4) {
            *current_button = 1;
        }
    } else if (IsKeyPressed(KEY_UP)) {
        *current_button -= 1;
        if (*current_button == 0) {
            *current_button = 3;
        }
    } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_SPACE) ||
               IsKeyPressed(KEY_ENTER)) {
        switch (*current_button) {
            case 1:
                current_scene = LOCAL_MULTIPLAYER;
                *current_button = 3;
                break;
            case 2:
                current_scene = ONLINE_MULTIPLAYER;
                *current_button = 3;
                break;
            case 3:
                CloseWindow();
                break;
            default:
                break;
        }
    }
}
static Camera2D fit_camera() {
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float scale = fminf(sw / LOGICAL_WIDTH, sh / LOGICAL_HEIGHT);
    return (Camera2D){
        .offset = {(sw - LOGICAL_WIDTH * scale) / 2,
                   (sh - LOGICAL_HEIGHT * scale) / 2},
        .target = {0, 0},
        .rotation = 0,
        .zoom = scale,
    };
}

void update_local_multiplayer(int* current_column_index,
                              falling_disc* the_falling_disc,
                              int* play_falling_disc_animation) {
    if (IsKeyPressed(KEY_U)) {
        undo_last_move();
    } else if (IsKeyPressed(KEY_R)) {
        while (undo_last_move() == 1) {
            ;
        }
    } else if (IsKeyPressed(KEY_RIGHT)) {
        if (*current_column_index < COLUMNS - 1) {
            *current_column_index += 1;
        }
    } else if (IsKeyPressed(KEY_LEFT)) {
        if (*current_column_index > 0) {
            *current_column_index -= 1;
        }
    } else if (IsKeyPressed(KEY_DOWN)) {
        if (*play_falling_disc_animation == 0 &&
            get_next_row_from_column(*current_column_index) != 0) {
            the_falling_disc->column = *current_column_index;
            the_falling_disc->position =
                (Vector2){.x = (*current_column_index * CELL_WALL_LENGTH) +
                               GRID_OFFSET_X + (2.5 * CELL_WALL_LENGTH),
                          .y = GRID_OFFSET_Y + (4.5 * CELL_WALL_LENGTH)};
            the_falling_disc->player = get_current_player();
            *play_falling_disc_animation = 1;
        }
    }
}

int main() {
    current_scene = MAIN_MENU;
    const int screenWidth = 620;
    const int screenHeight = 620;
    int current_column_index = 0;

    int play_falling_disc_animation = 0;
    falling_disc the_falling_disk = {0};

    Camera2D camera = {0};

    current_theme = &themes[LIGHT];
    init_c4();
    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE |
                   FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Connect Four");

    SetTargetFPS(120);
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose()) {
        switch (current_scene) {
            case MAIN_MENU:
                update_main_menu(&current_column_index);
                break;
            case LOCAL_MULTIPLAYER:
                update_local_multiplayer(&current_column_index,
                                         &the_falling_disk,
                                         &play_falling_disc_animation);
            default:
                break;
        }

        if (IsKeyPressed(KEY_ONE)) {
            current_theme = &themes[LIGHT];
        } else if (IsKeyPressed(KEY_TWO)) {
            current_theme = &themes[DARK];
        } else if (IsKeyPressed(KEY_THREE)) {
            current_theme = &themes[LIGHT_COLOR];
        }
        camera = fit_camera();
        BeginDrawing();
        ClearBackground(current_theme->background);
        BeginMode2D(camera);
        //----------
        switch (current_scene) {
            case MAIN_MENU:
                draw_main_menu(&current_column_index, camera);
                break;
            case LOCAL_MULTIPLAYER:
                draw_grid();
                animate_falling_disc(&play_falling_disc_animation,
                                     &the_falling_disk);
                draw_discs();
                draw_column_selector(current_column_index);
            default:
                break;
        }

        //---------
        EndMode2D();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}
