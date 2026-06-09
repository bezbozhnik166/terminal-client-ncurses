#include <ncurses.h>
#include <stdbool.h>

int main(void)
{
    initscr();
    noecho();
    cbreak();

    int row, column;
    getmaxyx(stdscr, row, column);
    char input_buffer[column]; 
    input_buffer[0] = '\0';

    WINDOW *win = newwin(3, column -2, row -3, 1);
    keypad(win, true);
    keypad(stdscr, true);
    wmove(win, 1, 1);
    box(win, 0, 0);
    refresh();
    wrefresh(win);

    int ch;
    int pos = 0;
    while ((ch = wgetch(win)) != '\n') {
        if (ch == KEY_BACKSPACE){
            werase(win);
            box(win, 0, 0);
            pos--;
            if (pos == -1)
                pos = 0;
            input_buffer[pos] = '\0';
            mvwprintw(win, 1, 1,"%s", input_buffer);
        }

        else if (ch == KEY_RIGHT) {
            pos++;
            wmove(win, 1, pos);
        }

        else if (ch == KEY_LEFT) {
            pos--;
            wmove(win, 1, pos);
        }

        else {
            werase(win);
            box(win, 0, 0);
            input_buffer[pos] = ch;
            input_buffer[pos + 1] = '\0';
            mvwprintw(win, 1, 1,"%s", input_buffer);
            pos++;
        }
    }

    endwin();

    return 0;
}
