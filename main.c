#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>

void str_delete_shift(char *str, int delete_index){

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
    bool exc = false;
    getmaxyx(stdscr, row, column);
    char input[column]; //dynamically allocate space

    WINDOW *win = newwin(3, column -2, row -3, 1);
    keypad(win, true);
    keypad(stdscr, true);
    wmove(win, 1, 1);
    box(win, 0, 0);
    refresh();
    wrefresh(win);

    int ch;
    int pos = 0;
    int input_length;
    while ((ch = wgetch(win)) != 'q') {
        if (pos != column - 4 && ch == KEY_BACKSPACE){ //remove from input
            mvwaddch(win, 1, pos, ' ');
            pos--;
        }

        else if (pos != column - 4 && ch == KEY_RIGHT){
            wmove(win, 1, pos+1);
            pos++;
        }

        else if (pos != column - 4 && ch == KEY_LEFT){
            wmove(win, 1, pos-1);
            pos--;
        }
        
        else if (pos != column - 4){
            if (pos < input_length){
                winsch(win, ch);
                wmove(win, 1, pos);
                pos++;
            }

            else{
                waddch(win, ch);
                input[pos] = ch;
                pos++;
            }
            wrefresh(win);
        }

        else {
            exc = true;
            break;
        }
    }

    endwin();

    if (exc == true)
        printf("you exceeded the input buffer\n");

    
    printf("input is: ");
    for (int i = 0; i < pos; i++){
        printf("%c", input[i]);
    }
    
    return 0;
}
