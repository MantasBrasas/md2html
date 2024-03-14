#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define SIZE 256


typedef enum {
    START = 0,
    PROPERTIES,
    HEADING,
    OLIST,
    INDENT,
    LINEBREAK,
    TEXT,
    ENDLINE,
    END,
} TokenType;

char* translate[9] = {"START",
                      "PROPERTIES", 
                      "HEADING", 
                      "OLIST", 
                      "INDENT",
                      "LINEBREAK", 
                      "TEXT", 
                      "ENDLINE", 
                      "END"};

typedef struct tk{
    int tokenType;
    char* value;
    struct tk* next;
} Token;

void addToken(Token* head, int type, char* val){
    Token* current = head;
    while(current->next != NULL){
        current = current->next;
    }
    
    current->next = malloc(sizeof(Token));
    current->next->tokenType = type;

    if(val != NULL){
        current->next->value = malloc(strlen(val));
        strncpy(current->next->value, val, strlen(val) + 1);
    }
    else{
        current->next->value = malloc(1);
        strncpy(current->next->value, "", 1);
    }
    
    current->next->next = NULL;

    //free(val);
    return;
}

int countChars(char* str, char search, char* ch){
    int count = 0;
    while(*ch == search){
        count++;
        ch++;
    }

    free(str);
    str = malloc(5);
    sprintf(str, "%d", count);

    return count;
}

Token* tokens;
bool props = false;

void tokenize(char* line, int lineCount){
    int index = 0;

    if(tokens == NULL){
        tokens = malloc(sizeof(Token));
        tokens->tokenType = 0;
        tokens->value = "";
        tokens->next = NULL;
    }

    char* ch = malloc(1);
    for(ch = line; *ch != '\0'; ch++){
        if(*ch == '\n'){
            addToken(tokens, ENDLINE, NULL);
            break;
        }

        int counter = 0;

        //PROPERTIES
        if(*ch == '-'){
            char* dashCount = malloc(1);
            counter = countChars(dashCount, '-', ch);
            if(counter == 3){
                if(lineCount == 0){
                    props = true;
                    addToken(tokens, PROPERTIES, NULL);
                }
                else
                if(props){
                    addToken(tokens, PROPERTIES, NULL);
                    props = false;
                }
                else{
                    addToken(tokens, LINEBREAK, NULL);
                }
            }
        }

        //INDENT
        if(*ch == '\t'){
            char* indentCount = malloc(1);
            countChars(indentCount, '\t', ch);
            addToken(tokens, INDENT, indentCount);
            continue;
        }
        if(*ch == ' '){
            
            char* spaceCount = malloc(1);
            int spaces = countChars(spaceCount, ' ', ch);
            if(spaces % 4 == 0){
                sprintf(spaceCount, "%d", spaces / 4);
                addToken(tokens, INDENT, spaceCount);
            }
            ch += spaces - 1;
            continue;
        }
        //HEADING
        if(*ch == '#'){
            char* headingCount = malloc(1);
            countChars(headingCount, '#', ch);
            addToken(tokens, HEADING, headingCount);
            ch++;
            continue;
        }
        //ORDERED LIST
        if(isdigit(*ch)){
            if(*(ch + 1) == '.' && *(ch + 2) == ' '){
                char* buffer = malloc(2);
                buffer[0] = *ch;
                buffer[1] = '\0';

                addToken(tokens, OLIST, buffer);
                ch += 2;
                continue;
            }
        }

        //search for everything else BEFORE THIS
        if(isalnum(*ch)){
            char* buffer = malloc(SIZE);

            while(isalnum(*ch) || *ch == ' '){
                buffer[counter] = *ch;
                ch++;
                counter++;
            }
            buffer[counter] = '\0';
            ch--;
            addToken(tokens, TEXT, buffer);
            continue;
        }
        
    }
    if(*ch == '\0'){
        addToken(tokens, ENDLINE, NULL);
    }

    return;
}