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
int verbose = 0;
config_t cfg;

/* Local variables */
static char *config_file = CONFIG_FILE;
static char *output_file = OUTPUT_FILE;
static char *input = NULL;
static malheur_task_t task = PROTOTYPE;

/**
 * Checks if the configuration is valid. The function currently checks
 * if all required parameters are set. Later versions might also set 
 * default values and only issue warning, if parameters are not available.
 */
void check_config()
{
    long l;
    const char *s;

    /* Check in input setting */    
    if (config_lookup_string(&cfg, "input.format", &s) != CONFIG_TRUE)
        fatal("'format' not defined in configuration 'input'");        
    if (config_lookup_int(&cfg, "input.mist_level", &l) != CONFIG_TRUE)
        fatal("'mist_level' not defined in configuration 'input'");        

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
    printf("Usage: malheur [options] <task> <input>\n"
           "Tasks:\n"
           "  examine         Examine reports and generate statistics\n"
           "  prototype       Extract prototypes from reports using labels\n"
           "  learn-classes   Learn a classification of reports using labels\n"
           "  learn-clusters  Learn a clustering of reports without labels\n"
           "Options:\n"
           "  -c <file>       Set configuration file.\n"
           "  -o <file>       Set output file.\n"           
           "  -v              Increase verbosity.\n"
           "  -V              Print version and copyright.\n"
           "  -h              Print this help screen.\n");
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
            config_file = optarg;
            break;
        case 'o':
            output_file = optarg;
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
        fatal("<task> and <input> arguments are required");

    /* Argument: Task */
    if (!strcasecmp(argv[0], "examine"))
        task = EXAMINE;
    else if (!strcasecmp(argv[0], "prototype"))
        task = PROTOTYPE;
    else if (!strcasecmp(argv[0], "learn-classes"))
        task = LEARN_CLASSES;
    else if (!strcasecmp(argv[0], "learn-clusters"))
        task = LEARN_CLUSTERS;
    else
        fatal("Unknown analysis task '%s' for Malheur", argv[0]);

    /* Argument: Input */
    input = argv[1];
    if (access(input, R_OK))
        fatal("Could not access '%s'.", input); 
}

/**
 * Initialize malheur tool
 * @param argc Number of arguments
 * @param argv Argument values
 */
static void malheur_init(int argc, char **argv)
{
    /* Parse options */
    parse_options(argc, argv);

    /* Init and load configuration */
    config_init(&cfg);
    if (config_read_file(&cfg, config_file) != CONFIG_TRUE)
        fatal("Could not read configuration (%s in line %d).",
              config_error_text(&cfg), config_error_line(&cfg));
    check_config();              
}

/**
 * Exits the malheur tool.
 */
static void malheur_exit()
{
    /* Destroy configuration */
    config_destroy(&cfg);
}
 
/**
 * Main function of Malheur
 * @param argc Number of arguments
 * @param argv Argument values
 * @return Exit code
 */
int main(int argc, char **argv)
{
    malheur_init(argc, argv);

    /* Perform task */
    switch (task) {
    case EXAMINE:
        break;
    case PROTOTYPE:
        break;
    case LEARN_CLASSES:
        break;
    case LEARN_CLUSTERS:
        break;
    }

    malheur_exit();
    return EXIT_SUCCESS;
}
