
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <signal.h>

void shuffle(int cards []);

int main (int argc, char ** argv) {

  /* Initializing variables */
  char buf [100];
  int players, spawned, i, current, j, count, lowTotal, lowPlayer, activePlayers, total, wr, id, out, tiedI, tiedJ, k;
  int dealt [4];
  int * playerPIDs;
  int pipes [2][2];
  char * args [3];
  int * totals, * playersLeft, * ids;
  int cards [52] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

  /* Checking for exactly 2 arguments */
  if (argc != 2) {
    sprintf(buf, "Dealer: You did not enter the correct amount of arguments! Exiting.\n");
    write(1, buf, strlen(buf));
    exit(1);
  }

  players = atoi(argv[1]);
  activePlayers = players;
  spawned = 0;

  /* Allocating space for some arrays we need */
  if ((playerPIDs = malloc(sizeof(int) * players)) == NULL) {
    sprintf(buf, "Unable to allocate space! Exiting\n");
    write(1, buf, strlen(buf));
    exit(1);
  }

  if ((totals = malloc(sizeof(int) * players)) == NULL) {
    sprintf(buf, "Unable to allocate space! Exiting\n");
    write(1, buf, strlen(buf));
    exit(1);
  }

  if ((ids = malloc(sizeof(int) * players)) == NULL) {
    sprintf(buf, "Unable to allocate space! Exiting\n");
    write(1, buf, strlen(buf));
    exit(1);
  }

  if ((playersLeft = malloc(sizeof(int) * players)) == NULL) {
    sprintf(buf, "Unable to allocate space! Exiting\n");
    write(1, buf, strlen(buf));
    exit(1);
  }

  args[0] = "player";
  args[2] = NULL;

  /* Spawning each child */
  for (i = 0; i < players; i++) {

    pipe(pipes[1]);
    pipe(pipes[0]);

    if ((playerPIDs[spawned] = fork()) ==  0) {

      dup2(pipes[0][0], 0);
      dup2(pipes[1][1], 1);

      close(pipes[0][0]);
      close(pipes[0][1]);
      close(pipes[1][0]);
      close(pipes[1][1]);

      sprintf(args[1], "%d", i);

      execv("player", args);
      sprintf(buf, "Error executing player process\n");
      write(1, buf, strlen(buf));
      exit(1);
    }
  }

  shuffle(cards);
  current = out = 0;
  lowTotal = 999;

  close(pipes[0][0]);
  close(pipes[1][1]);


  for (i = 0; i < players; i++) {
    playersLeft[i] = i + 1;
    ids[i] = i + 1;
  }

  /* This is where the action is happening. Dealing each player */
  while(activePlayers > 1) {
    for (i = 0; i < activePlayers; i++) {
      for (j = 0; j < 4; j++) {
        if (current < 52) {
          dealt[j] = cards[current++];
        } else {
          shuffle(cards);
          current = 0;
        }
      }

      id = playersLeft[i];

      sprintf(buf, "Dealing [%d, %d, %d, %d] to player %d\n", dealt[0], dealt[1], dealt[2], dealt[3], id);
      write(1, buf, strlen(buf));


      if ((wr = write(pipes[0][1], dealt, sizeof(int) * 4)) == -1) {
        sprintf(buf, "Error sending dealt cards.\n");
        write(1, buf, strlen(buf));
      }

      if ((count = read(pipes[1][0], &total, sizeof(int))) != -1) {
        totals[i] = total;
      }

      /* Handling a tie! */
      for (j = 0; j < i; j++) {
        if (totals[j] == totals[i]) {
          sprintf(buf, "Players %d and %d have tied. Dealing until tie is broken.\n", j, i);
          write(1, buf, strlen(buf));

          while (totals[j] == totals[i]) {
            tiedI = playersLeft[i];
            tiedJ = playersLeft[j];

            /* Dealing to i */
            for (k = 0; k < 4; k++) {
              if (current < 52) {
                dealt[k] = cards[current++];
              } else {
                shuffle(cards);
                current = 0;
              }
            }

            sprintf(buf, "Dealing [%d, %d, %d, %d] to player %d\n", dealt[0], dealt[1], dealt[2], dealt[3], tiedI);
            write(1, buf, strlen(buf));


            if ((wr = write(pipes[0][1], dealt, sizeof(int) * 4)) == -1) {
              sprintf(buf, "Error sending dealt cards.\n");
              write(1, buf, strlen(buf));
            }

            if ((count = read(pipes[1][0], &total, sizeof(int))) != -1) {
              totals[i] = total;
            }

            /* Dealing to j */
            for (k = 0; k < 4; k++) {
              if (current < 52) {
                dealt[k] = cards[current++];
              } else {
                shuffle(cards);
                current = 0;
              }
            }

            sprintf(buf, "Dealing [%d, %d, %d, %d] to player %d\n", dealt[0], dealt[1], dealt[2], dealt[3], tiedJ);
            write(1, buf, strlen(buf));


            if ((wr = write(pipes[0][1], dealt, sizeof(int) * 4)) == -1) {
              sprintf(buf, "Error sending dealt cards.\n");
              write(1, buf, strlen(buf));
            }

            if ((count = read(pipes[1][0], &total, sizeof(int))) != -1) {
              totals[j] = total;
            }
          }
        }
      }

      if (total < lowTotal) {
        lowTotal = total;
        lowPlayer = id;
      }

    }

    /* Handling post dealing stuff like printing out totals.*/
    for (i = 0; i < activePlayers; i++) {
      sprintf(buf, "Player %d reports total %d\n", playersLeft[i], totals[i]);
      write(1, buf, strlen(buf));
    }

    sprintf(buf, "Player %d is knocked out!\n", lowPlayer);
    write(1, buf, strlen(buf));

    /* kill(playerPIDs[lowPlayer - 1], SIGTERM); */

    activePlayers += -1;
    lowTotal = 999;

    ids[lowPlayer - 1] = 0;

    j = 0;
    for (i = 0; i < players; i++) {
      if (ids[i] != 0) {
        playersLeft[j] = ids[i];
        j++;
      }
    }

  }

  /* Finishing up the game */

  sprintf(buf, "***  Player %d is the winner!  ***\n", playersLeft[0]);
  write(1, buf, strlen(buf));

  sprintf(buf, "Ending the game.\n");
  write(1, buf, strlen(buf));

  for (i = 0; i < players; i++) {
    kill(playerPIDs[i], SIGTERM);
  }

  while (wait(NULL) > 0);

  free(playerPIDs);
  free(totals);
  free(ids);
  free(playersLeft);

  return 0;

}

void shuffle (int cards []) {

  int i, j, tmp;

  srand(time(0));

  for (i = 0; i < 52; i++) {
    j = i + (rand() % (52 - i));
    tmp = cards[i];
    cards[i] = cards[j];
    cards[j] = tmp;
  }

}
