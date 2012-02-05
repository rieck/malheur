/*
 * MALHEUR - Automatic Analysis of Malware Behavior
 * Copyright (c) 2009-2012 Konrad Rieck (konrad@mlsec.org)
 * University of Goettingen, Berlin Institute of Technology
 * --
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.  This program is distributed without any
 * warranty. See the GNU General Public License for more details. 
 * --
 */

#include "config.h"
#include "common.h"
#include "ftable.h"
#include "tests.h"

/* Test file */
#define TEST_FILE               "test.ft"
/* Number of stress runs */
#define STRESS_RUNS             10000
/* String length */
#define STR_LENGTH              1024

/* Global variables */
int verbose = 5;

/* Test structure */
typedef struct {
    feat_t f;
    char *s;
} test_t;

/* Test features */
test_t tests[] = {
    {0, "a b c d e f"},
    {-1, "a b c d e"},
    {1, "a b c d"},
    {0x10, "a b"},
    {0x100, "a"},
    {0xFFF, ""},
    {0, 0}
};

/* 
 * A simple static test for the feature table
 */
int test_static()
{
    int i, j, k, err = 0;
    fentry_t *f;

    test_printf("Creation and maintenance of feature table");

    /* Initialize table */
    ftable_init();
    for (i = 0; tests[i].s != 0; i++)
        ftable_put(tests[i].f, tests[i].s, strlen(tests[i].s) + 1);

    /* Randomly query elements */
    for (j = 0; j < 100; j++) {
        k = rand() % i;
        f = ftable_get(tests[k].f);

        /* Check for correct feature string */
        if (memcmp(f->data, tests[k].s, f->len)) {
            test_error("(%d) '%s' != '%s'", k, f->data, tests[k].s);
            /* ftable_print(); */
            err++;
        }
    }

    /* Destroy table */
    ftable_destroy();

    test_return(err, 100);
    return err;
}

/* 
 * A simple stress test for the feature table
 */
int test_stress()
{
    int i, j, err = 0;
    feat_t key;
    char buf[STR_LENGTH + 1];

    test_printf("Stress test for feature table");

    /* Initialize table */
    ftable_init();

    for (i = 0; i < STRESS_RUNS; i++) {
        /* Create random key and string */
        key = rand() % 100;
        for (j = 0; j < STR_LENGTH; j++)
            buf[j] = rand() % 10 + '0';
        buf[j] = 0;

        switch (rand() % 2) {
        case 0:
            /* Insert random string */
            ftable_put(key, buf, strlen(buf));
            break;
        case 1:
            /* Query for string */
            ftable_get(key);
            break;
        }
    }

    /* Destroy table */
    ftable_destroy();

    test_return(err, STRESS_RUNS);
    return err;
}

/* 
 * A test for loading and saving the feature table
 */
int test_load_save()
{
    int i, j, err = 0;
    gzFile *z;
    fentry_t *f;

    test_printf("Loading and saving of feature table");

    /* Initialize table */
    ftable_init();
    for (i = 0; tests[i].s != 0; i++)
        ftable_put(tests[i].f, tests[i].s, strlen(tests[i].s) + 1);

    /* Create and save feature vectors */
    if (!(z = gzopen(TEST_FILE, "wb9"))) {
        printf("Could not create file (ignoring)\n");
        return FALSE;
    }
    ftable_save(z);
    gzclose(z);
    ftable_destroy();

    /* Init and load */
    ftable_init();
    z = gzopen(TEST_FILE, "r");
    ftable_load(z);
    gzclose(z);

    /* Check elements */
    for (j = 0; j < i; j++) {
        f = ftable_get(tests[j].f);

        /* Check for correct feature string */
        if (memcmp(f->data, tests[j].s, f->len)) {
            test_error("(%d) '%s' != '%s'", j, f->data, tests[j].s);
            err++;
        }
    }

    /* Destroy table */
    ftable_destroy();
    unlink(TEST_FILE);

    test_return(err, i);
    return (err > 0);
}


/**
 * Main function
 */
int main(int argc, char **argv)
{
    int err = FALSE;

    err |= test_static();
    err |= test_stress();
    err |= test_load_save();

    return err;
}
