#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WIDTH 80
#define HEIGHT 25
//          Change color cell   поле всегда белого цвета

#define CELL_DRAW "o"
#define otstup " "
void init_colors();
int init_ncurses(void);
int row_mass(const int *array[], int j);
void zero_pole(int array[][WIDTH]);
void draw_cell(const int array[][WIDTH],int color_num);
int cell_mass(const int array[][WIDTH], int i, int j);
void change_speed(int ch, int *speed, int *endgame, int *color_num,int *paused);
void change_mtr(int array[][WIDTH], int temp_array[][WIDTH]);

void live_or_death(int array[][WIDTH], int temp_array[][WIDTH]);
void read_stdin_cell_mass(int cell_mass[HEIGHT][WIDTH]);

int main() {
    int color_num = 2;
    int array[HEIGHT][WIDTH];
    int temp_array[HEIGHT][WIDTH];

    zero_pole(array);
    zero_pole(temp_array);
    int gameover = 1;
    int speed = 12;
    int paused = 0;
    read_stdin_cell_mass(array);

    if (init_ncurses()) {
        return 1;
    }
    clear();
    while (gameover == 1) {
        
        int ch = getch();
        change_speed(ch, &speed, &gameover,&color_num,&paused);

        if(!paused) { 
            live_or_death(array, temp_array);
            change_mtr(array, temp_array);
        }
        draw_cell(array,color_num);
        if(paused) { 
            mvprintw(0, 5, "=== PAUSED === Press 's' to continue | Current color: %d", color_num);
        } else {
            mvprintw(0, 5, "=== RUNNING == Press 's' to pause    | Current color: %d", color_num);
        }
        mvprintw(1,5,"Arrows up,down --> Change color cell ");
        mvprintw(2,5,"Speed of simulation --> %i | a+ z-| Space - endgame\n", 21 - speed);
        refresh();
        if(!paused){
            napms(speed * 10);
        }else{
            napms(10);
        }
    }
    endwin();
    return 0;
}

/* считываем из файла входные*/
void read_stdin_cell_mass(int cell_mass[HEIGHT][WIDTH]) {
    int row = 0;
    int col = 0;
    int ch;
    int total_chars = 0;
    int max_chars = HEIGHT * WIDTH + HEIGHT;  // 25*80 + 25 переносов

    while (row < HEIGHT && total_chars < max_chars && (ch = fgetc(stdin)) != EOF) {
        total_chars++;
        if (ch == '0' || ch == '1') {
            if (col < WIDTH) {
                cell_mass[row][col] = (ch == '1') ? 1 : 0;
                col++;
            }
        } else if (ch == '\n' || ch == '\r') {
            if (col > 0) {
                row++;
                col = 0;
            }
        }
    }
}
/*

определение жизни клетки

*/
void live_or_death(int array[][WIDTH], int temp_array[][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int check = cell_mass(array, i, j);
            if (array[i][j]) {
                if ((check > 3) || (check < 2)) {
                    temp_array[i][j] = 0;
                } else {
                    temp_array[i][j] = 1;
                }
            } else {
                if (check == 3) {
                    temp_array[i][j] = 1;
                } else {
                    temp_array[i][j] = 0;
                }
            }
        }
    }
}
void draw_cell(const int array[][WIDTH],int color_num) {
    // заполнение клеток на основе нулей и едениц
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (array[i][j]) {
                attron(COLOR_PAIR(color_num) | A_BOLD);
                mvprintw(i + 2, j + 5, CELL_DRAW);
                attroff(COLOR_PAIR(color_num) | A_BOLD);
            }  // █

            else {
                attron(COLOR_PAIR(1));
                mvprintw(i + 2, j + 5, "'");
                attroff(COLOR_PAIR(1));
            }
        }
    }
}
void change_speed(int ch, int *speed, int *endgame, int *color_num,int *paused) {
    if(ch == KEY_UP && *color_num<8){
        (*color_num)++;
    }else if (ch == KEY_DOWN && *color_num > 2) {
        (*color_num)--;
    }else if (ch == 'a' && *speed > 1) {
        (*speed)--;
    } else if (ch == 'z' && *speed < 20) {
        (*speed)++;
    }else if (ch == 's') {
        *paused = !*paused; // переключаем паузу
        flushinp();
    }else if (ch == ' ') {
        *endgame = 0;
    }
}

int init_ncurses(void) {
    if (freopen("/dev/tty", "r", stdin) == NULL) {
        printf("Ошибка чтения файла");
        return 1;
    };
    initscr();              // Initialization screen
    noecho();               // cursos visible false
    nodelay(stdscr, TRUE);  // delay screen true
    keypad(stdscr, TRUE);

    curs_set(0);
    init_colors();
    return 0;
}
void init_colors() {
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_WHITE);
    init_pair(2, COLOR_BLACK, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_CYAN);
    init_pair(4, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(5, COLOR_BLUE, COLOR_BLUE);
    init_pair(6, COLOR_GREEN, COLOR_GREEN);
    init_pair(7, COLOR_RED, COLOR_RED);
    init_pair(8, COLOR_YELLOW, COLOR_YELLOW);
}
void zero_pole(int array[][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            array[i][j] = 0;
        }
    }
}

void change_mtr(int array[][WIDTH], int temp_array[][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            array[i][j] = temp_array[i][j];
        }
    }
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            temp_array[i][j] = 0;
        }
    }
}

int cell_mass(const int array[][WIDTH], int i, int j) {
    int summ = 0;
    if ((i - 1) < 0) {
        const int *array2[3] = {array[i + 1], array[i], array[HEIGHT - 1]};
        summ += row_mass(array2, j);
    } else if ((i + 1) == HEIGHT) {
        const int *array2[3] = {array[i - 1], array[i], array[0]};
        summ += row_mass(array2, j);
    } else {
        const int *array2[3] = {array[i - 1], array[i], array[i + 1]};
        summ += row_mass(array2, j);
    }
    return summ;
}

/*
просчёт соседей
принимает координату клетки из массива
*/
int row_mass(const int *array[], int j) {
    int summ = 0;
    if (j - 1 < 0) {
        summ += array[0][WIDTH - 1] + array[0][0] + array[0][1];
        summ += array[1][WIDTH - 1] + array[1][1];
        summ += array[2][WIDTH - 1] + array[2][0] + array[2][1];
    } else if (j + 1 == WIDTH) {
        summ += array[0][j - 1] + array[0][j] + array[0][0];
        summ += array[1][j - 1] + array[1][0];
        summ += array[2][j - 1] + array[2][j] + array[2][0];
    } else {
        summ += array[0][j - 1] + array[0][j] + array[0][j + 1];
        summ += array[1][j - 1] + array[1][j + 1];
        summ += array[2][j - 1] + array[2][j] + array[2][j + 1];
    }
    return summ;
}