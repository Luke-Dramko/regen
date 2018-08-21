//
//  rdparser.c
//  regen
//  A recursive descent parser that converts the regex into a tree
//
//  Created by Luke  Dramko on 7/18/18.
//  Copyright © 2018 Luke  Dramko. All rights reserved.
//

#include "rdparser.h"
#include "utils.h"

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
 @param regex a const string containing the regular expression to be parsed.
 @param length of the regex
 @param r a pointer to the index of the current character of regex.
 @return a pointer to the regular expession tree.
 
 Converts the regular expression into a parse tree.
 */
struct token * parse_regex(const char * regex, unsigned long length, int * r) {
    //A token consists of two parts - an instruction and an element.
    //The element could be a character class, or another element derived from a character class.  It could also
    //be a list of multiple mixed character/derived classes for operators that support more than entry in "element."
    //The instruction operates on the element.  Examples include +, *, ?, {n:m}, etc.
    struct token * parent = (struct token *)calloc(1, sizeof(struct token));  //The node in the tree
    struct token *(*children) = (struct token *(*))calloc(DEFAULT_ELEMENT_ARRAY_SIZE, sizeof(struct token *)); //The array that "element" points to.
    //children will be assigned to parent after everything is assigned to array.
    int clength = DEFAULT_ELEMENT_ARRAY_SIZE;
    struct token * class; //The current character class in the loop below.  Gauranteed to be assigned before use.
    int * code; //The current operator code in the loop below.  Gauranteed to be assigned before use.
    int nlength; //The current length in the loop below.  Gauranteed to be assigned before use.
    struct token *(*arr);
    
    //Check for allocation failure
    if (parent == NULL || children == NULL) {printf("calloc failure.\n"); exit(1);}
    
