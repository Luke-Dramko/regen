//
//  rdparser.h
//  regen
//
//  Created by Luke  Dramko on 7/18/18.
//  Copyright © 2018 Luke  Dramko. All rights reserved.
//

#include "global.h"

#ifndef rdparser_h
#define rdparser_h

struct token * parse_regex(const char * regex, unsigned long length, int * r);
struct token * charclass(const char * regex, unsigned long length, int * r);
char escape(const char * regex, unsigned long length, int * r);
int * repeat_times(const char * regex, unsigned long length, int * r);
struct token * notcharclass(const char * regex, unsigned long length, int *r);


#endif /* rdparser_h */

#ifndef DEFAULT_ELEMENT_ARRAY_SIZE
#define DEFAULT_ELEMENT_ARRAY_SIZE 10
#endif
