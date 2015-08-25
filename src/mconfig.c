/*
 * MALHEUR - Automatic Analysis of Malware Behavior
 * Copyright (c) 2009-2015 Konrad Rieck (konrad@mlsec.org)
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

#include "config.h"
#include "common.h"
#include "util.h"
#include "mconfig.h"

/* Macros to make config lines shorter */
#define G "generic"
#define F "features"
#define P "prototypes"
#define C "cluster"
#define Y "classify"

/* Default configuration */
static config_default_t defaults[] = {
    /* Input */
    {G, "input_format", CONFIG_TYPE_STRING, {.str = "text"}},
    {G, "event_delim", CONFIG_TYPE_STRING, {.str = "%0a%0d"}},
    {G, "state_dir", CONFIG_TYPE_STRING, {.str = "malheur.state"}},
    {G, "output_file", CONFIG_TYPE_STRING, {.str = "malheur.out"}},

    /* Features */
    {F, "ngram_len", CONFIG_TYPE_INT, {.num = 2}},
    {F, "vect_embed", CONFIG_TYPE_STRING, {.str = "bin"}},
    {F, "mist_level", CONFIG_TYPE_INT, {.num = 0}},
    {F, "hash_seed1", CONFIG_TYPE_INT, {.num = 0x1ea4501a}},
    {F, "hash_seed2", CONFIG_TYPE_INT, {.num = 0x75f3da43}},

    /* Prototypes */
    {P, "max_dist", CONFIG_TYPE_FLOAT, {.flt = 0.65}},
    {P, "max_num", CONFIG_TYPE_INT, {.num = 0}},

    /* Classification */
    {Y, "max_dist", CONFIG_TYPE_FLOAT, {.flt = 0.68}},

    /* Clustering */
    {C, "link_mode", CONFIG_TYPE_STRING, {.str = "complete"}},
    {C, "min_dist", CONFIG_TYPE_FLOAT, {.flt = 0.95}},
    {C, "reject_num", CONFIG_TYPE_INT, {.num = 10}},
    {C, "shared_ngrams", CONFIG_TYPE_FLOAT, {.flt = 0.0}},

    /* Terminating entry */
    {NULL}
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
    for (i = 0; i < d - 1; i++)
        fprintf(f, "       ");

    char *n = config_setting_name(cs);

    switch (config_setting_type(cs)) {
    case CONFIG_TYPE_GROUP:
        if (d > 0)
            fprintf(f, "%s = {\n", n);

        for (i = 0; i < config_setting_length(cs); i++)
            config_setting_fprint(f, config_setting_get_elem(cs, i), d + 1);

        if (d > 0) {
            for (i = 0; i < d - 1; i++)
                fprintf(f, "       ");
            fprintf(f, "};\n\n");
        }
        break;
    case CONFIG_TYPE_STRING:
        fprintf(f, "%s\t= \"%s\";\n", n, config_setting_get_string(cs));
        break;
    case CONFIG_TYPE_FLOAT:
        fprintf(f, "%s\t= %7.5f;\n", n, config_setting_get_float(cs));
        break;
    case CONFIG_TYPE_INT:
        fprintf(f, "%s\t= %ld;\n", n, (long) config_setting_get_int(cs));
        break;
    case CONFIG_TYPE_BOOL:
        fprintf(f, "%s\t= %s;\n", n, config_setting_get_bool(cs)
                ? "true" : "false");
        break;
    default:
        error("Unsupported type for configuration setting '%s'", n);
        break;
    }
}

/**
 * Print the configuration.
 * @param cfg configuration
 */
void config_print(config_t *cfg)
{
    config_setting_fprint(stdout, config_root_setting(cfg), 0);
}

/**
 * Print the configuration to a file.
 * @param f pointer to file stream
 * @param cfg configuration
 */
void config_fprint(FILE *f, config_t *cfg)
{
    config_setting_fprint(f, config_root_setting(cfg), 0);
}

