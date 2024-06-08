#include <ncurses.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "commands.h"
char* convert2buffer(WINDOW *win);
int size_of_line(int cursorY, WINDOW *win);
void downConsoleWrite();
int inputs(int cursorX, int cursorY, int input, unsigned short *console, WINDOW *win);
char* log_command(int cursorY, WINDOW *win);
int main(int argc, char **argv) {
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    nodelay(stdscr, FALSE);
    WINDOW *mainwin = stdscr;
    WINDOW *secondwin = NULL;
    int main_height, main_width;
    getmaxyx(mainwin, main_height, main_width);
    int cursorY = 0;
    int cursorX = 0;
    int input;
    unsigned short console = 0, highlighting =1;
    wmove(mainwin, cursorY, cursorX);
    while ((input = getch()) != 27) {  // Escape key to exit
        getyx(mainwin, cursorY, cursorX);
        inputs(cursorX, cursorY, input, &console, mainwin);
        getyx(mainwin, cursorY, cursorX);
        if(highlighting == 1){
            char* text = convert2buffer(mainwin);
            clear();
            highlight(text);
            free(text);
          }        
        wmove(mainwin, cursorY, cursorX);
        wrefresh(mainwin);
        if (console == 1 && secondwin == NULL) {
            int consoleX = 0, consoleY = 0;
            secondwin = newwin(main_height, main_width, 0, 0);
            keypad(secondwin, TRUE);
            noecho();
            cbreak();
            wmove(secondwin, consoleY, consoleX);
            wrefresh(secondwin);

            while (console == 1) {
                getyx(secondwin, cursorY, cursorX);
                wmove(secondwin, consoleY, consoleX);
                int consoleI = wgetch(secondwin);
                getyx(secondwin, consoleY, consoleX);
                int state = inputs(consoleX, consoleY, consoleI, &console, secondwin);
                getyx(secondwin, consoleY, consoleX);
                if(state == 1){
                    return 0;
                }
                else if(state == 3){
                    highlighting = 1;
                }
                else if(state ==4){
                    highlighting = 0;
                }

                wmove(secondwin, consoleY, consoleX);
                wrefresh(secondwin);
            }
        } else if (console == 0 && secondwin != NULL) {
            delwin(secondwin);
            secondwin = NULL;
            clear();
            refresh();
        }
    }

    wrefresh(mainwin);
    endwin();
    return 0;
}

// Function to get the length of the line at cursorY
int size_of_line(int cursorY, WINDOW *win) {
    int y, x;
    getmaxyx(win, y,x);
    int xpos = x - 1;

    int ch;
    move(cursorY, xpos);
    while ((ch = inch()) == ' ' && xpos > 0) {
        xpos--;
        wmove(win, cursorY, xpos);
    }
    return xpos + 1;
}

void downConsoleWrite() {
    move(LINES - 2, 0);
    hline(ACS_HLINE, COLS);
    move(LINES - 1, 0);
    printw("lines: %i   ", LINES);
}
int inputs(int cursorX, int cursorY, int input, unsigned short *console, WINDOW *win) {
    switch (input) {
        case KEY_UP:
            if (cursorY > 0) (cursorY)--;
            break;
        case KEY_DOWN:
            if (cursorY < LINES - 1) (cursorY)++;
            break;
        case KEY_RIGHT:
            if (cursorX < COLS - 1) (cursorX)++;
            break;
        case KEY_LEFT:
            if (cursorX > 0) (cursorX)--;
            break;
        case 10: // Enter key
            if (cursorY < LINES - 1 && *console == 0) {
                mvaddch(cursorY, cursorX+1, '\n');
                cursorY++;
                cursorX = 0;
            } else if (*console == 1) {                
                char *commandbuffer1 = log_command(cursorY, win);
                commandbuffer1 = remove_spaces(commandbuffer1);
                unsigned short command = identify_command(commandbuffer1);
                char *modifier = extract_modifier(commandbuffer1);
                free(commandbuffer1);
                mvwprintw(win, cursorY + 1, cursorX, "command: %hu modifier: %s ", command, modifier);
                char *output = execute_command(command, modifier, win);
                wrefresh(win);
                if(strcmp(output, "$EXIT$")==0){
                    return 1;
                }
                else if(strcmp(output, "HT")==0){
                    return 3;
                }
                else if(strcmp(output, "HF")==0){
                    return 4;
                }
            }
            break;
case KEY_BACKSPACE:
    if (cursorX > 0) {
        cursorX--;
        wmove(win, cursorY, cursorX);
        wdelch(win);
        //mvprintw(2, 0, "BACKSPACE DETECTED");
    } else if (cursorY > 0 && *console == 0) {
        (cursorY)--;
        cursorX = size_of_line(cursorY, win);
        wmove(win, cursorY, cursorX);
        wdelch(win);
        //mvprintw(2, 0, "BACKSPACE DETECTED2");
    }
    break;

        case 4: // Ctrl+D
            *console = (*console == 0) ? 1 : 0;
            break;
        default:
            if (isprint(input)) {
                waddch(win, input);
                if (cursorX < COLS - 1) cursorX++;
            }
            break;
    }
    wmove(win, cursorY, cursorX);
    wrefresh(win);
    return 0;
}

char* log_command(int cursorY, WINDOW *win) {

    int i;
    int len = COLS;
    char* commandbuffer = malloc(len+1);
    for (i = 0; i < len; i++) {
        commandbuffer[i] = mvwinch(win, cursorY, i);
    }
    commandbuffer[i] = '\0';
    //mvwprintw(win, 2, 0, "logged %s", commandbuffer);
    return commandbuffer;
}
