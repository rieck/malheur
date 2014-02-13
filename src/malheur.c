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

/* Global variables */
static char *output_file = OUTPUT_FILE;
static char malheur_dir[MAX_PATH_LEN];
static char **input_files = NULL;
static int input_len = 0;
static int reset = FALSE;
static int save = TRUE;
static char *fvec_dump = NULL;
static malheur_action_t action = PROTOTYPE;
static malheur_cfg_t mcfg;
static malheur_state_t mstate;

/* Option string */
#define OPTSTRING       "nro:m:hvVd:"

/**
 * Array of options of getopt_long()
 */
static struct option longopts[] = {
   { "maldir",          1, NULL, 'm' },
   { "outfile",         1, NULL, 'o' },
   { "reset",           0, NULL, 'r' },
   { "nostate",         0, NULL, 'n' },
   { "verbose",         0, NULL, 'v' }, 
   { "version",         0, NULL, 'V' },
   { "help",            0, NULL, 'h' },
   { "dump",            1, NULL, 'd' },
   /* start of config options */   
   { "input.format",           1, NULL, 1001 },
   { "input.mist_level",       1, NULL, 1002 },   
   { "input.mist_rlen",        1, NULL, 1003 },   
   { "input.mist_tlen",        1, NULL, 1004 },   
   { "features.ngram_delim",   1, NULL, 1005 },   
   { "features.ngram_len",     1, NULL, 1006 },
   { "features.vect_embed",    1, NULL, 1007 },
   { "features.lookup_table",  1, NULL, 1008 },
   { "prototypes.max_dist",    1, NULL, 1009 },   
   { "prototypes.max_num",     1, NULL, 1010 },   
   { "classify.max_dist",      1, NULL, 1011 },   
   { "cluster.link_mode",      1, NULL, 1012 },   
   { "cluster.min_dist",       1, NULL, 1013 },   
   { "cluster.reject_num",     1, NULL, 1014 },
   { "cluster.shared_ngrams",  1, NULL, 1015 },
   /* end of config options */
   { NULL,              0, NULL, 0 }
};

/**
 * Print usage of command line tool
 * @param argc Number of arguments
 * @param argv Argument values
 */
