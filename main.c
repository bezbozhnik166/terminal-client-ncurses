#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void str_delete_shift(char* str, int delete_index)
{
    if (delete_index < 0)
        return;

    for (int i = delete_index; str[i] != '\0'; i++)
    {
        str[i] = str[i + 1];
    }
}

int main(void)
{
    initscr();
    noecho();
    cbreak();

    int row, column;
    getmaxyx(stdscr, row, column);
    char* input_buffer = malloc(column*sizeof(char));
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
    int bufferlen = strlen(input_buffer);
    while ((ch = wgetch(win)) != '\n') {
        if (ch == KEY_BACKSPACE){
            werase(win);
            box(win, 0, 0);
            // printf("pos,%d\n",pos);
            // printf("bufferlen,%d\n",bufferlen);
            if (pos == bufferlen) {
                pos--;
                if (pos == -1)
                    pos = 0;
                input_buffer[pos] = '\0';
                mvwprintw(win, 1, 1,"%s", input_buffer);
                bufferlen = strlen(input_buffer);
            }
            else {
                str_delete_shift(input_buffer, pos);
                mvwprintw(win, 1, 1,"%s", input_buffer);
                bufferlen = strlen(input_buffer);
                wmove(win, 1, pos);
            }
        }

        // else if (ch == KEY_UP || ch == KEY_DOWN) {
        //     continue;
        // }

        else if (ch == KEY_RIGHT) {
            pos++;
            if (pos >= bufferlen)
                pos = bufferlen + 1;
            wmove(win, 1, pos);
        }

        else if (ch == KEY_LEFT) {
            pos--;
            if (pos == -1)
                pos = 0;
            wmove(win, 1, pos);
        }

        else {
            werase(win);
            box(win, 0, 0);
            wmove(win, 1, pos);
            input_buffer[pos] = ch;
            input_buffer[pos + 1] = '\0';
            mvwprintw(win, 1, 1,"%s", input_buffer);
            pos++;
            bufferlen = strlen(input_buffer);
        }
    }
    endwin();

    printf("%d\n",bufferlen);
    free(input_buffer);
    return 0;
}
