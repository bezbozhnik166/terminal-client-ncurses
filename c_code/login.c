#include <ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DEFAULT_LOCATION "usernameBox"

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
    WINDOW* opts;
    char* optFocus;
    char* curLocation;
    int curLocationInt;
} Focus;

void strShiftDelete(char* str, int delete_index){
    if (delete_index < 0)
        return;

    for (int i = delete_index; str[i] != '\0'; i++)
        str[i] = str[i + 1];
}

void strShiftAdd(char *str, int shift_index, char ch, int len){
    
    for (int i = len; i >= shift_index; i--)
    {
        str[i + 1] = str[i];
    }

    str[shift_index] = ch;
}

void refreshOpts(Focus focus, char* highlight){
	if (strcmp(highlight, "login") == 0) {
		werase(focus.opts);
		wattron(focus.opts, A_REVERSE);
		mvwprintw(focus.opts, 0, 17, "[login]");
		wattroff(focus.opts, A_REVERSE);
		mvwprintw(focus.opts, 0, 26, "[exit]");
	}

	else if (strcmp(highlight, "exit") == 0) {
		werase(focus.opts);
		wattron(focus.opts, A_REVERSE);
		mvwprintw(focus.opts, 0, 26, "[exit]");
		wattroff(focus.opts, A_REVERSE);
		mvwprintw(focus.opts, 0, 17, "[login]");
	}

	else if (strcmp(highlight, "") == 0) {
		werase(focus.opts);
		mvwprintw(focus.opts, 0, 17, "[login]");
		mvwprintw(focus.opts, 0, 26, "[exit]");
	}
	wrefresh(focus.opts);
}

void checkFocus(Focus* focus, char* verElements[]){
    for (int i = 0; i < 3; i++) {
        if (strcmp(focus->curLocation, verElements[i]) == 0) { // maybe make this it's own function
            focus->curLocationInt = i;
        }
    }
}

void dynamicRefresh(Focus* focus){
	if (strcmp(focus->curLocation, "usernameBox") == 0) {
		wrefresh(focus->usernameBox);
	}

	else if (strcmp(focus->curLocation, "passwordBox") == 0) {
		curs_set(1);
		refreshOpts(*focus, ""); // this is to stop highlighting opts
		wrefresh(focus->passwordBox);
	}

	else if (strcmp(focus->curLocation, "opts") == 0) {
		curs_set(0);
		refreshOpts(*focus, "login");
	}
}

