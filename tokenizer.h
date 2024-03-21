#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/* DEFINITION START */

#define SIZE 1024
#define TYPES 14

typedef struct tk{
    int tokenType;
    char* value;
    struct tk* right;
    struct tk* left;
} Token;

/* DEFINITION END */
/* GLOBAL VARIABLE START */

typedef enum {
    START = 0,
    END,
    PROPERTIES,
    HEADING,
    BLOCKQUOTE,
    ORDERED_LIST,
    UNORDERED_LIST,
    TASK_LIST,
    CODEBLOCK,
    LINEBREAK,
    FOOTNOTE,
    FOOTNOTE_INLINE,
    MEDIA,
    TEXT,
} TokenType;

char* translate[TYPES] = {"START",
                          "END",
                          "PROPERTIES",
                          "HEADING",
                          "BLOCKQUOTE",
                          "ORDERED_LIST",
                          "UNORDERED_LIST",
                          "TASK_LIST",
                          "CODELBOCK",
                          "LINEBREAK",
                          "FOOTNOTE",
                          "FOOTNOTE_INLINE",
                          "MEDIA",
                          "TEXT"};

Token* tokens; /* Points to head of Token Tree */
Token* currentToken; /* Points to current token in tree to be modified */

int footnoteCount = 1; /* total count of written footnotes */
int footnoteClosed = -1; /* total count of closed footnotes */
int footnoteInline = 1; /* total count of inline footnotes */

int bufferSize; /* stored length of text buffer */
char* buffer = ""; /* text buffer */

// debatable usage here
Token* refs[TYPES]; 
bool flags[TYPES] = {false};

/* GLOBAL VARIABLE END */
/* DEFINE TOKENIZATION FUNCTIONS */

/**
 * Creates a new Token dependent on Type and Val arguments
**/
Token* newToken(int type, char* val){ 
    Token* new = malloc(sizeof(Token));
    
    new->value = "";

    if(val == NULL && type == TEXT){
        if(bufferSize > 0){
            buffer[bufferSize] = '\0';
            new->value = buffer;
        }
    }
    else
    if(val != NULL){
        new->value = val;
    }
    
    new->right = NULL;
    new->left = NULL;

    return new;
}

/**
 * Adds a new token to the Token tree, depending on the Token's type
**/
void addToken(Token* token){
    char* type = token->tokenType;
    int   val  = token->value;

    if(type == TEXT){
        if(currentToken->left == NULL){
            currentToken->left = token;
            currentToken = currentToken->left;
        }
        else{
            fprintf(stderr, "left token already full\n");
        }
    }
    else{
        if(currentToken->right == NULL){
            currentToken->right = token;
            currentToken = currentToken->right;
        }
        else{
            fprintf(stderr, "right token already full\n");
        }
    }

    return;
}

/**
 * Converts an integer to a string
**/
char* intToStr(int i){
    int len = sprintf(NULL, "%d", i);
    char* str = malloc(len + 1);
    sprintf(str, "%d", i);
    str[len] = '\0';
    return str;
}

/**
 * Counts the number of occurences of `search` on string `str`
**/
int countChars(char* str, char search, char* ch){
    int count = 0;
    while(*ch == search){
        count++;
        ch++;
    }
    sprintf(str, "%d", count);

    return count;
}

