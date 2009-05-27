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
#include "fvec.h"
#include "ftable.h"

/* Global variables */
int verbose = 5;
config_t cfg;

/* Test structure */
typedef struct {
    char *str;
    char *dlm;   
    int nlen;
    int len;
} test_t;

/* Test array of strings */
test_t tests[] = {
   { " a:a a:a a:a a:a ", " ",  1, 1 },   
   { " a:a a:b a:c a:d ", " ",  1, 4 },   
   { " a:a b:c a:a b:c ", " :", 1, 3 },
   { " a:a a:b a:c a:d ", " :", 1, 4 },      
   { " a:a a:a a:a a:a ", " ",  2, 1 },   
   { " a:a a:b a:c a:d ", " ",  2, 3 },      
   { " a:a a:a a:a a:a ", " :", 2, 1 },   
   { " a:a a:a a:a a:a ", "",   1, 3 },   
   { " a:a a:b a:c a:d ", "",   1, 6 },      
   { " a:a a:a a:a a:a ", "",   2, 4 },
   { NULL, NULL, 0 }
};

/* Test file */
#define TEST_FILE               "test.fv"
/* Number of stress runs */
#define STRESS_RUNS             1000
/* String length */
#define STR_LENGTH              1024

/* 
 * A simple static test for the feature vectors
 */
int test_static() 
{
    int i, err = 0;
    fvec_t *f;  

    test_printf("Extraction of feature vectors");

    config_set_string(&cfg, "features.normalization", "l1");

    for (i = 0; tests[i].str; i++) {
        fvec_reset_delim();
        config_set_string(&cfg, "features.ngram_delim", tests[i].dlm);    
        config_set_int(&cfg, "features.ngram_length", tests[i].nlen);    
   
        /* Extract features */
        f = fvec_extract(tests[i].str, strlen(tests[i].str));
        
        /* Check for correct number of dimensions */
        if (f->len != tests[i].len) { 
            test_error("(%d) len %d != %d", i, f->len, tests[i].len);
            err++;
        }
            
        fvec_destroy(f);
    }
    
    test_return(err, i);
    return err;
}

/* 
 * A simple stress test for the feature table
 */
int test_stress() 
{
    int i, j, err = 0;
    fvec_t *f;  
    char buf[STR_LENGTH + 1];

    test_printf("Stress test for feature vectors");

    config_set_string(&cfg, "features.normalization", "l1");
    config_set_string(&cfg, "features.ngram_delim", "0");    

    ftable_init();

    for (i = 0; i < STRESS_RUNS; i++) {
        config_set_int(&cfg, "features.ngram_length", rand() % 10 + 1);    
   
        /* Create random key and string */
        for (j = 0; j < STR_LENGTH; j++)
            buf[j] = rand() % 10 + '0';
        buf[j] = 0;    
           
        /* Extract features */
        f = fvec_extract(buf, strlen(buf));
        /* Destroy features */            
        fvec_destroy(f);
    }
    
    ftable_destroy();
    
    test_return(err, STRESS_RUNS);
    return err;
}

/* 
 * A simple stress test for the feature table
 */
int test_stress_omp() 
{
    int i, j, err = 0;
    fvec_t *f;  
    char buf[STR_LENGTH + 1];

    test_printf("Stress test for feature vectors (OpenMP)");

    config_set_string(&cfg, "features.normalization", "l1");
    config_set_string(&cfg, "features.ngram_delim", "0");    

    ftable_init();

    #pragma omp parallel for 
    for (i = 0; i < STRESS_RUNS; i++) {
        config_set_int(&cfg, "features.ngram_length", rand() % 10 + 1);    
   
        /* Create random key and string */
        for (j = 0; j < STR_LENGTH; j++)
            buf[j] = rand() % 10 + '0';
        buf[j] = 0;    
           
        /* Extract features */
        f = fvec_extract(buf, strlen(buf));
        /* Destroy features */            
        fvec_destroy(f);
    }
    
    ftable_destroy();
    
    test_return(err, STRESS_RUNS);
    return err;
}


/* 
 * A simple load and save test case
 */
int test_load_save() 
{
    int i, j, err = 0;
    fvec_t *f, *g; 
    gzFile *z;
    
    test_printf("Loading and saving of feature vectors");

    fvec_reset_delim();
    config_set_string(&cfg, "features.normalization", "l1");
    config_set_string(&cfg, "features.ngram_delim", " ");
    config_set_int(&cfg, "features.ngram_length", 2);

    /* Create and save feature vectors */
    if (!(z = gzopen(TEST_FILE, "wb9"))) {
        printf("Could not create file (ignoring)\n");
        return FALSE;
    }

    for (i = 0; tests[i].str; i++) {
        f = fvec_extract(tests[i].str, strlen(tests[i].str));
        fvec_save(f, z);
        fvec_destroy(f);
    }
    gzclose(z);


    /* Load and compare feature vectors */
    z = gzopen(TEST_FILE, "r");    

    for (i = 0; tests[i].str; i++) {
        f = fvec_extract(tests[i].str, strlen(tests[i].str));
        g = fvec_load(z);
        
        /* Check dimensions and values */
        for (j = 0; j < f->len && j < g->len; j++) {
            if (f->dim[j] != g->dim[j]) {
                test_error("(%d) f->dim[%d] != g->dim[%d]", i, j, j);
                break;
            }    
            if (fabs(f->val[j] - g->val[j]) > 10e-10) {
                test_error("(%d) f->val[%d] != g->val[%d]", i, j, j);
                break;
            }                
        }  
        err += (j < f->len || j < g->len);
        
        fvec_destroy(f);
        fvec_destroy(g);
    }

    gzclose(z);
    unlink(TEST_FILE);
    
    test_return(err, i);
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
    config_setting_t *s = config_setting_add(config_root_setting(&cfg), 
                                             "features", CONFIG_TYPE_GROUP);

    /* Add important variables */    
    config_setting_add(s, "normalization", CONFIG_TYPE_STRING);
    config_setting_add(s, "ngram_length", CONFIG_TYPE_INT);
    config_setting_add(s, "ngram_delim", CONFIG_TYPE_STRING);                           
    
    err |= test_static(); 
    err |= test_stress();    
    err |= test_stress_omp();
    err |= test_load_save();
    
    config_destroy(&cfg);
    return err;
} 
