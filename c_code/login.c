#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_LOCATION "passwordBox"

// typedef struct user {
//     char *username;
// } user;

typedef struct{
    char* username;
    char* password;
    int usernameBoxCursor;
    int usernameBoxLen;
    int passwordBoxCursor;
    int passwordBoxLen;
} Input;

typedef struct{
    WINDOW* usernameBox;
    WINDOW* passwordBox;
    char* login;
    char* exit;
    char* curLocation;
} Focus;

void drawInput(Focus focus, Input input){
    if (strcmp(focus.curLocation, "usernameBox") == 0){
        werase(focus.usernameBox);
        mvwprintw(focus.usernameBox, 0, 0, "%s", input.username);
        wrefresh(focus.usernameBox);
    }

    else if (strcmp(focus.curLocation, "passwordBox") == 0) {
        werase(focus.passwordBox);
        mvwprintw(focus.passwordBox, 0, 0, "%s", input.password);
        wrefresh(focus.passwordBox);
    }
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

    Focus focus;
    focus.usernameBox = usernameBox;
    focus.passwordBox = passwordBox;
    focus.login = "[login]";
    focus.exit = "[exit]";
    focus.curLocation = DEFAULT_LOCATION;

    Input input;
    input.username = malloc(26);
    input.password = malloc(26);
    input.usernameBoxCursor = 0;
    input.usernameBoxLen = 0;
    input.passwordBoxCursor = 0;
    input.passwordBoxLen = 0;
    input.username[0] = '\0';
    input.password[0] = '\0';
    
    box(loginWin, 0, 0);
    refresh();
    wrefresh(loginWin);
    wrefresh(usernameBox);
    wrefresh(passwordBox);

                      // 0              1              2          3
    // char* elements[4] = {"usernameBox", "passwordBox", "[login]", "[exit]"};

    int running = 1;
    int ch;
    while (running) {
        switch (ch = wgetch(usernameBox)) {
            case KEY_BACKSPACE: 
                if (input.usernameBoxCursor > 0 && strcmp(focus.curLocation, "usernameBox") == 0) {
                    input.usernameBoxCursor--;
                    input.usernameBoxLen--;
                    input.username[input.usernameBoxCursor] = '\0';
                    drawInput(focus, input);
                }

                else if (input.passwordBoxCursor > 0 && strcmp(focus.curLocation, "passwordBox") == 0) {
                    input.passwordBoxCursor--;
                    input.passwordBoxLen--;
                    input.password[input.passwordBoxCursor] = '\0';
                    drawInput(focus, input);
                }
                break;

            case KEY_UP:
                focus.curLocation = "usernameBox";
                break;

            case KEY_DOWN:
                focus.curLocation = "passwordBox";
                break;

            case '\n':
                running = 0;;
                break;

            default:
                if (input.usernameBoxCursor < 26 && strcmp(focus.curLocation, "usernameBox") == 0) {
                    input.username[input.usernameBoxCursor] = ch;
                    input.usernameBoxCursor++;
                    input.usernameBoxLen++;
                    input.username[input.usernameBoxCursor] = '\0';
                    drawInput(focus, input); 
                }

                if (input.usernameBoxCursor < 26 && strcmp(focus.curLocation, "passwordBox") == 0) {
                    input.password[input.passwordBoxCursor] = ch;
                    input.passwordBoxCursor++;
                    input.passwordBoxLen++;
                    input.password[input.passwordBoxCursor] = '\0';
                    drawInput(focus, input); 
                }
                break;
        }
    }

    free(input.username);
    free(input.password);
    endwin();
    printf("%s\n", focus.curLocation);
}

int main()
{
    initLogin();
    return EXIT_SUCCESS;
}
