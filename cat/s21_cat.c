#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define all_val_struct_DEF                                            \
  structCat.numNonEmpty || structCat.endOfLine || structCat.numAll || \
      structCat.squeeze || structCat.tabs || structCat.endLineV ||    \
      structCat.tabsV || structCat.v
typedef struct {
  bool numNonEmpty;  // b - done
  bool endLineV;     // e - done
  bool endOfLine;    // E - done
  bool numAll;       // n - done
  bool squeeze;      // s - done
  bool tabs;         // T - done
  bool tabsV;        // t - done
  bool v;            // v -
} catInfo_t;         // ok

void NoArg(int fd) {
  char buf[4096];
  int bytes_read = read(fd, buf, 4096);
  if (bytes_read == -1) {
    fprintf(stderr, "No such file or directory\n");
  }
  while (bytes_read > 0) {
    printf("%.*s", bytes_read, buf);
    bytes_read = read(fd, buf, 4096);
  }
}  // проверить обнуляется ли буфер, если нет - обнулить + почитай о memset

int haveFlags(catInfo_t *structCat, char argv[]) {
  int flag_fun = true;
  FILE *file = fopen(argv, "r");
  char ch;
  char s_ch = '\n';
  int flag = 1;
  int flagForS = 0;
  int num = 0;
  if (file != NULL) {
    if (structCat->endLineV) {
      structCat->endOfLine = true;
    }
    if (structCat->tabsV) {
      structCat->tabs = true;
    }
    if (structCat->numNonEmpty && structCat->numAll) {
      structCat->numAll = false;
    }
    while (fscanf(file, "%c", &ch) == 1) {
      if (structCat->squeeze && (ch == '\n' && s_ch == '\n')) {
        if (!(flagForS)) {
          if ((structCat->endOfLine || structCat->endLineV) &&
              (structCat->numAll)) {
            num++;
            printf("%6d\t$\n", num);
          } else if (structCat->endOfLine || structCat->endLineV) {
            printf("$\n");
            flagForS = 1;
          } else if (structCat->numAll) {
            num++;
            printf("%6d\t\n", num);
          } else {
            printf("\n");
          }
          flagForS = 1;
        }
        s_ch = ch;
        if (flagForS) {
          continue;
        }
      }
      if (structCat->numAll) {
        if (flag) {
          num++;
          printf("%6d\t", num);
        }
        if (ch == '\n') {
          flag = 1;
        } else {
          flag = 0;
        }
      } else if (structCat->numNonEmpty) {
        if (s_ch == '\n' && ch != '\n') {
          num++;
          printf("%6d\t", num);
        }
      }
      if (structCat->endOfLine) {
        if (ch == '\n') {
          printf("$");
        }
      }
      if (structCat->tabs) {
        if (ch == '	') {
          printf("^");
          ch += 64;
        }
      }
      if (structCat->endLineV || structCat->tabsV || structCat->v) {
        if ((ch <= 31 && ch >= 0) && ch != '\n' && ch != '\t') {
          printf("^%c", ch + 64);
        } else if (ch == 127) {
          printf("^?");
        } else {
          printf("%c", ch);
        }
      } else {
        printf("%c", ch);
      }
      flagForS = 0;
      s_ch = ch;
    }
    fclose(file);
  } else {
    fprintf(stderr, "sorry, we can't open your file");
    flag_fun = false;
  }
  return flag_fun;
}

bool catPars(catInfo_t *info, char *argv) {
  int flag = true;
  ++argv;
  if (*argv == '-') {
    ++argv;
    if (strcmp(argv, "number-nonblank") == 0) {
      info->numNonEmpty = true;
    } else if (strcmp(argv, "number") == 0) {
      info->numAll = true;
    } else if (strcmp(argv, "squeeze-blank") == 0) {
      info->squeeze = true;
    } else {
      fprintf(stderr, "s21 cat: illegal option -- %s", argv);
      flag = false;
    }
  } else {
    for (char *fl = argv; *fl != '\0'; ++fl) {
      switch (*fl) {
        case 'b':
          info->numNonEmpty = true;
          break;
        case 'E':
          info->endOfLine = true;
          break;
        case 'e':
          info->endLineV = true;
          break;
        case 'n':
          info->numAll = true;
          break;
        case 's':
          info->squeeze = true;
          break;
        case 'T':
          info->tabs = true;
          break;
        case 't':
          info->tabsV = true;
          break;
        case 'v':
          info->v = true;
          break;
        default:
          fprintf(stderr, "s21 cat: illegal option -- %s\n", argv);
          flag = false;
      }
    }
  }
  return flag;
}

bool haveArg(int argc, char *argv[]) {
  int flag = true;
  catInfo_t structCat = {0, 0, 0, 0, 0, 0, 0, 0};
  for (int i = 1; i < argc; ++i) {
    if (*argv[i] == '-') {
      if (!catPars(&structCat, argv[i])) {
        flag = false;
      } else if (*argv[i] != '-') {
        i = argc;
      }
    } else {
      i = argc;
    }
  }
  if (!(all_val_struct_DEF)) {
    for (int i = 1; i < argc; i++) {
      if (*argv[i] != '-') {
        NoArg(open(argv[i], O_RDONLY));
      }
    }
  } else if ((all_val_struct_DEF)) {
    for (int i = 1; i < argc; i++) {
      if (*argv[i] != '-') {
        if (!haveFlags(&structCat, argv[i])) {
          flag = false;
        }
      }
    }
  }
  return flag;
}

int main(int argc, char *argv[]) {
  int flag = true;
  if (argc == 1) {
    NoArg(STDIN_FILENO);
  } else if (!haveArg(argc, argv)) {
    flag = false;
  }
  return flag;
}
