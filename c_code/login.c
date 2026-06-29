#include <ncurses.h>
#include <stdlib.h>

// typedef struct user {
//     char *username;
// } user;

typedef struct{
    char* username;
    char* password;
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
    use_default_colors();
    init_pair(1, COLOR_BLACK ,COLOR_WHITE);

    int win_height = 10;
    int win_width = 40;

    int starty = (LINES - win_height) / 2;
    int startx = (COLS - win_width) / 2;

    // mvprintw(starty + 1, startx - 10, "username:");
    WINDOW* loginWin = newwin(win_height, win_width, starty, startx);

    WINDOW* usernameBox = derwin(loginWin, 1, win_width - 14, 3, 12); 
    wbkgd(usernameBox, COLOR_PAIR(1));
    keypad(usernameBox, 1);

    // WINDOW* passwordBox = derwin(loginWin, 1, win_width - 12, 6, 12);  
    WINDOW* passwordBox = derwin(loginWin, 1, win_width - 14, 5, 12);  
    wbkgd(passwordBox, COLOR_PAIR(1));
    keypad(passwordBox, 1);

    mvwprintw(loginWin, 3, 2, "username:");
    mvwprintw(loginWin, 5, 2, "password:");

    wattron(loginWin, A_REVERSE);
    mvwprintw(loginWin, 7, win_width - 13, "[login]");
    wattroff(loginWin, A_REVERSE);
    mvwprintw(loginWin, 7, win_width - 20, "[exit]");

    Input input;
    input.username = malloc(26);
    input.password = malloc(26);
    input.cursor = 0;
    input.len = 0;
    input.username[0] = '\0';
    
    box(loginWin, 0, 0);
    refresh();
    wrefresh(loginWin);
    wrefresh(usernameBox);
    wrefresh(passwordBox);

    //                   0              1              2          3
    char* elements[4] = {"usernameBox", "passwordBox", "[login]", "[exit]"};

    int running = 1;
    int ch;
    while (running) {
        switch (ch = wgetch(usernameBox)) {
            case KEY_BACKSPACE: // in the current version why is the backspace not working
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
                if (input.cursor < 26) {
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
    free(input.password);
    endwin();
}

int main()
{
    initLogin();
    return EXIT_SUCCESS;
}
