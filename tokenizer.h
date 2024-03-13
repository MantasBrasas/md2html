#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define SIZE 256

typedef enum {
    START = 0,
    HEADING,
    TEXT,
    ENDLINE,
    END,
} TokenType;

typedef struct tk{
    int tokenType;
    char* value;
    struct tk* next;
} Token;

//maybe can get rid of this its only used once
Token* createToken(int type, char* val){
    Token* token = malloc(sizeof(Token));
    token->value = val;
    token->next = NULL;
    return token;
}

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

    return;
}

Token* tokens;

void tokenize(char* line){
    int index = 0;

    if(tokens == NULL){
        tokens = malloc(sizeof(Token));
        tokens->tokenType = 0;
        tokens->value = "";
        tokens->next = NULL;
    }

    char* ch;
    for(ch = line; *ch != '\0'; ch++){
        if(*ch == '\n'){
            addToken(tokens, ENDLINE, NULL);
            break;
        }

        int counter = 0;

        if(*ch == ' '){
            continue;
        }
        if(*ch == '#'){
            while(*ch == '#'){
                counter++;
                ch++;
            }

            int len = sprintf(NULL, "%d", counter);
            char* headingCount = malloc(len + 1);
            sprintf(headingCount, "%d", counter);
            addToken(tokens, HEADING, headingCount);
            continue;
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

void returnTokens(){
    Token* temp = tokens;
    if(tokens == NULL){
        return;
    }
    while(temp->next != NULL){
        printf("Token: %d, Value: %s\n", temp->tokenType, temp->value);
        temp = temp->next;
    }
    printf("Token: %d, Value: %s\n", temp->tokenType, temp->value);
    return;
}