#include <ncurses.h>
#include "login.h"
#include <stdlib.h>

typedef struct{
    char* username;
    char* password;
    int usernameBoxCursor;
    int usernameBoxLen;
    int passwordBoxCursor;
    int passwordBoxLen;
} Input;

typedef enum {
	LOGIN,
	EXIT,
	EMPTY
} Opts;

typedef enum {
	FOCUS_USERNAME,
	FOCUS_PASSWORD,
	FOCUS_LOGIN,
	FOCUS_EXIT,
	FOCUS_COUNT
} Focus;

typedef enum {
	DIR_UP,
	DIR_DOWN,
	DIR_RIGHT,
	DIR_LEFT,
	NEXT
} Direction;

typedef struct {
	WINDOW* loginWin;
	WINDOW* usernameBox;
	WINDOW* passwordBox;
    WINDOW* opts;
	Focus curFocus;
}Ui;

static const Focus navigation[FOCUS_COUNT][5] = {
	[FOCUS_USERNAME] = {
		[DIR_UP] = FOCUS_LOGIN,
		[DIR_DOWN] = FOCUS_PASSWORD,
		[DIR_RIGHT] = FOCUS_USERNAME,
		[DIR_LEFT] = FOCUS_USERNAME,
		[NEXT] = FOCUS_PASSWORD

	},

	[FOCUS_PASSWORD] = {
		[DIR_UP] = FOCUS_USERNAME,
		[DIR_DOWN] = FOCUS_LOGIN,
		[DIR_RIGHT] = FOCUS_PASSWORD,
		[DIR_LEFT] = FOCUS_PASSWORD,
		[NEXT] = FOCUS_LOGIN
	},

	[FOCUS_LOGIN] = {
		[DIR_UP] = FOCUS_PASSWORD,
		[DIR_DOWN] = FOCUS_USERNAME,
		[DIR_RIGHT] = FOCUS_EXIT,
		[DIR_LEFT] = FOCUS_LOGIN,
		[NEXT] = FOCUS_EXIT
	},

	[FOCUS_EXIT] = {
		[DIR_UP] = FOCUS_PASSWORD,
		[DIR_DOWN] = FOCUS_USERNAME,
		[DIR_RIGHT] = FOCUS_EXIT,
		[DIR_LEFT] = FOCUS_LOGIN,
		[NEXT] = FOCUS_USERNAME
	},
};

void LStrShiftAdd(char *str, int shift_index, char ch, int len){
    
    for (int i = len; i >= shift_index; i--)  str[i + 1] = str[i];

    str[shift_index] = ch;
}

void LStrShiftDelete(char* str, int delete_index){
    if (delete_index < 0)
        return;

    for (int i = delete_index; str[i] != '\0'; i++)
        str[i] = str[i + 1];
}

void drawInput(Ui ui, Input input){
    if ( ui.curFocus == FOCUS_USERNAME){
        werase(ui.usernameBox);
        mvwprintw(ui.usernameBox, 0, 0, "%s", input.username);
        wrefresh(ui.usernameBox);
    }

    else if ( ui.curFocus == FOCUS_PASSWORD) {
        werase(ui.passwordBox);
        mvwprintw(ui.passwordBox, 0, 0, "%s", input.password);
        wrefresh(ui.passwordBox);
    }
}