/**
 * The functions add default values to unspecified parameters.
 * @param cfg configuration
 */
static void config_default(config_t *cfg)
{
    int i, b;
    cfg_int j;
    const char *s;
    double f;
    config_setting_t *cs = NULL, *vs;
    char *token, *string, *tofree;

    for (i = 0; defaults[i].name; i++) {
        /* Lookup and create setting group */
        tofree = string = strdup(defaults[i].group);
        vs = config_root_setting(cfg);
        while ((token = strsep(&string, ".")) != NULL) {
            cs = config_setting_get_member(vs, token);
            if (!cs)
                cs = config_setting_add(vs, token, CONFIG_TYPE_GROUP);
            vs = cs;
        }
        free(tofree);

        switch (defaults[i].type) {
        case CONFIG_TYPE_STRING:
            if (config_setting_lookup_string(cs, defaults[i].name, &s))
                continue;

            /* Add default value */
            config_setting_remove(cs, defaults[i].name);
            vs = config_setting_add(cs, defaults[i].name,
                                    CONFIG_TYPE_STRING);
            config_setting_set_string(vs, defaults[i].val.str);
            break;
        case CONFIG_TYPE_FLOAT:
            if (config_setting_lookup_float(cs, defaults[i].name, &f))
                continue;

            /* Check for mis-interpreted integer */
            if (config_setting_lookup_int(cs, defaults[i].name, &j)) {
                config_setting_remove(cs, defaults[i].name);
                vs = config_setting_add(cs, defaults[i].name,
                                        CONFIG_TYPE_FLOAT);
                config_setting_set_float(vs, (double) j);
                continue;
            }

            /* Add default value */
            config_setting_remove(cs, defaults[i].name);
            vs = config_setting_add(cs, defaults[i].name, CONFIG_TYPE_FLOAT);
            config_setting_set_float(vs, defaults[i].val.flt);
            break;
        case CONFIG_TYPE_INT:
            if (config_setting_lookup_int(cs, defaults[i].name, &j))
                continue;

            /* Check for mis-interpreted float */
            if (config_setting_lookup_float(cs, defaults[i].name, &f)) {
                config_setting_remove(cs, defaults[i].name);
                vs = config_setting_add(cs, defaults[i].name,
                                        CONFIG_TYPE_INT);
                config_setting_set_int(vs, (long) round(f));
                continue;
            }

            /* Add default value */
            config_setting_remove(cs, defaults[i].name);
            vs = config_setting_add(cs, defaults[i].name, CONFIG_TYPE_INT);
            config_setting_set_int(vs, defaults[i].val.num);
            break;
        case CONFIG_TYPE_BOOL:
            if (config_setting_lookup_bool(cs, defaults[i].name, &b))
                continue;

            /* Check for mis-interpreted integer */
            if (config_setting_lookup_int(cs, defaults[i].name, &j)) {
                config_setting_remove(cs, defaults[i].name);
                vs = config_setting_add(cs, defaults[i].name,
                                        CONFIG_TYPE_BOOL);
                config_setting_set_bool(vs,
                                        j == 0 ? CONFIG_FALSE : CONFIG_TRUE);
                continue;
            }

            /* Add default value */
            config_setting_remove(cs, defaults[i].name);
            vs = config_setting_add(cs, defaults[i].name, CONFIG_TYPE_BOOL);
            config_setting_set_bool(vs, defaults[i].val.num);
            break;

        }
    }
}

/**
 * Checks if the configuration is valid and sane.
 * @return 1 if config is valid, 0 otherwise
 */
int config_check(config_t *cfg)
{
    int num;

    /* Add default values where missing */
    config_default(cfg);

    config_lookup_int(cfg, "features.ngram_len", &num);
    if (num < 1) {
        error("N-gram length needs to be > 0");
        return FALSE;
    }

    return TRUE;
}

/** @} */
