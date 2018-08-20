//
//  strgen.h
//  regen
//
//  Created by Luke  Dramko on 7/18/18.
//  Copyright © 2018 Luke  Dramko. All rights reserved.
//

#include "global.h"
#include "utils.h"

#ifndef strgen_h
#define strgen_h

#include <stdio.h>

char * generate_string(struct token *, float pval);
char * registry(struct token *, char * string, unsigned long * next, unsigned long * arrlen);
char * charclass0(struct token *, char * string, unsigned long * next, unsigned long * arrlen);
char * doinorder1(struct token *, char * string, unsigned long * next, unsigned long * arrlen);
char * plus2(struct token *, char * string, unsigned long * next, unsigned long * arrlen);
char * star3(struct token *, char * string, unsigned long * next, unsigned long * arrlen);
char * questionmark4(struct token *, char * string, unsigned long * next, unsigned long * arrlen);
char * exactlyn5(struct token *, char * string, unsigned long * next, unsigned long * arrlen);
char * betweennm6(struct token *, char * string, unsigned long * next, unsigned long * arrlen);
char * or7(struct token *, char * string, unsigned long * next, unsigned long * arrlen);

#endif /* strgen_h */

#ifndef STARTING_OUTPUT_STRING_ARRAY_SIZE
#define STARTING_OUTPUT_STRING_ARRAY_SIZE 32
#endif
