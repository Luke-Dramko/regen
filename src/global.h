//
//  global.h
//  regen
//
//  Created by Luke  Dramko on 7/18/18.
//  Copyright © 2018 Luke  Dramko. All rights reserved.
//

#ifndef global_h
#define global_h
struct token {
    int * code;
    void **element;
    unsigned long length;  //of element.
};

#endif /* global_h */

#ifndef MAX_DEC_CONT_VALUE
#define MAX_DEC_CONT_VALUE 0.98
#endif

#ifndef NOT_BASE_CHARACTER_SET
#define NOT_BASE_CHARACTER_SET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\"`~',./<>?;:[]{}|+=-_)(*&^%$#@!\\"
#endif

//Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
