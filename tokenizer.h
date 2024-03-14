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
    CODEBLOCK_FENCED,
    CODEBLOCK_INLINE,
    INDENT,
    LINEBREAK,
    TEXT,
    ENDLINE,
    END,
} TokenType;

char* translate[11] = {"START",
                      "PROPERTIES", 
                      "HEADING", 
                      "OLIST",
                      "CODEBLOCK_FENCED",
                      "CODEBLOCK_INLINE",
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

Token* tokens;
int bufferSize;
char* buffer = "";
bool props = false;

void addToken(int type, char* val){
    if(bufferSize != 0){
        buffer[bufferSize] = '\0';
        bufferSize = 0;
        addToken(TEXT, buffer);
    }
    Token* current = tokens;
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

void tokenize(char* line, int lineCount){
    if(tokens == NULL){
        tokens = malloc(sizeof(Token));
        tokens->tokenType = 0;
        tokens->value = "";
        tokens->next = NULL;
    }

    buffer = malloc(SIZE + 1);
    bufferSize = 0;

    char* ch = malloc(1);
    for(ch = line; *ch != '\0'; ch++){
        if(*ch == '\n' || *ch == '\r'){
            addToken(ENDLINE, NULL);
            break;
        }

        int counter = 0;

        //PROPERTIES & LINEBREAKS
        if(*ch == '-'){
            char* dashCount = malloc(1);
            counter = countChars(dashCount, '-', ch);
            printf("%d\n", counter);
            if(counter >= 3 && *(ch + counter) == '\n'){
                if(lineCount == 0){
                    props = true;
                    addToken(PROPERTIES, NULL);
                }
                else
                if(props){
                    addToken(PROPERTIES, NULL);
                    props = false;
                }
                else{
                    addToken(LINEBREAK, NULL);
                }
                ch += counter;
                continue;
            }
        }

        //FENCED & INLINE CODEBLOCKS
        //these need to grab rest of text as a single token, up to codeblock close
        if(*ch == '`'){
            char* backtickCount = malloc(1);
            counter = countChars(backtickCount, '`', ch);
            if(counter == 3){
                addToken(CODEBLOCK_FENCED, NULL);
                ch += 2;
            }
            else
            if(counter == 1){
                addToken(CODEBLOCK_INLINE, NULL);
            }
            continue;
        }
        //INDENT
        if(*ch == '\t'){
            char* indentCount = malloc(1);
            countChars(indentCount, '\t', ch);
            addToken(INDENT, indentCount);
            continue;
        }
        if(*ch == ' '){
            char* spaceCount = malloc(1);
            int spaces = countChars(spaceCount, ' ', ch);
            if(spaces % 4 == 0){
                sprintf(spaceCount, "%d", spaces / 4);
                addToken(INDENT, spaceCount);
                ch += spaces - 1;
                continue;
            }
        }
        //BLOCKQUOTE
        //HEADING
        if(*ch == '#'){
            char* headingCount = malloc(1);
            countChars(headingCount, '#', ch);
            if(*(ch + 1) == ' '){
                addToken(HEADING, headingCount);
                ch++;
                continue;
            }
        }
        //ORDERED LIST
        if(isdigit(*ch)){
            if(*(ch + 1) == '.' && *(ch + 2) == ' '){
                char* buffer = malloc(2);
                buffer[0] = *ch;
                buffer[1] = '\0';

                addToken(OLIST, buffer);
                ch += 2;
                continue;
            }
        }

       buffer[bufferSize] = *ch;
       bufferSize++;
    }
    if(*ch == '\0'){
        addToken(ENDLINE, NULL);
    }

    return;
}