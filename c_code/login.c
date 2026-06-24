#include <ncurses.h>
#include <stdlib.h>

// typedef struct user {
//     char *username;
// } user;

typedef struct{
    char* username;
    int cursor;
    int len;
} Input;

void drawInput(WINDOW* inputBox, Input input){
    werase(inputBox);
    mvwprintw(inputBox, 1, 1, "%s", input.username);
    wrefresh(inputBox);
}

void initLogin(){
    initscr();
    noecho();
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    int win_height = 10;
    int win_width = 40;

    int starty = (LINES - win_height) / 2;
    int startx = (COLS - win_width) / 2;

    // mvprintw(starty + 1, startx - 10, "username:");
    WINDOW* loginWin = newwin(win_height, win_width, starty, startx);

    WINDOW* usernameBox = derwin(loginWin, 1, 10, 4 , 15);
    wbkgd(usernameBox, COLOR_PAIR(1));

    keypad(usernameBox, 1);

    mvwprintw(loginWin, 4, 6, "username:");

    Input input;
    input.username = malloc(15);
    input.cursor = 0;
    input.len = 0;
    input.username[0] = '\0';

    box(loginWin, 0, 0);
    refresh();
    wrefresh(loginWin);

    int running = 1;
    int ch;
    while (running) {
        switch (ch = wgetch(usernameBox)) {
            case KEY_BACKSPACE:

            default:
                input.cursor++;
                input.len++;
                input.username[input.cursor] = ch;
                input.username[input.cursor + 1] = '\0';
                drawInput(usernameBox, input);

        }
        wrefresh(loginWin);
        wrefresh(usernameBox);
    }
    endwin();

}

int main()
{
    initLogin();
    return EXIT_SUCCESS;
}
