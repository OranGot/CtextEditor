
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "commands.h"
char* convert2buffer(WINDOW *win){
    int cols,lines;
    getmaxyx(win, lines,cols);
    char* buffer = malloc(lines*cols+1);
    if (buffer == NULL) {
    return NULL;
    }
    for(int i = 0; i<lines;i++){
        for(int j = 0; j<cols; j++){
            buffer[i*cols+j] =mvwinch(win, i, j) & A_CHARTEXT;
        }
    }
    buffer[lines*cols] = '\0';
    return buffer;
}
char* remove_spaces(char *buffer) {
    int len = strlen(buffer);
    int i, j = 0;
    for (i = 0; i < len; i++) {
        if (buffer[i] != ' ') {
            buffer[j] = buffer[i];
            j++;
        }
    }
    buffer[j] = '\0';
    return buffer;
}

unsigned short identify_command(char *buffer) {
    
    if (strncmp(buffer, ":q", 2) == 0) { // quit
        return 0;
    } else if (strncmp(buffer, ":o", 2) == 0) { // openfile
        return 1;
    } else if (strncmp(buffer, ":ht", 3) == 0) { // highlighter on
        return 2;
    } else if (strncmp(buffer, ":s", 2) == 0) { // save
        return 3;
    } else if (strncmp(buffer, ":hf", 3) == 0) { // highlighter off
        return 4;
    } else {
        return 10; // unknown command
    }
}
char* execute_command(unsigned short command, char* attrib, WINDOW *win){
    switch (command)
    {
    case 0:
        return "$EXIT$";
        break;
    case 1:
        FILE *file = fopen(attrib, "rb");
        if(file == NULL){
            mvwprintw(win, 0 , 15, "ERROR");
            break;
        }
        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        rewind(file);
        char *buffer = (char*)malloc(file_size + 1); 
        if (buffer == NULL) {
            mvwprintw(win, 0, 15, "No memory(");
            fclose(file);
        }
        size_t read_size = fread(buffer, 1, file_size, file);
        if (read_size != file_size) {
            mvwprintw(win, 0, 15, "another error(");
            free(buffer);
            fclose(file);
        }
    buffer[file_size] = '\0';
    fclose(file);
    return buffer;
    break;
    case 2:
    return "HT";
    break;
    case 3://need to add saving
    FILE *fptr = fopen(attrib, "w");
        if(fptr == NULL){
        mvwprintw(win, 0 , 15, "ERROR");
        break;
    }
    char* contents =convert2buffer(win);
    fprintf(fptr, "%s", contents);
    free(contents);        
    fclose(fptr);
    return NULL;
    break;
    case 4:
    return "HF";
    default:
    return NULL;
        break;
    }
}
char* extract_modifier(char *buffer) {
    int startpos = -1;
    int len = strlen(buffer);

    // Find the position of the '|'
    for (int i = 0; i < len; i++) {
        if (buffer[i] == '|') {
            startpos = i + 1; // Move start position to the character after '|'
            break;
        }
    }

    // If '|' is not found, return an empty string
    if (startpos == -1) {
        char *empty_str = (char*)malloc(1);
        empty_str[0] = '\0';
        return empty_str;
    }

    // Calculate the length of the modifier
    int modifierLength = len - startpos;

    // Allocate memory for the modifier string
    char *modifier = (char*)malloc((modifierLength + 1) * sizeof(char));

    // Copy the modifier substring
    strncpy(modifier, &buffer[startpos], modifierLength);
    modifier[modifierLength] = '\0'; // Null-terminate the string

    return modifier;
}
    const char *Ckeywords[] = {
        "break",
        "case",
        "char",
        "const",
        "continue",
        "default",
        "do",
        "double",
        "else",
        "enum",
        "extern",
        "float",
        "for",
        "goto",
        "if",
        "inline",
        "int",
        "long",
        "restrict",
        "return",
        "short",
        "signed",
        "sizeof",
        "static",
        "struct",
        "switch",
        "typedef",
        "union",
        "unsigned",
        "void",
        "volatile",
        "while",
        "@END@"
    };

void highlight(char* input){
    const char delimiters[] =";/() +-><*&\n,^.?!_{}[]:|@#$";
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK) ;
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_GREEN, COLOR_BLACK);
int* color_pairs = calloc(1, strlen(input)+1);
char* removed_chars = malloc(strlen(input)+1);
memset(removed_chars, 'i', strlen(input));
memset(color_pairs, 0, strlen(input));
char* output = malloc(strlen(input+1));
for (int i = 0; i < strlen(input); i++) {
    for (int o = 0; o < strlen(delimiters); o++) {
        if (input[i] == delimiters[o]) {
            removed_chars[i] = delimiters[o];
            break;
        }
    }
}
    char* token = strtok(input, delimiters);
    while (token != NULL) {

        int start_index = token - input;
        //mvprintw(3, 0, "start index is %i",start_index);
        for(int o = 0; strcmp(Ckeywords[o], "@END@") !=0;o++){
            if(strcmp(token, Ckeywords[o]) ==0){
                //mvprintw(1,0,"%s",token);
                color_pairs[start_index] = 4;
            }
        }
        token = strtok(NULL, delimiters);


    }
//mvprintw(4,0, "%s", removed_chars);
//reassemble
    for(int r = 0; r<strlen(input); r++){
        if(removed_chars[r] !='i'){
            output[r] = removed_chars[r];
        }
        else{
            output[r]  = input[r];
        }
    }
    output[strlen(input)+1] = '\0';
    //print
    int posx = 0;
    int posy = 0;
    int color;
    int prevcolor;    

    for(int y = 0; y<strlen(output); y++){
        color = color_pairs[y];
        if(color == 0){
            color = prevcolor;
        }
        attron(COLOR_PAIR(color));
        mvaddch(posy, posx, output[y]);
        attroff(COLOR_PAIR(color));
        if(posx<LINES)posx++;
        else{
            posx = 0;
            posy++;
        }
    prevcolor = color;
    }

    free(output);
    free(color_pairs);
    free(removed_chars);

}