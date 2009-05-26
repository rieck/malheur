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
#include "malheur.h"
#include "common.h"
#include "farray.h"
#include "util.h"

/* Global variables */
int verbose = 1;
config_t cfg;

/* Local variables */
static char *cfg_file = NULL;
static char *input = NULL;
static char *output = NULL;

/**
 * Checks if the configuration is valid. The function currently checks
 * if all required parameters are set. Later versions might also set 
 * default values and only issue warning, if parameters are not available.
 */
void check_config()
{
    long l;
    const char *s;

    /* Check in features setting */    
    if (config_lookup_int(&cfg, "features.ngram_length", &l) != CONFIG_TRUE)
        fatal("'ngram_length' not defined in configuration group 'features'");
    if (config_lookup_string(&cfg, "features.ngram_delim", &s) != CONFIG_TRUE)
        fatal("'ngram_delim' not defined in configuration group 'features'");
    if (config_lookup_string(&cfg, "features.normalization", &s) != CONFIG_TRUE)
        fatal("'normalization' not defined in configuration 'features'");
}

/**
 * Print usage of command line tool
 * @param argc Number of arguments
 * @param argv Argument values
 */
void print_usage(int argc, char **argv)
{
    printf("Usage: malheur [options] <input> <output>\n"
           "  -c <file>    Set configuration file.\n"
           "  -v           Increase verbosity.\n"
           "  -V           Print version and copyright.\n"
           "  -h           Print this help screen.\n");
}

/**
 * Print version and copyright information
 */
void print_version()
{
    printf(" MALHEUR - Automatic Malware Analysis on Steroids\n"
           " Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)\n"
           " Berlin Institute of Technology (TU Berlin).\n");
}

/**
 * Parse command line options
 * @param argc Number of arguments
 * @param argv Argument values
 */
void parse_options(int argc, char **argv)
{
    int ch;
    while ((ch = getopt(argc, argv, "c:hvV")) != -1) {
        switch (ch) {
        case 'v':
            verbose++;
            break;
        case 'c':
            cfg_file = optarg;
            break;
        case 'V':
            print_version();
            exit(EXIT_SUCCESS);
            break;
        case 'h':
        case '?':
            print_usage(argc, argv);
            exit(EXIT_SUCCESS);
            break;
        }
    }

    argc -= optind;
    argv += optind;

    if (argc != 2)
        fatal("Input and output arguments required");

    input = argv[0];
    output = argv[1];

    /* Sanity checks */
    if (!cfg_file)
        fatal("No configuration specified (Option: -c <file>).");
}

/**
 * Main function of Malheur
 * @param argc Number of arguments
 * @param argv Argument values
 * @return Exit code
 */
int main(int argc, char **argv)
{
    /* Parse options */
    parse_options(argc, argv);

    /* Init and load configuration */
    config_init(&cfg);
    if (config_read_file(&cfg, cfg_file) != CONFIG_TRUE)
        fatal("Could not read configuration (%s in line %d).",
              config_error_text(&cfg), config_error_line(&cfg));
    check_config();              

    farray_t *a = farray_create_dir(input);
    farray_destroy(a);

    /* Destroy configuration */
    config_destroy(&cfg);

    return EXIT_SUCCESS;
}
