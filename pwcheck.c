#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Comment or uncomment the following line to toggle debugging.
#define DEBUG 1
#define AUTHOR "Ondřej Zobal"
#define VERSION "0.1"

const int POSITIONAL_ARGS_TOTAL = 2;

// Whit the values set to -1 it is easier to see whether the argument processing
// function works as it should.
#ifdef DEBUG
int level = -1;
int param = -1;
#endif

// When not debuging the value will be set to 1 which will act as the default it
// the user doesn't wish to change it.
#ifndef DEBUG
int level = 1;
int param = 1;
#endif

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

// Function for processing arguments.
int processArgs(int argc, char *argv[]) {
  int positionalArgTracker = 0;
  for (int i = 1; i < argc; i++) {
    // Processing flags
    if (strComp(argv[i], "--stats")) {
      stats = true;
    }

    // Processing positional arguments
    else {
      if (!isStrInt(argv[i])) {
        fprintf(stderr, "A positive number was expected in place of \"%s\"!\n",
                argv[i]);
        return 2;
      }

      switch (positionalArgTracker) {
      case 0:
        level = atoi(argv[i]);
        if (level < 1 || level > 4) {
          fprintf(stderr,
                  "The given level value (%d) has to be between 1 and 4! Using "
                  "1...\n",
                  level);
        }
        break;
      case 1:
        param = atoi(argv[i]);
        if (param < 0) {
          fprintf(stderr,
                  "The given PARAM value (%d) is not positive! Using 1...\n",
                  param);
        }
        break;
      default:
        fprintf(stderr, "Too many postional arguments found (%d)!\n",
                positionalArgTracker);
        break;
      }
      ++positionalArgTracker;
    }
  }
  if (positionalArgTracker == POSITIONAL_ARGS_TOTAL) {
    // Finsihed looping through all given arguments and no values are
    // missing.
    return 0;
  }

  fprintf(stderr, "You can set custom values with: '%s level param'\n",
          argv[0]);
  fprintf(stderr, "Using default values, level: %d, param: %d\n", level, param);
  return 0;
}

// TODO Likely doesn't even work.
int countRepSubstrings(char *str, int length, int size) {
  int repetitions = 0;

  for (int i = 0; i < length - size; i++) {
    bool identical = true;

    for (int j = 0; j < length; j++) {
      for (int k = 0; k < size; k++) {
        if (i + j == k) {
          continue;
        }

        if (str[i + j] != str[k]) {
          printf("\n%d + %d = %d, %d\n", i, j, i + j, k);
          identical = false;
          break;
        }
      }
    }

    if (identical) {
      repetitions++;
    }
  }

  fprintf(stderr, "Chugnala %d\n", repetitions);
  return repetitions;
}

int processPassword(char *pass, int level, int param, bool *uniqueChars,
                    int *shortest, int *average) {
  int length = 0;
  int upper = 0;
  int lower = 0;
  int number = 0;
  int special = 0;
  int repSequence = 0;

  int repetition = 0;
  char lastChar = 0;

  // Analysing the password.
  for (int i = 0; pass[i] != '\0' && pass[i] != '\n'; i++) {
    if (pass[i] >= 'a' && pass[i] <= 'z') {
      lower++;
    } else if (pass[i] >= 'A' && pass[i] <= 'Z') {
      upper++;
    } else if (pass[i] >= '0' && pass[i] <= '9') {
      number++;
    } else if (pass[i] >= ' ' && pass[i] <= '~') {
      special++;
    }

    // Processing Character Sequences
    // Thanks to this approach a Sequence of 6 identical symbols will only
    // count as 1 repetition when param is set to 2.
    // If current char is the same as the previous one.
    if (i != 0 && lastChar == pass[i]) {
      // Increment the repetition counter.
      repetition++;
      // Check if repetition length reached param.
      if (repetition + 1 == param) {
        // Increment the counter of repetition sequences.
        repSequence++;
      }
    } else {
      repetition = 0;
    }
    lastChar = pass[i];

    length++;
  }

#ifdef DEBUG
  fprintf(stderr,
          "Length: %d, Upeer: %d, Lower: %d, Numbers: %d, Special: %d, "
          "Repetitions: %d\n",
          length, upper, lower, number, special, repSequence);
#endif

  if (length < 1) {
    return 0;
  }

  if (level >= 1 && upper < 0 && lower < 0) {
    printf("\nHelun1\n");
    return 1;
  }
  if (level >= 2 && upper < param && lower < param && number < param &&
      special > param) {
    printf("\nHelun2\n");
    return 1;
  }
  if (level >= 3 && repSequence != 0) {
    printf("\nHelun3\n");
    return 1;
  }
  if (level == 4 && countRepSubstrings(pass, length, param)) {
    return 1;
  }

  return 0;
}

void printStats() { return; }

int main(int argc, char *argv[]) {
#ifdef DEBUG
  fprintf(stderr, "!THIS IS A DEBUG BUILD!\n");
#endif
  // Obtaining user's parameters.
  int exitCode = processArgs(argc, argv);
#ifdef DEBUG
  fprintf(stderr, "level: %d, param: %d, stats: %d\n", level, param, stats);
#endif
  if (exitCode) {
    return exitCode;
  }

  // TODO Managing the stdin
  char password[101];
  int ch;
  int counter;
  while ((ch = getchar()) != EOF) {
    printf("%c", ch); // TODO remove
    password[counter] = ch;
    counter++;
  }

  int statShortest = -1;
  int statAverage = -1;
  bool statUniqueChars['~' - ' ' + 1];

  // TODO Validating individual passwords.
  // for every password
  if (!processPassword(password, level, param, statUniqueChars, &statShortest,
                       &statAverage)) {
    printf("Success: %s\n", password);
  }

  /*
  if (stats) {
    printStats();
  }
*/
}
