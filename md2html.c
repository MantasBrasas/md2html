#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEADER 0
#define BLOCKQUOTE 1
#define ORDERED_LIST 2
#define UNORDERED_LIST 3
#define CODEBLOCK 4
#define TASK_LIST 5

FILE* md;
FILE* html;

int flagsDelta[6] = {0};
int flagsCurrent[6] = {0};

void shortenLine(char* line, int n){
    if(strlen(line) <= n || line == NULL){
        *line = '\0';
        return;
    }
    memmove(line, line + n, strlen(line) - n + 1);
    return;
}

void parse(char* line){
    //DO NOT ATTEMPT TO PARSE CODE WRITTEN IN THE .MD FILE!
    if(flagsDelta[CODEBLOCK] != 0){
        return;
    }
    //printf("begin parse on:\n%s", line);
    if(strncmp(line, "#", 1) == 0){
        //printf("header %d found\n", flagsDelta[0]++);
        flagsDelta[0] += 1;
        shortenLine(line, 1);
        parse(line);
        return;
    }
    if(strncmp(line, "```", 3) == 0){
        shortenLine(line, 3)
        if(flagsCurrent[CODEBLOCK] == 1){
            flagsDelta[CODEBLOCK] = -1;
            return;
        }
        flagsDelta[CODEBLOCK] = 1;
        return;
    }
}

void writeTags(char* line){
    if(line == NULL){
        return;
    }
    //check flags!
    if(flagsDelta[HEADER] != 0){
        char* header = malloc(6);
        if(flagsDelta[HEADER] > 0){
            sprintf(header, "<h%d>\n\t", flagsDelta[HEADER]);
            fwrite(header, 6, 1, html);\
            shortenLine(line, 1);

            flagsCurrent[HEADER] = flagsDelta[HEADER];
            flagsDelta[HEADER] = -1;
        }
        else if(flagsCurrent[HEADER] != 0){
            sprintf(header, "</h%d>\n", flagsCurrent[HEADER]);
            fwrite(header, 6, 1, html);

            flagsDelta[HEADER] = 0;
            flagsCurrent[HEADER] = 0;
        }
        free(header);
    }
    else
    if(flagsDelta[CODEBLOCK] != 0){
        if(flagsDelta[CODEBLOCK] == 1){
            char* codeblock = malloc(17 + strlen(line));
            sprintf(codeblock, "<pre class=\"lang_%s", line);
            fwrite(codeblock, 16 + strlen(line), 1, html);
            fwrite("\">", 2, 1, html);

            flagsCurrent[CODEBLOCK] = 1;
        }
        else
        if(flagsCurrent[CODEBLOCK] == 1){
            fwrite("</pre>\n", 7, 1, html);

            flagsCurrent[CODEBLOCK] = 0;
        }
        flagsDelta[CODEBLOCK] = 0;
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