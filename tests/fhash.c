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
#include "fhash.h"

int verbose = 5;

/* Test structure */
typedef struct {
    feat_t  f;
    char*   s;
} test_t;

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
 * A simple static test for the feature hash
 */
int test_static() 
{
    int i, j, k, err = FALSE;
    fentry_t *f;

    /* Initialize hash */
    fhash_init();
    for (i = 0; tests[i].s != 0; i++) 
        fhash_put(tests[i].f, tests[i].s, strlen(tests[i].s) + 1);

    /* Print table */    
    fhash_print();    

    /* Randomly query elements */    
    for (j = 0; j < 50; j++) {
        k = rand() % i;
        f = fhash_get(tests[k].f); 
        err |= strcmp(f->data, tests[k].s);
    }    
    
    /* Destroy hash */
    fhash_destroy();

    return err;
}

/* 
 * A simple stress test for the feature hash
 */
int test_stress() 
{
    int i, j, err = FALSE;
    fentry_t *f;
    feat_t key;
    char buf[30];

    /* Initialize hash */
    fhash_init();
    
    for (i = 0; i < 1000; i++) {
        /* Create random key and string */
        key = rand() % 100;
        for (j = 0; j < 29; j++)
            buf[j] = rand() % 10 + '0';
        buf[j] = 0;    
        
        switch(rand() % 3) {
        case 0:
            /* Insert random string */
            fhash_put(key, buf, strlen(buf));
            break;
        case 1:
            /* Query for string */
            f = fhash_get(key);    
            break;
        case 2:
            /* Delete random string */
            fhash_remove(key);            
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
