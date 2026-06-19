#include <ncurses.h>

typedef struct user {
    char *username;
}user;

void initLogin(){
    initscr();

    int win_height = 3;
    int win_width = 20;

    int starty = (LINES - win_height) / 2;
    int startx = (COLS - win_width) / 2;

    mvprintw(starty + 1, startx - 10, "username:");
    WINDOW *usernameBox = newwin(win_height, win_width, starty, startx);
    box(usernameBox, 0, 0);
    refresh();
    wrefresh(usernameBox);

    getch();
    endwin();

}

int main(void)
{
    initLogin();
    return 0;
}