    int c = 0; //represents the current child (array index)
    //r represents the index of the current character of the regex.
    for (; *r < length; c++) {
        
        //Exit out of one layer deeper.
        if (regex[*r] == ')') {
            if (children[0] == NULL) {
                printf("Parentheses contain nothing.\n");
                exit(4);
            } else {
                *r = *r + 1;
                parent->element = (void **)children;
                parent->length = c;
                parent->code = (int *)malloc(sizeof(int));
                if (!(parent->code)) { printf("malloc failure.\n"); exit(1); }
                parent->code[0] = 1;
                return parent;
            }
        }
        
        //If we need to grow the array "children", we must do it before assignment.
        if (c == clength - 1) {
            clength += 10;
            children = (struct token *(*))realloc(children, clength * sizeof(struct token *));
            if (children == NULL) { printf("realloc failure.\n"); exit(1); }
        }
        
        //Parentheses cause the program to go a level deeper in the tree.
        if (regex[*r] == '(') {
            //Recusively build tree
            *r = *r + 1;
            class = parse_regex(regex, length, r);
        } else {
            //Generate a character class
            class = charclass(regex, length, r);
        }
        
        /* This section recognizes and interprets various functional opterators (ex. *, +, |).
         Each struct token has 3 components:
         1. A pointer to an int (possibly an array) containing the opcode
         2. A pointer to an array of struct token * (or char *, but that's handeled in the character class function).
         3. The length of the array in 2.
         These values are stored in the variables code, r, and nlength, respectively.  They are
         assigned to a new struct node at the bottom of the loop.
         */
        
        
        if (regex[*r] == '*') { //*: Operator code 2
            code = (int *)malloc(sizeof(int));   //Allocate memory for operator code.
            arr = calloc(1, sizeof(struct token *)); //An array of struct token * of size one.
            if (!code || !arr) { printf("memory allocation failure.\n"); exit(1); }
            arr[0] = class;  //This token, of the * operator, is one of possibly many in the parent 1 operator token
            code[0] = 2;  //2 indictates *
            nlength = 1;  //The * operator works on only one element at a time.
            *r = *r + 1;
        } else if (regex[*r] == '+') {  //+: Operator code 3
            code = (int *)malloc(sizeof(int));
            arr = calloc(1, sizeof(struct token *)); //An array of struct token * of size one.
            if (!code || !arr) { printf("memory allocation failure.\n"); exit(1); }
            arr[0] = class;
            code[0] = 3;
            nlength = 1;
            *r = *r + 1;
        } else if (regex[*r] == '?') {  //?: Operator code 4
            code = (int *)malloc(sizeof(int));
            arr = calloc(1, sizeof(struct token *)); //An array of struct token * of size one.
            if (!code || !arr) { printf("memory allocation failure.\n"); exit(1); }
            arr[0] = class;
            code[0] = 4;
            nlength = 1;
            *r = *r + 1;
        } else if (regex[*r] == '{') { //{n} or {n:m}.  The repeat_times function will figure out which one.
            *r = *r + 1;
            code = repeat_times(regex, length, r);
            arr = calloc(1, sizeof(struct token *)); //An array of struct token * of size one.
            if (!arr) {printf("memory allocation failure.\n"); exit(1); }
            arr[0] = class;
            nlength = 1;
        } else if (regex[*r] == '|') { //Operator code 7
            code = (int *)malloc(sizeof(int));
            int arrlen = 3;
            arr = calloc(arrlen, sizeof(struct token *));
            if (!code || !arr) { printf("memory allocation failure.\n"); exit(1); }
            arr[0] = class;
            code[0] = 7;
            nlength = 1;
            
            //There can be many character classes or-ed together.
            while (regex[*r] == '|') {
                *r = *r + 1;
                
                //The | operator operates on two (or more) char classes.
                if (regex[*r] == '(') {
                    *r = *r + 1;
                    class = parse_regex(regex, length, r);
                } else {
                    class = charclass(regex, length, r);
                }
                
                if (nlength > arrlen - 1) {
                    arrlen += 3;
                    arr = realloc(arr, arrlen * sizeof(struct token *));
                    if (!arr) { printf("realloc failure.\n"); exit(1); }
                }
                arr[nlength] = class;
                nlength++;
            }
        } else {
            code = (int *)malloc(sizeof(int));
            arr = calloc(1, sizeof(struct token *)); //An array of struct token * of size one.
            if (!code || !arr) { printf("malloc failure.\n"); exit(1); }
            arr[0] = class;
            code[0] = 1;
            nlength = 1;
        }
        
        struct token * new = calloc(1, sizeof(struct token));
        if (!new) {printf("calloc failure.\n"); exit(1); }
        new->element = (void **)arr;
        new->length = nlength;
        new->code = code;
        children[c] = new;
    }
    
    //Attach array to parent
    parent->element = (void *(*))children;
    parent->length = c;
    parent->code = malloc(sizeof(int));
    (parent->code)[0] = 1;
    
    return parent;
}

/*
 @param regex the expression to generate classes from
 @param length the length of the regex
 @param r a pointer the chunk of memory keeping track of the current position in 'regex'.
 @return a pointer to a struct token representing the character class.
 
 A character class is the basic data type all other data types in the parse tree are derived from.  A character
 class consists of a set of characters, represented by an array.  A terminal null character is provided to make
 the class a string for debugging purposes.
 
 This function generates a character class from notation provided in the regex.  Character classes can be defined
 with the notation [...].  There are 3 special values supported inside a character class: a-z, A-Z, and 0-9.  They
 define the lower case alphabet, the upper case alphabet, and the digits, respectively.  Anything else inside the
 character class (escept escapes, see below) is interpreted literally.
 Examples: [a-z], [a-z0-9], [89hjq], [gt&*+a-z]
 
 The escape character treats the next character in the expression literally, disregarding its value as a special
 character.  Inside a character class, you only need to escape ']' or '['
 */
struct token * charclass(const char * regex, unsigned long length, int * r) {
    struct token * class = (struct token *)calloc(1, sizeof(struct token));
    class->code = (int *)malloc(sizeof(int));
    (class->code)[0] = 0;  //A character class has operator code zero
    
