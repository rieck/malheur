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

#include "config.h"
#include "common.h"
#include "ftable.h"

int verbose = 5;

/* Test structure */
typedef struct {
    feat_t  f;
    char*   s;
} test_t;

/* Test features */
test_t tests[] = {
    {    1, "this is a test" },
    {   -1, "another test case" },
    {    0, "again a test string" },    
    {  0x10, "still a test case" },
    { 0x100, "more testing" },
    { 0xFFF, "" },         
    {     0, 0 }
};

/* 
 * A simple static test for the feature table
 */
int test_static() 
{
    int i, j, k, err = FALSE;
    fentry_t *f;

    printf("TEST: Creation and maintenance of feature table.\n"); 

    /* Initialize table */
    ftable_init();
    for (i = 0; tests[i].s != 0; i++) 
        ftable_put(tests[i].f, tests[i].s, strlen(tests[i].s) + 1);

    /* Randomly query elements */    
    for (j = 0; j < 50; j++) {
        k = rand() % i;
        f = ftable_get(tests[k].f); 

        /* Check for correct feature string */  
        if (strcmp(f->data, tests[k].s)) {
            printf("FAIL: %s != %s\n", f->data, tests[k].s);
            ftable_print();
            err = TRUE;
        }    
    }    
    
    /* Destroy table */
    ftable_destroy();

    return err;
}

/* 
 * A simple stress test for the feature table
 */
int test_stress() 
{
    int i, j, err = FALSE;
    fentry_t *f;
    feat_t key;
    char buf[30];

    printf("TEST: Stress test of feature table.\n");

    /* Initialize table */
    ftable_init();
    
    for (i = 0; i < 2000; i++) {
        /* Create random key and string */
        key = rand() % 100;
        for (j = 0; j < 29; j++)
            buf[j] = rand() % 10 + '0';
        buf[j] = 0;    
        
        switch(rand() % 3) {
        case 0:
            /* Insert random string */
            ftable_put(key, buf, strlen(buf));
            break;
        case 1:
            /* Query for string */
            f = ftable_get(key);    
            break;
        case 2:
            /* Delete random string */
            ftable_remove(key);            
            break;
         } 
    }       
    return err;
}

/**
 * Main function
 */
int main(int argc, char **argv)
{
    int err = FALSE;
    
    err |= test_static(); 
    err |= test_stress();
    
    return err;
} 
