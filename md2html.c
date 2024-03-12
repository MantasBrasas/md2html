#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define HEADER 0
#define BLOCKQUOTE 1
#define ORDERED_LIST 2
#define UNORDERED_LIST 3
#define CODEBLOCK 4
#define TASK_LIST 5
#define INDENT 6

FILE* md;
FILE* html;

int fD[7] = {0};
int fC[7] = {0};

#define NEST_DEPTH 10
int olNest[NEST_DEPTH] = {0};

/*
void modifyNest(int* nest, int newSize){
    int* tempNest = malloc(sizeof(nest) / sizeof(int));
    for(int i = 0; i < (sizeof(nest) / sizeof(int)); i++){
        tempNest[i] = nest[i];
    }

}
*/

void shortenLine(char* line, int n){
    if(strlen(line) <= n || line == NULL){
        *line = '\0';
        return;
    }
    memmove(line, line + n, strlen(line) - n + 1);
    return;
}

void writeText(char* line, bool endline){
    for(char* point = line; *point != '\0'; point++){
        if(*point == '\n'){
            fwrite("\n", endline, 1, html);
            return;
        }
        fwrite(point, 1, 1, html);
    }
    return;
}

void parse(char* line){
    //DO NOT ATTEMPT TO PARSE CODE WRITTEN IN THE .MD FILE!
    if(fD[CODEBLOCK] != 0){
        return;
    }

    //handle weird nested lists
    int indentCount = 0;
    if(fC[CODEBLOCK] == 0){
        if(strncmp(line, "\t", 1) == 0){
            for(char* ch = line; *ch == '\t'; ch++){
                indentCount += 1;
            }
        }

        if(indentCount < fC[INDENT]){
            for(int i = indentCount; i <= fC[INDENT]; i++){
                olNest[i] *= -1;
            }
        }
    }
    fC[INDENT] = indentCount;

    if(strncmp(line, "#", 1) == 0){
        for(char* ch = line; *ch == '#'; ch++){
         fD[0] += 1;
        }
        shortenLine(line, fD[0]);
        return;
    }
    if(strncmp(line, "```", 3) == 0){
        shortenLine(line, 3);
        if(fC[CODEBLOCK] == 1){
         fD[CODEBLOCK] = -1;
            return;
        }
     fD[CODEBLOCK] = 1;
        return;
    }

    int len = sprintf(NULL, "%d. ", abs(olNest[fC[INDENT]]) + 1);
    char* ol = malloc(len + 1);
    sprintf(ol, "%d. ", abs(olNest[fC[INDENT]]) + 1);

    if(strncmp(line + fC[INDENT], ol, strlen(ol)) == 0){
        if(olNest[fC[INDENT]] < 0){
            olNest[fC[INDENT]] *= -1;
        }
        olNest[fC[INDENT]]++;

        if(olNest[fC[INDENT]] == 1){
         fD[ORDERED_LIST] = 1;
        }

        shortenLine(line + fC[INDENT], 3);
        fC[ORDERED_LIST] = 1;
    }
    else if(fC[INDENT] == 0){
        olNest[0] *= -1;
    }

    
    printf("ol: %s, indentCount: %d, olNest = [", ol, indentCount);
    for(int i = 0; i < NEST_DEPTH - 1; i++){
        printf("%d\t", olNest[i]);
    }
    printf("%d]\n", olNest[NEST_DEPTH]);
    

    return;
}

bool writeBlockTags(char* line){
    if(line == NULL){
        return true;
    }
    //check flags!
    if(fD[CODEBLOCK] != 0){
        if(fD[CODEBLOCK] == 1){
            char* codeblock = malloc(17 + strlen(line));
            sprintf(codeblock, "<pre class=\"lang_%s", line);
            fwrite(codeblock, 16 + strlen(line), 1, html);
            fwrite("\">\n", 3, 1, html);
            shortenLine(line, 3);

            fC[CODEBLOCK] = 1;
        }
        else
        if(fD[CODEBLOCK] == -1){
            fwrite("</pre>\n", 7, 1, html);

            fC[CODEBLOCK] = 0;
        }
        fD[CODEBLOCK] = 0;
        return true;
    }
    
    else
    if(olNest[0] != 0){  
        for(int i = 0; i < fC[INDENT]; i++){
            fwrite("\t", 1, 1, html);
        }
        if(fD[ORDERED_LIST] == 1){
            fwrite("<ol>\n", 5, 1, html);
        fD[ORDERED_LIST] = 0;
        }
        for(int i = 0; i < NEST_DEPTH; i++){
            if(olNest[i] < 0){
                fwrite("</ol>\n", 6, 1, html);
                olNest[i] = 0;
            }
        }
        return false;
    }
    
    return true;
}

void writeLine(char* line, bool endline){
    if(fD[HEADER] > 0){
        char* header = malloc(4);
        sprintf(header, "<h%d>", fD[HEADER]);
        fwrite(header, 4, 1, html);\
        shortenLine(line, 1);
        free(header);
        endline = false;
    }
    else 
    if(olNest[0] > 0){
        for(int i = 0; i < fC[INDENT] + 1; i++){
            fwrite("\t", 1, 1, html);
        }
        shortenLine(line, fC[INDENT]);
        fwrite("<li>", 4, 1, html);
        endline = false;
    }

    writeText(line, endline);

    if(fD[HEADER] > 0){
        char* header = malloc(6);
        sprintf(header, "</h%d>\n", fD[HEADER]);
        fwrite(header, 6, 1, html);
        fD[HEADER] = 0;
        free(header);
    }
    else
    if(olNest[0] > 0){
        fwrite("</li>\n", 6, 1, html);
    }
}

int main(int argc, char* argv[]){
    md = fopen("sample.md", "r");
    html = fopen("sample_HTML.html", "w");

    char* line = malloc(256);
    char* ch = "";

    //write title

    //read properties
    fgets(line, 256, md);
    if(strncmp(line, "---\n", 4) == 0){
        fwrite("<ul class=\"properties\">\n", 24, 1, html);
        while(fgets(line, 256, md) != NULL){
            if(strncmp(line, "---\n", 4) == 0){
                fwrite("</ul>\n", 6, 1, html);
                break;
            }
            fwrite("<li>", 4, 1, html);
            writeText(line, false);
            fwrite("</li>\n", 6, 1, html);
        }
    }

    //write properties
    bool endline;
    while(fgets(line, 256, md) != NULL){
        //parse
        parse(line);
        //open tags & write text
        writeLine(line, writeBlockTags(line));
    }

    fclose(md);
    fclose(html);
    free(line);
    free(ch);

    return 0;
}