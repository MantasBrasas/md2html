#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tokenizer.h"

#define SIZE 256

char* intToString(int value){
    int len = sprintf(NULL, "%d", value);
    //does not end the string, could be bad?
    char* out = malloc(len);
    sprintf(out, "%d", value);
    return out;
}

FILE* md;
FILE* html;

void writeToken(Token* t){
    char* type = malloc(10);
    type = translate[t->tokenType];
    char* val = t->value;

    fwrite("Token:\t", 7, 1, html);
    fwrite(type, strlen(type), 1, html);

    for(int i = 0; i < (12 - strlen(type)); i++){
        fwrite(" ", 1, 1, html);
    }

    fwrite("Value:\t", 7, 1, html);
    fwrite(val, strlen(val), 1, html);

    free(type);
    free(val);
    return;
}

void returnTokens(){
    Token* temp = tokens;
    if(tokens == NULL){
        return;
    }
    printf("\n");
    while(temp->next != NULL){
        writeToken(temp);
        fwrite("\n", 1, 1, html);
        temp = temp->next;
    }
    writeToken(temp);
    return;
}

int lineCount = 0;

int main(int argc, char* argv[]){
    md = fopen("sampleTwo.md", "r");
    html = fopen("sampleTwo.html", "w");

    char* line = malloc(SIZE);

    while(fgets(line, SIZE, md) != NULL){
        tokenize(line, lineCount);
        lineCount++;
    }

    addToken(tokens, END, NULL);

    returnTokens();

    free(tokens);
    fclose(html);
    fclose(md);
    return 0;
}