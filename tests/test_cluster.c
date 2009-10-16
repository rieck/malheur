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

/* Number of stress runs */
#define STRESS_RUNS             20
/* String length */
#define STR_LENGTH              500
/* Number of vector */
#define NUM_VECTORS             500
/* Number of elements in test data */
#define DATA_LEN                15
/* Number of correct prototypes */
#define DATA_CLUSTER            5

/* Data set for clustering */
static char *test_data[] = {
    "XX YY XX YY XX 11", "XX YY XX ZZ XX 66", "XX ZZ XX YY XX YY",
    "AA BB AA BB AA 22", "AA BB AA CC AA 77", "AA CC AA BB AA BB",
    "MM NN MM NN MM 33", "MM NN MM OO MM 88", "MM OO MM NN MM NN",
    "UU VV UU VV UU 44", "UU VV UU WW UU 99", "UU WW UU VV UU VV",
    "RR SS RR SS RR 55", "RR SS RR TT RR 00", "RR TT RR SS RR SS"
};

/**
 * Test clustering
 */
int test_cluster_complete()
{
    int i, j, k, err = 0;

    test_printf("Linkage clustering (complete)");

    /* Prepare test data */ ;
    farray_t *fa = farray_create("test");
    for (i = 0; i < DATA_LEN; i++) {
        fvec_t *f = fvec_extract(test_data[i], strlen(test_data[i]), NULL);
        farray_add(fa, f, "test");
    }

    /* Get clustering */
    config_set_string(&cfg, "cluster.link_mode", "complete");
    cluster_t *c = cluster_linkage(fa, 0);

    /* Check number of clusters */
    err += (c->num != DATA_CLUSTER);

    /* Check match of clusters */
    for (k = 0; k < DATA_LEN; k += DATA_LEN / DATA_CLUSTER)
        for (j = 0; j < DATA_LEN / DATA_CLUSTER - 1; j++)
            err += c->cluster[k + j] != c->cluster[k + j + 1];

    /* Clean up */
    cluster_destroy(c);
    farray_destroy(fa);

    test_return(err, 1 + DATA_CLUSTER * (DATA_LEN / DATA_CLUSTER - 1));
    return err;
}

/**
 * Test clustering
 */
int test_cluster_single()
{
    int i, j, k, err = 0;

    test_printf("Linkage clustering (single)");

    /* Prepare test data */ ;
    farray_t *fa = farray_create("test");
    for (i = 0; i < DATA_LEN; i++) {
        fvec_t *f = fvec_extract(test_data[i], strlen(test_data[i]), NULL);
        farray_add(fa, f, "test");
    }

    /* Get clustering */
    config_set_string(&cfg, "cluster.link_mode", "single");
    cluster_t *c = cluster_linkage(fa, 0);

    /* Check number of clusters */
    err += (c->num != DATA_CLUSTER);

    /* Check position of prototypes */
    for (k = 0; k < DATA_LEN; k += DATA_LEN / DATA_CLUSTER)
        for (j = 0; j < DATA_LEN / DATA_CLUSTER - 1; j++)
            err += c->cluster[k + j] != c->cluster[k + j + 1];

    /* Clean up */
    cluster_destroy(c);
    farray_destroy(fa);

    test_return(err, 1 + DATA_CLUSTER * (DATA_LEN / DATA_CLUSTER - 1));
    return err;
}

/**
 * Test clustering
 */
int test_cluster_average()
{
    int i, j, k, err = 0;

    test_printf("Linkage clustering (average)");

    /* Prepare test data */ ;
    farray_t *fa = farray_create("test");
    for (i = 0; i < DATA_LEN; i++) {
        fvec_t *f = fvec_extract(test_data[i], strlen(test_data[i]), NULL);
        farray_add(fa, f, "test");
    }

    /* Get clustering */
    config_set_string(&cfg, "cluster.link_mode", "average");
    cluster_t *c = cluster_linkage(fa, 0);

    /* Check number of clusters */
    err += (c->num != DATA_CLUSTER);

    /* Check position of prototypes */
    for (k = 0; k < DATA_LEN; k += DATA_LEN / DATA_CLUSTER)
        for (j = 0; j < DATA_LEN / DATA_CLUSTER - 1; j++)
            err += c->cluster[k + j] != c->cluster[k + j + 1];

    /* Clean up */
    cluster_destroy(c);
    farray_destroy(fa);

    test_return(err, 1 + DATA_CLUSTER * (DATA_LEN / DATA_CLUSTER - 1));
    return err;
}

/* 
 * A simple stress test for clustering
 */
int test_stress()
{
    int i, j, k, err = 0;
    fvec_t *f;
    farray_t *fa;
    char buf[STR_LENGTH + 1], label[32];

    test_printf("Stress test for clustering");

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

        /* Extract prototypes */
        cluster_t *c = cluster_linkage(fa, 0);

        /* Destroy features */
        cluster_destroy(c);
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

    err |= test_cluster_complete();
    err |= test_cluster_average();
    err |= test_cluster_single();
    err |= test_stress();

    ftable_destroy();

    config_destroy(&cfg);
    return err;
}
