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
#include "cluster.h"
#include "class.h"
#include "export.h"

/* Global variables */
int verbose = 0;
config_t cfg;
runtime_t runtime;

/* Global variables */
static char *output_file = OUTPUT_FILE;
static char malheur_dir[MAX_PATH_LEN];
static char **input_files = NULL;
static int input_len = 0;
static int reset = FALSE;
static malheur_task_t task = PROTOTYPE;
static malheur_cfg_t mcfg;

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
           "  increment    Incremental analysis of malware reports\n"
           "  excluster	   Cluster malware reports using exact clustering\n"
           "Options:\n"
           "  -m <file>    Set malheur directory. [%s]\n"
           "  -o <file>    Set output file for analysis. [%s]\n"
           "  -t           Reset internal state of Malheur.\n"
           "  -v           Increase verbosity.\n"
           "  -V           Print version and copyright.\n"
           "  -h           Print this help screen.\n",
           malheur_dir, output_file);
}

/**
 * Parse command line options
 * @param argc Number of arguments
 * @param argv Argument values
 */
static void parse_options(int argc, char **argv)
{
    int ch;
    while ((ch = getopt(argc, argv, "ts:o:m:hvV")) != -1) {
        switch (ch) {
        case 't':
            reset = TRUE;
            break;
        case 'v':
            verbose++;
            break;
        case 'm':
            strncpy(malheur_dir, optarg, MAX_PATH_LEN);
            malheur_dir[MAX_PATH_LEN - 1] = 0;
            break;
        case 'o':
            output_file = optarg;
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
    } else if (!strcasecmp(argv[0], "increment")) {
        task = INCREMENT;
    } else if (!strcasecmp(argv[0], "excluster")) {
        task = EXCLUSTER;
    } else {
        fatal("Unknown analysis task '%s'", argv[0]);
    }

    /* Assign input files */
    input_files = argv + 1;
    input_len = argc - 1;

    /* Check for output fle */
    if (task == CLASSIFY && access(mcfg.proto_file, R_OK))
        fatal("No prototype file for classifcation available");
}


/**
 * Initialize malheur tool
 * @param argc Number of arguments
 * @param argv Argument values
 */
static void malheur_init(int argc, char **argv)
{
    long lookup;

    /* Prepare dir */
    snprintf(malheur_dir, MAX_PATH_LEN, "%s/%s", getenv("HOME"), MALHEUR_DIR);

    /* Parse options */
    parse_options(argc, argv);

    /* Prepare malheur files */
    snprintf(mcfg.reject_file, MAX_PATH_LEN, "%s/%s", malheur_dir, REJECT_FILE);
    snprintf(mcfg.config_file, MAX_PATH_LEN,"%s/%s", malheur_dir, CONFIG_FILE);
    snprintf(mcfg.proto_file, MAX_PATH_LEN, "%s/%s", malheur_dir, PROTO_FILE);
    snprintf(mcfg.state_file, MAX_PATH_LEN, "%s/%s", malheur_dir, STATE_FILE);

    /* Check for directories and files */
    if (access(malheur_dir, F_OK))
        if(mkdir(malheur_dir, 0700)) 
            fatal("Could not create directory '%s'.", malheur_dir);
                    
    /* Copy configuration file */
    if (access(mcfg.config_file, R_OK)) {
        if (verbose > 0)
            printf("Copying configuration to '%s'.\n", mcfg.config_file);
        copy_file(GLOBAL_CONFIG_FILE, mcfg.config_file);
    }

    /* Init and load configuration */
    config_init(&cfg);
    if (config_read_file(&cfg, mcfg.config_file) != CONFIG_TRUE)
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
    
    /* Reset current state */
    if (reset) {
        unlink(mcfg.reject_file);
        unlink(mcfg.proto_file);
        unlink(mcfg.state_file);
    }
    
    /* Init runtimes to zero */
    memset(&runtime, 0, sizeof(runtime_t));
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
 * Saves the internal Malheur state. The state is used during incremental
 * analysis to distinguig clusters obtained during different runs
 * @param run Current run of analysis
 * @param proto Number of prototypes
 * @param rej Number of rejected reports
 */
static void malheur_save_state(int run, int proto, int rej)
{
    FILE *f;

    if (verbose > 0)
        printf("Saving internal state to '%s'.\n", mcfg.state_file);
    
    f = fopen(mcfg.state_file, "w");
    if (!f) {
        error("Could not open state file '%s'.", mcfg.state_file);
        return;
    }
        
    fprintf(f, "run = %d\nprototypes = %d\nrejected = %d\n", 
            run, proto, rej);
    
    fclose(f);
}

/**
 * Loads the internal Malheur state. The state is used during incremental
 * analysis to distinguig clusters obtained during different runs
 * @return previous number of run
 */
static int malheur_load_state()
{
    FILE *f;
    int ret, run, proto, rej;
    
    if (access(mcfg.state_file, R_OK))
        return 0;
    
    f = fopen(mcfg.state_file, "r");
    if (!f) {
        error("Could not open state file '%s'.", mcfg.state_file);
        return 0;
    }
        
    ret = fscanf(f, "run = %d\nprototypes = %d\nrejected = %d\n", 
                &run, &proto, &rej);
    
    if (ret != 3) {
        error("Could not parse state file '%s'.", mcfg.state_file);
        return 0;
    }
    
    fclose(f);
    return run;
}

/**
 * Determines prototypes for the given malware reports
 */
static void malheur_prototype()
{
    assign_t *as;
    farray_t *fa, *pr;
    
    /* Load data */
    fa = malheur_load();

    /* Extract prototypes */
    pr = proto_extract(fa, &as);
    if (verbose > 1)
        farray_print(pr);

    /* Save prototypes */
    farray_save_file(pr, mcfg.proto_file);

    /* Export prototypes */
    export_proto(pr, fa, as, output_file);

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
    assign_t *as;
    farray_t *fa, *pr, *pn, *re;

    /* Load data */
    rt_start(extract);
    fa = malheur_load();
    rt_stop(extract);

    /* Extract prototypes */
    rt_start(proto);
    pr = proto_extract(fa, &as);
    rt_stop(proto);

    /* Cluster prototypes and extrapolate */
    rt_start(cluster);
    cluster_t *c = cluster_linkage(pr, 0);
    cluster_extrapolate(c, as);
    cluster_trim(c);
    rt_stop(cluster);

    /* Save prototypes */
    pn = cluster_get_prototypes(c, as, pr);
    farray_save_file(pn, mcfg.proto_file);
    farray_destroy(pn);

    /* Save rejected feature vectors */
    re = cluster_get_rejected(c, fa);
    farray_save_file(re, mcfg.reject_file);
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
 * Clusters the given malware reports using exact clustering
 */
static void malheur_excluster()
{
    /* Load data */
    rt_start(extract);
    farray_t *fa = malheur_load();
    rt_stop(extract);

    /* Cluster prototypes and extrapolate */
    rt_start(cluster);
    cluster_t *c = cluster_linkage(fa, 0);
    cluster_trim(c);
    rt_stop(cluster);

    /* Clean up */
    cluster_destroy(c);
    farray_destroy(fa);
}

/**
 * Classify the given malware reports
 */
static void malheur_classify()
{
    assign_t *as;
    farray_t *fa, *pr, *re;

    /* Load data */
    fa = malheur_load();

    /* Load prototypes */
    pr = farray_load_file(mcfg.proto_file);

    /* Apply classification */
    as = classify_apply(fa, pr);

    /* Save rejected feature vectors */
    re = classify_get_rejected(as, fa);
    farray_save_file(re, mcfg.reject_file);
    farray_destroy(re);

    /* Export classification */
    export_class(pr, fa, as, output_file);

    /* Clean up */
    assign_destroy(as);
    farray_destroy(pr);
    farray_destroy(fa);
}

/**
 * Classify the given malware reports
 */
static void malheur_increment()
{
    farray_t *pr = NULL, *tmp, *pn, *re;
    assign_t *as = NULL; 

    /* Load internal state */
    rt_start(state);
    int run = malheur_load_state();
    rt_stop(state);

    /* Load data including rejected stuff */
    rt_start(extract);
    farray_t *fa = malheur_load();
    rt_stop(extract);
    
    rt_start(state);
    if (!access(mcfg.reject_file, F_OK)) {
        tmp = farray_load_file(mcfg.reject_file);
        fa = farray_merge(fa, tmp);
    }
    rt_stop(state);

    /* Classification */
    if (!access(mcfg.proto_file, R_OK)) {
        rt_start(state);
        pr = farray_load_file(mcfg.proto_file);
        rt_stop(state);
        
        rt_start(classify);
        as = classify_apply(fa, pr);
        tmp = classify_get_rejected(as, fa);
        rt_stop(classify);
        
        /* Export results */
        export_increment1(pr, fa, as, output_file);
        
        /* Clean up */
        farray_destroy(fa);
        farray_destroy(pr);
        assign_destroy(as);
        fa = tmp;        
    } else {
        /* Export results */
        export_increment1(pr, fa, as, output_file);
    }

    /* Extract prototypes */
    rt_start(proto);
    pr = proto_extract(fa, &as);
    rt_stop(proto);
    
    /* Cluster prototypes and extrapolate */
    rt_start(cluster);
    cluster_t *c = cluster_linkage(pr, run + 1);
    cluster_extrapolate(c, as);
    cluster_trim(c);
    rt_stop(cluster);

    /* Save prototypes and rejected feature vectors */
    rt_start(state);
    pn = cluster_get_prototypes(c, as, pr);
    farray_append_file(pn, mcfg.proto_file);
    re = cluster_get_rejected(c, fa);
    farray_save_file(re, mcfg.reject_file);
    malheur_save_state(run + 1, pn->len, re->len);
    rt_stop(state);

    /* Export results */
    export_increment2(c, pr, fa, as, output_file);

    /* Clean up */
    cluster_destroy(c);
    assign_destroy(as);

    farray_destroy(re);
    farray_destroy(pn);
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
    
    printf("%f %f %f %f %f %f %f %f\n", runtime.extract_time, runtime.proto_time, 
       runtime.cluster_time, runtime.classify_time, runtime.state_time, 
       runtime.distproto_time, runtime.distclust_time, runtime.distclass_time);
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
    case INCREMENT:
        malheur_increment();
        break;
    case EXCLUSTER:
        malheur_excluster();
        break;
    }

    malheur_exit();
    return EXIT_SUCCESS;
}
