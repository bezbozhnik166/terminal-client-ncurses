#include <ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// typedef enum {
//     UP,
//     DOWN
// }upDown;

typedef enum {
    FOCUS_USERNAME,
    FOCUS_PASSWORD,
    OPTIONS
}FocusLocation;

typedef enum {
    LOGIN,
    EXIT,
    EMPTY
}OptsFocus;

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
    OptsFocus optFocus;
    FocusLocation curLocation;
} Focus;

void strShiftDelete(char* str, int delete_index){
    if (delete_index < 0)
        return;

    for (int i = delete_index; str[i] != '\0'; i++)
        str[i] = str[i + 1];
}

void strShiftAdd(char *str, int shift_index, char ch, int len){
    
    for (int i = len; i >= shift_index; i--)  str[i + 1] = str[i];

    str[shift_index] = ch;
}

void refreshOpts(Focus focus, OptsFocus highlight){
    if (highlight == LOGIN) {
	werase(focus.opts);
	wattron(focus.opts, A_REVERSE);
	mvwprintw(focus.opts, 0, 17, "[login]");
	wattroff(focus.opts, A_REVERSE);
	mvwprintw(focus.opts, 0, 26, "[exit]");
    }

    else if (highlight == EXIT) {
	werase(focus.opts);
	wattron(focus.opts, A_REVERSE);
	mvwprintw(focus.opts, 0, 26, "[exit]");
	wattroff(focus.opts, A_REVERSE);
	mvwprintw(focus.opts, 0, 17, "[login]");
    }

    else if (highlight == EMPTY) {
	werase(focus.opts);
	mvwprintw(focus.opts, 0, 17, "[login]");
	mvwprintw(focus.opts, 0, 26, "[exit]");
    }
    wrefresh(focus.opts);
}

void dynamicRefresh(Focus* focus){
    switch (focus->curLocation) {
	case FOCUS_USERNAME:
	    wrefresh(focus->usernameBox);
	    break;

	case FOCUS_PASSWORD:
	    curs_set(1);
	    refreshOpts(*focus, EMPTY); // this is to stop highlighting opts
	    wrefresh(focus->passwordBox);
	    break;

	case OPTIONS:
	    curs_set(0);
	    refreshOpts(*focus, LOGIN);
	    break;
    }

}

void moveFocus(Focus* focus, bool increment){
    if (increment) {
        if (focus->curLocation < OPTIONS)
            focus->curLocation++;
    } else {
        if (focus->curLocation > FOCUS_USERNAME)
            focus->curLocation--;
    }
}

void drawInput(Focus focus, Input input){
    if ( focus.curLocation == FOCUS_USERNAME){
        werase(focus.usernameBox);
        mvwprintw(focus.usernameBox, 0, 0, "%s", input.username);
        wrefresh(focus.usernameBox);
    }

    else if ( focus.curLocation == FOCUS_PASSWORD) {
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

    WINDOW* loginWin = newwin(win_height, win_width, starty, startx);

    WINDOW* usernameBox = derwin(loginWin, 1, win_width - 14, 3, 12); 
    wbkgd(usernameBox, COLOR_PAIR(1));
    keypad(usernameBox, 1);

    WINDOW* passwordBox = derwin(loginWin, 1, win_width - 14, 5, 12);  
    wbkgd(passwordBox, COLOR_PAIR(1));
    keypad(passwordBox, 1);

    WINDOW* opts = derwin(loginWin, 1, win_width - 2, 7, 1);

    mvwprintw(loginWin, 3, 2, "username:");
    mvwprintw(loginWin, 5, 2, "password:");

    Focus focus;
    focus.usernameBox = usernameBox;
    focus.passwordBox = passwordBox;
    focus.opts = opts;
    focus.optFocus = EMPTY;
    focus.curLocation = FOCUS_USERNAME;

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

    refreshOpts(focus, EMPTY);
    wrefresh(loginWin);
    wrefresh(opts);
    wrefresh(passwordBox);
    wrefresh(usernameBox);

    int running = 1;
    int ch;
    while (running) {
        switch (ch = wgetch(usernameBox)) {
	    case KEY_BACKSPACE: 
			if (input.usernameBoxCursor > 0 &&  focus.curLocation == FOCUS_USERNAME && input.usernameBoxLen > 0) {
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

			else if (input.passwordBoxCursor > 0 &&  focus.curLocation == FOCUS_PASSWORD && input.passwordBoxLen > 0) {
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
		if ( focus.curLocation == OPTIONS) {
		    refreshOpts(focus, LOGIN);
		}

		else if ( focus.curLocation == FOCUS_USERNAME) {
		    input.usernameBoxCursor--;
		    if (input.usernameBoxCursor < 0) input.usernameBoxCursor = 0;
		    wmove(focus.usernameBox, 0, input.usernameBoxCursor);
		    wrefresh(usernameBox);
		}

		else if ( focus.curLocation == FOCUS_PASSWORD) {
		    input.passwordBoxCursor--;
		    if (input.passwordBoxCursor < 0) input.passwordBoxCursor = 0;
		    wmove(focus.passwordBox, 0, input.passwordBoxCursor);
		    wrefresh(passwordBox);
		}
		break;

	    case KEY_RIGHT: 
		if ( focus.curLocation == OPTIONS) refreshOpts(focus, EXIT);

		else if (input.usernameBoxCursor < 25 &&  focus.curLocation == FOCUS_USERNAME) {
		    input.usernameBoxCursor++;
		    if (input.usernameBoxCursor > input.usernameBoxLen) input.usernameBoxCursor = input.usernameBoxLen;
		    wmove(focus.usernameBox, 0, input.usernameBoxCursor);
		    wrefresh(usernameBox);
		}

		else if (input.passwordBoxCursor < 25 &&  focus.curLocation == FOCUS_PASSWORD) {
		    input.passwordBoxCursor++;
		    if (input.passwordBoxCursor > input.passwordBoxLen) input.passwordBoxCursor = input.passwordBoxLen;
		    wmove(focus.passwordBox, 0, input.passwordBoxCursor);
		    wrefresh(passwordBox);
		}
		break;

		case KEY_UP:
			moveFocus(&focus, false);
			dynamicRefresh(&focus); 
			break;

		case KEY_DOWN:
			moveFocus(&focus, true);
			dynamicRefresh(&focus);
			break;

		case '\n':
			running = 0;
			break;

	    default: 
		if (input.usernameBoxCursor < 25 &&  focus.curLocation == FOCUS_USERNAME && input.usernameBoxLen < 25) {
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

			if (input.passwordBoxCursor < 25 &&  focus.curLocation == FOCUS_PASSWORD && input.passwordBoxLen < 25) {
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
