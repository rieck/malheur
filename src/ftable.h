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

#ifndef FTABLE_H
#define FTABLE_H

#include "fvec.h"
#include "uthash.h"

/** 
 * Entry of feature table
 */
typedef struct {
    feat_t key;            /**< Feature key */
    char *data;            /**< Feature data */
    int len;               /**< Length of data */
    UT_hash_handle hh;     /**< Uthash handle */
} fentry_t;

void ftable_put(feat_t, char *, int);
fentry_t *ftable_get(feat_t);
void ftable_init();
void ftable_destroy();
unsigned long ftable_size();
void ftable_print();
void ftable_remove(feat_t);
void ftable_save(gzFile *z);
void ftable_load(gzFile *z);
int ftable_enabled();

#endif                          /* FTABLE_H */
