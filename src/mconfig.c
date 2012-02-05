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
 
 /** 
 * @defgroup mconfig Configuration functions
 * Functions for configuration of the Malheur tool. Additionally default
 * values for each configruation parameter are specified in this module. 
 * @author Konrad Rieck
 * @{
 */

#include "common.h"
#include "util.h"
#include "mconfig.h"

/* External variables */
extern int verbose;

/* Default configuration */
static config_default_t defaults[] = {
    /* Input */
    {"input", "format", { .sval = "text" }, CONFIG_TYPE_STRING},
    {"input", "mist_level", { .ival = 0 }, CONFIG_TYPE_INT},
    {"input", "mist_rlen", { .ival = 0 }, CONFIG_TYPE_INT},
    {"input", "mist_tlen", { .ival = 0 }, CONFIG_TYPE_INT},

    /* Features */
    {"features", "ngram_len", { .ival = 2 }, CONFIG_TYPE_INT},
    {"features", "ngram_delim", { .sval = "%20%0a%0d" }, CONFIG_TYPE_STRING},
    {"features", "vect_embed", { .sval = "bin" }, CONFIG_TYPE_STRING},
    {"features", "lookup_table", { .ival = 0 }, CONFIG_TYPE_INT},
    {"features", "hash_seed1", { .ival = 0x1ea4501a }, CONFIG_TYPE_INT},
    {"features", "hash_seed2", { .ival = 0x75f3da43 }, CONFIG_TYPE_INT},

    /* Prototypes */
    {"prototypes", "max_dist", { .fval = 0.65 }, CONFIG_TYPE_FLOAT},
    {"prototypes", "max_num", { .ival = 0 }, CONFIG_TYPE_INT},

    /* Classification */
    {"classify", "max_dist", { .fval = 0.68 }, CONFIG_TYPE_FLOAT},

    /* Clustering */
    {"cluster", "link_mode", { .sval = "complete" }, CONFIG_TYPE_STRING},    
    {"cluster", "min_dist", { .fval = 0.95 }, CONFIG_TYPE_FLOAT},
    {"cluster", "reject_num", { .ival = 10 }, CONFIG_TYPE_INT},
    {"cluster", "shared_ngrams", { .fval = 0.0 }, CONFIG_TYPE_FLOAT},

    /* Terminating entry */
    {NULL, NULL, { .ival = 0 }, 0}
};

/**
 * Print a configuration setting. 
 * @param f File stream to print to
 * @param cs Configuration setting
 * @param d Current depth.
 */
static void config_setting_fprint(FILE *f, config_setting_t * cs, int d)
{
    assert(cs && d >= 0);

    int i;
    for (i = 0; i < d; i++)
        fprintf(f, "  ");

    char *n = config_setting_name(cs);

    switch (config_setting_type(cs)) {
    case CONFIG_TYPE_GROUP:
        if (d > 0)
            fprintf(f, "%s = {\n", n);

        for (i = 0; i < config_setting_length(cs); i++)
            config_setting_fprint(f, config_setting_get_elem(cs, i), d + 1);

        if (d > 0) {
            for (i = 0; i < d; i++)
                fprintf(f, "  ");
            fprintf(f, "};\n");
        }
        break;
    case CONFIG_TYPE_STRING:
        fprintf(f, "%s\t= \"%s\";\n", n, config_setting_get_string(cs));
        break;
    case CONFIG_TYPE_FLOAT:
        fprintf(f, "%s\t= %7.5f;\n", n, config_setting_get_float(cs));
        break;
    case CONFIG_TYPE_INT:
        fprintf(f, "%s\t= %d;\n", n, config_setting_get_int(cs));
        break;
    default:
        error("Unsupported type for configuration setting '%s'", n);
    }
}

/**
 * Print the Malheur configuration.
 * @param cfg configuration
 */
void config_print(config_t *cfg)
{
    printf("Malheur configuration\n");
    config_setting_fprint(stdout, config_root_setting(cfg), 0);
}

/**
 * Print the Malheur configuration to a file. 
 * @param f pointer to file stream
 * @param cfg configuration
 */
void config_fprint(FILE *f, config_t *cfg)
{
    config_setting_fprint(f, config_root_setting(cfg), 0);
}

/**
 * Checks if the configuration is valid. The function checks if all 
 * required parameters are set and adds default values if necessary.
 * @param cfg configuration
 */
void config_check(config_t *cfg)
{
    int i, j;
    const char *s;
    double f;
    config_setting_t *cs, *vs;

    for (i = 0; defaults[i].name; i++) {
        /* Lookup setting group */
        cs = config_lookup(cfg, defaults[i].group);
        if (!cs)
            cs = config_setting_add(config_root_setting(cfg),
                                    defaults[i].group, CONFIG_TYPE_GROUP);

        /* (1) Check for string */
        if (defaults[i].type == CONFIG_TYPE_STRING) {
            if (config_setting_lookup_string(cs, defaults[i].name, &s))
                continue;

            /* Add default value */
            vs = config_setting_add(cs, defaults[i].name, CONFIG_TYPE_STRING);
            config_setting_set_string(vs, defaults[i].val.sval);
        /* (2) Check for float */            
        } else if (defaults[i].type == CONFIG_TYPE_FLOAT) {
            if (config_setting_lookup_float(cs, defaults[i].name, &f))
                continue;

            /* Check for mis-interpreted integer */
            if (config_setting_lookup_int(cs, defaults[i].name, &j)) {
                config_setting_remove(cs, defaults[i].name);
                vs = config_setting_add(cs, defaults[i].name, CONFIG_TYPE_FLOAT);
                config_setting_set_float(vs, (double) j);
                continue;
            }

            /* Add default value */
            vs = config_setting_add(cs, defaults[i].name, CONFIG_TYPE_FLOAT);
            config_setting_set_float(vs, defaults[i].val.fval);
        /* (3) Check for integer */            
        } else if (defaults[i].type == CONFIG_TYPE_INT) {
            if (config_setting_lookup_int(cs, defaults[i].name, &j))
                continue;

            /* Check for mis-interpreted float */
            if (config_setting_lookup_float(cs, defaults[i].name, &f)) {
                config_setting_remove(cs, defaults[i].name);
                vs = config_setting_add(cs, defaults[i].name, CONFIG_TYPE_INT);
                config_setting_set_int(vs, (long) round(f));
                continue;
            }

            /* Add default value */
            vs = config_setting_add(cs, defaults[i].name, CONFIG_TYPE_INT);
            config_setting_set_int(vs, defaults[i].val.ival);
        } else {
            error("Unknown configuration type");
        }
    }
}

/** @} */
