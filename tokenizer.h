#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define SIZE 1024

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
    FOOTNOTE,
    FOOTNOTE_INLINE,
    MEDIA,
    INDENT,
    LINEBREAK,
    TEXT,
    ENDLINE,
    END,
} TokenType;

char* translate[21] = {"START",
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
                      "FOOTNOTE",
                      "FOOTNOTE_INLINE",
                      "MEDIA",
                      "INDENT",
                      "LINEBREAK", 
                      "TEXT", 
                      "ENDLINE", 
                      "END"};

typedef struct tk{
    int tokenType;
    char* value;
    struct tk* right;
    struct tk* left;
} Token;

Token* tokens;
Token* currentToken;
int bufferSize;
int footnoteCount = 1;
int footnoteClosed = -1;
int footnoteInline = 1;
char* buffer = "";
bool props = false;

Token* addToken(int type, char* val){
    Token* token = malloc(sizeof(Token));

    token->tokenType = type;

    if(type == TEXT){
        token->value = malloc(bufferSize);
        token->value = buffer;

        bufferSize = 0;
        buffer = malloc(SIZE);
    }
    else
    if(val == NULL){
        token->value = "";
    }
    else{
        token->value = val;
    }
    
    token->right = NULL;
    token->left = NULL;

    return token;
}

char* intToStr(int i){
    int len = sprintf(NULL, "%d", i);
    char* str = malloc(len + 1);
    sprintf(str, "%d", i);
    str[len] = '\0';
    return str;
}

int countChars(char* str, char search, char* ch){
    int count = 0;
    while(*ch == search){
        count++;
        ch++;
    }
    sprintf(str, "%d", count);

    return count;
}