void changeOpts(Ui focus, Opts highlight){
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

void dynamicRefresh(Ui* ui){
    switch (ui->curFocus) {
		case FOCUS_USERNAME:
			curs_set(1);
			changeOpts(*ui, EMPTY); // this is to stop highlighting opts
			wrefresh(ui->usernameBox);
			break;

		case FOCUS_PASSWORD:
			curs_set(1);
			changeOpts(*ui, EMPTY); // this is to stop highlighting opts
			wrefresh(ui->passwordBox);
			break;

		default:
			curs_set(0);
			changeOpts(*ui, LOGIN);
			break;
		}
}

User initLogin(){
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

	User user;

	Ui ui = {
		.loginWin = loginWin,
		.usernameBox = usernameBox,
		.passwordBox = passwordBox,
		.opts = opts,
		.curFocus = FOCUS_USERNAME
	};

	Input input = {
		.username = malloc(26),
		.password = malloc(26),
		.usernameBoxCursor = 0,
		.usernameBoxLen = 0,
		.passwordBoxCursor = 0,
		.passwordBoxLen = 0,
	};
    input.username[0] = '\0';
    input.password[0] = '\0';

    box(loginWin, 0, 0);
    refresh();

	changeOpts(ui, EMPTY);
    wrefresh(loginWin);
    wrefresh(opts);
    wrefresh(passwordBox);
    wrefresh(usernameBox);

	int running = 1;
	int ch;
	while (running){
		switch (ch = wgetch(usernameBox)) {
			case KEY_BACKSPACE:
				if (input.usernameBoxCursor > 0 && ui.curFocus == FOCUS_USERNAME && input.usernameBoxLen > 0) {
					input.usernameBoxCursor--;
					input.usernameBoxLen--;

					if(input.usernameBoxLen == input.usernameBoxCursor){
						input.username[input.usernameBoxCursor] = '\0';
						drawInput(ui, input);
					}

					else {
						LStrShiftDelete(input.username, input.usernameBoxCursor);
						drawInput(ui, input);
						wmove(ui.usernameBox, 0, input.usernameBoxCursor);
						wrefresh(ui.usernameBox);
					}
				}

				else if (input.passwordBoxCursor > 0 &&  ui.curFocus == FOCUS_PASSWORD && input.passwordBoxLen > 0) {
					input.passwordBoxCursor--;
					input.passwordBoxLen--;

					if (input.passwordBoxLen == input.passwordBoxCursor) {
						input.password[input.passwordBoxCursor] = '\0';
						drawInput(ui, input);
					}

					else {
						LStrShiftDelete(input.password, input.passwordBoxCursor);
						drawInput(ui, input);
						wmove(ui.passwordBox, 0, input.passwordBoxCursor);
						wrefresh(ui.passwordBox);
					}

				}
				break;

			case KEY_UP:
				ui.curFocus = navigation[ui.curFocus][DIR_UP];
				dynamicRefresh(&ui);
				break;

			case KEY_DOWN:
				ui.curFocus = navigation[ui.curFocus][DIR_DOWN];
				dynamicRefresh(&ui);
				break;

			case KEY_RIGHT:
				ui.curFocus = navigation[ui.curFocus][DIR_RIGHT];

				if (ui.curFocus == FOCUS_EXIT){
					changeOpts(ui, EXIT);
				}

				else if (input.usernameBoxCursor < 25 &&  ui.curFocus == FOCUS_USERNAME) {
					input.usernameBoxCursor++;
					if (input.usernameBoxCursor > input.usernameBoxLen) input.usernameBoxCursor = input.usernameBoxLen;
					wmove(ui.usernameBox, 0, input.usernameBoxCursor);
					wrefresh(usernameBox);
				}

				else if (input.passwordBoxCursor < 25 &&  ui.curFocus == FOCUS_PASSWORD) {
					input.passwordBoxCursor++;
					if (input.passwordBoxCursor > input.passwordBoxLen) input.passwordBoxCursor = input.passwordBoxLen;
					wmove(ui.passwordBox, 0, input.passwordBoxCursor);
					wrefresh(passwordBox);
				}

				break;

			case KEY_LEFT:
				ui.curFocus = navigation[ui.curFocus][DIR_LEFT];

				if (ui.curFocus == FOCUS_LOGIN){
					changeOpts(ui, LOGIN);
				}

				if ( ui.curFocus == FOCUS_USERNAME) {
					input.usernameBoxCursor--;
					if (input.usernameBoxCursor < 0) input.usernameBoxCursor = 0;
					wmove(ui.usernameBox, 0, input.usernameBoxCursor);
					wrefresh(usernameBox);
				}

				else if ( ui.curFocus == FOCUS_PASSWORD) {
					input.passwordBoxCursor--;
					if (input.passwordBoxCursor < 0) input.passwordBoxCursor = 0;
					wmove(ui.passwordBox, 0, input.passwordBoxCursor);
					wrefresh(passwordBox);
				}
				break;

			case '\n':
				if (ui.curFocus == FOCUS_LOGIN) {
					user.username = input.username;
					user.password = input.password;
					curs_set(1);
					running = 0;
				}
				else if (ui.curFocus == FOCUS_EXIT) {
					running = 0;
				}
				break;

			default:
				if (input.usernameBoxCursor < 25 &&  ui.curFocus == FOCUS_USERNAME && input.usernameBoxLen < 25) {
					if (input.usernameBoxLen == input.usernameBoxCursor){
						input.username[input.usernameBoxCursor] = ch;
						input.usernameBoxCursor++;
						input.usernameBoxLen++;
						input.username[input.usernameBoxCursor] = '\0';
						drawInput(ui, input); 
					}
					
					else {
						LStrShiftAdd(input.username, input.usernameBoxCursor, ch, input.usernameBoxLen);
						input.usernameBoxCursor++;
						input.usernameBoxLen++;
						drawInput(ui, input);
						wmove(ui.usernameBox, 0, input.usernameBoxCursor);
						wrefresh(ui.usernameBox);
					}
				}

					if (input.passwordBoxCursor < 25 &&  ui.curFocus == FOCUS_PASSWORD && input.passwordBoxLen < 25) {
						if (input.passwordBoxLen == input.passwordBoxCursor){
							input.password[input.passwordBoxCursor] = ch;
							input.passwordBoxCursor++;
							input.passwordBoxLen++;
							input.password[input.passwordBoxCursor] = '\0';
							drawInput(ui, input); 
						}

						else {
							LStrShiftAdd(input.password, input.passwordBoxCursor, ch, input.passwordBoxLen);
							input.passwordBoxCursor++;
							input.passwordBoxLen++;
							drawInput(ui, input);
							wmove(ui.passwordBox, 0, input.passwordBoxCursor);
							wrefresh(ui.passwordBox);
						}
					}
					break;
		}
	}
    free(input.username);
    free(input.password);
    endwin();

	return user;
}
