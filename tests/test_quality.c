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
 */

#include "tests.h"
#include "quality.h"

/* Global variables */
int verbose = 0;
config_t cfg;

/* Test structure */
typedef struct {
    unsigned int y[4];          /* True labels */
    unsigned int a[4];          /* Assigned labels */
    double e[5];                /* Quality measures */
} test_t;

/* Quality test cases */
test_t tests[] = {
    {{0, 0, 1, 1}, {1, 1, 2, 2}, {1.0000, 1.0000, 1.0000, 1.0000, 1.0000}},
    {{0, 0, 0, 0}, {1, 1, 3, 3}, {1.0000, 0.5000, 0.6667, 0.5000, 0.0000}},
    {{0, 0, 1, 1}, {1, 1, 1, 1}, {0.5000, 1.0000, 0.6667, 0.5000, 0.0000}},
    {{1, 2, 3, 4}, {1, 1, 1, 1}, {0.2500, 1.0000, 0.4000, 0.2500, 0.0000}},
    {{1, 1, 2, 2}, {1, 1, 1, 3}, {0.7500, 0.7500, 0.7500, 0.6250, 0.2500}},
    {{1, 1, 1, 1}, {1, 1, 1, 1}, {-1, -1, -1, -1, -1}}
};

/**
 * Test the generic quality function
 */
int test_quality()
{
    int i, j, err = 0;
    double *e;

    test_printf("Computing quality measures");
    for (i = 0; tests[i].e[0] > -1; i++) {
        e = quality(tests[i].y, tests[i].a, 4);
        for (j = 0; j < 5; j++)
            err += fabs(tests[i].e[j] - e[j]) > 1e-3;
    }

    test_return(err, i * 5);
    return err;
}

/**
 * Main function
 */
int main(int argc, char **argv)
{
    int err = FALSE;

    err |= test_quality();

    config_destroy(&cfg);
    return err;
}
