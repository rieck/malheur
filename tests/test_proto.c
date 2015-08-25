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
#include "ftable.h"
#include "proto.h"
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
#define DATA_LEN    		15
/* Number of correct prototypes */
#define DATA_PROTO  		5

/* Data set for prototype extraction */
static char *test_data[] = {
    "XX YY XX YY XX YY", "XX YY XX ZZ XX YY", "XX ZZ XX YY XX YY",
    "AA BB AA BB AA BB", "AA BB AA CC AA BB", "AA CC AA BB AA BB",
    "MM NN MM NN MM NN", "MM NN MM OO MM NN", "MM OO MM NN MM NN",
    "UU VV UU VV UU VV", "UU VV UU WW UU VV", "UU WW UU VV UU VV",
    "RR SS RR SS RR SS", "RR SS RR TT RR SS", "RR TT RR SS RR SS"
};

/**
 * Test the extraction of prototypes
 */
int test_proto()
{
    int i, err = 0;
    double dist[DATA_LEN * DATA_PROTO];

    test_printf("Prototype extraction");

    /* Prepare test data */ ;
    farray_t *fa = farray_create("test");
    for (i = 0; i < DATA_LEN; i++) {
        fvec_t *f = fvec_extract(test_data[i], strlen(test_data[i]), NULL);
        farray_add(fa, f, "test");
    }

    /* Extract prototypes */
    assign_t *as;
    farray_t *pr = proto_extract(fa, &as);
    assign_destroy(as);

    /* Check number of prototypes */
    err += (pr->len != DATA_PROTO);

    /* Check position of prototypes */
    farray_dist(fa, pr, dist);
    for (i = 0; i < DATA_PROTO; i++)
        err += fabs(dist[i * DATA_LEN + i]) > 1e-3;

    /* Clean up */
    farray_destroy(pr);
    farray_destroy(fa);

    test_return(err, 1 + DATA_PROTO);
    return err;
}

/*
 * A simple stress test for prototype extraction
 */
int test_stress()
{
    int i, j, k, err = 0;
    fvec_t *f;
    farray_t *fa;
    char buf[STR_LENGTH + 1], label[32];

    test_printf("Stress test for prototype extraction");

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
        assign_t *as;
        farray_t *pr = proto_extract(fa, &as);
        assign_destroy(as);

        /* Destroy features */
        farray_destroy(fa);
        farray_destroy(pr);
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
    config_set_string(&cfg, "generic.event_delim", " ");
    config_set_string(&cfg, "features.vect_embed", "cnt");

    ftable_init();

    err |= test_proto();
    err |= test_stress();

    ftable_destroy();

    config_destroy(&cfg);
    return err;
}
