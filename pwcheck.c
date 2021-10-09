#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 1

const int POSITIONAL_ARGS_TOTAL = 2;

int level = -1;
int param = -1;
bool stats = false;

// A function for comparing two char arrays.
int strComp(char *first, char *second) {
  for (int i = 0; 1; i++) {
    if (first[i] != second[i]) {
      return 0;
    } else if (first[i] == '\0') {
      return 1;
    }
  }
}

// This function will return 1 if a given char array is an int.
int isStrInt(char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    if (str[i] < '0' || str[i] > '9') {
      return 0;
    }
  }
  return 1;
}

int processArgs(int argc, char *argv[]) {
  int positionalArgTracker = 0;
  for (int i = 1; i < argc; i++) {
    if (strComp(argv[i], "--stats")) {
      stats = true;
    }

    else {
      if (!isStrInt(argv[i])) {
        fprintf(stderr, "A number was expected in place of \"%s\"!\n", argv[i]);
        return 2;
      }

      switch (positionalArgTracker) {
      case 0:
        level = atoi(argv[i]);
        if (level < 1 && level > 4) {
          fprintf(stderr,
                  "The given level value (%d) has to be between 1 and 4!\n",
                  level);
          return 4;
        }
        break;
      case 1:
        param = atoi(argv[i]);
        if (param < 0) {
          fprintf(stderr, "The given PARAM value (%d) is not positive!\n",
                  param);
          return 5;
        }
        break;
      default:
        fprintf(stderr, "Too many postional arguments found (%d)!",
                positionalArgTracker);
        break;
      }
      ++positionalArgTracker;
    }

    if (positionalArgTracker == POSITIONAL_ARGS_TOTAL) {
      // Finsihed looping through all given arguments and no values are
      // missing.
      return 0;
    }

    fprintf(stderr, "Only %d out of %d positional arguments were given!\n",
            positionalArgTracker, POSITIONAL_ARGS_TOTAL);
    return 3;
  }

  fprintf(stderr, "%d positional arguments requred!\n", POSITIONAL_ARGS_TOTAL);
  return 1;
}

int main(int argc, char *argv[]) {
  // Obtaining user's parameters.
  int exitCode = processArgs(argc, argv);
  if (exitCode) {
#ifdef DEBUG
    fprintf(stderr, "level: %d, param: %d, stats: %d\n", level, param, stats);
#endif
    return exitCode;
  }

  // TODO Managing the stdin

  // TODO Validating individual passwords.
}
