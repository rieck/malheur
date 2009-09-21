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
#include "mconfig.h"
#include "ftable.h"
#include "fmath.h"
#include "proto.h"
#include "util.h"
#include "export.h"

/* Global variables */
int verbose = 0;
config_t cfg;

/* Local variables */
static char *config_file = CONFIG_FILE;
static char *output_file = NULL;
static char *proto_file = NULL;
static malheur_task_t task = PROTOTYPE;
static int lookup_table = FALSE;
static int html_output = FALSE;

/**
 * Print usage of command line tool
 * @param argc Number of arguments
 * @param argv Argument values
 */
void print_usage(int argc, char **argv)
{
    printf("Usage: malheur [options] <task> <input> ...\n"
           "Tasks:\n"
           "  distance      Compute a distance matrix from malware reports\n"
           "  prototype     Extract prototypes from malware reports\n"
           "  cluster       Cluster malware reports into similar groups\n"
           "  classify      Classify malware reports using labeled prototypes\n"
           "Options:\n"
           "  -c <file>     Set configuration file.\n"
           "  -p <file>     Set prototype file.\n"  
           "  -o <file>     Set output file for analysis.\n"   
           "  -l            Enable feature lookup table.\n"
           "  -t            Enable HTML output for analysis.\n"
           "  -v            Increase verbosity.\n"
           "  -V            Print version and copyright.\n"
           "  -h            Print this help screen.\n");
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
    while ((ch = getopt(argc, argv, "o:p:ltc:hvV")) != -1) {
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
            case 'p':
                proto_file = optarg;
                break;
            case 'l':
                lookup_table = TRUE;
                break;
            case 't':
                html_output = TRUE;
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
    if (!strcasecmp(argv[0], "prototype"))
        task = PROTOTYPE;
    else if (!strcasecmp(argv[0], "distance"))
        task = DISTANCE;
    else if (!strcasecmp(argv[0], "cluster"))
        task = CLUSTER;
    else if (!strcasecmp(argv[0], "classify"))
        task = CLASSIFY;
    else
        fatal("Unknown analysis task '%s'", argv[0]);

    /* Check for output fle */
    if (!output_file)
        fatal("No output file specified. See '-o' option.");
}

/**
 * Determines prototypes for the given malware reports
 * @param argv command line argumwents
 * @param argc length of arguments 
 */
static void malheur_prototype(int argc, char **argv)
{
    /* Load data */
    farray_t *fa = NULL;
    for (int i = 0; i < argc; i++) {
        /* Argument: Input */
        if (access(argv[i], R_OK)) {
            warning("Could not access '%s'. Skipping", argv[i]);         
            continue;
        }
        
        farray_t *f = farray_extract(argv[i]);
        fa = farray_merge(fa, f);
    } 
    
    /* Extract prototypes */
    farray_t *pr = proto_extract(fa);
    
    if (verbose > 1)
        farray_print(pr);
    
    /* Export prototypes */
    if (html_output)
        export_proto_html(pr, fa, output_file);
    else 
        export_proto_text(pr, fa, output_file);
    
    /* Save prototype vectors */
    if (proto_file) 
        proto_save_file(pr, proto_file);
    
    /* Clean up */
    farray_destroy(pr);
    farray_destroy(fa);
}

/**
 * Clusters the given malware reports
 * @param argv command line argumwents
 * @param argc length of arguments 
 */
static void malheur_cluster(int argc, char **argv)
{
    /* Load data */
    farray_t *fa = NULL;
    for (int i = 0; i < argc; i++) {
        /* Argument: Input */
        if (access(argv[i], R_OK)) {
            warning("Could not access '%s'. Skipping", argv[i]);         
            continue;
        }
        
        farray_t *f = farray_extract(argv[i]);
        fa = farray_merge(fa, f);
    } 

    /* Extract prototypes */
    farray_t *pr = proto_extract(fa);    

    /* TODO */

    /* Save prototype vectors */
    if (proto_file) 
        proto_save_file(pr, proto_file);
    
    /* Clean up */
    farray_destroy(pr);    
    farray_destroy(fa);        
}

/**
 * Classify the given malware reports
 * @param argv command line argumwents
 * @param argc length of arguments 

 */
static void malheur_classify(int argc, char **argv)
{
    if (!proto_file)
        error("No prototype file specified.");

    /* Load data */
    farray_t *pr = proto_load_file(proto_file);

    /* Load data */
    farray_t *fa = NULL;
    for (int i = 0; i < argc; i++) {
        /* Argument: Input */
        if (access(argv[i], R_OK)) {
            warning("Could not access '%s'. Skipping", argv[i]);         
            continue;
        }
        
        farray_t *f = farray_extract(argv[i]);
        fa = farray_merge(fa, f);
    } 

    /* TODO */
    
    /* Clean up */
    farray_destroy(pr);
    farray_destroy(fa);        
}


/**
 * Computes a distance matrix and saves the result to a file
 * @param argv command line argumwents
 * @param argc length of arguments 
 */
static void malheur_distance(int argc, char **argv)
{
    /* Load data */
    farray_t *fa = NULL;
    for (int i = 0; i < argc; i++) {
        /* Argument: Input */
        if (access(argv[i], R_OK)) {
            warning("Could not access '%s'. Skipping", argv[i]);         
            continue;
        }
        
        farray_t *f = farray_extract(argv[i]);
        fa = farray_merge(fa, f);
    } 

    /* Allocate distance matrix */
    double *d = malloc(fa->len * fa->len * sizeof(double));
    if (!d)
        fatal("Could not allocate similarity matrix");
        
    /* Compute distance matrix */
    farray_dist(fa, fa, d);
    
    /* Save distance matrix */
    if (html_output)
        export_distance_html(d, fa, output_file);
    else
        export_distance_text(d, fa, output_file);
    
    /* Clean up */
    free(d);
    farray_destroy(fa);
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
        fatal("Could not read configuration (%s in line %d)",
              config_error_text(&cfg), config_error_line(&cfg));
    
    /* Check configuration */
    config_check(&cfg);
    if (verbose > 1)
        config_print(&cfg);
    
    /* Init feature lookup table */
    if (lookup_table)
        ftable_init();
}

/**
 * Exits the malheur tool.
 */
static void malheur_exit()
{
    if (lookup_table)
        ftable_destroy();
    
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
        case DISTANCE:
            malheur_distance(argc, argv);
            break;
        case PROTOTYPE:
            malheur_prototype(argc, argv);
            break;
        case CLUSTER:
            malheur_cluster(argc, argv);
            break;
        case CLASSIFY:
            malheur_classify(argc, argv);
            break;
    }
    
    malheur_exit();
    return EXIT_SUCCESS;
}
