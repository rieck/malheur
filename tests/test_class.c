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
#include "mconfig.h"
#include "farray.h"
#include "ftable.h"
#include "cluster.h"
#include "fmath.h"

/* Global variables */
config_t cfg;
int verbose = 0;

/* Test structure */
typedef struct {
    char *str;                  /* Test report */
    char *label;                /* Test label */
} test_t;

/* Number of stress runs */
#define STRESS_RUNS             20
/* String length */
#define STR_LENGTH              500
/* Number of vector */
#define NUM_VECTORS             500

/* Training set for classification */
static test_t train_data[] = {
    {"A B B B X", "1"}, {"A A B B X", "1"}, {"B B A B X", "1"},
    {"X Y B B Z", "2"}, {"A B Z X Y", "2"}, {"A X Y B Z", "2"},
    {NULL, NULL}
};

/* Test set for classification */
static test_t test_data[] = {
    {"A A B B X", "1"}, {"Z A B B X", "1"}, {"A B B X A", "1"},
    {"A A B B X", "1"}, {"X Y B Z Z", "2"}, {"B Z X Y X", "2"},
    {"A X Y B Z", "2"}, {"A X B B Z", "2"}, {"A Z X Y A", "2"},
    {NULL, NULL},
};

/**
 * Simple test cases classification
 */
int test_classify()
{
    int i, k, err = 0;
    fvec_t *f;

    test_printf("Nearest-neighbor classification");

    /* Prepare training data */
    farray_t *fa1 = farray_create("train");
    for (i = 0; train_data[i].str; i++) {
        f = fvec_extract(train_data[i].str, strlen(train_data[i].str), NULL);
        farray_add(fa1, f, train_data[i].label);
    }

    /* Prepare testing data */
    farray_t *fa2 = farray_create("train");
    for (i = 0; test_data[i].str; i++) {
        f = fvec_extract(test_data[i].str, strlen(test_data[i].str), NULL);
        farray_add(fa2, f, test_data[i].label);
    }

    /* Classification of test data */
    config_set_float(&cfg, "classify.max_dist", 1.41);
    assign_t *a = class_assign(fa2, fa1);
    
    /* Check predicted labels */
    for (k = 0; test_data[k].str; k++) {
        char *l = farray_get_label(fa1, a->proto[k]);
        err += strcmp(l, test_data[k].label) != 0;
    }

    /* Clean up */
    assign_destroy(a);
    farray_destroy(fa1); 
    farray_destroy(fa2);

    test_return(err, i);
    return err;
}

/* 
 * A simple stress test for classification
 */
int test_stress()
{
    int i, j, k, err = 0;
    fvec_t *f;
    farray_t *fa;
    char buf[STR_LENGTH + 1], label[32];

    test_printf("Stress test for classification");

    for (i = 0; i < STRESS_RUNS; i++) {
        /* Create array */
        fa = farray_create("test");

        for (j = 0; j < NUM_VECTORS; j++) {
            for (k = 0; k < STR_LENGTH; k++)
                buf[k] = rand() % 10 + '0';
            buf[k] = 0;

            /* Extract features */
            f = fvec_extract(buf, strlen(buf), "test");
            snprintf(label, 32, "label%.2d", rand() % 10);

            /* Add to array */
            farray_add(fa, f, label);
        }

        assign_t *a = class_assign(fa, fa);
        assign_destroy(a);
        farray_destroy(fa);
    }

    test_return(err, STRESS_RUNS);
    return err;
}


/**
 * Main function
 */
int main(int argc, char **argv)
{
    int err = FALSE;

    /* Create config */
    config_init(&cfg);
    config_check(&cfg);

    ftable_init();
    err |= test_classify();
    err |= test_stress();
    ftable_destroy();

    config_destroy(&cfg);
    return err;
}
