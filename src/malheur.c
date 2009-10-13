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
#include "cluster.h"
#include "class.h"

/* Global variables */
int verbose = 0;
config_t cfg;

/* Files */
static char *config_file = CONFIG_FILE;
static char *output_file = OUTPUT_FILE;
static char *proto_file = PROTO_FILE;
static char *reject_file = REJECT_FILE;
static char *state_file = STATE_FILE;

/* Command line stuff */
static char **input_files = NULL;
static int input_len = 0;
static malheur_task_t task = PROTOTYPE;

/**
 * Print usage of command line tool
 * @param argc Number of arguments
 * @param argv Argument values
 */
static void print_usage(int argc, char **argv)
{
    printf("Usage: malheur [options] <task> <input> ...\n"
           "Tasks:\n"
           "  distance     Compute a distance matrix from malware reports\n"
           "  prototype    Extract prototypes from malware reports\n"
           "  cluster      Cluster malware reports into similar groups\n"
           "  classify     Classify malware reports using labeled prototypes\n"
           "  combined     Combined analysis of malware reports\n"
           "Options:\n"
           "  -c <file>    Set configuration file. [%s]\n"
           "  -p <file>    Set prototype file. [%s]\n"  
           "  -r <file>    Set rejected file. [%s]\n" 
           "  -s <file>    Set internal state file. [%s]\n" 
           "  -o <file>    Set output file for analysis. [%s]\n"
           "  -t           Reset internal state of Malheur.\n"            
           "  -v           Increase verbosity.\n"
           "  -V           Print version and copyright.\n"
           "  -h           Print this help screen.\n", 
           CONFIG_FILE, PROTO_FILE, REJECT_FILE, STATE_FILE, OUTPUT_FILE);
}

/**
 * Parse command line options
 * @param argc Number of arguments
 * @param argv Argument values
 */
static void parse_options(int argc, char **argv)
{
    int ch;
    while ((ch = getopt(argc, argv, "ts:o:p:r:c:hvV")) != -1) {
        switch (ch) {
            case 't':
                unlink(proto_file);
                unlink(reject_file);
                unlink(state_file);
                break;
            case 'v':
                verbose++;
                break;
            case 'c':
                config_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'r':
                reject_file = optarg;
                break;
            case 'p':
                proto_file = optarg;
                break;
            case 'V':
                malheur_version(stdout);
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
    
    if (argc < 2)
        fatal("<task> and <input> arguments are required");
    
    /* Argument: Task */
    if (!strcasecmp(argv[0], "prototype")) {
        task = PROTOTYPE;
    } else if (!strcasecmp(argv[0], "distance")) {
        task = DISTANCE;
    } else if (!strcasecmp(argv[0], "cluster")) {
        task = CLUSTER;
    } else if (!strcasecmp(argv[0], "classify")) {
        task = CLASSIFY;
        if (access(proto_file, R_OK)) 
            fatal("Prototype file '%s' not found.", proto_file);
    } else {
        fatal("Unknown analysis task '%s'", argv[0]);
    }

    /* Assign input files */
    input_files = argv + 1;
    input_len = argc - 1;

    /* Check for output fle */
    if (!output_file)
        fatal("No output file specified. See '-o' option.");
}


/**
 * Loads data from archives/directories into feature vectors 
 * @return array of feature vectors
 */
static farray_t *malheur_load()
{
    farray_t *fa = NULL;
    for (int i = 0; i < input_len; i++) {
        /* Argument: Input */
        if (access(input_files[i], R_OK)) {
            warning("Could not access '%s'. Skipping", input_files[i]);         
            continue;
        }
        
        farray_t *f = farray_extract(input_files[i]);
        fa = farray_merge(fa, f);
    } 
    return fa;
}

/**
 * Determines prototypes for the given malware reports
 */
static void malheur_prototype()
{
    /* Load data */
    farray_t *fa = malheur_load();
    
    /* Extract prototypes */
    farray_t *pr = proto_extract(fa);
    assign_t *as = proto_assign(fa, pr);
    
    if (verbose > 1)
        farray_print(pr);
    
    /* Export prototypes */
    export_proto(pr, fa, as, output_file);
    
    /* Save prototypes */
    farray_save_file(pr, proto_file);
    
    /* Clean up */
    assign_destroy(as);
    farray_destroy(pr);
    farray_destroy(fa);
}

/**
 * Clusters the given malware reports
 */
static void malheur_cluster()
{
    /* Load data */
    farray_t *fa = malheur_load();

    /* Extract prototypes */
    farray_t *pr = proto_extract(fa);    
    assign_t *as = proto_assign(fa, pr);

    /* Cluster prototypes and extrapolate */
    cluster_t *c = cluster_linkage(pr);
    cluster_extrapolate(c, as);
    cluster_trim(c);
    
    /* Save prototypes */
    farray_t *pn = cluster_get_prototypes(c, as, pr);
    farray_save_file(pn, proto_file);
    farray_destroy(pn);

    /* Save rejected feature vectors */
    farray_t *re = cluster_get_rejected(c, fa);
    farray_save_file(re, reject_file);
    farray_destroy(re);    
    
    /* Export clustering */
    export_cluster(c, pr, fa, as, output_file);    
    
    /* Clean up */
    cluster_destroy(c);
    assign_destroy(as);
    farray_destroy(pr);    
    farray_destroy(fa);        
}

/**
 * Classify the given malware reports
 */
static void malheur_classify()
{
    /* Load data */
    farray_t *fa = malheur_load();

    /* Load prototypes */
    farray_t *pr = farray_load_file(proto_file);
    assign_t *as = proto_assign(fa, pr);

    /* Apply classification */
    class_apply(as, fa);
    
    /* Save rejected feature vectors */
    farray_t *re = class_get_rejected(as, fa);
    farray_save_file(re, reject_file);
    farray_destroy(re);
    
    /* Export classification */
    export_class(pr, fa, as, output_file);
        
    /* Clean up */
    assign_destroy(as);
    farray_destroy(pr);
    farray_destroy(fa);
}


/**
 * Computes a distance matrix and saves the result to a file
 */
static void malheur_distance()
{
    /* Load data */
    farray_t *fa = malheur_load();

    /* Allocate distance matrix */
    double *d = malloc(fa->len * fa->len * sizeof(double));
    if (!d)
        fatal("Could not allocate similarity matrix");
        
    /* Compute distance matrix */
    farray_dist(fa, fa, d);
    
    /* Save distance matrix */
    export_dist(d, fa, output_file);
    
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
    long lookup;

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
    config_lookup_int(&cfg, "features.lookup_table", &lookup);
    if (lookup)
        ftable_init();
}

/**
 * Exits the malheur tool.
 */
static void malheur_exit()
{
    long lookup;

    /* Destroy feature lookup table */
    config_lookup_int(&cfg, "features.lookup_table", &lookup);
    if (lookup)
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
            malheur_distance();
            break;
        case PROTOTYPE:
            malheur_prototype();
            break;
        case CLUSTER:
            malheur_cluster();
            break;
        case CLASSIFY:
            malheur_classify();
            break;
    }
    
    malheur_exit();
    return EXIT_SUCCESS;
}
