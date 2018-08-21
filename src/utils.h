//
//  utils.h
//  regen
//
//  Created by Luke  Dramko on 7/18/18.
//  Copyright © 2018 Luke  Dramko. All rights reserved.
//
#include "global.h"

#ifndef utils_h
#define utils_h

void print_tree(struct token *);
void grpSymChk(const char *, unsigned long length);
float random_float(void);
unsigned long random_in(unsigned long low, unsigned long high);
void free_tree(struct token *);
void help_message(void);

#endif /* utils_h */
