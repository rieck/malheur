/*
 * MALHEUR - Automatic Analysis of Malware Behavior
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

#ifndef UTIL_H
#define UTIL_H

#include "config.h"

#ifdef HAVE_UTHASH_UTHASH_H
#include <uthash/uthash.h>
#endif
#ifdef HAVE_UTHASH_H
#include <uthash.h>
#endif

/* Progress bar stuff */
#define PROGBAR_LEN     52
#define PROGBAR_EMPTY   ':'
#define PROGBAR_FULL    '#'
#define PROGBAR_DONE    '#'
#define PROGBAR_FRONT   '|'

/**
 * Counts for predicted labels 
 */
typedef struct {
    unsigned int label;         /**< Predicted label */
    double count;               /**< Number of elemtns with label */
    UT_hash_handle hh;          /**< Hash table entry */
} count_t;

/* Fatal message */
#ifndef fatal
#define fatal(...)     {err_msg("Error", __func__, __VA_ARGS__); exit(-1);}
#endif
/* Error message */
#ifndef error
#define error(...)     {err_msg("Error", __func__, __VA_ARGS__);}
#endif
/* Warning message */
#ifndef warning
#define warning(...)   {err_msg("Warning", __func__, __VA_ARGS__);}
#endif

/** 
 * Structure for indexed data. The structure enables comparing and sorting 
 * data along with its indices, such that the order of indices can be later 
 * retrieved.
 */
typedef struct {
    void *ptr;                  /**< Pointer to original data */
    int idx;                    /**< Index number */
    int (*cmp) (const void *, const void *);    /**< Comparison function */
} index_t;

/* Utility functions functions */
void err_msg(char *, const char *, char *, ...);
void prog_bar(double, double, double);
double time_stamp();
int decode_string(char *);
char *file_suffix(char *file);
char *load_file(char *, char *);
int copy_file(char *src, char *dst);
void list_dir_entries(char *dir, int *, int *);
void list_arc_entries(char *arc, int *, int *);

/* Version */
void malheur_version(FILE *f);

/* Comparison function */
int *qsort_idx(void *b, size_t n, size_t w,
               int (*c) (const void *, const void *));
int cmp_feat(const void *, const void *);
int cmp_index(const void *, const void *);

/* Useful math functions */
int array_max(double *, int);
int array_min(double *, int);
long tria_size(long);
long tria_pos(long, long, long);

#endif                          /* UTIL_H */
