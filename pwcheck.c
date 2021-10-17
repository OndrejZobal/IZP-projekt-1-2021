/*
** 1. IZP projekt.
** @author Ondřej Zobal <xzobal01@stud.fit.vutbr.cz>
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Comment or uncomment the following line to toggle debugging.
//#define DEBUG 1
#define AUTHOR "Ondřej Zobal"
#define VERSION "0.1"

const int POSITIONAL_ARGS_TOTAL = 2;
const int MAX_PASSWORD_LENGTH = 100;
const char MIN_ALLOWED_CHAR = ' ';
const char MAX_ALLOWED_CHAR = '~';
const char UNIQUE_CHAR_ARRAY_LENGTH = MAX_ALLOWED_CHAR - MIN_ALLOWED_CHAR + 1;

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

/*
** A function for comparing strings.
** @param first first string to be compared.
** @param second second string to be compared.
** @return 1 if strings are identical, 0 if thay differ.
*/
int strComp(char *first, char *second) {
  for (int i = 0; 1; i++) {
    if (first[i] != second[i]) {
      return 0;
    } else if (first[i] == '\0') {
      return 1;
    }
  }
}

/*
** Tests if a string is a valid integer.
** @param str the string that will be tested
** @return 1 if the string is a number, 0 if it isn't.
*/
int isStrInt(char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    if (str[i] < '0' || str[i] > '9') {
      return 0;
    }
  }
  return 1;
}

/*
** Parses a string as a 'level' value.
** @param str the string to be tested.
** @return The numeric value of the parsed str.
*/
int parseLevel(char str[]) {
  int level = atoi(str);
  if (level < 1 || level > 4) {
    fprintf(stderr,
            "The given level value (%d) has to be between 1 and 4! Using "
            "1...\n",
            level);
    // return -1;
  }
  return level;
}

/*
** Parses a string as a 'param' value.
** @param str the string to be tested.
** @return The numeric value of the parsed str.
*/
int parseParam(char str[]) {
  int param = atoi(str);
  // Todo edge cases for level and param combinations.
  if (param < 1) {
    fprintf(stderr, "The given PARAM value (%d) is not positive! Using 1...\n",
            param);
    // return -1;
  }
  return param;
}

/*
** A function that sets the program's variables according to the users
*arguments.
** @param argc Number of indexes in the argv array.
** @param argv array of command line argumets.
** @return an error code.
*/
int processArgs(int argc, char *argv[]) {
  int positionalArgTracker = 0;
  for (int i = 1; i < argc; i++) {
    // Processing flags
    if (argv[i][0] == '-') {
      if (strComp(argv[i], "--stats")) {
        stats = true;
      } else if (strComp(argv[i], "-l")) {
        // Acounting for the value that has to follow the flag
        if (++i >= argc) {
          return 1;
        }
        if ((level = parseLevel(argv[i])) == -1) {
          return 1;
        }
      } else if (strComp(argv[i], "-p")) {
        // Acounting for the value that has to follow the flag
        if (++i >= argc) {
          return 1;
        }
        if ((param = parseParam(argv[i])) == -1) {
          return 1;
        }
      } else {
        fprintf(stderr, "Unknown flag: %s", argv[i]);
        return 2;
      }
      continue;
    }

    // Processing positional arguments
    if (!isStrInt(argv[i])) {
      fprintf(stderr, "A positive number was expected in place of \"%s\"!\n",
              argv[i]);
      return 2;
    }

    switch (positionalArgTracker) {
    case 0:
      level = parseLevel(argv[i]);
      break;
    case 1:
      param = parseParam(argv[i]);
      break;
    }
    ++positionalArgTracker;
  }
  /*
  if (positionalArgTracker == POSITIONAL_ARGS_TOTAL) {
    // Finsihed looping through all given arguments and no values are
    // missing.
    return 0;
  }

  fprintf(stderr, "You can set custom values with: '%s level param'\n",
          argv[0]);
  fprintf(stderr, "Using default values, level: %d, param: %d\n", level, param);
  return 0;
*/
  return 0;
}

/*
** This function usually works!
** Searches the input string for repeating substrings.
*arguments.
** @param str the string to be tested for substrings.
** @return 1 if the message contains substrings, 0 if it doesn't.
*/
int countRepSubstrings(char *str, int length, int size) {

  // This loops moves the substring offset.
  for (int i = 0; i < length - size; i++) {

    // This one will be iterating through the str
    for (int j = 0; j < length - size; j++) {
      int identical = 0;
      // And this one will be comparing the individual substrings to the rest of
      // the strings.
      for (int k = 0; k < size; k++) {
        if (i == j) {
          continue;
        }

        // fprintf(stdout, "%d + %d = %d; %d + %d = %d\n", i, k, i + k, j, k,
        //        j + k);
        if (str[i + k] == str[j + k]) {
          identical++;
        }
      }
      if (identical >= size) {
        return true;
      }
    }
  }

  return false;
}

