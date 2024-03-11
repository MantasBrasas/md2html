#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* md;
FILE* html;

int flagsDelta[6] = {0};
int flagsCurrent[6] = {0};
/*
0: header
1: blockquote
2: ordered list
3: unordered list
4: codeblock
5: task list
*/

void shortenLine(char* line, int n){
    if(strlen(line) <= n || line == NULL){
        *line = '\0';
        return;
    }
    memmove(line, line + n, strlen(line) - n + 1);
    return;
}

void parse(char* line){
    //printf("begin parse on:\n%s", line);
    if(strncmp(line, "#", 1) == 0){
        //printf("header %d found\n", flagsDelta[0]++);
        flagsDelta[0] += 1;
        shortenLine(line, 1);
        parse(line);
    }
}

void writeText(char* line){
    for(char* point = line; *point != '\0'; point++){
        if(*point == '\n'){
            fwrite("\n", 1, 1, html);
            return;
        }
        fwrite(point, 1, 1, html);
    }
    return;
}

void writeTags(char* line){
    //dont write empty line
    if(line == NULL){
        return;
    }
    //check flags!
    if(flagsDelta[0] != 0){
        char* header = malloc(6);
        if(flagsDelta[0] > 0){
            sprintf(header, "<h%d>\n\t", flagsDelta[0]);
            fwrite(header, 6, 1, html);
            flagsCurrent[0] = flagsDelta[0];
            flagsDelta[0] = -1;
            shortenLine(line, 1);
        }
        else if(flagsCurrent[0] != 0){
            sprintf(header, "</h%d>\n", flagsCurrent[0]);
            fwrite(header, 6, 1, html);
            flagsDelta[0] = 0;
            flagsCurrent[0] = 0;
        }
        free(header);
    }
}

int main(int argc, char* argv[]){
    md = fopen("sample.md", "r");
    html = fopen("sample_HTML.html", "w");

    char* line = malloc(256);
    char* ch = "";

    while(fgets(line, 256, md) != NULL){
        //parse
        parse(line);
        //open tags
        writeTags(line);
        //write text
        writeText(line);
        //close tags
        writeTags(line);
    }

    fclose(md);
    fclose(html);
    free(line);
    free(ch);

    return 0;
}