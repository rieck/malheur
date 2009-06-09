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
#include "common.h"
#include "util.h"

/** Progress bar (with NULL) */
static char pb_string[PROGBAR_LEN + 1];
/** Start timestamp measured */
static double pb_start = 0;


/**
 * Print the Malheur configuration is valid. 
 * @param cfg configuration
 */
void print_config(config_t *cfg)
{
    double f;
    long i, j, k;
    const char *s, *t;

    printf("Malheur configuration\n");

    /* Input section */
    config_lookup_string(cfg, "input.format", &s);
    printf("  input.format:           %s\n", s);
    
    config_lookup_int(cfg, "input.mist_level", &i);
    config_lookup_int(cfg, "input.mist_report_len", &j);
    config_lookup_int(cfg, "input.mist_thread_len", &k);   
    printf("  input.mist_*:           level=%ld, report_len=%ld, "
           "thread_len=%ld\n", i, j, k);

    /* Feature section */
    config_lookup_int(cfg, "features.ngram_length", &i);
    config_lookup_string(cfg, "features.ngram_delim", &s);
    config_lookup_string(cfg, "features.normalization", &t);
    printf("  features.ngram_*:       len=%ld, delim=%s\n", i, s);
    printf("  features.normalization: %s\n", t);

    /* Analysis setting */    
    config_lookup_float(cfg, "analysis.prototype_radius", &f);
    config_lookup_int(cfg, "analysis.prototype_labels", &i);
    printf("  analysis.prototype_*:   radius=%g, labels=%s\n", f, 
           i ? "true" : "false");
}

/**
 * Checks if the configuration is valid. The function currently checks
 * if all required parameters are set. Later versions might also set 
 * default values and only issue warning, if parameters are not available.
 * @param cfg configuration
 * @return error message or NULL
 */
char *check_config(config_t *cfg)
{
    double f;
    long l;
    const char *s;

    /* Check in input setting */    
    if (config_lookup_string(cfg, "input.format", &s) != CONFIG_TRUE)
        return "'format' not defined in configuration 'input'";        
    if (config_lookup_int(cfg, "input.mist_level", &l) != CONFIG_TRUE)
        return "'mist_level' not defined in configuration 'input'";   
    if (config_lookup_int(cfg, "input.mist_report_len", &l) != CONFIG_TRUE)
        return "'mist_report_len' not defined in configuration 'input'";   
    if (config_lookup_int(cfg, "input.mist_thread_len", &l) != CONFIG_TRUE)
        return "'mist_thread_len' not defined in configuration 'input'";   

    /* Check in features setting */    
    if (config_lookup_int(cfg, "features.ngram_length", &l) != CONFIG_TRUE)
        return "'ngram_length' not defined in configuration group 'features'";
    if (config_lookup_string(cfg, "features.ngram_delim", &s) != CONFIG_TRUE)
        return "'ngram_delim' not defined in configuration group 'features'";
    if (config_lookup_string(cfg, "features.normalization", &s) != CONFIG_TRUE)
        return "'normalization' not defined in configuration 'features'";

    /* Check in analysis setting */    
    if (config_lookup_float(cfg, "analysis.prototype_radius", &f) != CONFIG_TRUE)
        return "'prototype_radius' not defined in configuration group 'analysis'";
    if (config_lookup_int(cfg, "analysis.prototype_labels", &l) != CONFIG_TRUE)
        return "'prototype_labels' not defined in configuration group 'analysis'";

    return NULL;
}

/**
 * Print a formated error/warning message. See the macros error and 
 * warning in util.h
 * @param p Prefix string, e.g. "Error"
 * @param f Function name
 * @param m Format string
 */
void err_msg(char *p, const char *f, char *m, ...)
{
    va_list ap;
    char s[256] = { " " };

    va_start(ap, m);
    vsnprintf(s, 256, m, ap);
    va_end(ap);

    fprintf(stderr, "%s: %s", p, s);
    if (errno)
        fprintf(stderr, ": %s", strerror(errno));
    fprintf(stderr, " [%s]\n", f);

    errno = 0;
}

/**
 * Print a progress bar in a given range.
 * @param min Minimum value 
 * @param max Maximum value
 * @param in Current value
 */
void prog_bar(double min, double max, double in)
{
    int i;
    double perc, ptime = 0;
    char *descr = "";

    perc = (in - min) / (max - min);

    if (pb_start == 0 || fabs(in - min) < 1e-12) {
        /* Start of progress */
        pb_start = time_stamp();
        for (i = 0; i < PROGBAR_LEN; i++)
            pb_string[i] = PROGBAR_EMPTY;
        descr = "start";
        perc = 0.0;
    } else if (fabs(in - max) < 1e-12) {
        /* End of progress */
        for (i = 0; i < PROGBAR_LEN; i++)
            pb_string[i] = PROGBAR_FULL;
        ptime = time_stamp() - pb_start;
        descr = "total";
        perc = 1.0;
        pb_start = 0;
    } else {
        /* Middle of progress */
        int len = round(perc * PROGBAR_LEN);
        for (i = 0; i < len; i++)
            if (i < len - 1)
                pb_string[i] = PROGBAR_DONE;
            else
                pb_string[i] = PROGBAR_FRONT;
        ptime = (max - in) * (time_stamp() - pb_start) / (in - min);
        descr = "   in";
    }

    int mins = floor(ptime / 60);
    int secs = floor(ptime - mins * 60);
    pb_string[PROGBAR_LEN] = 0;

    printf("\r  [%s] %5.1f%%  %s %.2dm %.2ds", pb_string,
           perc * 100, descr, mins, secs);
    fflush(stdout);
    fflush(stderr);
}

/**
 * Return a timestamp of the real time
 * @return time stamp
 */
double time_stamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

/**
 * Decode a string with URI encoding. The function operates 
 * in-place. A trailing NULL character is appended to the string.
 * @param str Stirng to escape.
 * @return length of decoded sequence
 */
int decode_string(char *str) 
{
    int j, k, r;
    char hex[5] = "0x00";
    
    /* Loop over string */
    for (j = 0, k = 0; j < strlen(str); j++, k++) {
        if (str[j] != '%') {
            str[k] = str[j];
        } else {   
            /* Check for truncated string */
            if (strlen(str) - j < 2)
                    break;

            /* Parse hexadecimal number */
            hex[2] = str[++j];
            hex[3] = str[++j];
            sscanf(hex, "%x", (unsigned int *) &r);
            str[k] = r;
        }
    }

    return k;
}

/**
 * Extracts the suffix from a file name. If the file does not
 * have a suffix, the function returns "unknown". 
 */ 
char *file_suffix(char *file)
{
    char *name = file + strlen(file) - 1;

    /* Determine dot in file name */
    while (name != file && *name != '.') 
        name--;

    /* Check for files with no suffix */
    if (name == file)
        name = "unknown";
    else
        name++;
        
    return name;
}

#ifndef HAVE_FUNC_LOG2
/** 
 * Logarithm of x to base 2
 * @param x input value
 * @return logarithm 
 */
double log2(double x)
{
    return log10(x) * 3.32192809488736234;
}
#endif

#ifndef HAVE_FUNC_ROUND
/** 
 * Round function 
 * @param x input value
 * @return integer number
 */
long round(double x)
{
    double f = floor(x);
    if (s - f >= 0.5)
        return (long) f + 1;
    return (long) f;
}
#endif
