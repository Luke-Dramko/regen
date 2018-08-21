//
//  utils.c
//  regen
//
//  Created by Luke  Dramko on 7/18/18.
//  Copyright © 2018 Luke  Dramko. All rights reserved.
//

#include "utils.h"

void print_tree(struct token * root) {
    printf("address = %p\n", root);
    printf("opcode = %d\n", root->code[0]);
    printf("  address of opcode = %p\n", root->code);
    if (root->code[0] == 5) {
        printf("n = %d\n", root->code[1]);
    } else if (root->code[0] == 6) {
        printf("n = %d\n", root->code[1]);
        printf("m = %d\n", root->code[2]);
    }
    printf("length = %lu\n", root->length);
    if (root->code[0] == 0) {
        printf("character class = %s\n", (char *)(root->element));
        printf("  address of character class = %p\n", root->element);
        printf("-----------------------------\n");
    } else {
        printf("  address of element array = %p\n", root->element);
        printf("-----------------------------\n");
        int i = 0;
        for (; i < root->length; i++) {
            printf("From %p\n", root);
            struct token * temp = ((struct token **)(root->element))[i];
            print_tree(temp);
        }
    }
    
}

/*
 * Checks that the grouping symbols (), {}, and [] are equal in number and open and close in the right
 * order (ie. there is always a ( before there is a ).
 * Terminates program execution upon discovery of an error.
 */
void grpSymChk(const char * regex, unsigned long length) {
    int paren = 0;   //()
    int cbrace = 0;  //{}
    int bracket = 0; //[]
    
    short notincc = 1; //Non-bracket grouping symbols don't count if they're inside a character class
    
    unsigned long i = 0;
    for (; i < length; i++) {
        //Skip escaped characters
        if (regex[i] == '\\' && i + 2 <= length) {
            i += 2;
        }
        
        //Count grouping symbols
        if (regex[i] == '(' && notincc) { paren++; }
        if (regex[i] == '{' && notincc) { cbrace++; }
        if (regex[i] == '[') { bracket++; notincc = 0; }
        if (regex[i] == ')' && notincc) { paren--;  }
        if (regex[i] == '}' && notincc) { cbrace--; }
        if (regex[i] == ']') { bracket--; notincc = 1;}
        
        //If any grouping symbol is negative, a closing symbol comes before an opening.
        if (paren < 0) {
            printf("Closing parenthesis comes before opening parenthesis.\n");
            exit(4);  //Indicates syntax error in regex.
        }
        if (cbrace < 0) {
            printf("Closing curly brace comes before opening curly brace.\n");
            exit(4);
        }
        if (bracket < 0) {
            printf("Closing bracket comes before opening bracket.\n");
            exit(4);
        }
    }
    
    //Make sure all pairs of parenthesis are closed.
    if (paren > 0) {
        printf("Opening parenthesis not closed.\n");
        exit(4);
    }
    if (cbrace > 0) {
        printf("Opening curly brace not closed.\n");
        exit(4);
    }
    if (bracket > 0) {
        printf("Opening bracket not closed (character class not closed).\n");
        exit(4);
    }
    //Good to go.
}

/*
 Generates a random value between 0 and 1
 */
float random_float() {
    return (float)rand() / (float)RAND_MAX;
}

/*
 Returns a random value between low and high, inclusive to the lowwer bound but exclusive to the upper.
 */
unsigned long random_in(unsigned long low, unsigned long high) {
    return ((unsigned long)rand() % (high - low)) + low;
}

/*
 Frees a tree and all elements associated with it, recursively.
 */
void free_tree(struct token * subtree) {
    unsigned long i = 0;
    if (subtree->code[0] == 0) {
        free(subtree->element);
    } else {
        for (; i < subtree->length; i++) {
            free_tree(((struct token **)(subtree->element))[i]);
        }
        free(subtree->element);
    }
    free(subtree->code);
    free(subtree);
}

void help_message() {
    printf("Usage:\n\nregen [OPTIONS] [PATTERN] [OPTIONS]\n\n");
    printf("regen is a utility that helps with visualizing regular expressions.  It interprets\n");
    printf("the pattern [PATTERN] and uses it to generate a string that the regular expression could\n");
    printf("match.\n\n");
    printf("Examples can be written out to a file, specified by a second argument or with the flag -f.\n\n");
    printf("Options\n\n");
    printf("  -d By default, regen uses the current time as a seed for random number generation to give seemingly\n");
    printf("     more random results.  -d turns this feature off.\n\n");
    printf("  -f must be followed by a file name.  It specifies the file name the output is to be written to.\n");
    printf("     If a second argument provides a file name and -f is used in addition, -f overrides.\n\n");
    printf("  -l indicates the number of example strings that should be written out.  The argument must be\n");
    printf("     followed by an integer.\n\n");
    printf("  -o if a file is being written and a file with the provided name already exists, overwrite it\n");
    printf("     without asking.\n\n");
    printf("  -p the * and + operators use random numbers to determine how many times to reproduce the\n");
    printf("     given segment.  Choose a number between 0 and 1.  A value closer to 1 will result in more\n");
    printf("     characters for * and +.  The devault value is .90 and the maximum value is %f.\n\n", MAX_DEC_CONT_VALUE);
    printf("Examples\n\n");
    printf("$ regen '([a-z]+.)*' -d -o -f out.txt\n");
    printf("$ regen '[0-9]*' out.txt -l 20\n\n");
    printf("Notes on regular expression syntax and support:\n\n");
    printf("Regular expression syntax varies slightly from language to language.\n");
    printf("The ^ (matches start of string) and $ (matches end of string) characters are not supported as they\n");
    printf("are meaningless in this context.\n");
    printf("The ^ symbol can be used to negate a character class.  Character classes with the ^ symbol (eg [^ab])\n");
    printf("will include all of the following characters:\n\n");
    printf("   %s\n\n", NOT_BASE_CHARACTER_SET);
    printf("except those included within the [].  The a-z, A-Z, and 0-9 shortcuts may be used in conjunction with ^.\n");
}
