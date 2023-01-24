#pragma once

#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int pattern;         // e
  int upp_low_case;    // i
  int invert_match;    // v
  int match_line;      // c
  int match_file;      // l
  int num_line;        // n
  int pattern_noName;  // h
  int file_no_wrong;   // s
  int patt_file;       // f
  int full_match;      // o
} grepInfo_t;

typedef struct {
  int i;
  int val_regexec;
  int sum_file;
  int flag_for_l;
  int count_for_c;
  int count_for_n;
  int flag_for_lc;
  int res_comp;
  int val_getline;
} handler_arg_t;

void parser(int argc, char *argv[]);
void printLine(FILE *f, char *line, grepInfo_t *info, char *argv,
               handler_arg_t *val_arg);
void flag_o(char *patterns, char *line, grepInfo_t *info,
            handler_arg_t *val_arg, char *argv);
void read_file(char *pattern, int *flag_f);
void handler_flags(int argc, char *argv[], char *patterns, grepInfo_t *info);
void handler_print(grepInfo_t *info, char *line, FILE *f, char *argv,
                   handler_arg_t *val_arg);
void calculate_flag_lc(grepInfo_t *info, handler_arg_t *val_arg);
void handler_lc(grepInfo_t *info, handler_arg_t *val_arg, char *argv);
