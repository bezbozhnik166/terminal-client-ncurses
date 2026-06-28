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
    mvwprintw(inputBox, 0, 0, "%s", input.username);
    wrefresh(inputBox);
}

void initLogin(){
    initscr();
    noecho();
    start_color();
    // init_pair(1, COLOR_BLACK, COLOR_RED);
    init_pair(1, COLOR_BLACK ,COLOR_WHITE);
    // init_pair(2, COLOR_WHITE ,COLOR_BLACK);
    int win_height = 10;
    int win_width = 40;

    int starty = (LINES - win_height) / 2;
    int startx = (COLS - win_width) / 2;

    // mvprintw(starty + 1, startx - 10, "username:");
    WINDOW* loginWin = newwin(win_height, win_width, starty, startx);

    WINDOW* usernameBox = derwin(loginWin, 1, win_width - 12, 4, 8); //why is it that neither of these windows get highlighted ? Am i adding them to the wrong location ?
    wbkgd(usernameBox, COLOR_PAIR(1));

    WINDOW* passwordBox = derwin(loginWin, 1, win_width - 12, 6, 8);  //why is it that neither of these windows get highlighted ? Am i adding them to the wrong location ?
    wbkgd(passwordBox, COLOR_PAIR(1));

    wbkgd(stdscr, COLOR_PAIR(2));


    mvwprintw(loginWin, 4, 2, "username:");
    mvwprintw(loginWin, 6, 2, "password:");
    Input input;
    input.username = malloc(15);
    input.cursor = 0;
    input.len = 0;
    input.username[0] = '\0';

    box(loginWin, 0, 0);
    refresh();
    wrefresh(loginWin);
    wrefresh(usernameBox);
    wrefresh(passwordBox);

    int running = 1;
    int ch;
    while (running) {
        switch (ch = wgetch(usernameBox)) {
            case KEY_BACKSPACE:
                if (input.cursor > 0) {
                    input.cursor--;
                    input.len--;
                    input.username[input.cursor] = '\0';
                    drawInput(usernameBox, input);
                }
                break;

            case KEY_UP:
                break;

            case KEY_DOWN:
                break;

            case '\n':
                running = 0;;
                break;

            default:
                if (input.cursor < 14) {
                    input.username[input.cursor] = ch;
                    input.cursor++;
                    input.len++;
                    input.username[input.cursor] = '\0';
                    drawInput(usernameBox, input);
                }
                break;

        }
        wrefresh(loginWin);
        wrefresh(usernameBox);
    }

    free(input.username);
    endwin();
}

int main()
{
    initLogin();
    return EXIT_SUCCESS;
}
