/*
 * MALHEUR - Automatic Analysis of Malware Behavior
 * Copyright (c) 2009-2012 Konrad Rieck (konrad@mlsec.org)
 * University of Goettingen, Berlin Institute of Technology 
 * --
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.  This program is distributed without any
 * warranty. See the GNU General Public License for more details. 
 * --
 */

#ifndef STATE_H
#define STATE_H

#include <farray.h>

typedef struct {
    unsigned int run;             /* Current run */
    unsigned int num_proto;       /* Number of prototype reports */
    unsigned int num_reject;      /* Number of rejected reports */
    farray_t *prototypes;	  /* Array of prototype vectors */
    farray_t *rejected;           /* Array of rejected vectors */
} state_t;

void state_init();
void state_save();
void state_load();

#endif                          /* STATE_H */
