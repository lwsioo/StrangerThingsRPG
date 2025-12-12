#include "gamelib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
  srand(time(NULL));
  int choice;

  do {
    do {
      system("clear");
      printf("\n" RED "======================================\n" RESET);
      printf(RED "              COSESTRANE           " RESET "\n");
      printf(RED "======================================" RESET "\n");
      printf("  [1] 🛠️  Imposta gioco\n");
      printf("  [2] 🕹️  Gioca\n");
      printf("  [3] ❌ Termina gioco\n");
      printf("  [4] 📜 Visualizza crediti\n");
      printf(YEL "--------------------------------------" RESET "\n");
      printf("  Scelta: ");

      if (scanf("%d", &choice) != 1) {
        choice = 0;
      }

      while (getchar() != '\n');
    } while (choice < 1 || choice > 4);

    switch (choice) {
    case 1:
      system("clear");
      impostaGioco();
      break;
    case 2:
      system("clear");
      printf("gioca...\n");
      break;
    case 3:
      system("clear");
      printf("Chiusura in corso");
      fflush(stdout);

      for (int i = 0; i < 3; i++) {
        clock_t stop = clock() + (CLOCKS_PER_SEC / 2);
        while (clock() < stop) {
        }

        printf(".");
        fflush(stdout);
      }

      printf("\nArrivederci!\n");
      break;
    case 4:
      system("clear");
      printCredits();
      break;
    }

  } while (choice != 3);

  return 0;
}