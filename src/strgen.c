//
//  strgen.c
//  regen
//
//  Created by Luke  Dramko on 7/18/18.
//  Copyright © 2018 Luke  Dramko. All rights reserved.
//

#include "strgen.h"

float continue_value = .90;

/*
 The functions in this file work together to create a string that matches the input regex.  The regex is represented
 as a parse tree where nodes in the tree ("tokens") represent either a charater class (one or more characters) or
 an operator on the subtree, which could consist of character classes or other operators.
 
 The goal is to create a string, and to that end, the wrapper function generate_string prepares an initial array
 and sets up variables relating to its length and the next spot avaliable in the string.  A pointer to this string
 is passed back once a function is done working with it.
 
 Characters are only added to the string through the charclass0 function; other operators simply specify which character
 classes get to add their characters then.
 
 Because random values are used to generate the string, it's not possible to know exactly how big it'll be in the end.
 Thus, reallocation of the string in a vector-like fashion is required.
 
 With the exception of the function generate_string, all functions have the same parameters and return values:
 @param subtree A pointer to the root of the subtree being parsed into the string
 @param string a pointer to the char array which holds the string the regex output is being written to
 @param next The next avaliable spot in the string
 @param arrlen The current length of the string array.  Will be increased as necessary.
 @return the pointer to the output string
 
 The only function meant to be called by functions outside of this file is generate_string.
 */

/*
 * Instruction code 0: element is a character class.  Select one element from the character class.
 * Instruction code 1: execute all elements in sequence.  Can be thought of as () as well.
 * Instruction code 2: *  Repeat everything in element until random stop point
 * Instruction code 3: +  Add one, then repeat until random stop point.
 * Instruction code 4: ?  50/50 shot of it being there.
 * Instruction code 5: {n} Repeat n of times.
 * Instruction code 6: {n:m} Repeat at least n but no more than m times.
 * Instruction code 7: | select one of element, at random.  Similar to 0, but not necessarily a character class.
 */

/*
 @param tree A parse tree representing a regex
 @return A string that regex could match
 
 generate_string is a wrapper for the other functions in this file; it sets up the calls to subsequent functions.
 In particular, it allocates the starting character array and sets up its length variable
 */
char * generate_string(struct token * tree, float pval) {
    char * string = (char *)calloc(STARTING_OUTPUT_STRING_ARRAY_SIZE, sizeof(char));
    unsigned long arrlen = STARTING_OUTPUT_STRING_ARRAY_SIZE;
    unsigned long next = 0;
    
    //Set up the continue value for * and +, which is a global variable.
    continue_value = pval;
    
    string = registry(tree, string, &next, &arrlen);
    
    //Terminate string
    string[next] = '\0';
    
    return string;
}

/*
 @param subtree A pointer to the root of the subtree being parsed
 @param string a pointer to the char array which holds the string the regex output is being written to
 @param next The next avaliable spot in the string
 @param arrlen The current length of the string array.  Will be increased as necessary.
 @return the pointer to the output string
 
 Determines the appropriate function to call based on the operator code of the current root of the subtree.
 All calls to subtrees are rounted through registry.
 */
char * registry(struct token * subtree, char * string, unsigned long * next, unsigned long * arrlen) {
    int * code = subtree->code;
    if (code[0] == 0) {
        string = charclass0(subtree, string, next, arrlen);
    } else if (code[0] == 1) {
        string = doinorder1(subtree, string, next, arrlen);
    } else if (code[0] == 2) {
        string = star3(subtree, string, next, arrlen);
    } else if (code[0] == 3) {
        string = plus2(subtree, string, next, arrlen);
    } else if (code[0] == 4) {
        string = questionmark4(subtree, string, next, arrlen);
    } else if (code[0] == 5) {
        string = exactlyn5(subtree, string, next, arrlen);
    } else if (code[0] == 6) {
        string = betweennm6(subtree, string, next, arrlen);
    } else if (code[0] == 7) {
        string = or7(subtree, string, next, arrlen);
    }
    
    return string;
}

/*
 Selects one element from the character class and adds it to the string. arrlen is increased as needed.
 
 This is the only function which actually writes to the string.
 
 The function assumes that the current value of "next" is the value to write to in the string.  It increments
 "next" afterword.
 */
char * charclass0(struct token * subtree, char * string, unsigned long * next, unsigned long * arrlen) {
    if (*next == *arrlen - 2) {
        *arrlen *= 2;
        string = (char *)realloc(string, *arrlen * sizeof(char));
        if (string == NULL) { printf("realloc failure.\n"); exit(1); }
    }
    
    //Select a single random character from the character class and add it to the string.
    string[*next] = ((char *)(subtree->element))[random_in(0, subtree->length)];
    *next = *next + 1;
    return string;
}


char * doinorder1(struct token * subtree, char * string, unsigned long * next, unsigned long * arrlen) {
    unsigned long i = 0;
    for (; i < subtree->length; i++) {
        string = registry(((struct token *(*))(subtree->element))[i], string, next, arrlen);
    }
    
    return string;
}

/*
 Calls the subree at least once, then a random number of times
 */
char * plus2(struct token * subtree, char * string, unsigned long * next, unsigned long * arrlen) {
    do {
        string = registry(((struct token *(*))(subtree->element))[0], string, next, arrlen);
    } while(continue_value > random_float());
    
    return string;
}

/*
 Calls the subtree 0 or more times.
 */
char * star3(struct token * subtree, char * string, unsigned long * next, unsigned long * arrlen) {
    while(continue_value > random_float()) {
       string = registry(((struct token *(*))(subtree->element))[0], string, next, arrlen);
    }
    return string;
}

/*
 Has a 50% chance of calling the subtree
 */
char * questionmark4(struct token * subtree, char * string, unsigned long * next, unsigned long * arrlen) {
    if (random_float() > 0.5) {
        string = registry(((struct token *(*))(subtree->element))[0], string, next, arrlen);
    }
    return string;
}

/*
 Calls the subtree exactly n times, as defined by the second element of the operator code
 */
char * exactlyn5(struct token * subtree, char * string, unsigned long * next, unsigned long * arrlen) {
    unsigned long i = 0;
    for (; i < subtree->code[1]; i++) {
        string = registry(((struct token *(*))(subtree->element))[0], string, next, arrlen);
    }
    return string;
}

/*
 Calls the subtree between n and m times, as defined by the second and third elements of the operator code.
 */
char * betweennm6(struct token * subtree, char * string, unsigned long * next, unsigned long * arrlen) {
    unsigned long i = 0;
    unsigned long max = random_in(subtree->code[1], subtree->code[2] + 1);
    for (; i < max; i++) {
        string = registry(((struct token *(*))(subtree->element))[0], string, next, arrlen);
    }
    return string;
}

/*
 Calls one of the elements in the subtree
 */
char * or7(struct token * subtree, char * string, unsigned long * next, unsigned long * arrlen) {
    string = registry(((struct token *(*))(subtree->element))[random_in(0, subtree->length)], string, next, arrlen);
    return string;
}