    //Detect invalid characters (These are operators; if they are to be used as characters, they should be escaped).
    char l = regex[*r];
    if (l == '*' || l == '+' || l == '?' || l == '{' || l == '}' || l == '|') {
        printf("Character %c invalid as character class.\n", l);
        exit(4);
    }
    
    //The . character counts for any character except newlines.
    if (regex[*r] == '.') { //NOT_BASE_CHARACTER_SET includes all of the visible, non-whitespace characters in ASCII
        size_t notlen = strlen(NOT_BASE_CHARACTER_SET);
        class->element = calloc(notlen + 1, sizeof(char));
        if (class->element == NULL) {printf("calloc failure.\n"); exit(1); }
        memcpy(class->element, NOT_BASE_CHARACTER_SET, (notlen + 1) * sizeof(char));
        class->length = notlen;
        *r = *r + 1;
        return class;
    }
    
    //Support for escape sequences
    if (regex[*r] == '\\') { //Is an escape character
        //Add support for character-class escapes like (NOT YET IMPLEMENTED).
        
        class->element = calloc(2, sizeof(char)); //zero-s out allocated memory.
        if (class->element == NULL) {printf("calloc failure.\n"); exit(1); }
        //The second space is for the string terminating null character.
        //While the null character doesn't actually have to be there, it makes it easy to print for debugging.
        ((char *)(class->element))[0] = escape(regex, length, r);
        class->length = 1;
        return class;  //The escape function does all necessary incrementing, so this function doesn't have to.
    }
    
    //Support for character classes defined by [...]
    if (regex[*r] == '[') {  //Generate character class from bracket notation
        *r = *r + 1;
        if (regex[*r] == ']') {
            printf("Empty character class.\n");
            exit(4);
        } else if (regex[*r] == '^') {
            *r = *r + 1; //account for the ^
            return notcharclass(regex, length, r);
        }
        int numchars = 0;
        unsigned long clen = 27;  //Size of the English alphabet, plus one.
        char * chars = (char *)calloc(clen, sizeof(char)); //Adds the terminating NULL
        if (!chars) { printf("calloc failure.\n"); exit(1); }
        
        //Opening and closing brackets are checked before this class is called, but \0 is checked anyway.
        while (regex[*r] != ']' && regex[*r] != '\0') {
            //Handles special value a-z inside a character class
            if (*r + 2 < length && regex[*r] == 'a' && regex[*r + 1] == '-' && regex[*r + 2] == 'z') {
                if (numchars + 26 > clen - 1) {
                    clen += 26;
                    chars = (char *)realloc(chars, clen * sizeof(char));
                    if (!chars) { printf("realloc failure.\n"); exit(1); }
                    chars[clen - 1] = '\0'; //realloc does not initialize memory to zero.
                }
                int i = 97;
                for (; i < 123; i++ ) {chars[numchars] = i; numchars++; }; //Add a-z by their ascii values
                *r += 3;  //Move up *r by three spots (a, -, and z were used).
                
                //Handles special value A-Z inside a character class
            } else if (*r + 2 < length && regex[*r] == 'A' && regex[*r + 1] == '-' && regex[*r + 2] == 'Z') {
                if (numchars + 26 > clen - 1) {
                    clen += 26;
                    chars = (char *)realloc(chars, clen * sizeof(char));
                    if (!chars) { printf("realloc failure.\n"); exit(1); }
                }
                int i = 65;
                for (; i < 91; i++ ) {chars[numchars] = i; numchars++; };
                *r += 3;
                
                //Handles special value 0-9 inside a character class
            } else if (*r + 2 < length && regex[*r] == '0' && regex[*r + 1] == '-' && regex[*r + 2] == '9') {
                //Resize array if needed.
                if (numchars + 10 > clen - 1) {
                    clen += 10;
                    chars = (char *)realloc(chars, clen * sizeof(char));
                    if (!chars) { printf("realloc failure.\n"); exit(1); }
                }
                int i = 48;
                for (; i < 58; i++ ) {chars[numchars] = i; numchars++; }; //Fill in the values 0 through 9 in the array
                *r += 3;
            } else {
                if (numchars + 1 > clen - 1) {
                    clen += 10;
                    chars = (char *)realloc(chars, clen * sizeof(char));
                    if (!chars) { printf("realloc failure.\n"); exit(1); }
                }
                chars[numchars] = escape(regex, length, r);
                numchars++;
            }
        }
        chars[numchars] = '\0'; //Makes it a string for easy debugging.
        //Reallocation scheme guarantees the extra spot exists.
        
        *r = *r + 1; //Account for the ] at the end of the class.
        
        class->element = (void **)chars;
        class->length = numchars;
        
        return class;
    }
    
    
    //Character class consisting of single character.
    class->element = calloc(2, sizeof(char)); //zero-s out allocated memory.
    if (class->element == NULL) {printf("calloc failure.\n"); exit(1); }
    ((char *)(class->element))[0] = regex[*r];
    class->length = 1;
    *r = *r + 1;
    return class;
}

