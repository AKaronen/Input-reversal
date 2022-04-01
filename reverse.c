#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

// Create a struct for double linked list
struct lines{
    char *string; // string for text
    struct lines *prev; // pointer to previous node (left)
    struct lines *next; // pointer to next node (right)
};

// function to read lines and add them to the structs string variable
int read_lines(struct lines **line, FILE* stream){
    // get pointer to linked list's first node and stream where we want to read the text
    struct lines *lp;
    struct lines *new;
    char *tmp = NULL;
    size_t len = 0;

    // reading the input stream as long it has something to read
    while (getline(&tmp, &len, stream) != -1){
        // if the input steam is stdin, we stop reading when line contains only line break
        if(strcmp(tmp, "\n") == 0 && fileno(stream) == STDIN_FILENO){
            fprintf(stdout, "Ending input...\n\n");
            return 0;
        }
        // if the  node is null (list is empty), create a node
        if (*line == NULL){
            if ((*line = lp = malloc(sizeof(struct lines))) == NULL){
                fprintf(stderr, "malloc failed\n"); // if memory allocation goes wrong
                return -1;
            }
            lp->prev = lp->next = NULL; 
        }
        // if we need new node, create a one
        else{ 
            if ((new = lp->next = malloc(sizeof(struct lines))) == NULL){
                fprintf(stderr, "malloc failed\n"); // if memory allocation goes wrong
                return -1;
            }

            new->next = NULL; // put new node's next pointer to null (there is no node yet)
            new->prev = lp; // put previous node's previous pointer point to new node just created
            lp = new; // define new node to be current node for next round
        }

        // allocate memory for the nodes string variable
        if ((lp->string = malloc(strlen(tmp) + 1)) == NULL){
            fprintf(stderr, "malloc failed\n"); // if memory allocation goes wrong
            return -1;
        }
        // string is in variable tmp, put nodes string to be tmp
        strcpy(lp->string, tmp);
    }

    return 0;
}

// function to print lines in reverse order
void print_lines(struct lines *line, FILE *stream){
    // we get pointer to linked list's first node and output stream where to print lines
    struct lines *lp;
    lp = line;
    // finding the last node of the list going from beginning to an end
    while(1){
        if(lp->next == NULL){ // if next pointer is null we have found the last node
            break;
        }
        lp = lp->next; // define current node for next round
    }
    // when we have founded the last node we can print the string from last to fist node (reverse order)
    printf("\nPrinting in reverse\n");
    while(1){
        fprintf(stream, "%s", lp->string);
        if(lp->prev == NULL){ // if previous pointer is null we have found the fist node and printed all the string
            break;
        }
        lp = lp->prev; // define current node for next round
    }
}

// function to remove double linked list and free the memory
void delete_lines(struct lines *line){
    // we got pointer to the first node
    struct lines *lp;
    lp = line;
    while (lp != NULL){ // removing nodes as long as there are none left
        line = lp->next;
        free(lp->string);
        free(lp);
        lp = line;
    }
}


// main function where we find out where to read and write
int main(int argc, char* argv[]){
    // we get number of command line parameters and command line parameters as string array
    struct lines *line = NULL; // declare the first node to be null

    // if user has not given any other command line parameters than the programs name
    if(argc == 1){
        // reading from stdin and writing for stdout
        if (read_lines(&line, stdin) == -1)
            exit(1);
        print_lines(line, stdout);
    }




    // if user has given one additional command line parameter (input stream)
    if(argc == 2){

            // opening the file given by user, for read mode only
            FILE* fileInput = fopen(argv[1], "r");
            if (fileInput == NULL){
                fprintf(stderr, "cannot open file '%s'\n", argv[1]); // if could not open the file
                exit(1);
            }
            // Changing the input pipe to be file 
            dup2(fileno(fileInput), STDIN_FILENO);
            // reading lines and creating a list
            if (read_lines(&line, fileInput) == -1)
                exit(1);
            fclose(fileInput); // closing input file

            print_lines(line, stdout); // printing given lines in reverse order


    }


    // if user has given two additional command line parameters (input stream and output stream)
    if(argc == 3){

        // checking if the given files are same, if same user will get error message
        if(strcmp(argv[1],argv[2]) == 0){
            fprintf(stderr, "Input and output file must differ\n");
        }
        
        else{ // if the input file differ from output file

            // opening the input file given by user, only read mode
            FILE* fileInput = fopen(argv[1], "r");
            if (fileInput == NULL){
                fprintf(stderr, "cannot open file '%s'\n", argv[1]); // if could not open the file
                exit(1);
            }
            // Changing the input pipe
            dup2(fileno(fileInput), STDIN_FILENO);

            // reading lines and creating a list
            if (read_lines(&line, fileInput) == -1)
                exit(1);
            fclose(fileInput); // closing the input file


            // opening the output file given by user, only write mode
            FILE* fileOutput = fopen(argv[2], "w");
            if (fileOutput == NULL){
                fprintf(stderr, "cannot open file '%s'\n", argv[2]); // if could not open the file
                exit(1);
            }
            // Changing the output pipe
            dup2(fileno(fileOutput), STDOUT_FILENO);
            print_lines(line, fileOutput); // printing lines in reverse order into output stream given by user
            fclose(fileOutput); // closing output file
        }
    }

    // if user has given more arguments than needed, he/she will be given instructions how to run this program correctly
    if(argc>3){
        printf("usage: reverse <input> <output>\n");
        exit(1);
    }
    
    // finally freeing the memory
    delete_lines(line);
    
    return 0;
}

