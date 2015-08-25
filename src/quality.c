/*
 * MALHEUR - Automatic Analysis of Malware Behavior
 * Copyright (c) 2009-2015 Konrad Rieck (konrad@mlsec.org)
 * University of Goettingen, Berlin Institute of Technology
 * --
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.  This program is distributed without any
 * warranty. See the GNU General Public License for more details.
 * --
 */

/**
 * @defgroup quality Quality functions
 * The module contains functions for evaluating the quality of
 * classification and clustering methods.
 * @author Konrad Rieck
 * @{
 */
#include "config.h"
#include "common.h"
#include "quality.h"
#include "util.h"

/* External variables */
extern int verbose;
extern config_t cfg;

/**
 * Computes quality measures for the label countment.
 * The function returns a static array. The code is not thread-safe.
 * @param y Labels of data points
 * @param a Assignments to clusters or classes
 * @param n Number of data points
 * @return array with quality values
 */
double *quality(unsigned int *y, unsigned int *a, int n)
{
    assert(y && a && n > 0);
    static double r[5];
    double mn, ac, bc, cc, dc;
    hist_t *h, *hi;
    count_t *ai;
    int i, j;

    /* Compute precision. This is ugly code. */
    h = hist_create(a, y, n);
    for (hi = h, i = 0, ac = 0; hi != NULL; hi = hi->hh.next, i++) {
        for (ai = hi->count, mn = 0; ai != NULL; ai = ai->hh.next)
            if (ai->count > mn)
                mn = ai->count;
        ac += mn;
    }
    hist_destroy(h);
    r[Q_PRECISION] = ac / n;

    /* Compute recall. This is again ugly code. */
    h = hist_create(y, a, n);
    for (hi = h, i = 0, ac = 0; hi != NULL; hi = hi->hh.next, i++) {
        for (ai = hi->count, mn = 0; ai != NULL; ai = ai->hh.next)
            if (ai->count > mn)
                mn = ai->count;
        ac += mn;
    }
    hist_destroy(h);
    r[Q_RECALL] = ac / n;

    /* Compute f-measure */
    r[Q_FMEASURE] = (2 * r[Q_RECALL] * r[Q_PRECISION]) /
        (r[Q_RECALL] + r[Q_PRECISION]);

    /* Compute similarity coefficients */
    ac = bc = cc = dc = 0;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            ac += (a[i] == a[j] && y[i] == y[j]) ? 1 : 0;
            bc += (a[i] != a[j] && y[i] != y[j]) ? 1 : 0;
            cc += (a[i] != a[j] && y[i] == y[j]) ? 1 : 0;
            dc += (a[i] == a[j] && y[i] != y[j]) ? 1 : 0;
        }
    }
    r[Q_RAND] = (ac + bc) / (ac + bc + cc + dc);
    r[Q_ARAND] = 2 * (ac * bc - cc * dc) /
        ((ac + dc) * (dc + bc) + (ac + cc) * (cc + bc));

    return r;
}

/**
 * Creates an histogram for each label containing assignments.
 * @param y Labels of data points
 * @param a Assignments to clusters or classes
 * @param n Number of data points
 * @return histogram struct
 */
hist_t *hist_create(unsigned int *y, unsigned int *a, int n)
{
    assert(y && a && n > 0);
    hist_t *entry, *hist = NULL;
    count_t *count = NULL;
    int i;

    /* Loop over  labels */
    for (i = 0; i < n; i++) {
        HASH_FIND_INT(hist, &y[i], entry);
        if (!entry) {
            entry = malloc(sizeof(hist_t));
            if (!entry) {
                error("Could not allocate histogram");
                hist_destroy(hist);
                return NULL;
            }

            /* Create new entry */
            entry->label = y[i];
            entry->count = NULL;
            entry->total = 0;

            /* Add entry */
            HASH_ADD_INT(hist, label, entry);
        }
        entry->total++;

        HASH_FIND_INT(entry->count, &a[i], count);
        if (!count) {
            count = malloc(sizeof(count_t));
            if (!count) {
                error("Could not allocate countments");
                hist_destroy(hist);
                return NULL;
            }
            count->label = a[i];
            count->count = 0;

            /* Add entry */
            HASH_ADD_INT(entry->count, label, count);
        }
        count->count++;
    }

    return hist;
}

/**
 * Prints the contents of a histogram for labels
 * @param h histogram struct
 */
void hist_print(hist_t * h)
{
    hist_t *hi;
    count_t *ai;

    for (hi = h; hi != NULL; hi = hi->hh.next) {
        printf("Label: %u\n", hi->label);
        printf("Total: %f\n", hi->total);

        printf("countments: ");
        for (ai = hi->count; ai != NULL; ai = ai->hh.next)
            printf("%u (%f) ", ai->label, ai->count);
        printf("\n");
    }
}

/**
 * Destroys a histogram.
 * @param h histrogram struct
 */
void hist_destroy(hist_t * h)
{
    hist_t *hi;
    count_t *ai;

    /* Iterate over classes */
    while (h) {
        hi = h;

        /* Delete indices */
        while (hi->count) {
            ai = hi->count;
            HASH_DEL(hi->count, ai);
            free(ai);
        }

        HASH_DEL(h, hi);
        free(hi);
    }
}

/** @} */
