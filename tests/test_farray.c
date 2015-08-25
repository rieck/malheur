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
 */

#include "tests.h"
#include "mconfig.h"
#include "farray.h"
#include "fmath.h"

/* Global variables */
int verbose = 0;
config_t cfg;

/* Test file */
#define TEST_FILE               "test.fa"
/* Number of stress runs */
#define STRESS_RUNS             10
/* String length */
#define STR_LENGTH              2000
/* Number of vector */
#define NUM_VECTORS             200

/*
 * A simple stress test for feature arrays
 */
int test_stress()
{
    int i, j, k, err = 0;
    fvec_t *f;
    farray_t *fa;
    char buf[STR_LENGTH + 1], label[32];

    test_printf("Stress test for feature arrays");

    for (i = 0; i < STRESS_RUNS; i++) {
        /* Create array */
        fa = farray_create("test");

        for (j = 0; j < NUM_VECTORS; j++) {
            for (k = 0; k < STR_LENGTH; k++)
                buf[k] = rand() % 10 + '0';
            buf[k] = 0;

            /* Extract features */
            f = fvec_extract(buf, strlen(buf), "test");

            /* Get label */
            snprintf(label, 32, "label%.2d", rand() % 10);

            /* Add to array */
            farray_add(fa, f, label);
        }

        /* Destroy features */
        farray_destroy(fa);
    }

    test_return(err, STRESS_RUNS);
    return err;
}

/*
 * A simple stress test for feature arrays using OpenMP
 */
int test_stress_omp()
{
    int i, j, k, err = 0;
    char buf[STR_LENGTH + 1], label[32];

    test_printf("Stress test for feature arrays (OpenMP)");

    for (i = 0; i < STRESS_RUNS; i++) {
        /* Create array */
        farray_t *fa = farray_create("test");

#pragma omp parallel for
        for (j = 0; j < NUM_VECTORS; j++) {
            for (k = 0; k < STR_LENGTH; k++)
                buf[k] = rand() % 10 + '0';
            buf[k] = 0;

            /* Extract features */
            fvec_t *f = fvec_extract(buf, strlen(buf), "test");

#pragma omp critical
            {
                /* Get label */
                snprintf(label, 32, "label%.2d", rand() % 10);
                /* Add to array */
                farray_add(fa, f, label);
            }
        }

        /* Destroy features */
        farray_destroy(fa);
    }

    test_return(err, STRESS_RUNS);
    return err;
}

/*
 * A simple stress test for feature arrays
 */
int test_load_save()
{
    int i, j, k, err = 0;
    char buf[STR_LENGTH + 1], label[32];
    gzFile *z;

    test_printf("Loading and saving of feature arrays");

    /* Create array */
    farray_t *fa = farray_create("test");
    for (j = 0; j < NUM_VECTORS; j++) {
        for (k = 0; k < STR_LENGTH; k++)
            buf[k] = rand() % 10 + '0';
        buf[k] = 0;

        /* Extract features and add to array */
        fvec_t *f = fvec_extract(buf, strlen(buf), "test");
        snprintf(label, 32, "label%.2d", rand() % 10);
        farray_add(fa, f, label);
    }

    /* Create and save feature vectors */
    if (!(z = gzopen(TEST_FILE, "wb9"))) {
        printf("Could not create file (ignoring)\n");
        return FALSE;
    }
    farray_save(fa, z);
    gzclose(z);

    /* Load saved array */
    z = gzopen(TEST_FILE, "r");
    farray_t *fb = farray_load(z);
    gzclose(z);
    unlink(TEST_FILE);

    /* Compare each vector mathematically */
    for (i = 0; i < fa->len; i++) {
        fvec_t *c = fvec_sub(fa->x[i], fb->x[i]);
        err += fvec_norm1(c) > 10e-9;
        fvec_destroy(c);
    }

    err += fa->len != fb->len;
    err += fa->mem != fb->mem;

    /* Destroy features */
    farray_destroy(fa);
    farray_destroy(fb);

    test_return(err, NUM_VECTORS + 2);
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

    err |= test_stress();
    err |= test_stress_omp();
    err |= test_load_save();

    config_destroy(&cfg);
    return err;
}
