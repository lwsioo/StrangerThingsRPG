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
      printf(RED "              COSE STRANE             " RESET "\n");
      printf(RED "======================================" RESET "\n");
      printf("  [1] > IMPOSTA GIOCO\n");
      printf("  [2] > GIOCA\n");
      printf("  [3] > TERMINA GIOCO\n");
      printf("  [4] > VISUALIZZA CREDITI\n");
      printf("  [5] > REGOLE\n");
      printf(YEL "--------------------------------------" RESET "\n");
      printf("  Scelta: ");

      if (scanf("%d", &choice) != 1) {
        choice = 0;
      }

      while (getchar() != '\n'); // pulizia buffer
    } while (choice < 1 || choice > 5);

    switch (choice) {
    case 1:
      system("clear");
      impostaGioco();
      break;
    case 2:
      system("clear");
      printf("Avvio sessione di gioco");
      fflush(stdout);

      for (int i = 0; i < 3; i++) {
        clock_t stop = clock() + (CLOCKS_PER_SEC / 2);
        while (clock() < stop) {
          // attesa
        }
        printf(".");
        fflush(stdout);
      }
      gioca();
      break;
    case 3:
      system("clear");
      printf("Chiusura in corso");
      fflush(stdout);

      for (int i = 0; i < 3; i++) {
        system("clear");
        pulisci_risorse();
        clock_t stop = clock() + (CLOCKS_PER_SEC / 2);
        while (clock() < stop) {
          // attesa
        }
        printf(".");
        fflush(stdout);
      }
      break;
    case 4:
      system("clear");
      printCredits();
      break;
    case 5:
      system("clear");
      printRules();
      break;
    }
  } while (choice != 3);

  return 0;
}