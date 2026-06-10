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

void str_add_shift(char *str, int shift_index, char ch)
{
    size_t len = strlen(str);

    for (int i = len; i >= shift_index; i--)
    {
        str[i + 1] = str[i];
    }

    str[shift_index] = ch;
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
    WINDOW *main_win = newwin(row - 3, column - 2, 0, 1);
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
        bufferlen = strlen(input_buffer);

        if (ch == KEY_BACKSPACE){
            werase(win);
            box(win, 0, 0);

            if (pos == bufferlen) {
                pos--;
                if (pos == -1)
                    pos = 0;
                input_buffer[pos] = '\0';
                mvwprintw(win, 1, 1,"%s", input_buffer);
                bufferlen = strlen(input_buffer);
            }
            else {
                pos--;
                str_delete_shift(input_buffer, pos);
                mvwprintw(win, 1, 1,"%s", input_buffer);
                bufferlen = strlen(input_buffer);
                wmove(win, 1, pos + 1);
            }
        }

        else if (ch == KEY_RIGHT) {
            pos++;
            if (pos > bufferlen)
                pos = bufferlen;
            wmove(win, 1, pos + 1);
        }

        else if (ch == KEY_LEFT) {
            pos--;
            if (pos == -1)
                pos = 0;
            wmove(win, 1, pos + 1);
        }

        else {
            werase(win);
            box(win, 0, 0);
            wmove(win, 1, pos + 1);
            input_buffer[pos] = ch;
            if (pos == bufferlen)
            {
                input_buffer[pos + 1] = '\0';
                mvwprintw(win, 1, 1,"%s", input_buffer);
                pos++;
            }
            else {
                str_add_shift(input_buffer, pos, ch);
                mvwprintw(win, 1, 1,"%s", input_buffer);
                pos++;
            }
        }
        werase(main_win);
        box(main_win, 0, 0);
        mvwprintw(main_win,1, 1, "pos: %d",pos);
        mvwprintw(main_win,2, 1, "bufferlen: %d",bufferlen);
        wrefresh(main_win);
    }
    endwin();

    printf("%d\n",bufferlen);
    free(input_buffer);
    return 0;
}