/*
** Chechks if the password passes the requirements.
** @param pass The password to be tested.
** @param level The level of password reuqirements
** @param param A suplemental parameter
** @param uniqueChars A pointer to an array tracking printable ascii codes that
*were seen before.
** @param shortest A pointer to var holidng the length of the shortest passowrd
*seen yet.
** @param averageSum The sum of all password lengths.
** @param averageCount The amount of passwords added to the averageSum variable.
** @return returns true if the password passed the requirements and false if it
*did not.
*/
bool processPassword(char *pass, int level, int param, bool *uniqueChars,
                     int *shortest, int *averageSum, int *averageCount) {
  int length = 0;
  int upper = 0;
  int lower = 0;
  int repSequence = 0;

  int repetition = 0;
  char lastChar = 0;

  bool hasUpper, hasLower, hasNum, hasSpecial = false;
  int charGroupCount = 0;

  // Analysing the password.
  for (int i = 0; pass[i] != '\0' && pass[i] != '\n'; i++) {
    uniqueChars[((int)pass[i]) - MIN_ALLOWED_CHAR] = true;
    if (pass[i] >= 'a' && pass[i] <= 'z') {
      lower++;
      if (!hasLower) {
        hasLower = true;
        charGroupCount++;
      }
    } else if (pass[i] >= 'A' && pass[i] <= 'Z') {
      upper++;
      if (!hasUpper) {
        hasUpper = true;
        charGroupCount++;
      }
    } else if (pass[i] >= '0' && pass[i] <= '9') {
      if (!hasNum) {
        hasNum = true;
        charGroupCount++;
      }
    } else if (pass[i] >= ' ' && pass[i] <= '~') {
      if (!hasSpecial) {
        hasSpecial = true;
        charGroupCount++;
      }
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

  *averageSum += length;
  *averageCount += 1;
  if (length < *shortest || *shortest == 0) {
    *shortest = length;
  }
#ifdef DEBUG
  fprintf(stderr,
          "Length: %d, Upeer: %d, Lower: %d, Numbers: %d, Special: %d, "
          "Repetitions: %d\n",
          length, upper, lower, number, special, repSequence);
#endif

  if (level >= 1 && (upper < 1 || lower < 1)) {
    return false;
  }
  if (level >= 2 && charGroupCount < param) {
    return false;
  }
  if (level >= 3 && (repSequence != 0 || param == 1)) {
    return false;
  }
  if (level == 4 && countRepSubstrings(pass, length, param)) {
    return false;
  }

  return true;
}

/*
** Prints the stats summary to stdout.
** @param uniques an array of bools parallel to the ASCII values of printable
*characters seen before in passwords.
** @param shortest the shortest password seen.
** @param averageSum The sum of all password lengths.
** @param averageCount The amount of passwords added to the averageSum variable.
*/
void printStats(bool uniques[], int shortest, int avgSum, int avgCount) {
  int uniqueCount = 0;
  for (int i = 0; i < UNIQUE_CHAR_ARRAY_LENGTH; i++) {
    if (uniques[i]) {
      uniqueCount++;
    }
  }

  fprintf(stdout, "Statistika:\n");
  fprintf(stdout,
          "Ruznych znaku: %d\nMinimalni delka: %d\nPrumerna delka: %.1f\n",
          uniqueCount, shortest, ((float)avgSum) / avgCount);
}

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

  int statShortest = 0;
  int statAverageSum = 0;
  int statAverageCount = 0;
  bool statUniqueChars[UNIQUE_CHAR_ARRAY_LENGTH];
  // The array already contains data when it get's created and I am not sure
  // why. It should be memory corruption, but there is nothing that could cause
  // memoryc corruption that runs before it and also the rest of the memory
  // seems fine as well
  for (int i = 0; i < UNIQUE_CHAR_ARRAY_LENGTH; i++) {
    statUniqueChars[i] = false;
  }

  // TODO Validating individual passwords.
  // for every password
  int counter = 0;
  char ch;
  while ((ch = getchar()) != EOF) {
    char password[MAX_PASSWORD_LENGTH];
    if (ch == '\n' || counter == MAX_PASSWORD_LENGTH) {
      password[counter] = '\0';
#ifdef DEBUG
      fprintf(stderr, "Testing: %sEND\n", password);
#endif
      if (processPassword(password, level, param, statUniqueChars,
                          &statShortest, &statAverageSum, &statAverageCount)) {
        printf("%s\n", password);
      }

      counter = 0;
      password[counter] = '\0';
    } else {
      password[counter] = ch;
      counter++;
    }
  }

  if (stats) {
    printStats(statUniqueChars, statShortest, statAverageSum, statAverageCount);
  }
}
