#ifndef COMMANDS_H
#define COMMANDS_H

unsigned short identify_command(char *buffer);
char* remove_spaces(char *buffer);
char* execute_command(unsigned short command, char* attrib, WINDOW *win);
char* extract_modifier(char *buffer);
void highlight(char* input);
char* convert2buffer(WINDOW *win);
// #define KEYWORD_IF " if " //keywords for highlighting
// #define KEYWORD_WHILE " while "
// #define KEYWORD_FOR " for "
// #define KEYWORD_TRUE " true "
// #define KEYWORD_FALSE " false "
// #define KEYWORD_CHARACTER " char "
// #define KEYWORD_INTEGER " int "
// #define KEYWORD_BREAK " break "
// #define KEYWORD_DEFINE "#define "
// #define KEYWORD_NULL " NULL "
// #define KEYWORD_STRUCT " struct "
// #define KEYWORD_ELSE " else "
// #define KEYWORD_FOR " for "
// #define KEYWORD_FLOAT " float "
// #define KEYWORD_DOUBLE " double "
// #define KEYWORD_UNSIGNED " unsigned "
// #define KEYWORD_SHORT " short "
// #define KEYWORD_LONG " long "
// #define KEYWORD_SIGNED " signed "
// #define KEYWORD_UNION " union" 
// #define KEYWORD_TYPEDEF " typedef "
// #define KEYWORD_VOID " void "
// #define KEYWORD_RETURN " return "
// #define KEYWORD_DO " do "
// #define KEYWORD_SWITCH " switch "
// #define KEYWORD_INCLUDE "#include"
// #define KEYWORD_CASE "case "

#endif