/*
 The function defines a character class with ^ in it.  It is responsible for creating the entire class by itself,
 not just the element of the class.
 */
struct token * notcharclass(const char * regex, unsigned long length, int * r) {
    //We enter the function with the [ and ^ consumed by charclass.
    char notclass[] = NOT_BASE_CHARACTER_SET;
    unsigned long notlength = strlen(notclass);
    unsigned long j;  //Index of notclass while searching notclass for a particular character
    char i; //The specific character's ascii value as generated by the loops for a-z, A-Z, and 0-9
    
    while (regex[*r] != ']') {
        //Remove a-z
        if (*r + 2 < length && regex[*r] == 'a' && regex[*r + 1] == '-' && regex[*r + 2] == 'z') {
            i = 97;
            for (; i < 123; i++ ) {
                j = 0;
                while (j < notlength && notclass[j] != i) { //Find the character in the NOT class
                    j++;
                }
                //Delete character, if that character was in the base NOT class to begin with
                if (j < notlength) {
                    notclass[j] = notclass[notlength - 1];
                    notlength--;
                    notclass[notlength] = '\0';
                }
            }
            *r += 3; //Account for a, -, and z
        //Remove A-Z
        } else if (*r + 2 < length && regex[*r] == 'A' && regex[*r + 1] == '-' && regex[*r + 2] == 'Z') {
            i = 65;
            for (; i < 91; i++ ) {
                j = 0;
                while (j < notlength && notclass[j] != i) { //Find the character in the NOT class
                    j++;
                }
                //Delete character, if that character was in the base NOT class to begin with
                if (j < notlength) {
                    notclass[j] = notclass[notlength - 1];
                    notlength--;
                    notclass[notlength] = '\0';
                }
            }
            *r += 3;
        //Remomve 0-9
        } else if (*r + 2 < length && regex[*r] == '0' && regex[*r + 1] == '-' && regex[*r + 2] == '9') {
            i = 48;
            for (; i < 58; i++ ) {
                j = 0;
                while (j < notlength && notclass[j] != i) { //Find the character in the NOT class
                    j++;
                }
                //Delete character, if that character was in the base NOT class to begin with
                if (j < notlength) {
                    notclass[j] = notclass[notlength - 1];
                    notlength--;
                    notclass[notlength] = '\0';
                }
            }
            *r += 3;
        } else {
            j = 0;
            char c = escape(regex, length, r);  //Increments r for you
            while (j < notlength && notclass[j] != c) { //Find the character in the NOT class
                j++;
            }
            //Delete character, if that character was in the base NOT class to begin with
            if (j < notlength) {
                notclass[j] = notclass[notlength - 1];
                notlength--;
                notclass[notlength] = '\0';
            }
        }
    }
    *r = *r + 1; //Accounts for the ].
    
    struct token * nottoken = (struct token *)calloc(1, sizeof(struct token));
    nottoken->length = notlength;
    nottoken->element = (void **)calloc(notlength + 1, sizeof(char));
    strcpy((char *)nottoken->element, notclass);
    nottoken->code = calloc(1, sizeof(int)); //Initializes to zero, so code == 0.
    
