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

#ifndef QUALITY_H
#define QUALITY_H

#include "util.h"

#ifdef HAVE_UTHASH_UTHASH_H
#include <uthash/uthash.h>
#elif HAVE_UTHASH_H
#include <uthash.h>
#else
#include "uthash.h"
#endif

/* Definitions of quality measures */
#define Q_PRECISION     0       /* Precision */
#define Q_RECALL        1       /* Recall */
#define Q_FMEASURE      2       /* F-measure */
#define Q_RAND          3       /* Rand index */
#define Q_ARAND         4       /* Adjusted rand */

/** 
 * Histogram bin for labels. The histogram is used to compute
 * performance measure such as precision and recall over a set
 * of predicited labels. 
 */
typedef struct {
    unsigned int label;         /**< True label */
    double total;               /**< Number of elements with labels */
    count_t *count;             /**< Predicted labels */
    UT_hash_handle hh;          /**< Hash table entry */
} hist_t;

/* Evaluation functions */
hist_t *hist_create(unsigned int *, unsigned int *, int);
void hist_print(hist_t *);
void hist_destroy(hist_t *);
double *quality(unsigned int *, unsigned int *, int);

#endif                          /* EVAL_H */
