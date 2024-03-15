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
    LIST,
    OLIST,
    TASKLIST,
    BLOCKQUOTE,
    CODEBLOCK_FENCED,
    CODEBLOCK_INLINE,
    BOLD,
    ITALIC,
    STRIKETHROUGH,
    HIGHLIGHT,
    INDENT,
    LINEBREAK,
    TEXT,
    ENDLINE,
    END,
} TokenType;

char* translate[18] = {"START",
                      "PROPERTIES", 
                      "HEADING",
                      "LIST", 
                      "OLIST",
                      "TASKLIST",
                      "BLOCKQUOTE",
                      "CODEBLOCK_FENCED",
                      "CODEBLOCK_INLINE",
                      "BOLD",
                      "ITALIC",
                      "STRIKETHROUGH",
                      "HIGHLIGHT",
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
    //free(str);
    //str = malloc(5);
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

        //BACKSLASH


        //HORIZONTAL RULE!
        if(bufferSize == 0){
            int index = 0;
            if(*ch == '*' || *ch == ' '){
                while(*(ch + index) == '*' || *(ch + index) == ' '){
                    if(*(ch + index) == '*'){
                        counter++;
                    }
                    index++;
                }
            }
            else
            if(*ch == '_' || *ch == ' '){
                while(*(ch + index) == '_' || *(ch + index) == ' '){
                    if(*(ch + index) == '_'){
                        counter++;
                    }
                    index++;
                }
            }
            else
            if(*ch == '-' || *ch == ' '){
                while(*(ch + index) == '-' || *(ch + index) == ' '){
                    if(*(ch + index) == '-'){
                        counter++;
                    }
                    index++;
                }
                if(counter == index && counter == 3){
                    if(lineCount == 0){
                        props = true;
                        addToken(PROPERTIES, NULL);
                        ch += 3;
                        continue;
                    }
                    else
                    if(props){
                        props = false;
                        addToken(PROPERTIES, NULL);
                        ch += 3;
                        continue;
                    }
                }
            }
            if(counter >= 3){
                addToken(LINEBREAK, NULL);
                ch += index;
                continue;
            }
        }


        //STRIKETHROUGH
        if(*ch == '~' && *(ch + 1) == '~'){
            addToken(STRIKETHROUGH, NULL);
            ch++;
            continue;
        }

        //HIGHLIGHT
        if(*ch == '=' && *(ch + 1) == '='){
            addToken(HIGHLIGHT, NULL);
            ch++;
            continue;
        }


        //BOLD & ITALICS
        if(*ch == '_' || *ch == '*'){
            char* countUnderscore = malloc(1);
            int underscoreCount = countChars(countUnderscore, '_', ch);
            int starCount = countChars(countUnderscore, '*', ch);
            //ITALICS
            if(underscoreCount == 1 || starCount == 1){
                addToken(ITALIC, NULL);
            }
            else
            //BOLD
            if(underscoreCount == 2 || starCount == 2){
                addToken(BOLD, NULL);
                ch++;
            }
            continue;
        }

        //UNORDERED/TASK LISTS
        if(*ch == '-'){
            if(*(ch + 1) == ' '){
                if(*(ch + 2) == '[' && *(ch + 4) == ']' && *(ch + 5) == ' '){
                    if(*(ch + 3) != ' '){
                        addToken(TASKLIST, "1");
                    }
                    else{
                        addToken(TASKLIST, "0");
                    }
                    ch += 5;
                }
                else{
                    addToken(LIST, NULL);
                    ch ++;
                }
                continue;
            }
        }

        //FENCED & INLINE CODEBLOCKS
        if(*ch == '`'){
            char* backtickCount = malloc(1);
            counter = countChars(backtickCount, '`', ch);
            if(counter >= 3){
                addToken(CODEBLOCK_FENCED, NULL);
                ch += 2;
            }
            else
            if(counter < 3){
                addToken(CODEBLOCK_INLINE, NULL);
                ch += counter - 1;
                
            }
            while(*ch != '`'){
                buffer[bufferSize] = *ch;
                ch++;
                bufferSize++;
            }
            if(strlen(buffer) > 0){
                buffer[bufferSize] = '\0';
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
        if(*ch == '>'){
            char* quoteCount = malloc(1);
            counter = countChars(quoteCount, '>', ch);
            addToken(BLOCKQUOTE, quoteCount);
            ch += counter - 1;
            continue;
        }

        //HEADING
        if(*ch == '#'){
            char* headingCount = malloc(1);
            counter = countChars(headingCount, '#', ch);
            if(*(ch + counter) == ' '){
                addToken(HEADING, headingCount);
                ch += counter;
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

        //TEXT
        buffer[bufferSize] = *ch;
        bufferSize++;
    }
    if(*ch == '\0'){
        addToken(ENDLINE, NULL);
    }

    return;
}