void tokenize(char* line, int lineCount){
    if(tokens == NULL){
        tokens = malloc(sizeof(Token));
        tokens->tokenType = 0;
        tokens->value = NULL;
        tokens->right = NULL;
        tokens->left = NULL;
    }

    currentToken = tokens;
    while(currentToken->right != NULL){
        currentToken = currentToken->right;
    }

    buffer = malloc(SIZE);
    bufferSize = 0;

    char* ch = malloc(1);
    for(ch = line; *ch != '\0'; ch++){
        //TEXT
        /*
        if(bufferSize > 0){
            currentToken->left = addToken(TEXT, NULL);
            continue;
        }
        */

        //ENDLINES
        if(*ch == '\n' || *ch == '\r'){
            break;
        }

        int counter = 0;

        //BACKSLASH (CHARACTER ESCAPE)
        if(*ch == '\\'){
            buffer[bufferSize] = *(ch + 1);
            ch++;
            bufferSize++;
            continue;
        }

        //FOOTNOTES & LINKS
        if(*ch == '['){
            if(*(ch + 1) == '^'){
                while(*(ch + counter + 2) != ']'){
                    counter++;
                }
                if(*(ch + counter + 3) == ':'){
                    currentToken->right = addToken(FOOTNOTE, intToStr(footnoteClosed));
                    currentToken = currentToken->right;
                    footnoteClosed--;
                    ch += counter + 3;
                    continue;
                }
                else
                {
                    currentToken->right = addToken(FOOTNOTE, intToStr(footnoteCount));
                    currentToken = currentToken->right;
                    footnoteCount++;
                    ch += counter + 2;
                    continue;
                }
            }
            currentToken->right = addToken(MEDIA, NULL);
            currentToken = currentToken->right;
            Token* refPoint = currentToken;

            ch++;
            while(*ch != ']'){
                buffer[bufferSize] = *ch;
                ch++;
                bufferSize++;
            }
            ch++;

            currentToken->left = addToken(TEXT, NULL);
            currentToken = currentToken->left;

            bufferSize = 0;
            buffer = malloc(SIZE);

            if(*ch == '('){
                ch++;
                while(*ch != ')'){
                    buffer[bufferSize] = *ch;
                    ch++;
                    bufferSize++;
                }
                ch++;

                currentToken->left = addToken(TEXT, NULL);
                continue;
            }
            else{
                //improper link format or i need to reconsider this code
                fprintf(stderr, "Media block started and not properly finished on line %d\n", lineCount);
                //exit(EXIT_FAILURE);
            }
        }

        //INLINE FOOTNOTE
        if(*ch == '^'){
            if(*(ch + 1) == '['){
                currentToken->left = addToken(TEXT, NULL);
                currentToken->right = addToken(FOOTNOTE_INLINE, NULL);
                currentToken = currentToken->right;

                ch += 2;
                while(*ch != ']'){
                    if(*ch == '\\'){
                        ch++;
                        continue;
                    }
                    buffer[bufferSize] = *ch;
                    ch++;
                    bufferSize++;
                }

                currentToken->left = addToken(TEXT, NULL);
                continue;
            }
        }
        

        //HORIZONTAL RULE & PROPERTIES
        // if(bufferSize == 0){
        //     int index = 0;
        //     if(*ch == '*' || *ch == ' '){
        //         while(*(ch + index) == '*' || *(ch + index) == ' '){
        //             if(*(ch + index) == '*'){
        //                 counter++;
        //             }
        //             index++;
        //         }
        //     }
        //     else
        //     if(*ch == '_' || *ch == ' '){
        //         while(*(ch + index) == '_' || *(ch + index) == ' '){
        //             if(*(ch + index) == '_'){
        //                 counter++;
        //             }
        //             index++;
        //         }
        //     }
        //     else
        //     if(*ch == '-' || *ch == ' '){
        //         while(*(ch + index) == '-' || *(ch + index) == ' '){
        //             if(*(ch + index) == '-'){
        //                 counter++;
        //             }
        //             index++;
        //         }
        //         if(counter == index && counter == 3){
        //             if(lineCount == 0){
        //                 props = true;
        //                 addToken(PROPERTIES, NULL);
        //                 ch += 3;
        //                 continue;
        //             }
        //             else
        //             if(props){
        //                 props = false;
        //                 addToken(PROPERTIES, NULL);
        //                 ch += 3;
        //                 continue;
        //             }
        //         }
        //     }
        //     if(counter >= 3){
        //         addToken(LINEBREAK, NULL);
        //         ch += index;
        //         continue;
        //     }
        // }


        //STRIKETHROUGH
        // if(*ch == '~' && *(ch + 1) == '~'){
        //     addToken(STRIKETHROUGH, NULL);
        //     ch++;
        //     continue;
        // }

        //HIGHLIGHT
        // if(*ch == '=' && *(ch + 1) == '='){
        //     addToken(HIGHLIGHT, NULL);
        //     ch++;
        //     continue;
        // }


        //BOLD & ITALICS
        // if(*ch == '_' || *ch == '*'){
        //     char* countUnderscore = malloc(1);
        //     int underscoreCount = countChars(countUnderscore, '_', ch);
        //     int starCount = countChars(countUnderscore, '*', ch);
        //     //ITALICS
        //     if(underscoreCount == 1 || starCount == 1){
        //         addToken(ITALIC, NULL);
        //     }
        //     else
        //     //BOLD
        //     if(underscoreCount == 2 || starCount == 2){
        //         addToken(BOLD, NULL);
        //         ch++;
        //     }
        //     continue;
        // }

        //UNORDERED/TASK LISTS
        // if(*ch == '-'){
        //     if(*(ch + 1) == ' '){
        //         if(*(ch + 2) == '[' && *(ch + 4) == ']' && *(ch + 5) == ' '){
        //             if(*(ch + 3) != ' '){
        //                 addToken(TASKLIST, "1");
        //             }
        //             else{
        //                 addToken(TASKLIST, "0");
        //             }
        //             ch += 5;
        //         }
        //         else{
        //             addToken(LIST, NULL);
        //             ch ++;
        //         }
        //         continue;
        //     }
        // }

        //FENCED & INLINE CODEBLOCKS
        // if(*ch == '`'){
        //     char* backtickCount = malloc(1);
        //     counter = countChars(backtickCount, '`', ch);
        //     if(counter >= 3){
        //         addToken(CODEBLOCK_FENCED, NULL);
        //         ch += 2;
        //     }
        //     else
        //     if(counter < 3){
        //         addToken(CODEBLOCK_INLINE, NULL);
        //         ch += counter - 1;
                
        //     }
        //     while(*ch != '`'){
        //         buffer[bufferSize] = *ch;
        //         ch++;
        //         bufferSize++;
        //     }
        //     if(strlen(buffer) > 0){
        //         buffer[bufferSize] = '\0';
        //     }
        //     continue;
        // }

        //INDENT
        // if(*ch == '\t'){
        //     char* indentCount = malloc(1);
        //     countChars(indentCount, '\t', ch);
        //     addToken(INDENT, indentCount);
        //     continue;
        // }
        // if(*ch == ' '){
        //     char* spaceCount = malloc(1);
        //     int spaces = countChars(spaceCount, ' ', ch);
        //     if(spaces % 4 == 0){
        //         sprintf(spaceCount, "%d", spaces / 4);
        //         addToken(INDENT, spaceCount);
        //         ch += spaces - 1;
        //         continue;
        //     }
        // }

        //BLOCKQUOTE
        // if(*ch == '>'){
        //     char* quoteCount = malloc(1);
        //     counter = countChars(quoteCount, '>', ch);
        //     addToken(BLOCKQUOTE, quoteCount);
        //     ch += counter - 1;
        //     continue;
        // }

        //HEADING
        // if(*ch == '#'){
        //     char* headingCount = malloc(1);
        //     counter = countChars(headingCount, '#', ch);
        //     if(*(ch + counter) == ' '){
        //         addToken(HEADING, headingCount);
        //         ch += counter;
        //         continue;
        //     }
        // }
        // //ORDERED LIST
        // if(isdigit(*ch)){
        //     if(*(ch + 1) == '.' && *(ch + 2) == ' '){
        //         char* buffer = malloc(2);
        //         buffer[0] = *ch;
        //         buffer[1] = '\0';

        //         addToken(OLIST, buffer);
        //         ch += 2;
        //         continue;
        //     }
        // }

        //TEXT
        buffer[bufferSize] = *ch;
        bufferSize++;
    }
    return;
}