    return nottoken;
}

/*
 If the current character is an escape character, returns the value of the next character in the array (if one exists)
 If the current character is not an escape character, returns the current character.
 Increments *r to account for changes.
 */
char escape(const char * regex, unsigned long length, int * r) {
    if (regex[*r] == '\\') { //Is an escape character
        if (*r + 1 < length) {
            *r = *r + 2;
            return regex[*r - 1];
        } else {
            printf("Regular expression ends with escape character that escapes nothing.\n");
            exit(4);
        }
        
    }
    
    //Not an escape character.
    *r = *r + 1;
    return regex[*r - 1];
}


/*
 The repeat_times function parses the {n} and {n:m} operators (codes 5 and 6, respectively). Numbers are checked
 for validity; they must be positive and fit into an signed integer.
 
 This function only returns the operator code, either 5 or 6, and the associated information in their respective arrays.
 (Operator code 5 has the exact number of times to repeat as its second array element, and operator code 6 has the
 upper and lower bounds of the range to repeat in its second and third array elements).
 */
int * repeat_times(const char * regex, unsigned long length, int * r) {
    int num1 = -1;
    unsigned short numstr_len = 10;
    char numstr[10];
    
    int i = 0;
    while(regex[*r] > 47 && regex[*r] < 58 && *r < length && i < numstr_len - 2) {
        numstr[i] = regex[*r];
        *r = *r + 1;
        i++;
    }
    numstr[i] = '\0';
    
    //The grpSymChk function, called before this, guarantees there is a closing curly brace.
    
    //Check for overflow
    num1 = atoi(numstr);
    if (num1 < 0) { printf("Number %s is too large.\n", numstr); exit(4); }
    
    if (regex[*r] == '}') {  //Operator {n}, code = 5
        int * code = calloc(2, sizeof(int));
        if (!code) { printf("calloc failure.\n"); exit(1); }
        *r = *r + 1;
        code[0] = 5;
        code[1] = num1;
        return code;
    } else if (regex[*r] > 47 && regex[*r] < 58) {
        printf("Number beginning with %s is too large.\n", numstr);
        exit(4);
    } else if (regex[*r] != ':' && regex[*r] != ',') {
        printf("Invalid character: {%s%c\n", numstr, regex[*r]);
        printf("Format this operator like: {2:8} or {2,8}\n");
        exit(4);
    }
    //This means theres a : or , left, which is the {n:m} operator (code = 6)
    *r = *r + 1;
    i = 0;
    
    //Transfer the characters making up the 'm' portion of the {n:m} to a temporary substring which will
    //be fed into the atoi function.
    while(regex[*r] > 47 && regex[*r] < 58 && *r < length && i < numstr_len - 2) {
        numstr[i] = regex[*r];
        *r = *r + 1;
        i++;
    }
    numstr[i] = '\0';
    
    if (regex[*r] != '}') {
        if (regex[*r] > 47 && regex[*r] < 58) {
            printf("Number beginning with %s to large.\n", numstr);
        } else {
            printf("Invalid closing character '}'.\n");
        }
        exit(4);
    }
    *r = *r + 1;  //Account for closing }
    
    //The {n:m} operator requires 3 integers as part of its operator code.  The first is the operator code itself, 6.
    //The second is the lower bound, n, and the third is the upper bound, m.
    int * code = (int *)calloc(3, sizeof(int));
    if (!code) { printf("calloc failure.\n"); exit(1); }
    code[0] = 6;
    code[1] = num1;
    
    code[2] = atoi(numstr);
    if (code[2] < 0) { printf("Number %s is too large.\n", numstr); exit(4); }
    if (code[1] > code[2]) {
        printf("n must be less than or equal to m in {n:m} or {n,m}).\n");
        printf("You had {%d:%d}.\n", code[1], code[2]);
        exit(4);
    }
    
    return code;
}
