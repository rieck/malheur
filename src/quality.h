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

#ifndef QUALITY_H
#define QUALITY_H

#include "uthash.h"

/* Definitions of quality measures */
#define Q_PRECISION     0       /* Precision */
#define Q_RECALL        1       /* Recall */
#define Q_FMEASURE      2       /* F-measure */
#define Q_RAND          3       /* Rand index */
#define Q_ARAND         4       /* Adjusted rand */

/* Assignment to each true label */
typedef struct {
    unsigned int label; /* Predicted label */
    double count;       /* Occurences of label */
    UT_hash_handle hh;  /* Hash table entry */
} assign_t;

/* Histogram bin for each true label */
typedef struct {
    unsigned int label;     /* Label */
    double total;           /* Number of elements */
    assign_t *assign;       /* Assignments  */
    UT_hash_handle hh;      /* Hash table entry */
} hist_t;

/* Evaluation functions */
hist_t *hist_create(unsigned int *, unsigned int *, int);
void hist_print(hist_t *);
void hist_destroy(hist_t *);
double *quality(unsigned int *, unsigned int *, int );

#endif                          /* EVAL_H */
