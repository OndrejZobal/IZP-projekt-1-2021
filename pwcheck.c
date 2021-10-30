/*
** 1. IZP projekt.
** @author Ondřej Zobal <xzobal01@stud.fit.vutbr.cz>
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Comment or uncomment the following line to toggle debugging.
//#define DEBUG 1

#define POSITIONAL_ARGS_TOTAL 2
#define MAX_PASSWORD_LENGTH 100
#define MIN_ALLOWED_CHAR ' '
#define MAX_ALLOWED_CHAR '~'
#define UNIQUE_CHAR_ARRAY_LENGTH (MAX_ALLOWED_CHAR - MIN_ALLOWED_CHAR + 1)

/*
** A function for comparing strings.
** @param first first string to be compared.
** @param second second string to be compared.
** @return 1 if strings are identical, 0 if they differ.
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
  if (level < 1) {
    fprintf(stderr,
            "The given level value (%d) has to be between 1 and 4! Using "
            "1...\n",
            level);
  }

  if (level > 4) {
    return 4;
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
  if (param < 1) {
    fprintf(stderr, "The given PARAM value (%d) is not positive! Using 1...\n",
            param);
  }
  return param;
}

/*
** A function that sets the program's variables according to the users
*arguments.
** @param argc Number of indexes in the argv array.
** @param argv array of command line arguments.
** @return an error code.
*/
bool processArgs(int argc, char *argv[], int *level, int *param, bool *stats) {
  int positionalArgTracker = 0;
  for (int i = 1; i < argc; i++) {
    // Processing flags
    if (argv[i][0] == '-') {
      if (strComp(argv[i], "--stats")) {
        (*stats) = true;
      } else if (strComp(argv[i], "-l")) {
        // Accounting for the value that has to follow the flag
        if (++i >= argc) {
          return false;
        }
        if (((*level) = parseLevel(argv[i])) == -1) {
          return false;
        }
      } else if (strComp(argv[i], "-p")) {
        // Accounting for the value that has to follow the flag
        if (++i >= argc) {
          return false;
        }
        if (((*param) = parseParam(argv[i])) == -1) {
          return false;
        }
      } else {
        fprintf(stderr, "Unknown flag: %s", argv[i]);
        return false;
      }
      continue;
    }

    // Processing positional arguments
    if (!isStrInt(argv[i])) {
      fprintf(stderr, "A positive number was expected in place of \"%s\"!\n",
              argv[i]);
      return false;
    }

    switch (positionalArgTracker) {
    case 0:
      (*level) = parseLevel(argv[i]);
      break;
    case 1:
      (*param) = parseParam(argv[i]);
      break;
    }
    ++positionalArgTracker;
  }

  return true;
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
  for (int i = 0; i < length - size + 1; i++) {

    // This one will be iterating through the str
    for (int j = 0; j < length - size + 1; j++) {
      int identical = 0;
      // And this one will be comparing the individual substrings to the rest of
      // the strings.
      for (int k = 0; k < size; k++) {
        if (i == j) {
          continue;
        }

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
** This function analyses the nature of the password.
** @param lower An address to an int containing the lowest length of password
** @param pass The password to be tested.
** @param uniqueChars A pointer to an array tracking printable ASCII codes that
** @param lower An Address to int where the amount of lowercase characters seen
*in pass will be stored
** @param upper An Address to int where the amount of uppercase characters seen
*in pass will be stored
** @param charGroupCount An Address to int where the amount of different
*character groups seen in password.
** @param length An Address to int where the length of the password will be
*stored.
** @param length An Address to int where the count of repeating characters in
*the password will be stored.
** @param reqSequence The number of groups of repeating characters
*/
void analyzePassword(char *pass, int param, bool *uniqueChars, int *lower,
                     int *upper, int *charGroupCount, int *length,
                     int *repSequence) {
  bool hasUpper = false;
  bool hasLower = false;
  bool hasNum = false;
  bool hasSpecial = false;
  char lastChar = 0;
  int repetition = 0;

  // Analysing the password.
  for (int i = 0; pass[i] != '\0' && pass[i] != '\n'; i++) {
    uniqueChars[((int)pass[i]) - MIN_ALLOWED_CHAR] = true;
    if (pass[i] >= 'a' && pass[i] <= 'z') {
      (*lower)++;
      if (!hasLower) {
        hasLower = true;
        (*charGroupCount)++;
      }
    } else if (pass[i] >= 'A' && pass[i] <= 'Z') {
      (*upper)++;
      if (!hasUpper) {
        hasUpper = true;
        (*charGroupCount)++;
      }
    } else if (pass[i] >= '0' && pass[i] <= '9') {
      if (!hasNum) {
        hasNum = true;
        (*charGroupCount)++;
      }
    } else if (pass[i] >= ' ' && pass[i] <= '~') {
      if (!hasSpecial) {
        hasSpecial = true;
        (*charGroupCount)++;
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
        (*repSequence)++;
      }
    } else {
      repetition = 0;
    }
    lastChar = pass[i];

    (*length)++;
  }
}

/*
** Chechks if the password passes the requirements.
** @param pass The password to be tested.
** @param level The level of password requirements
** @param param An additional parameter
** @param uniqueChars A pointer to an array tracking printable ASCII codes that
*were seen before.
** @param shortest A pointer to var holding the length of the shortest password
*seen yet.
** @param averageSum The sum of all password lengths.
** @param averageCount The amount of passwords added to the averageSum variable.
** @return returns true if the password passed the requirements and false if it
*did not.
*/
bool processPassword(char pass[], int level, int param, bool *uniqueChars,
                     int *shortest, int *averageSum, int *averageCount) {
  int length = 0;
  int upper = 0;
  int lower = 0;
  int repSequence = 0;

  int charGroupCount = 0;

  analyzePassword(pass, param, uniqueChars, &lower, &upper, &charGroupCount,
                  &length, &repSequence);

  *averageSum += length;
  *averageCount += 1;
  if (length < *shortest || *shortest == 0) {
    *shortest = length;
  }
#ifdef DEBUG
  fprintf(stderr, "Length: %d, Upper: %d, Lower: %d,, Repetitions: %d\n",
          length, upper, lower, repSequence);
#endif

  if (level >= 1 && (upper < 1 || lower < 1)) {
    return false;
  }
  if (level >= 2 && charGroupCount < (param > 4 ? 4 : param)) {
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
** @param uniques an array of booleans parallel to the ASCII values of printable
*characters seen before in passwords.
** @param shortest the shortest password seen.
** @param averageSum The sum of all password lengths.
** @param averageCount The amount of passwords added to the averageSum variable.
*/
void printStats(bool uniques[], int shortest, int avgSum, int avgCount) {
  int uniqueCount = 0;
  float avg = 0;
  for (int i = 0; i < UNIQUE_CHAR_ARRAY_LENGTH; i++) {
    if (uniques[i]) {
      uniqueCount++;
    }
  }

  if (avgCount != 0) {
    avg = ((float)avgSum) / avgCount;
  }

  fprintf(stdout, "Statistika:\n");
  fprintf(stdout,
          "Ruznych znaku: %d\nMinimalni delka: %d\nPrumerna delka: %.1f\n",
          uniqueCount, shortest, avg);
}

int main(int argc, char *argv[]) {
  int level = 1;
  int param = 1;
  bool stats = false;

  // Obtaining user's parameters.
  if (!processArgs(argc, argv, &level, &param, &stats)) {
    return 1;
  }

  int statShortest = 0;
  int statAverageSum = 0;
  int statAverageCount = 0;
  bool statUniqueChars[UNIQUE_CHAR_ARRAY_LENGTH];

  for (int i = 0; i < UNIQUE_CHAR_ARRAY_LENGTH; i++) {
    statUniqueChars[i] = false;
  }

  int counter = 0;
  int intCh = 0;
  char password[MAX_PASSWORD_LENGTH + 1];
  while ((intCh = getchar()) != EOF) {
    char ch = intCh;

    if (counter == MAX_PASSWORD_LENGTH - 1 && ch != '\n') {
      fprintf(stderr, "Password cannot be longer than 100 characters!");
      return 1;
    }

    if (ch == '\n') {
      password[counter] = '\0';

#ifdef DEBUG
      fprintf(stderr, "Testing: %sEND\n", password);
#endif

      if (processPassword(password, level, param, statUniqueChars,
                          &statShortest, &statAverageSum, &statAverageCount)) {
        fprintf(stdout, "%s\n", password);
      }

      counter = 0;
    } else {
      password[counter] = ch;
      counter++;
    }
  }

  if (stats) {
    printStats(statUniqueChars, statShortest, statAverageSum, statAverageCount);
  }
}
