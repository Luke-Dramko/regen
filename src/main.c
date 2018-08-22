//
//  main.c
//  regen
//
//  Created by Luke  Dramko on 7/18/18.
//  Copyright © 2018 Luke  Dramko. All rights reserved.
//

//Header files
#include "global.h"
#include "utils.h"
#include "strgen.h"
#include "rdparser.h"

int main(int argc, const char * argv[]) {
    //Necessary input parameters
    const char * regex = NULL;
    unsigned long length = 0;  //of the regex, a character array.
    
    //Command line options
    const char * outflname = NULL;
    const char * inlines = NULL;
    const char * indec = NULL;     //The decimal value used for * and + in  the regex
    short overwrite = 0;
    short timerandom = 1;
    
    //Process command line arguments
    unsigned int i = 1; //Because argv[0] is the name of the function
    for (; i < argc; i++) {
        // --help prints an informational message and then exits.
        if (!strcmp(argv[i], "--help")) {
            //Call to function with informational message
            help_message();
            exit(0);
        }
        
        // -f provides the file name (for output).  When provided by -f, the file name can't be overridden by being the
        // second non-option argument.
        if (!strcmp(argv[i], "-f") && i + 1 < argc) {
            outflname = argv[i + 1];
            i++;
            continue;
        }
        
        // -l provides the number of times we want this regex to be evaluated.  Each time, the output is put on a
        // new line.
        if (!strcmp(argv[i], "-l") && i + 1 < argc) {
            inlines = argv[i + 1];
            i++;
            continue;
        }
        
        // -p provides the decimal value used for * and +
        if (!strcmp(argv[i], "-p") && i + 1 < argc) {
            indec = argv[i + 1];
            i++;
            continue;
        }
        
        // -o allows automatic overwriting of any output files present without a prompt.
        if (!strcmp(argv[i], "-o")) {
            overwrite = 1; //Basically the same as "true"
            continue;
        }
        
        // -d turns off time-based random number generation
        if (!strcmp(argv[i], "-d")) {
            timerandom = 0; //Basically the same as "true"
            continue;
        }
        
        // The first non-option argument is the regex pattern
        if (regex == NULL) {
            regex = argv[i];
            continue;
        }
        
        if (outflname == NULL) {
            outflname = argv[i];
            continue;
        }
        
        printf("Invalid argument \"%s\".\n", argv[i]);
        exit(2);
    }
    
    //Set up default values
    FILE * flptr = stdout;
    float cont = .9;
    int lines = 1;
    //overwrite's default (0) is set when it is initialized.
    
    /*
     Check validity of args and overwrite defaults, where applicable
     */
    if (!regex) {
        printf("Usage:\n\n   regen [OPTIONS] [PATTERN] [OPTIONS]\n\nTry \"regen --help\" for more information.\n");
        exit(2);
    }
    length = strlen(regex);
    
    if (timerandom) {
        srand((unsigned int)time(0));
    }
    
    if (outflname) {
        if(access(outflname, F_OK ) != -1 ) { // file exists
            if (!overwrite) {
                char * answer = NULL;
                size_t len;
                printf("Overwrite existing file %s? (y/n) ", outflname);
                getline(&answer, &len, stdin);
                if (answer[0] != 'y' && answer[0] != 'Y') {
                    exit(0); //User has opted not to overwrite the file.
                }
                free(answer);
            }
        }
        // file doesn't exist or overwrite
        flptr = fopen(outflname, "w");
    }
    
    //the cont value (for * and +) must be between 0 and MAX_DEC_CONT_VALUE, inclusive.
    if (indec) {
        cont = atof(indec);
        printf("boolean = %d\n", cont > MAX_DEC_CONT_VALUE || cont <= 0);
        printf("boolean = %d\n", cont > MAX_DEC_CONT_VALUE);
        printf("boolean = %d\n", cont <= 0);
        if (cont > MAX_DEC_CONT_VALUE || cont < 0) {
            printf("Invalid decimal value for -p \"%s\".\n", indec);
            printf("Values must be inside the range [0,%f).\n", MAX_DEC_CONT_VALUE);
            exit(2);
        }
    }
    
    if (inlines) {
        int temp = atoi(inlines);  //Using temp value to detect negatives instead of having them be very large positives.
        if (temp <= 0) {
            printf("Invalid integer for -l: \"%s\".\n", inlines);
            printf("Value must be an integer greater than zero.\n");
            exit(2);
        }
        lines = temp;
    }
    
    //Run a quick syntax check for grouping symbols - makes it easier on the parser.
    grpSymChk(regex, length);
    
    //Calling the recursive descent parser
    int position = 0;
    struct token * parsed_regex = parse_regex(regex, length, &position);
    
    //Generate the string and write it to the screen/file
    for (i = 0; i < lines; i++) {
        char * str = generate_string(parsed_regex, cont);
        fprintf(flptr, "%s\n", str);
        free(str);
    }
    
    //Clean up
    if (outflname) {
        fclose(flptr);
    }
    
    //Free stuff
    free_tree(parsed_regex);
    
    return 0;
}