void changeFocus(Focus* focus, bool increment, char* verElements[]){
    checkFocus(focus, verElements);

    if (increment == true ) {
        focus->curLocationInt++;
	if (focus->curLocationInt > 2)
	    focus->curLocationInt = 2;
        focus->curLocation = verElements[focus->curLocationInt]; 
    }

    if (increment == false ) {
        focus->curLocationInt--;
	if (focus->curLocationInt < 0)
	    focus->curLocationInt = 0;
        focus->curLocation = verElements[focus->curLocationInt]; 
    }
	
}

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

    WINDOW* opts = derwin(loginWin, 1, win_width - 2, 7, 1);
    // wbkgd(opts, COLOR_PAIR(1));

    mvwprintw(loginWin, 3, 2, "username:");
    mvwprintw(loginWin, 5, 2, "password:");

    Focus focus;
    focus.usernameBox = usernameBox;
    focus.passwordBox = passwordBox;
    focus.opts = opts;
    focus.optFocus = "";
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

    refreshOpts(focus, "");
    wrefresh(loginWin);
    wrefresh(opts);
    wrefresh(passwordBox);
    wrefresh(usernameBox);

    char* verElements[3] = {"usernameBox", "passwordBox", "opts"};

    int running = 1;
    int ch;
    while (running) {
        switch (ch = wgetch(usernameBox)) {
	    case KEY_BACKSPACE: 
                if (input.usernameBoxCursor > 0 && strcmp(focus.curLocation, "usernameBox") == 0 && input.usernameBoxLen > 0) {
		    input.usernameBoxCursor--;
		    input.usernameBoxLen--;

		    if(input.usernameBoxLen == input.usernameBoxCursor){
			input.username[input.usernameBoxCursor] = '\0';
			drawInput(focus, input);
		    }

		    else {
			strShiftDelete(input.username, input.usernameBoxCursor);
			drawInput(focus, input);
			wmove(focus.usernameBox, 0, input.usernameBoxCursor);
			wrefresh(focus.usernameBox);
		    }
                }

                else if (input.passwordBoxCursor > 0 && strcmp(focus.curLocation, "passwordBox") == 0 && input.passwordBoxLen > 0) {
                    input.passwordBoxCursor--;
                    input.passwordBoxLen--;

		    if (input.passwordBoxLen == input.passwordBoxCursor) {
			input.password[input.passwordBoxCursor] = '\0';
			drawInput(focus, input);
		    }

		    else {
			strShiftDelete(input.password, input.passwordBoxCursor);
			drawInput(focus, input);
			wmove(focus.passwordBox, 0, input.passwordBoxCursor);
			wrefresh(focus.passwordBox);
		    }

                }
                break;

	    case KEY_LEFT:
		    if (strcmp(focus.curLocation, "opts") == 0) {
			    refreshOpts(focus, "login");
		    }

		    else if (strcmp(focus.curLocation, "usernameBox") == 0) {
			    input.usernameBoxCursor--;
			    if (input.usernameBoxCursor < 0)
				    input.usernameBoxCursor = 0;
			    wmove(focus.usernameBox, 0, input.usernameBoxCursor);
			    wrefresh(usernameBox);
		    }

		    else if (strcmp(focus.curLocation, "passwordBox") == 0) {
			    input.passwordBoxCursor--;
			    if (input.passwordBoxCursor < 0)
				    input.passwordBoxCursor = 0;
			    wmove(focus.passwordBox, 0, input.passwordBoxCursor);
			    wrefresh(passwordBox);
		    }
		    break;

	    case KEY_RIGHT: 
		    if (strcmp(focus.curLocation, "opts") == 0) {
			    refreshOpts(focus, "exit");
		    }

		    else if (input.usernameBoxCursor < 25 && strcmp(focus.curLocation, "usernameBox") == 0) {
			input.usernameBoxCursor++;
			if (input.usernameBoxCursor > input.usernameBoxLen)
			    input.usernameBoxCursor = input.usernameBoxLen;
			wmove(focus.usernameBox, 0, input.usernameBoxCursor);
			wrefresh(usernameBox);
		    }

		    else if (input.passwordBoxCursor < 25 && strcmp(focus.curLocation, "passwordBox") == 0) {
			input.passwordBoxCursor++;
			if (input.passwordBoxCursor > input.passwordBoxLen)
			    input.passwordBoxCursor = input.passwordBoxLen;
			wmove(focus.passwordBox, 0, input.passwordBoxCursor);
			wrefresh(passwordBox);
		    }
		    break;

            case KEY_UP:
                changeFocus(&focus, false, verElements);
                dynamicRefresh(&focus); 
                break;

            case KEY_DOWN:
                changeFocus(&focus, true, verElements);
                dynamicRefresh(&focus);
                break;

            case '\n':
                running = 0;
                break;

	    default: 
		if (input.usernameBoxCursor < 25 && strcmp(focus.curLocation, "usernameBox") == 0 && input.usernameBoxLen < 25) {
		    if (input.usernameBoxLen == input.usernameBoxCursor){
			input.username[input.usernameBoxCursor] = ch;
			input.usernameBoxCursor++;
			input.usernameBoxLen++;
			input.username[input.usernameBoxCursor] = '\0';
			drawInput(focus, input); 
		    }
		    
		    else {
			strShiftAdd(input.username, input.usernameBoxCursor, ch, input.usernameBoxLen);
			input.usernameBoxCursor++;
                        input.usernameBoxLen++;
			drawInput(focus, input);
                        wmove(focus.usernameBox, 0, input.usernameBoxCursor);
			wrefresh(focus.usernameBox);
		    }
		}

                if (input.passwordBoxCursor < 25 && strcmp(focus.curLocation, "passwordBox") == 0 && input.passwordBoxLen < 25) {
		    if (input.passwordBoxLen == input.passwordBoxCursor){
			input.password[input.passwordBoxCursor] = ch;
			input.passwordBoxCursor++;
			input.passwordBoxLen++;
			input.password[input.passwordBoxCursor] = '\0';
			drawInput(focus, input); 
		    }

		    else {
			strShiftAdd(input.password, input.passwordBoxCursor, ch, input.passwordBoxLen);
			input.passwordBoxCursor++;
                        input.passwordBoxLen++;
			drawInput(focus, input);
                        wmove(focus.passwordBox, 0, input.passwordBoxCursor);
			wrefresh(focus.passwordBox);
		    }
                }
                break;
        }
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