/**
 * Iteratively turns a single line from a .MD file into Tokens which are added to the existing tree
**/
void tokenize(char* line, int lineCount){
    if(tokens == NULL){
        tokens = newToken(START, NULL);

        currentToken = tokens;
    }

    buffer = malloc(SIZE);
    bufferSize = 0;

    char* ch = malloc(1);
    for(ch = line; *ch != '\0'; ch++){
        //ENDLINES
        if(*ch == '\n' || *ch == '\r'){
            //durr
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
        // if(*ch == '['){
        //     if(*(ch + 1) == '^'){
        //         while(*(ch + counter + 2) != ']'){
        //             counter++;
        //         }
        //         if(*(ch + counter + 3) == ':'){
        //             currentToken->right = addToken(FOOTNOTE, intToStr(footnoteClosed));
        //             currentToken = currentToken->right;
        //             footnoteClosed--;
        //             ch += counter + 3;
        //             continue;
        //         }
        //         else
        //         {
        //             currentToken->right = addToken(FOOTNOTE, intToStr(footnoteCount));
        //             currentToken = currentToken->right;
        //             footnoteCount++;
        //             ch += counter + 2;
        //             continue;
        //         }
        //     }
        //     currentToken->right = addToken(MEDIA, NULL);
        //     currentToken = currentToken->right;
        //     Token* refPoint = currentToken;

        //     ch++;
        //     while(*ch != ']'){
        //         buffer[bufferSize] = *ch;
        //         ch++;
        //         bufferSize++;
        //     }
        //     ch++;

        //     currentToken->left = addToken(TEXT, NULL);
        //     currentToken = currentToken->left;

        //     bufferSize = 0;
        //     buffer = malloc(SIZE);

        //     if(*ch == '('){
        //         ch++;
        //         while(*ch != ')'){
        //             buffer[bufferSize] = *ch;
        //             ch++;
        //             bufferSize++;
        //         }
        //         ch++;

        //         currentToken->left = addToken(TEXT, NULL);
        //         continue;
        //     }
        //     else{
        //         //improper link format or i need to reconsider this code
        //         fprintf(stderr, "Media block started and not properly finished on line %d\n", lineCount);
        //         //exit(EXIT_FAILURE);
        //     }
        // }

        //INLINE FOOTNOTE
        // if(*ch == '^'){
        //     if(*(ch + 1) == '['){
        //         currentToken->left = addToken(TEXT, NULL);
        //         currentToken = currentToken->left;
        //         currentToken->right = addToken(FOOTNOTE_INLINE, NULL);
        //         currentToken = currentToken->right;

        //         ch += 2;
        //         while(*ch != ']'){
        //             if(*ch == '\\'){
        //                 ch++;
        //                 continue;
        //             }
        //             buffer[bufferSize] = *ch;
        //             ch++;
        //             bufferSize++;
        //         }

        //         currentToken->left = addToken(TEXT, NULL);
        //         currentToken = currentToken->left;
        //         continue;
        //     }
        // }

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
        //         if(counter == index && counter == 3 && (lineCount == 1 || flags[PROPERTIES] == true)){
        //             inlineToken(PROPERTIES);
        //             ch += 3;
        //             continue;
        //         }
        //     }
        //     if(counter >= 3){
        //         currentToken->right = addToken(LINEBREAK, NULL);
        //         currentToken = currentToken->right;
        //         ch += index;
        //         continue;
        //     }
        // }

        //STRIKETHROUGH
        // if(*ch == '~' && *(ch + 1) == '~'){
        //     inlineToken(STRIKETHROUGH);
        //     ch++;
        //     continue;
        // }

        // //HIGHLIGHT
        // if(*ch == '=' && *(ch + 1) == '='){
        //     inlineToken(HIGHLIGHT);
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
        //         //addToken(ITALIC, NULL);
        //         inlineToken(ITALIC);
        //     }
        //     else
        //     //BOLD
        //     if(underscoreCount == 2 || starCount == 2){
        //         //addToken(BOLD, NULL);
        //         inlineToken(BOLD);
        //         ch++;
        //     }
        //     continue;
        // }

        //UNORDERED/TASK LISTS
        // if(*ch == '-'){
        //     if(*(ch + 1) == ' '){
        //         if(*(ch + 2) == '[' && *(ch + 4) == ']' && *(ch + 5) == ' '){
        //             char* val = "1";
        //             if(*(ch + 3) != ' '){
        //                 val = "0";
        //             }
        //             currentToken->right = addToken(TASKLIST, val);
        //             currentToken = currentToken->right;

        //             flags[TASKLIST] = true;
        //             refs[TASKLIST] = currentToken;

        //             ch += 5;
        //         }
        //         else{
        //             currentToken->right = addToken(LIST, NULL);
        //             currentToken = currentToken->right;
                    
        //             flags[LIST] = true;
        //             refs[LIST] = currentToken;
                    
        //             ch++;
        //         }
        //         continue;
        //     }
        // }

        //FENCED & INLINE CODEBLOCKS
        // if(*ch == '`'){
        //     char* backtickCount = malloc(1);
        //     counter = countChars(backtickCount, '`', ch);
        //     if(counter >= 3){
        //         // currentToken->right = addToken(CODEBLOCK_FENCED, NULL);
        //         // currentToken = currentToken->right;
                
        //         inlineToken(CODEBLOCK_FENCED);
                
        //         ch += 2;
        //     }
        //     else
        //     if(counter < 3){
        //         inlineToken(CODEBLOCK_INLINE);

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

        //     currentToken->right = addToken(INDENT, indentCount);
        //     currentToken = currentToken->right;

        //     continue;
        // }
        // if(*ch == ' '){
        //     char* spaceCount = malloc(1);
        //     int spaces = countChars(spaceCount, ' ', ch);
        //     if(spaces % 4 == 0){
        //         sprintf(spaceCount, "%d", spaces / 4);

        //         currentToken->right = addToken(INDENT, spaceCount);
        //         currentToken = currentToken->right;

        //         ch += spaces - 1;
        //         continue;
        //     }
        // }

        //BLOCKQUOTE
        // if(*ch == '>'){
        //     char* quoteCount = malloc(1);
        //     counter = countChars(quoteCount, '>', ch);

        //     currentToken->right = addToken(BLOCKQUOTE, quoteCount);
        //     currentToken = currentToken->right;

        //     flags[BLOCKQUOTE] = true;
        //     refs[BLOCKQUOTE] = currentToken;
            
        //     ch += counter - 1;
        //     continue;
        // }

        //HEADING
        // if(*ch == '#'){
        //     char* headingCount = malloc(1);
        //     counter = countChars(headingCount, '#', ch);
        //     if(*(ch + counter) == ' '){
        //         currentToken->right = addToken(HEADING, headingCount);
        //         currentToken = currentToken->right;

        //         flags[HEADING] = true;
        //         refs[HEADING] = currentToken;

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