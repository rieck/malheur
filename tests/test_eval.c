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
#include "eval.h"

/* Global variables */
int verbose = 0;
config_t cfg;

int test_fmeasure()
{
    int err = 0;

#if 0    
    int y[] = {1, 1, 1, 0, 0, 0};
    int c[] = {5, 5, 2, 2, 3, 3};


    hist_t *h = hist_create(y, c, 6);
    hist_print(h);
    hist_destroy(h);
#endif    
    
    return err;
}

/**
 * Main function
 */
int main(int argc, char **argv)
{
    int err = FALSE;
    
    err |= test_fmeasure(); 

    config_destroy(&cfg);
    return err;
} 


