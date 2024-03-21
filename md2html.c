#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tokenizer.h"

char* intToString(int value){
    int len = sprintf(NULL, "%d", value);
    //does not end the string, could be bad?
    char* out = malloc(len);
    sprintf(out, "%d", value);
    return out;
}

FILE* md;
FILE* out;

void writeToken(Token* t){
    char* type = malloc(strlen(translate[t->tokenType]));
    type = translate[t->tokenType];
    char* val = t->value;

    fwrite("Token:\t", 7, 1, out);
    fwrite(type, strlen(type), 1, out);

    for(int i = 0; i < (20 - strlen(type)); i++){
        fwrite(" ", 1, 1, out);
    }

    fwrite("Value:\t", 7, 1, out);
    // if(val != NULL && strlen(val) > 0){
    //     char* cPoint = val;
    //     int max = strlen(val);
    //     if(12 < strlen(val)){
    //         max = 12;
    //     }
    //     for(int i = 0; i < max; i++){
    //         fwrite(cPoint + i, 1, 1, out);
    //     }
    //     if(12 < strlen(val)){
    //         fwrite("...", 3, 1, out);
    //     } 
    // }
    if(val != NULL){
        fwrite(val, strlen(val), 1, out);
    }
    else{
        fwrite(" ", 1, 1, out);
    }

    if(t->left != NULL){
        fwrite("\nLEFT:  ", 8, 1, out);
        writeToken(t->left);
    }
    if(t->right != NULL){
        fwrite("\nRIGHT: ", 8, 1, out);
        writeToken(t->right);
    }

    if(t->left == NULL && t->right == NULL){
        fwrite("\nEND\n", 5, 1, out);
    }   
}

void returnTokens(){
    if(tokens == NULL){
        return;
    }

    writeToken(tokens);
    return;
}

int lineCount = 1;

int main(int argc, char* argv[]){
    md = fopen("sampleThree.md", "r");
    out = fopen("sampleThree.tokens", "w");

    char* line = malloc(SIZE);

    while(fgets(line, SIZE, md) != NULL){
        tokenize(line, lineCount);
        lineCount++;
    }

    addToken(newToken(TEXT, NULL));
    addToken(newToken(END, NULL));

    fwrite("START: ", 7, 1, out);
    returnTokens();

    free(tokens);
    fclose(out);
    return 0;
}