static void print_usage(void)
{
    printf("Usage: malheur [options] <action> <dataset>\n"
        "Actions:\n"
        "  distance       Compute distance matrix for malware reports\n"
        "  prototype      Extract prototypes from malware reports\n"
        "  protodist      Compute distance matrix for prototypes\n"        
        "  cluster        Cluster malware reports into similar groups\n"
        "  classify       Classify malware reports using labeled prototypes\n"
        "  increment      Incremental analysis of malware reports\n"
        "  info           Print information about internal state of Malheur\n"
        "Options:\n"
        "  -m <maldir>    Set malheur directory. [%s]\n"
        "  -o <outfile>   Set output file for analysis. [%s]\n"
        "  -d <dumpfile>  Dump feature vectors to file in libsvm format.\n"
        "  -r             Reset internal state of Malheur.\n"
        "  -n             Don't save internal state of Malher.\n"
        "  -v             Increase verbosity.\n"
        "  -V             Print version and copyright.\n"
        "  -h             Print this help screen.\n"
        "See manual page malheur(1) for more information.\n",
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
    
    /* reset getopt */
    optind = 0;
    
    while ((ch = getopt_long(argc, argv, OPTSTRING, longopts, NULL)) != -1) {
        switch (ch) {
        case 'n': 
            save = FALSE;
            break;
        case 'r':
            reset = TRUE;
            break;
        case 'v':
        case 'm':
            /* Empty. See load_config() */
            break;
        case 'o':
            output_file = optarg;
            break;
        case 'V':
            malheur_version(stdout);
            exit(EXIT_SUCCESS);
            break;
        case 'd':
            fvec_dump = optarg;
            break;
        case 'h':
        case '?':
            print_usage();
            exit(EXIT_SUCCESS);
            break;

        /* long options */
        case 1001:
            config_set_string(&cfg, "input.format", optarg);    
            break;
        case 1002:
            config_set_int(&cfg, "input.mist_level", atoi(optarg));    
            break;
        case 1003:
            config_set_int(&cfg, "input.mist_rlen", atoi(optarg));    
            break;
        case 1004:
            config_set_int(&cfg, "input.mist_tlen", atoi(optarg));    
            break;
        case 1005:
            config_set_string(&cfg, "features.ngram_delim", optarg);    
            break;
        case 1006:
            config_set_int(&cfg, "features.ngram_len", atoi(optarg));    
            break;
        case 1007:
            config_set_string(&cfg, "features.vect_embed", optarg);    
            break;
        case 1008:
            config_set_int(&cfg, "features.lookup_table", atoi(optarg));    
            break;
        case 1009:
            config_set_float(&cfg, "prototypes.max_dist", atof(optarg));    
            break;
        case 1010:
            config_set_int(&cfg, "prototypes.max_num", atoi(optarg));    
            break;
        case 1011:
            config_set_float(&cfg, "classify.max_dist", atof(optarg));    
            break;
        case 1012:
            config_set_string(&cfg, "cluster.link_mode", optarg);    
            break;
        case 1013:
            config_set_float(&cfg, "cluster.min_dist", atof(optarg));    
            break;
        case 1014:
            config_set_int(&cfg, "cluster.reject_num", atoi(optarg));    
            break;
        case 1015:
            config_set_int(&cfg, "cluster.shared_ngrams", atoi(optarg));    
            break;
        }
    }

    /* Check configuration */
    config_check(&cfg);

    argc -= optind;
    argv += optind;

    if (argc < 1)
        fatal("the <action> argument is required");

    /* Argument: action */
    if (!strcasecmp(argv[0], "prototype")) {
        action = PROTOTYPE;
    } else if (!strcasecmp(argv[0], "distance")) {
        action = DISTANCE;
    } else if (!strcasecmp(argv[0], "cluster")) {
        action = CLUSTER;
    } else if (!strcasecmp(argv[0], "classify")) {
        action = CLASSIFY;
    } else if (!strcasecmp(argv[0], "increment")) {
        action = INCREMENT;
    } else if (!strcasecmp(argv[0], "protodist")) {
        action = PROTODIST;
    } else if (!strcasecmp(argv[0], "info")) {
        action = INFO;
    } else {
        fatal("Unknown analysis action '%s'", argv[0]);
    }
    
    if (argc < 2 && action != PROTODIST && action != INFO) 
        fatal("the <dataset> argument is required");

    /* Assign input files */
    input_files = argv + 1;
    input_len = argc - 1;
}

/**
 * Load configuration
 * @param argc Number of arguments
 * @param argv Argument values
 */
static void load_config(int argc, char **argv)
{
    char cfg_path[MAX_PATH_LEN];
    int ch;

    /* Prepare dir */
    snprintf(malheur_dir, MAX_PATH_LEN, "%s", MALHEUR_DIR);

    /* Check for config file in command line */
    while ((ch = getopt_long(argc, argv, OPTSTRING, longopts, NULL)) != -1) {
        switch (ch) {
            case 'm':
                strncpy(malheur_dir, optarg, MAX_PATH_LEN);
                malheur_dir[MAX_PATH_LEN - 1] = 0;
                break;
            case 'v':
                verbose++;
                break;
            case '?':
            default:
                /* empty */
                break;
        }
    }

    /* Prepare malheur files */
    snprintf(cfg_path, MAX_PATH_LEN, "%s/%s", malheur_dir, CONFIG_FILE);

    /* Check for directories and files */
    if (access(malheur_dir, F_OK))
        fatal("Malheur directory '%s'", malheur_dir);

    /* Copy configuration file */
    if (access(cfg_path, R_OK)) {
        if (verbose > 0)
            printf("Copying configuration to '%s'.\n", cfg_path);
        copy_file(GLOBAL_CONFIG_FILE, cfg_path);
    }

    /* Init and load configuration */
    config_init(&cfg);
    if (config_read_file(&cfg, cfg_path) != CONFIG_TRUE)
        fatal("Could not read configuration (%s in line %d)",
              config_error_text(&cfg), config_error_line(&cfg));

    /* Check configuration */
    config_check(&cfg);
    if (verbose > 1)
        config_print(&cfg);
}


/**
 * Initialize malheur tool
 * @param argc Number of arguments
 * @param argv Argument values
 */
static void malheur_init(int argc, char **argv)
{
    int lookup;
    double shared;

    /* Load configuration */
    load_config(argc, argv);

    /* Parse options */
    parse_options(argc, argv);

    /* Prepare malheur files */
    snprintf(mcfg.reject_file, MAX_PATH_LEN, "%s/%s", malheur_dir, REJECT_FILE);
    snprintf(mcfg.config_file, MAX_PATH_LEN,"%s/%s", malheur_dir, CONFIG_FILE);
    snprintf(mcfg.proto_file, MAX_PATH_LEN, "%s/%s", malheur_dir, PROTO_FILE);
    snprintf(mcfg.state_file, MAX_PATH_LEN, "%s/%s", malheur_dir, STATE_FILE);

    /* Init feature lookup table */
    config_lookup_int(&cfg, "features.lookup_table", &lookup);
    config_lookup_float(&cfg, "cluster.shared_ngrams", &shared);
    if (lookup || shared > 0.0) {
        ftable_init();
    }
    
    /* Reset current state */
    if (reset) {
        unlink(mcfg.reject_file);
        unlink(mcfg.proto_file);
        unlink(mcfg.state_file);
    }

    memset(&mstate, 0, sizeof(mstate));
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
            warning("Could not access '%s'", input_files[i]);
            continue;
        }

        farray_t *f = farray_extract(input_files[i]);
        fa = farray_merge(fa, f);
    }
    
    if (!fa) 
        fatal("No data available.");
    
    /* Dump feature vectors to file */
    if (fvec_dump)
        farray_save_libsvm_file(fa, fvec_dump);

    return fa;
}

