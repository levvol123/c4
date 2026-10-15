#ifndef C4_H
#define C4_H

#define ROWS 6
#define COLUMNS 7

int get_color(int row, int column);
int evaluate_position_on_last_move(int column);
void place_disc(int column);
void init_c4();
int undo_last_move();
int get_next_row_from_column(int column);
int get_current_player();

#endif