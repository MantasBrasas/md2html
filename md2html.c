#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define size sizeof(char) * 256

/*
FLAGS
-> should be a global variable
int array, flag[n] == 1 if flag is true, for the blockquote, & ordered/unordered lists the flag > 0 when true,
when flag[n] = k, it means we are k levels nested into the blockquote/ordered/unordered list
0: blockquote
1: ordered list
2: unordered list
3: code block
4: task list
5: paragraph

INLINE
*/
int flags[6] = {0};

char* parseFlags(char* line){
    char* prefix = buildPrefix();
    //check for properties
    if(strncmp(line, "---\n", 4) == 0){
        parseProperties();
    }
    //check for header
    else if(strncmp(line, "#", 1) == 0){
        parseHeader(line);
    }
    else if(strncmp(line, ">", 1) == 0){
        flags[0]++;
        for(char* point = line; *point == '>'; point++){
            flags[0]++;
        }
    }
    else if(strncmp(line, "1.", 2) == 0){
        flags[1]++;
    }
    else if(strncmp(line, "- ", 2) == 0){
        flags[2]++;
    }
    else if(strncmp(line, "```", 3) == 0){
        flags[3] = 1;
    }
    else if(strncmp(line, "- [ ] ", 6) == 0){
        flags[4]++;
    }
    else{
        flags[5] = 1;
        parseParagraph();
    }

    return prefix;
}

char* buildPrefix(){
    char* prefix = "";

    int tabs = flags[0] + flags[1] + flags[2] + flags[4];

    for(int i = 0; i < tabs; i++){
        *prefix = *updatePrefix(prefix, "\t");
    }

    return prefix;
}

char* updatePrefix(char* left, char* right){
    int sizeL = sizeof(left) / sizeof(char);
    int sizeR = sizeof(right) / sizeof(char);
    char* newPrefix = malloc(sizeof(char) * (sizeL + sizeR));

    strncat(newPrefix, left, sizeL);
    strncat(newPrefix, right, sizeR);

    return newPrefix;
}

void parseProperties(){
    return;
}

void parseHeader(char* line){
    return;
}

void writeLine(){
    return;
}

int main(int argc, char *argv[]){

    FILE *md;
    FILE *html;
    
    md = fopen("sample.md", "r");
    if(md == NULL){
        fprintf(stderr, "could not read input file.\n");
        return -1;
    }
    html = fopen("sample_HTML.html", "w");

    char *buffer = malloc(size);
    char *point = malloc(sizeof(char));

    int len;
    int *pFlag;

    *pFlag = 0;

    fgets(buffer, size, md);

    //CHECK FILE FOR `---`:
    if(strncmp(buffer, "---\n", 4) == 0){

        //WRITE <ul> TO START UNORDERED LIST
        fwrite("<ul>\n", sizeof(char) * 5, 1, html);

        //LOOP THROUGH PROPERTIES AND WRITE THEM AS <li> ELEMENTS
        while(fgets(buffer, size, md)){
            //IF WE ARE AT THE END OF PROPERTIES
            if(strncmp(buffer, "---\n", 4) == 0){
                //CLOSE UNORDERED LIST & EXIT LOOP
                fwrite("</ul>\n", sizeof(char) * 6, 1, html);
                break;
            }
            //WRITE LINE AS <li> ITEM
            fwrite("\t<li>", sizeof(char) * 5, 1, html);
            for(point = buffer; *point != '\n'; point++){
                fwrite(point, sizeof(char), 1, html);
            }
            fwrite("</li>\n", sizeof(char) * 6, 1, html);
        }
    }

    //MAIN LOOP
    while(fgets(buffer, size, md) != NULL){

        //HEADING CHECK
        int hCount = 0;
        for(point = buffer; *point != '\0'; point++){
            if(*point == '#'){
                hCount++;
            }
            else{
                break;
            }
        }

        //CODEBLOCKS
        if(strncmp(buffer, "```", 3) == 0){
            terminate(pFlag, html);

            if(*pFlag == 0){
                fwrite("<pre>\n", sizeof(char) * 6, 1, html);
                *pFlag = 2;
            }
            else if(*pFlag == 2){
                fwrite("</pre>\n", sizeof(char) * 7, 1, html);
                *pFlag = 0;
            }

            fgets(buffer, size, md);
        }


        if(hCount > 0){
            //TERMINATE TAG
            terminate(pFlag, html);

            //BEGIN HEADING
            char *str = malloc(7);
            snprintf(str, 5, "<h%d>", hCount);
            fwrite(str, 4, 1, html);

            for(point = buffer + hCount + 1; *point != '\n'; point++){
                fwrite(point, sizeof(char), 1, html);
            }

            snprintf(str, 7, "</h%d>\n", hCount);
            fwrite(str, 6, 1, html);
            free(str);
        }
        else{
            if(*pFlag == 0){
                fwrite("<p>\n\t", sizeof(char) * 5, 1, html);
                *pFlag = 1;
            }
            else if(*pFlag == 1){
                fwrite("\t", sizeof(char), 1, html);
            }
            for(point = buffer; *point != '\0'; point++){
                fwrite(point, sizeof(char), 1, html);
            }
        }
    }

    fclose(md);
    fclose(html);

    free(buffer);
    free(point);

    return 0;

}