/**
 * Saves the internal Malheur state. The state is used during incremental
 * analysis to distinguig clusters obtained during different runs
 * @param run Current run of analysis
 * @param proto Number of prototypes
 * @param rej Number of rejected reports
 */
static void malheur_save_state()
{
    FILE *f;

    if (verbose > 0)
        printf("Saving internal state to '%s'.\n", mcfg.state_file);
    
    f = fopen(mcfg.state_file, "w");
    if (!f) {
        error("Could not open state file '%s'.", mcfg.state_file);
        return;
    }
        
    fprintf(f, "run = %u\nprototypes = %u\nrejected = %u\n", 
            mstate.run, mstate.num_proto, mstate.num_reject);
    
    fclose(f);
}

/**
 * Loads the internal Malheur state. The state is used during incremental
 * analysis to distinguig clusters obtained during different runs
 */
static int malheur_load_state()
{
    FILE *f;
    int ret;
    
    if (access(mcfg.state_file, R_OK))
        return FALSE;
    
    f = fopen(mcfg.state_file, "r");
    if (!f) {
        error("Could not open state file '%s'.", mcfg.state_file);
        return FALSE;
    }
        
    ret = fscanf(f, "run = %u\nprototypes = %u\nrejected = %u\n", 
                &mstate.run, &mstate.num_proto, &mstate.num_reject);
    
    if (ret != 3) {
        error("Could not parse state file '%s'.", mcfg.state_file);
        return FALSE;
    }
    
    fclose(f);
    return TRUE;
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
    if (save)
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
    fa = malheur_load();

    /* Extract prototypes */
    pr = proto_extract(fa, &as);

    /* Cluster prototypes and extrapolate */
    cluster_t *c = cluster_linkage(pr, 0);
    cluster_extrapolate(c, as);
    cluster_trim(c);

    /* Save prototypes */
    pn = cluster_get_prototypes(c, as, pr);
    if (save)
        farray_save_file(pn, mcfg.proto_file);
    farray_destroy(pn);

    /* Save rejected feature vectors */
    re = cluster_get_rejected(c, fa);
    if (save)
        farray_save_file(re, mcfg.reject_file);
    farray_destroy(re);

    /* Export clustering */
    export_cluster(c, pr, fa, as, output_file);
    
    /* Export shared n-grams */
    export_shared_ngrams(c, fa, output_file);

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
    assign_t *as;
    farray_t *fa, *pr, *re;

    /* Check for prototype file */
    if (access(mcfg.proto_file, R_OK))
        fatal("No prototype file for classifcation available");

    /* Load data */
    fa = malheur_load();

    /* Load prototypes */
    pr = farray_load_file(mcfg.proto_file);

    /* Apply classification */
    as = class_assign(fa, pr);

    /* Save rejected feature vectors */
    re = class_get_rejected(as, fa);
    if (save)
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
    assign_t *as; 

    /* Load internal state */
    malheur_load_state();

    /* Load data including rejected stuff */
    farray_t *fa = malheur_load();
    if (!access(mcfg.reject_file, F_OK)) {
        tmp = farray_load_file(mcfg.reject_file);
        fa = farray_merge(fa, tmp);
    }

    /* Classification */
    if (!access(mcfg.proto_file, R_OK)) {
        pr = farray_load_file(mcfg.proto_file);

        /* Apply classification */
        as = class_assign(fa, pr);
        tmp = class_get_rejected(as, fa);
        
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
    pr = proto_extract(fa, &as);
    
    /* Cluster prototypes and extrapolate */
    cluster_t *c = cluster_linkage(pr, mstate.run + 1);
    cluster_extrapolate(c, as);
    cluster_trim(c);

    /* Save prototypes vectors */
    pn = cluster_get_prototypes(c, as, pr);
    if (save)
        farray_append_file(pn, mcfg.proto_file);

    /* Save rejeted feature vectors */
    re = cluster_get_rejected(c, fa);
    if (save)
        farray_save_file(re, mcfg.reject_file);

    /* Update state */
    mstate.run++;
    mstate.num_proto = pn->len;
    mstate.num_reject = re->len;

    /* Save state */
    if (save) 
        malheur_save_state();
    
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
 * Display information about internal state of Malheur
 */
static void malheur_info()
{
    /* Load internal state */
    if (!malheur_load_state()) {
        printf("No internal state stored in %s\n", malheur_dir);
        return;
    }

    printf("Internal state from %s\n", mcfg.state_file);
    printf("       Malheur run: %u\n", mstate.run);
    printf(" Stored prototypes: %u\n", mstate.num_proto);
    printf("  Rejected reports: %u\n", mstate.num_reject);

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
 * Computes a distance matrix for the prototypes and saves the result to a file
 */
static void malheur_protodist()
{
    farray_t *pr;

    /* Check for prototype file */
    if (access(mcfg.proto_file, R_OK)) 
        fatal("No prototype file for classifcation available");
    
    /* Load prototypes */
    pr = farray_load_file(mcfg.proto_file);
    if (verbose > 1)
        farray_print(pr);
    
    /* Allocate distance matrix */
    double *d = malloc(pr->len * pr->len * sizeof(double));
    if (!d)
        fatal("Could not allocate similarity matrix");

    /* Compute distance matrix */
    farray_dist(pr, pr, d);

    /* Save distance matrix */
    export_dist(d, pr, output_file);

    /* Clean up */
    free(d);
    farray_destroy(pr);
}

/**
 * Exits the malheur tool.
 */
static void malheur_exit()
{
    int lookup;

    /* Destroy feature lookup table */
    config_lookup_int(&cfg, "features.lookup_table", &lookup);
    if (lookup) {
        if (verbose > 0)
            ftable_print();
        ftable_destroy();
    }

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

    /* Perform action */
    switch (action) {
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
    case PROTODIST:
        malheur_protodist();
        break;
    case INFO:
        malheur_info();
    }

    malheur_exit();
    return EXIT_SUCCESS;
}
