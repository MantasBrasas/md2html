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
int flags[6] = {0, 0, 0, 0, 0, 0};

void parseProperties(FILE* fileIn, FILE* fileOut){
    char* buffer = malloc(size);
    char* point = "";

    fwrite("<ul>\n", 5, 1, fileOut);
    
    while(fgets(buffer, size, fileIn) != NULL){
        if(strncmp(buffer, "---\n", 4) == 0){
            fwrite("</ul>\n", 6, 1, fileOut);
            return;
        }
        fwrite("\t<li>", 5, 1, fileOut);
        for(point = buffer; *point != '\n' & *point != '\0'; point++){
            fwrite(point, 1, 1, fileOut);
        }
        fwrite("</li>\n", 6, 1, fileOut);
    }
    return;
}

void parseHeader(char* line, FILE* fileOut){
    char* point = "";
    int headerSize = 0;

    for(point = line; *point == '#'; point++){
        headerSize++;
    }

    if(headerSize > 6){
        fprintf(stderr, "invalid header length");
    }

    char* heading = malloc(6);
    sprintf(heading, "<h%d>", headerSize);
    fwrite(heading, 4, 1, fileOut);

    for(point = line + headerSize + 1; *point != '\0' && *point != '\n'; point++){
        fwrite(point, 1, 1, fileOut);
    }
    
    sprintf(heading, "</h%d>\n", headerSize);
    fwrite(heading, 6, 1, fileOut);

    return;
}

void writeLine(char* prefix, int prefixLength, char* line, FILE* fileOut){
    char* point = malloc(1);
    fwrite(prefix, prefixLength, 1, fileOut);
    for(point = line; *point != '\0'; point++){
        if(*point == '\n'){
            fwrite("\n", 1, 1, fileOut);
            return;
        }
        fwrite(point, 1, 1, fileOut);
    }
    return;
}

int updatePrefix(char* prefix, char* left, char* right){
    int sizeL = sizeof(left);
    int sizeR = sizeof(right);
    char* newPrefix = malloc(sizeL + sizeR);

    strncat(newPrefix, left, sizeL);
    strncat(newPrefix, right, sizeR);

    *prefix = *newPrefix;

    return sizeL + sizeR;
}

int buildPrefix(char* prefix){
    int prefixLength = 0;
    int tabs = flags[0] + (flags[1] - 1) + flags[2] + flags[4];

    for(int i = 0; i < tabs; i++){
        prefixLength = updatePrefix(prefix, prefix, "\t");
    }

    return prefixLength;
}

int updateFlags(char* prefix, int index, int value){
    int prefixLength = 0;
    flags[index] = value;
    prefixLength = buildPrefix(prefix);
    return prefixLength;
}

int parseFlags(char* prefix, char* line, FILE* fileIn, FILE* fileOut){
    int prefixLength = 0;
    //ordered list handling
    int len = sprintf(NULL, "%d. ", flags[1] + 1);
    char* olCompare = malloc(len + 1);
    sprintf(olCompare, "%d. ", flags[1] + 1);

    //check for header
    if(strncmp(line, "#", 1) == 0){
        parseHeader(line, fileOut);
        return prefixLength;
    }
    
    else{
        //CHECK FOR BLOCKQUOTE
        if(strncmp(line, ">", 1) == 0){
            int quoteCount = 0;
            for(char* point = line; *point == '>'; point++){
                quoteCount++;
            }
            if(quoteCount > flags[0]){
                for(int i = 0; i < quoteCount - flags[0]; i++){
                    prefixLength = updateFlags(prefix, 0, flags[0] + 1);
                    writeLine(prefix, prefixLength, "<div>\n", fileOut);
                }
            }
        }
        else{
            prefixLength = updateFlags(prefix, 0, 0);
        }
        
        //CHECK FOR ORDERED LIST
        if(strncmp(line, olCompare, len) == 0){
            prefixLength = updateFlags(prefix, 1, flags[1] + 1);
        }
        else{
            prefixLength = updateFlags(prefix, 1, 0);
        }

        if(strncmp(line, "- ", 2) == 0){
            prefixLength = updateFlags(prefix, 2, flags[2] + 1);
        }
        else{
            prefixLength = updateFlags(prefix, 2, 0);
        }

        if(strncmp(line, "```", 3) == 0){
            prefixLength = updateFlags(prefix, 3, flags[3] + 1);
        }
        
        if(strncmp(line, "- [ ] ", 6) == 0){
            prefixLength = updateFlags(prefix, 4, flags[4] + 1);
        }
        
        if(prefixLength == 0){
            if(flags[5] == 0){
                prefixLength = updateFlags(prefix, 5, 1);
                fwrite("<p>\n\t", 4, 1, fileOut);
            }
            else{
                fwrite("\t", 1, 1, fileOut);
            }
            
            for(char* point = line; *point != '\0'; point++){
                if(*point == '\n'){
                    fwrite("\n</p>\n", 5, 1, fileOut);
                    
                    break;
                }
                fwrite(point, 1, 1, fileOut);
            }
        }
    }
    return prefixLength;
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

    //PARSE PROPERTIES
    fgets(buffer, size, md);
    if(strncmp(buffer, "---\n", 4) == 0){
        parseProperties(md, html);
    }

    while(fgets(buffer, size, md) != NULL){
        char* prefix = "";
        //CLEAR LEADING SPACES/INDENTS IF NOT IN CODEBLOCK OR LIST
        if(flags[3] == 0 && flags[2] == 0 && flags[1] == 0){
            point = buffer;
            while((*point == ' ') || (*point == '\t')){
                buffer++;
                point++;
            }
        }

        parseFlags(prefix, buffer, md, html);
    }

    fclose(md);
    fclose(html);

    free(buffer);
    free(point);

    return 0;

}