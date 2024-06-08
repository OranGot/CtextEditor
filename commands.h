#ifndef COMMANDS_H
#define COMMANDS_H

unsigned short identify_command(char *buffer);
char* remove_spaces(char *buffer);
char* execute_command(unsigned short command, char* attrib, WINDOW *win);
char* extract_modifier(char *buffer);
void highlight(char* input);
char* convert2buffer(WINDOW *win);

#endif
