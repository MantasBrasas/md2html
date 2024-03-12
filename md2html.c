#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define SIZE 256

enum TokenType {
    HEADING,
    TEXT,
};

typedef struct {
    int tokenType;
    bool optional;
    char* value;
    Token next;
} Token;

Token* createToken(int type, char* val){
    Token* token = malloc(sizeof(Token));
    if(val != NULL){
        token->optional = true;
        token->value = val;
    }
    else{
        token->optional = false;
    }
    token->value = val;

    token.next = NULL;
}

FILE* md;
FILE* html;

Token tokenize(char* line){
    Token tokens;

    char* ch;
    for(ch = line; ch != "\0"; ch++){
        if(ch == "#"){
            int headingCount = 0;
            while(ch == "#"){
                ch++;
                headingCount++;
            }
        }
    }
}

int main(int argc, char* argv[]){
    md = fopen("sample.md", "r");
    html = fopen("sample.html", "w");

    char* line = malloc(SIZE);

    while(fgets(line, SIZE, md) != NULL){
        for(char* ch = line; *ch != '\0'; ch++){
            if(ch == "\n"){
                fwrite("\n", 1, 1, html);
                break;
            }
            fwrite(ch, 1, 1, html);
        }
    }

    fclose(html);
    fclose(md);
    return 0;
}