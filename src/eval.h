/*
 * MALHEUR - Automatic Malware Analysis on Steroids
 * Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
 * Berlin Institute of Technology (TU Berlin).
 * --
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.  This program is distributed without any
 * warranty. See the GNU General Public License for more details. 
 * --
 */

#ifndef EVAL_H
#define EVAL_H

#include "uthash.h"

#if 0
typedef struct {
    int num;            /* Number */
    double count;       /* Occurences of number */
    UT_hash_handle hh;  /* Hash table entry */
} num_t ;

typedef struct {
    int label;          /* Label of class */
    double total;       /* Number of elements */
    num_t *index;       /* Indices of class */
    UT_hash_handle hh;  /* Hash table entry */
} hist_t;

hist_t *hist_create(int *, int *, int);
void hist_print(hist_t *);
void hist_destroy(hist_t *);
#endif


#endif                          /* EVAL_H */
