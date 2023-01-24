#include "s21_grep.h"

int main(int argc, char *argv[]) {
  if (argc > 2) {
    parser(argc, argv);
  } else {
    fprintf(stderr, "not enough arguments\n");
  }
}

void parser(int argc, char *argv[]) {
  int flag_f = true;
  char all_pat[10000] = {0};
  grepInfo_t info = {0};
  int res_getopt;
  while ((res_getopt = (getopt_long(argc, argv, "e:ivclnhsf:o", NULL, NULL))) !=
         -1) {
    switch (res_getopt) {
      case 'e':
        info.pattern = true;
        strcat(all_pat, optarg);
        strcat(all_pat, "|");
        break;
      case 'i':
        info.upp_low_case = true;
        break;
      case 'v':
        info.invert_match = true;
        break;
      case 'c':
        info.match_line = true;
        break;
      case 'l':
        info.match_file = true;
        break;
      case 'n':
        info.num_line = true;
        break;
      case 'h':
        info.pattern_noName = true;
        break;
      case 's':
        info.file_no_wrong = true;
        break;
      case 'f':
        info.patt_file = true;
        (read_file(all_pat, &flag_f));
        break;
      case 'o':
        info.full_match = true;
        break;
      default:
        fprintf(stderr, "sorry, i can't work with your flag");
    }
  }
  if (flag_f) {
    handler_flags(argc, argv, all_pat, &info);
  }
}

void handler_flags(int argc, char *argv[], char *patterns, grepInfo_t *info) {
  handler_arg_t val_arg = {0};
  if (info->pattern || info->patt_file) {
    val_arg.i = optind;
    if (patterns[strlen(patterns) - 1] == '|')
      patterns[strlen(patterns) - 1] = '\0';
  } else {
    val_arg.i = optind + 1;
    strcat(patterns, argv[optind]);
  }
  if (info->invert_match) info->full_match = false;
  for (; val_arg.i < argc; (val_arg.i)++) {
    val_arg.flag_for_l = 0, val_arg.count_for_c = 0, val_arg.count_for_n = 0,
    val_arg.flag_for_lc = 0;
    FILE *f = fopen(argv[val_arg.i], "r");
    regex_t preg;
    char *line_buf = NULL;
    size_t line_want = 0;
    if (info->pattern || info->patt_file) {
      val_arg.sum_file = argc - optind;
    } else {
      val_arg.sum_file = argc - optind - 1;
    }
    if (info->upp_low_case)
      val_arg.res_comp = regcomp(&preg, patterns, REG_EXTENDED | REG_ICASE);
    else
      val_arg.res_comp = regcomp(&preg, patterns, REG_EXTENDED);
    if (f != NULL && val_arg.res_comp == 0) {
      while ((val_arg.val_getline = getline(&line_buf, &line_want, f) > 0)) {
        (val_arg.count_for_n)++;
        val_arg.val_regexec = regexec(&preg, line_buf, 0, NULL, 0);
        if (info->full_match && !(info->match_file) && !(info->match_line)) {
          flag_o(patterns, line_buf, info, &val_arg, argv[val_arg.i]);
          continue;
        } else if (!(info->match_file) && !(info->match_line)) {
          printLine(f, line_buf, info, argv[val_arg.i], &val_arg);
        } else if (val_arg.val_regexec == 0 && !(info->invert_match)) {
          calculate_flag_lc(info, &val_arg);
        } else if (val_arg.val_regexec != 0 && (info->invert_match)) {
          calculate_flag_lc(info, &val_arg);
        }
      }
      handler_lc(info, &val_arg, argv[val_arg.i]);
      val_arg.count_for_n = 0;
      fclose(f);
      free(line_buf);
    } else if (!(info->file_no_wrong)) {
      fprintf(stderr, "grep: %s: No such file or directory\n", argv[val_arg.i]);
    }
    regfree(&preg);
  }
}

