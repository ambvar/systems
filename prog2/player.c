
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char ** argv) {
  int i, total;
  int cards [4];

  while ((i = read(0, cards, sizeof(int) * 4)) != -1) {

    total = cards[0] + cards[1] + cards[2] + cards[3];
    write(1, &total, sizeof(int));

  }


  return 0;

}
