#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>


struct lines
{
    char *string;
    struct lines *prev;
    struct lines *next;
};

int read_lines(struct lines **line, FILE* stream)
{
    struct lines *lp;
    struct lines *new;
    char *tmp = NULL;
    size_t len = 0;

    while (getline(&tmp, &len, stream) != -1)
    {
        if(strcmp(tmp, "\n") == 0 && fileno(stream) == STDIN_FILENO){
            fprintf(stdout, "Ending input...\n\n");
            return 0;
        }
        if (*line == NULL)
        {
            if ((*line = lp = malloc(sizeof(struct lines))) == NULL)
            {
                fprintf(stderr, "malloc failed\n");
                return -1;
            }
            lp->prev = lp->next = NULL;
        }
        else
        { 
            if ((new = lp->next = malloc(sizeof(struct lines))) == NULL)
            {
                fprintf(stderr, "malloc failed\n");
                return -1;
            }

            new->next = NULL;
            new->prev = lp;
            lp = new;
        }

        if ((lp->string = malloc(strlen(tmp) + 1)) == NULL)
        {
            fprintf(stderr, "malloc failed\n");
            return -1;
        }

        strcpy(lp->string, tmp);
    }

    return 0;
}

void print_lines(struct lines *line, FILE *stream)
{
    struct lines *lp;
    lp = line;
    while(1){
        if(lp->next == NULL){
            break;
        }
        lp = lp->next;
    }

    printf("\nPrinting in reverse\n");
    while(1){
        fprintf(stream, "%s", lp->string);
        if(lp->prev == NULL){
            break;
        }
        lp = lp->prev;
    }
}

void delete_lines(struct lines *line)
{
    struct lines *lp;

    lp = line;
    while (lp != NULL)
    {
        line = lp->next;
        free(lp->string);
        free(lp);
        lp = line;
    }
}

int main(int argc, char* argv[])
{
    struct lines *line = NULL;

    if(argc == 1){
        if (read_lines(&line, stdin) == -1)
            exit(1);
        print_lines(line, stdout);
    }





    if(argc == 2){

            // Changing the input pipe
            FILE* fileInput = fopen(argv[1], "r");
            if (fileInput == NULL)
            {
                fprintf(stderr, "cannot open file '%s'\n", argv[1]);
                exit(1);
            }
            dup2(fileno(fileInput), STDIN_FILENO);

            if (read_lines(&line, fileInput) == -1)
                exit(1);
            fclose(fileInput);

            print_lines(line, stdout);


    }



    if(argc == 3){


        if(strcmp(argv[1],argv[2]) == 0){
            fprintf(stderr, "Input and output file must differ\n");
        }
        
        else{

            // Changing the input pipe
            FILE* fileInput = fopen(argv[1], "r");
            if (fileInput == NULL)
            {
                fprintf(stderr, "cannot open file '%s'\n", argv[1]);
                exit(1);
            }
            dup2(fileno(fileInput), STDIN_FILENO);


            if (read_lines(&line, fileInput) == -1)
                exit(1);
            fclose(fileInput);


            // Changing the output pipe
            FILE* fileOutput = fopen(argv[2], "w");
            if (fileOutput == NULL)
            {
                fprintf(stderr, "cannot open file '%s'\n", argv[2]);
                exit(1);
            }

            dup2(fileno(fileOutput), STDOUT_FILENO);
            print_lines(line, fileOutput);
            fclose(fileOutput);
        }
    }


    if(argc>3){
        printf("usage: reverse <input> <output>\n");
        exit(1);
    }
    
    delete_lines(line);

    

    return 0;
}

