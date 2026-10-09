#include <stdio.h>

#define BOARD_SIZE 7*6
#define ROWS 6
#define COLUMNS 7

typedef enum {EMPTY, DISC_RED, DISC_BLUE} disc;

disc discs[ROWS][COLUMNS] = {0};
int next_disc[COLUMNS];
disc next_colour;

int column_moves[BOARD_SIZE];
int row_moves[BOARD_SIZE];
int number_of_moves;


void init_c4(){
    for (int i = 0; i < COLUMNS; i++)
    {
        next_disc[i] = ROWS;
    }
    next_colour = DISC_RED;
}

int get_color(int row, int column){
    switch (discs[row][column])
    {
        case DISC_RED:
            return 1;
            break;
        case DISC_BLUE:
            return 2;
            break;    
        default:
            return 0;
            break;
    }
}
void place_disc(int column){
    if(next_disc[column] > 0){
        next_disc[column] -=1; 

        discs[next_disc[column]][column] = next_colour;
        //printf("PLACED AT ROW:%d, COLUMN:%d \n",next_disc[column], column);

        if(next_colour == DISC_RED){
            next_colour = DISC_BLUE;
        }
        else{
            next_colour = DISC_RED;
        }
        number_of_moves +=1;
        column_moves[number_of_moves] = column;
        row_moves[number_of_moves] = next_disc[column];
    }
}

void undo_last_move(){
    if(number_of_moves > 0){
        discs[row_moves[number_of_moves]][column_moves[number_of_moves]] = EMPTY;
        next_disc[column_moves[number_of_moves]] +=1;
        number_of_moves -=1;
        if(next_colour == DISC_RED){
            next_colour = DISC_BLUE;
        }
        else{
            next_colour = DISC_RED;
        }
    }

}

int evaluate_position(){
    int count = 0;
    //evaluate rows ->start at ---x---
    for (int i = 0; i < ROWS; i++)
    {
        /* code */
    }
    
    //evaluate columns
    //evaluate diagonals


    return 0;
}

int evaluate_position_on_last_move(int column){
    int red;
    int blue;
    //evaluate row ->start at ---x---

    



    //evaluate column

    
    
    //evaluate diagonals
}
