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

#include "tests.h"

/** Global time stamp */
static double start_time;

/**
 * Print a message for a starting test
 * @param fmt Format string.
 */
void test_printf(char *fmt, ...)
{
    va_list ap;
    char s[256] = { " " };
    int i;

    /* Assemble format string */
    va_start(ap, fmt);
    vsnprintf(s, 256, fmt, ap);
    va_end(ap);

    /* Print line */
    printf("%s ", s);
    for (i = strlen(s) + 1; i < LINE_WIDTH; i++)
        printf(".");
    printf(" ");

    /* Save start time */
    start_time = time_stamp();

    fflush(stdout);
}

/**
 * Print a message for a failed test
 * @param fmt Format string.
 */
void test_error(char *fmt, ...)
{
    va_list ap;
    char s[256] = { " " };
    int i;

    /* Assemble format string */
    va_start(ap, fmt);
    vsnprintf(s, 256, fmt, ap);
    va_end(ap);

    /* Print line */
    printf("\nError: %s", s);
    for (i = strlen(s) + 6; i < LINE_WIDTH; i++)
        printf(" ");

    fflush(stdout);
}


/**
 * Print return message for a test 
 * @param e number of failed tests
 * @param m number of all tests
 */
void test_return(int e, int m)
{
    double time = time_stamp() - start_time;
    printf("%s [%3.0f%%] %.2fs\n", e == 0 ? "  OK" : "FAIL",
           100.0 * (m - e) / m, time);
}
