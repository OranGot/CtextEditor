
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "commands.h"
char* convert2buffer(WINDOW *win){
    int cols,lines;
    getmaxyx(win, lines,cols);
    char* buffer = malloc(lines * (cols + 1) + 1);
    if (buffer == NULL) {
    return NULL;
    }
    size_t pos = 0;
    for(int i = 0; i<lines;i++){
        for(int j = 0; j<cols; j++){
            buffer[pos++] =(char)mvwinch(win, i, j)& A_CHARTEXT;
        }
        buffer[pos++] = '\n';
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
        return "NULL";
    }
    char* contents =convert2buffer(stdscr);
    if (contents == NULL) { // Check if convert2buffer failed
        fclose(fptr); // Close the file before returning
        mvwprintw(win, 0, 15, "ERROR: convert2buffer returned NULL");
        return "NULL";
    }
    fprintf(fptr, "%s",contents);
    free(contents);
    fclose(fptr);
    return "NULL";
    break;
    case 4:
    return "HF";
    default:
    return "NULL";
        break;
    }
    free(attrib);
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
    const char delimiters[] = " ;/()+-><*&\n,^.?!_{}[]:|@#$\001";
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_GREEN, COLOR_BLACK);

    int input_len = strlen(input);
    int* color_pairs = calloc(input_len + 1, sizeof(int));
    char* removed_chars = malloc(input_len + 1);
    char* input_copy = strdup(input);  // Create a copy of input for strtok to modify
    memset(removed_chars, 1, input_len);
    memset(color_pairs, 0, input_len);

    // Identify delimiter positions
    for (int i = 0; i < input_len; i++) {
        for (int o = 0; o < sizeof(delimiters) - 1; o++) {
            if (input[i] == delimiters[o]) {
                removed_chars[i] = input[i];
                break;
            }
        }
    }

    char* token = strtok(input_copy, delimiters);
    while (token != NULL) {
        int start_index = token - input_copy;
        for (int o = 0; strcmp(Ckeywords[o], "@END@") != 0; o++) {
            if (strcmp(token, Ckeywords[o]) == 0) {
                color_pairs[start_index] = 4;
            }

        }
        if(color_pairs[start_index] == 0){
            color_pairs[start_index] = 1;
        }
        token = strtok(NULL, delimiters);
    }

    // Reassemble the output
    char* output = malloc(input_len + 1);
    for (int r = 0; r < input_len; r++) {
        if (input_copy[r] == '\0') {
            output[r] = removed_chars[r];
        } else {
            output[r] = input[r];
        }
    }
    output[input_len] = '\0';  // Ensure null-termination

    // Print with highlighting
    int posx = 0, posy = 0;
    int prevcolor = 1;  // Default color
    for (int y = 0; y < input_len; y++) {

        int color = color_pairs[y];
        if (color == 0 && prevcolor != 0) {
            color = prevcolor;
        }
        if (output[y] == '\n') {
            posx = 0;
            posy++;
            attroff(COLOR_PAIR(color));
            mvaddch(posy, posx, output[y]);
            attron(COLOR_PAIR(1));

        }
        else{
        attron(COLOR_PAIR(color));
        mvaddch(posy, posx, output[y]);
        attroff(COLOR_PAIR(color));
        posx++;
        }


        prevcolor = color;
    }

    free(input_copy);
    free(output);
    free(color_pairs);
    free(removed_chars);
}
