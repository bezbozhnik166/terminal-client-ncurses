#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
    int len;
    char* buffer;
    int cursor;
} typeBox;

typedef struct {
    int row;
    int column;
    char* buffer[255]; 
    int curRow;
} chatBox;

void drawChat(WINDOW* mainWin, chatBox output, int drawIndex, int row){
    int screenSize = row + drawIndex;

    werase(mainWin);
    box(mainWin, 0, 0);

    int idx = 0;
    for (int i = drawIndex; i < output.curRow; i++){
        mvwprintw(mainWin, idx + 1, 1, "%d",i);
        mvwprintw(mainWin, idx + 1, 3, "%s",output.buffer[i]);
        idx++;
    }

    wrefresh(mainWin);
}

void submit(WINDOW* mainWin, typeBox input, chatBox* output){

    strcpy(output->buffer[output->curRow], input.buffer);
}

void strShiftAdd(char *str, int shift_index, char ch, int len){
    // size_t len = strlen(str);
    // if (len >= capacity - 1)
    //     return;
    
    for (int i = len; i >= shift_index; i--)
    {
        str[i + 1] = str[i];
    }

    str[shift_index] = ch;
}

void strShiftDelete(char* str, int delete_index){
    if (delete_index < 0)
        return;

    for (int i = delete_index; str[i] != '\0'; i++)
        str[i] = str[i + 1];
}

void drawInputBox(WINDOW* inputBox, typeBox input){
    werase(inputBox);
    box(inputBox, 0, 0);
    mvwprintw(inputBox, 1, 1, "%s", input.buffer);
    wrefresh(inputBox);
    // wmove(inputBox, 1, input.cursor + 1);
}

void backspaceLastChar(WINDOW* inputBox, typeBox input){
    werase(inputBox);
    box(inputBox, 0, 0);
    input.buffer[input.cursor] = '\0';
    mvwprintw(inputBox, 1, 1, "%s", input.buffer);
    wrefresh(inputBox);
}

void backspaceMidChar(WINDOW* inputBox, typeBox input){
    werase(inputBox);
    box(inputBox, 0, 0);
    strShiftDelete(input.buffer, input.cursor);
    mvwprintw(inputBox, 1, 1, "%s", input.buffer);
    wrefresh(inputBox);
    wmove(inputBox, 1, input.cursor + 1);
}

int main(int argc, char *argv[])
{
    initscr();
    noecho();

    int row, column;

    getmaxyx(stdscr, row, column);

    WINDOW *inputBox = newwin(3, column -2, row -3, 1);
    WINDOW *mainWin = newwin(row - 3, column - 2, 0, 1);

    typeBox input;
    chatBox output;
    
    getmaxyx(mainWin, output.row, output.column);

    output.curRow = 0;

    // for (int i = 0; i < 255; i++) {
    //     output.buffer[i] = malloc(column);
    // }

    input.len = 0;
    input.cursor = 0;
    input.buffer = malloc(column);

    keypad(inputBox, true);
    keypad(stdscr, true);
    wmove(inputBox, 1, 1); // add +1 when you move inside inputBox;
    box(inputBox, 0, 0);
    box(mainWin, 0, 0);
    refresh();
    wrefresh(mainWin);
    wrefresh(inputBox);

    int running = 1;
    int ch;
    while (running) {
        switch (ch = wgetch(inputBox)) {
            case KEY_UP:
                wprintw(mainWin,"KEY_UP");
                break;

            case KEY_DOWN:
                wprintw(mainWin,"KEY_DOWN");
                break;

            case KEY_RIGHT:
                input.cursor++;
                if (input.cursor > input.len)
                    input.cursor = input.len;
                wmove(inputBox, 1, input.cursor + 1);
                break;

            case KEY_LEFT:
                input.cursor--;
                if (input.cursor < 0) 
                    input.cursor = 0;
                wmove(inputBox, 1, input.cursor + 1);
                break;

            case KEY_BACKSPACE:
                input.cursor--;
                input.len--;
                if (input.len < 0)
                    input.len = 0;
                if (input.cursor < 0) 
                    input.cursor = 0;

                if (input.cursor == input.len)
                    backspaceLastChar(inputBox, input);

                else {
                    backspaceMidChar(inputBox, input);
                }

                break;

            case '\n':
                if (strcmp(input.buffer, "!exit") == OK) {
                    running = 0;
                }

                else { // why does this seg fault ?
                    output.buffer[output.curRow] = malloc(input.len);
                    strcpy(output.buffer[output.curRow], input.buffer); 
                    output.curRow++;
                    if (output.curRow > output.row) {
                        drawChat(mainWin, output, output.curRow - output.row , output.row);
                        wrefresh(mainWin);
                        input.buffer[0] = '\0';
                        input.len = 0;
                        input.cursor = 0;
                        drawInputBox(inputBox, input);
                    }
                    else {
                        drawChat(mainWin, output, 0, output.row);
                        wrefresh(mainWin);
                        input.buffer[0] = '\0';
                        input.len = 0;
                        input.cursor = 0;
                        drawInputBox(inputBox, input);
                    }
                }
                break;

            case KEY_DC:
                input.buffer[0] = '\0';
                input.len = 0;
                input.cursor = 0;
                drawInputBox(inputBox, input);
                break;

            default:
                if (input.len + 1 != column - 3) {
                    if (input.len == input.cursor) {
                        input.buffer[input.cursor] = ch;
                        input.buffer[input.cursor + 1] = '\0';
                        input.cursor++;
                        input.len++;
                        drawInputBox(inputBox, input);
                    }
                    else {     
                        strShiftAdd(input.buffer, input.cursor, ch, input.len); 
                        input.cursor++;
                        input.len++;
                        drawInputBox(inputBox, input);
                        wmove(inputBox, 1, input.cursor + 1);
                    }
                }
                break;
        }

    }

    endwin();

    printf("%s\n", input.buffer);
    printf("%d\n", input.len);
    return 0;
}
