#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main (int argc, char ** argv) {
  char buf [50];
  char c;
  char ** words;
  char tmp [30];
  int wordCount, lineCount, wordLength, maxLength, i, j, k;
  int in, lineIndex, begin, outFile, q, p, t, tmpLine, fd;
  int * lines;

  wordCount = lineCount = wordLength = maxLength = lineIndex = 0;

  fd = open(argv[1], O_RDONLY);

  if (fd == -1) {
    sprintf(buf, "No file could be opened\n");
    write(1, buf, strlen(buf));
    return 0;
  }

  lseek(fd, 0, SEEK_SET);

  /* Read in file and get counts like line and word count */
  while (read(fd, &c, 1)) {
    wordLength++;
    if (c == ' ') {
      wordCount++;
      if (maxLength < wordLength) {
        maxLength =  wordLength;
      }
      wordLength = 0;
    } else if (c == '\n') {
      wordCount++;
      lineCount++;
      if (maxLength < wordLength) {
        maxLength =  wordLength;
      }
      wordLength = 0;
    }
  }


  lseek(fd, 0, SEEK_SET);
  /*char words [wordCount + lineCount][30];*/
  if ((words = malloc((wordCount + lineCount) * sizeof(char*))) == NULL) {
    sprintf(buf, "Unable to allocate space! Exiting\n");
    write(1, buf, strlen(buf));
    return 0;
  }

  for (i = 0; i < wordCount + lineCount; i++) {
    if ((words[i] = malloc((maxLength + 1) * sizeof(char))) == NULL) {
      sprintf(buf, "Unable to allocate space! Exiting\n");
      write(1, buf, strlen(buf));
      return 0;
    }
  }


  i = j = 0;

  /* Reading the file again to save it to a 2D array sized based on the sizes */
  while (read(fd, &c, 1)) {
    if (c == '\n') {
      words[i][j] = '\0';
      i++;
      j = 0;
      words[i][0] = c;
      i++;
    } else if (c == ' ') {
      words[i][j] = '\0';
      i++;
      j = 0;
    } else {
      words[i][j] = c;
      j++;
    }
  }

  close(fd);


  if ((lines = (int *)malloc(lineCount * sizeof(int))) == NULL) {
    sprintf(buf, "Unable to allocate space! Exiting\n");
    write(1, buf, strlen(buf));
    return 0;
  }

  /* This is where the sorting of each line is. Each line is separated by
      a new line character */
  for (k = 0; k < lineCount; k++) {
    lines[k] = lineIndex;
    begin = lineIndex;
    while (words[lineIndex][0] != '\n') {
      lineIndex++;
    }

    for (i = begin; i < lineIndex - 1; i++) {
      for (j = i + 1; j < lineIndex; j++) {
        if (strcmp(words[i], words[j]) > 0) {
          strcpy(tmp, words[i]);
          strcpy(words[i], words[j]);
          strcpy(words[j], tmp);
        }
      }
    }

    lineIndex++;

  }

  /* Now, sorting the order of the lines using an array that contains the
      index of each line */
  for (i = 0; i < lineCount - 1; i++) {
    for (j = i + 1; j < lineCount; j++) {
      if (strcmp(words[lines[i]], words[lines[j]]) > 0) {
        tmpLine = lines[i];
        lines[i] = lines[j];
        lines[j] = tmpLine;
      }
    }
  }

  /* Checking if any lines are equivalent and getting rid of it! */
  for (i = 0; i < lineCount - 1; i++) {
    for (j = i + 1; j < lineCount; j++) {
      q = lines[i];
      p = lines[j];
      while (words[q][0] != '\n' || words[p][0] != '\n') {
        if (strcmp(words[q], words[p]) == 0) {
          q++; p++;

          if (words[q][0] == '\n' && words[p][0] == '\n') {
            for (t = j; t < lineCount - 1; t++) {
              lines[t] = lines[t + 1];
            }
            lineCount = lineCount - 1;
            if (i != 0) {
              i--; j--;
            } else {
              j--; i = 0;
            }
          }
        } else {
          break;
        }
      }
    }
  }

  /* Finally, spitting out the results! */
  if (argc != 3) {
    for (i = 0; i < lineCount; i++) {
      in = lines[i];
      while (words[in][0] != '\n') {
        sprintf(buf, "%s ", words[in]);
        write(1, buf, strlen(buf));
        in++;
      }
      sprintf(buf, "\n");
      write(1, buf, strlen(buf));
    }
  } else {
    outFile = open(argv[2], O_WRONLY | O_CREAT, 0777);
    if (outFile == -1) {
      sprintf(buf, "File coult not be created or opened. Exiting\n");
      write(1, buf, strlen(buf));
      return 0;
    }
    for (i = 0; i < lineCount; i++) {
      in = lines[i];
      while (words[in][0] != '\n' && in <= wordCount + lineCount + 1) {
        sprintf(buf, "%s ", words[in]);
        write(outFile, buf, strlen(buf));
        in++;
      }
      sprintf(buf, "\n");
      write(outFile, buf, strlen(buf));
    }
  }

  free(words);
  free(lines);

  return 0;
}