void handler_lc(grepInfo_t *info, handler_arg_t *val_arg, char *argv) {
  if (val_arg->flag_for_lc == true &&
      (val_arg->sum_file == 1 || info->pattern_noName)) {
    printf("1\n");
    printf("%s\n", argv);
  } else if (val_arg->flag_for_lc == true && val_arg->sum_file > 1) {
    printf("%s:1\n", argv);
    printf("%s\n", argv);
  } else if (info->match_file || info->match_line) {
    if (val_arg->flag_for_l == true)
      printf("%s\n", argv);
    else if (info->match_line) {
      if (val_arg->sum_file > 1 && !(info->pattern_noName))
        printf("%s:%d\n", argv, val_arg->count_for_c);
      else
        printf("%d\n", val_arg->count_for_c);
    }
  }
}

void calculate_flag_lc(grepInfo_t *info, handler_arg_t *val_arg) {
  if (info->match_file && info->match_line) {
    val_arg->flag_for_lc = true;
  } else if (info->match_file)
    val_arg->flag_for_l = true;
  else if (info->match_line)
    val_arg->count_for_c++;
}

void handler_print(grepInfo_t *info, char *line, FILE *f, char *argv,
                   handler_arg_t *val_arg) {
  if (val_arg->sum_file > 1 && !(info->pattern_noName) && info->num_line &&
      !(info->pattern_noName))
    printf("%s:%d:%s", argv, val_arg->count_for_n, line);
  else if (val_arg->sum_file > 1 && !(info->pattern_noName))
    printf("%s:%s", argv, line);
  else if (info->num_line)
    printf("%d:%s", val_arg->count_for_n, line);
  else
    printf("%s", line);
  if (feof(f)) {
    printf("\n");
  }
}

void printLine(FILE *f, char *line, grepInfo_t *info, char *argv,
               handler_arg_t *val_arg) {
  if (val_arg->val_regexec == 0 && !(info->invert_match)) {
    handler_print(info, line, f, argv, val_arg);
  } else if (val_arg->val_regexec != 0 && info->invert_match) {
    handler_print(info, line, f, argv, val_arg);
  }
}

void read_file(char *pattern, int *flag_f) {
  FILE *f = fopen(optarg, "r");
  if (f != NULL) {
    char *line = NULL;
    size_t lineWant = 0;
    while (getline(&line, &lineWant, f) > 0) {
      if (strlen(line) != 0 && line[strlen(line) - 1] == '\n') {
        line[strlen(line) - 1] = '\0';
      }
      if ((line && pattern)) {
        strcat(pattern, line);
        strcat(pattern, "|");
      }
    }
    free(line);
    fclose(f);
  } else {
    *flag_f = false;
    fprintf(stderr, "sorry, flag_f cant open %s\n", optarg);
  }
}

void flag_o(char *patterns, char *line, grepInfo_t *info,
            handler_arg_t *val_arg, char *argv) {
  int buf = 0;
  regex_t preg;
  regmatch_t pmatch[1];
  if (info->upp_low_case)
    regcomp(&preg, patterns, REG_EXTENDED | REG_ICASE);
  else
    regcomp(&preg, patterns, REG_EXTENDED);
  while (1) {
    if (regexec(&preg, line, 1, pmatch, 0)) {
      break;
    }
    if (val_arg->sum_file > 1 && !(info->pattern_noName) && info->num_line &&
        buf != val_arg->count_for_n)
      printf("%s:%d:", argv, val_arg->count_for_n);
    else if (val_arg->sum_file > 1 && !(info->pattern_noName) &&
             buf != val_arg->count_for_n)
      printf("%s:", argv);
    else if (info->num_line && buf != val_arg->count_for_n)
      printf("%d:", val_arg->count_for_n);
    for (int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; ++i) {
      printf("%c", line[i]);
    }
    printf("\n");
    line += pmatch[0].rm_eo;
    buf = val_arg->count_for_n;
  }
  regfree(&preg);
}
