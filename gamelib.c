#include "gamelib.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Giocatore *players[4] = {NULL, NULL, NULL, NULL};
static int undici_scelto = 0;

struct Zona_MondoReale *prima_zona_mondoreale = NULL;
struct Zona_SopraSotto *prima_zona_soprasotto = NULL;
static int mappa_pronta = 0;

const char *getNomeZona(enum TipoZona z) {
  switch (z) {
  case bosco:
    return "Bosco";
  case scuola:
    return "Scuola";
  case laboratorio:
    return "Laboratorio";
  case caverna:
    return "Caverna";
  case strada:
    return "Strada";
  case giardino:
    return "Giardino";
  case supermercato:
    return "Supermercato";
  case centrale_elettrica:
    return "Centrale Elettrica";
  case deposito_abbandonato:
    return "Deposito Abbandonato";
  case stazione_polizia:
    return "Stazione Polizia";
  default:
    return "Sconosciuto";
  }
}
const char *getNomeNemico(enum TipoNemico n) {
  switch (n) {
  case nessun_nemico:
    return "Nessun nemico";
  case billi:
    return "Billi";
  case democane:
    return "Democane";
  case demotorzone:
    return "Demotorzone";
  default:
    return "Sconosciuto";
  }
}
const char *getNomeOggetto(enum TipoOggetto o) {
  switch (o) {
  case nessun_oggetto:
    return "Nessun oggetto";
  case bicicletta:
    return "Bicicletta";
  case bussola:
    return "Bussola";
  case maglietta_fuocoinferno:
    return "Maglietta fuocoinferno";
  case schitarrata_metallica:
    return "Schitarrata metallica";
  default:
    return "Sconosciuto";
  }
}

void printCredits() {
  printf("\n" RED "==========================================" RESET "\n");
  printf(RED "=                CREDITI                 =" RESET "\n");
  printf(RED "==========================================" RESET "\n");
  printf("\n");
  printf("  " GRN "Sviluppatore:" RESET " Alessio Bragetti\n");
  printf("  " GRN "Progetto:" RESET " Stranger Terminal (C Edition)\n");
  printf("  " GRN "Versione:" RESET " 1.0.0\n");
  printf("  " GRN "Anno:" RESET " 2025\n");
  printf("\n");
  printf("  --------------------------------------\n");
  printf("  Realizzato per l'esame di:\n");
  printf("  Programmazione I / Laboratorio\n");
  printf("  --------------------------------------\n");
  printf("\n" RED "Premere INVIO per tornare al menu..." RESET "\n");

  getchar();
}

void svuota_mappa() {
  struct Zona_MondoReale *temp_mr = prima_zona_mondoreale;
  while (temp_mr != NULL) {
    struct Zona_MondoReale *prossimo_mr = temp_mr->avanti;
    // Liberiamo anche la zona speculare nel soprasotto tramite il link
    if (temp_mr->link_soprasotto != NULL) {
      free(temp_mr->link_soprasotto);
    }
    free(temp_mr);
    temp_mr = prossimo_mr;
  }
  prima_zona_mondoreale = NULL;
  prima_zona_soprasotto = NULL;
  mappa_pronta = 0;
}

void genera_mappa() {
  svuota_mappa();

  struct Zona_MondoReale *ultima_mondoreale = NULL;
  struct Zona_SopraSotto *ultima_soprasotto = NULL;
  int demotorzone_inserito = 0;
  int stanza_demotorzone = rand() % 15;

  for (int i = 0; i < 15; i++) {
    struct Zona_MondoReale *nuova_mondoreale =
        (struct Zona_MondoReale *)malloc(sizeof(struct Zona_MondoReale));
    struct Zona_SopraSotto *nuova_soprasotto =
        (struct Zona_SopraSotto *)malloc(sizeof(struct Zona_SopraSotto));

    nuova_mondoreale->tipo = rand() % 10;
    nuova_soprasotto->tipo = nuova_mondoreale->tipo;

    int r_nemico = rand() % 10;
    if (r_nemico < 4)
      nuova_mondoreale->nemico = nessun_nemico;
    else if (r_nemico < 8)
      nuova_mondoreale->nemico = democane;
    else
      nuova_mondoreale->nemico = billi;

    if (i == stanza_demotorzone && !demotorzone_inserito) {
      nuova_soprasotto->nemico = demotorzone;
    } else {
      nuova_soprasotto->nemico = (rand() % 2 == 0) ? nessun_nemico : democane;
    }

    int r_oggetto = rand() % 10;
    if (r_oggetto < 2) {
      nuova_mondoreale->oggetto = bicicletta; // 20% probabilità
    } else if (r_oggetto < 4) {
      nuova_mondoreale->oggetto = bussola; // 20% probabilità
    } else if (r_oggetto < 6) {
      nuova_mondoreale->oggetto = maglietta_fuocoinferno; // 20% probabilità
    } else if (r_oggetto < 7) {
      nuova_mondoreale->oggetto = schitarrata_metallica; // 10% probabilità
    } else {
      nuova_mondoreale->oggetto = nessun_oggetto; // 30% probabilità
    }

    nuova_mondoreale->link_soprasotto = nuova_soprasotto;
    nuova_soprasotto->link_soprasotto = nuova_mondoreale;
    nuova_mondoreale->avanti = NULL;
    nuova_soprasotto->avanti = NULL;

    if (prima_zona_mondoreale == NULL) {
      prima_zona_mondoreale = nuova_mondoreale;
      prima_zona_soprasotto = nuova_soprasotto;
      nuova_mondoreale->indietro = NULL;
      nuova_soprasotto->indietro = NULL;
    } else {
      ultima_mondoreale->avanti = nuova_mondoreale;
      nuova_mondoreale->indietro = ultima_mondoreale;
      ultima_soprasotto->avanti = nuova_soprasotto;
      nuova_soprasotto->indietro = ultima_soprasotto;
    }
    ultima_mondoreale = nuova_mondoreale;
    ultima_soprasotto = nuova_soprasotto;
  }
  mappa_pronta = 1;
  printf(GRN "Mappa di 15 zone generata correttamente!" RESET "\n");
}

void stampa_mappa() {
  int scelta;
  printf(
      "\nQuale mappa vuoi stampare?\n1) Mondo Reale\n2) Soprasotto\nScelta: ");
  scanf("%d", &scelta);
  while (getchar() != '\n')
    ;

  if (scelta == 1) {
    struct Zona_MondoReale *curr = prima_zona_mondoreale;
    int i = 0;
    printf(RED "\n--- MAPPA MONDO REALE ---" RESET "\n");
    while (curr != NULL) {
      printf("[%d] Tipo: %s | Nemico: %s | Oggetto: %s\n", i++,
             getNomeZona(curr->tipo), getNomeNemico(curr->nemico),
             getNomeOggetto(curr->oggetto));
      curr = curr->avanti;
    }
  } else {
    struct Zona_SopraSotto *curr = prima_zona_soprasotto;
    int i = 0;
    printf(RED "\n--- MAPPA SOPRASOTTO ---" RESET "\n");
    while (curr != NULL) {
      printf("[%d] Tipo: %s | Nemico: %s\n", i++, getNomeZona(curr->tipo),
             getNomeNemico(curr->nemico));
      curr = curr->avanti;
    }
  }
}

void menu_mappa() {
  int scelta_mappa;
  do {
    printf(RED "\n--- GESTIONE MAPPA ---\n" RESET);
    printf("1) Genera Mappa (15 zone)\n");
    printf("2) Inserisci zona\n");
    printf("3) Cancella zona\n");
    printf("4) Stampa mappa\n");
    printf("5) Stampa singola zona\n");
    printf("6) Chiudi mappa ed esci\n");
    printf("Scelta: ");
    scanf("%d", &scelta_mappa);
    while (getchar() != '\n')
      ;

    switch (scelta_mappa) {
    case 1:
      genera_mappa();
      break;
    case 4:
      stampa_mappa();
      break;
    case 6: {
      int contatore = 0;
      int ha_demotorzone = 0;
      struct Zona_SopraSotto *temp = prima_zona_soprasotto;

      // Conta le zone e verifica la presenza del Demotorzone
      while (temp != NULL) {
        contatore++;
        if (temp->nemico == demotorzone) {
          ha_demotorzone++;
        }
        temp = temp->avanti;
      }

      if (contatore >= 15 && ha_demotorzone == 1) {
        mappa_pronta = 1; // Imposta la variabile statica richiesta
        printf(GRN "Mappa validata e chiusa con successo!" RESET "\n");
      } else {
        printf(RED "Errore: la mappa deve avere almeno 15 zone e UN "
                   "demotorzone (attualmente: %d zone, %d demotorzone)." RESET
                   "\n",
               contatore, ha_demotorzone);
        scelta_mappa = 0; // RESETTA scelta_mappa per non uscire dal ciclo
      }
      break;
    }
    }
  } while (scelta_mappa != 6 && mappa_pronta == 1);
}

void impostaGioco() {
  int nPlayers = 0;

  // Pulizia memoria se il gioco viene reimpostato
  for (int i = 0; i < 4; i++) {
    if (players[i] != NULL) {
      free(players[i]);
      players[i] = NULL;
    }
  }

  do {
    printf("Inserire il numero dei giocatori (1-4): ");
    if (scanf("%d", &nPlayers) != 1) {
      while (getchar() != '\n')
        ;
      continue;
    }
  } while (nPlayers < 1 || nPlayers > 4);

  while (getchar() != '\n')
    ;

  for (int i = 0; i < nPlayers; i++) {
    // ALLOCAZIONE DINAMICA: prenota lo spazio per la struct
    players[i] = (struct Giocatore *)malloc(sizeof(struct Giocatore));

    char nome_temp[26];
    int nome_valido;

    do {
      nome_valido = 1;
      printf("\n" RED "GIOCATORE %d" RESET " - Inserire il nome: ", i + 1);
      fgets(nome_temp, sizeof(nome_temp), stdin);
      nome_temp[strcspn(nome_temp, "\n")] = 0;

      if (strlen(nome_temp) == 0) {
        nome_valido = 0;
        continue;
      }
      for (int j = 0; j < (int)strlen(nome_temp); j++) {
        if (!isalpha(nome_temp[j]) && !isspace(nome_temp[j])) {
          nome_valido = 0;
          break;
        }
      }
    } while (!nome_valido);

    strcpy(players[i]->nome, nome_temp);

    // Lancio dadi da 20 per le abilità
    players[i]->attacco_psichico = (rand() % 20) + 1;
    players[i]->difesa_psichica = (rand() % 20) + 1;
    players[i]->fortuna = (rand() % 20) + 1;

    printf(GRN "Statistiche: ATK:%d, DEF:%d, LUCK:%d\n" RESET,
           players[i]->attacco_psichico, players[i]->difesa_psichica,
           players[i]->fortuna);

    // Scelta del profilo/bonus
    int scelta;
    printf("\nScegli un bonus:\n");
    printf("1) +3 ATK / -3 DEF\n");
    printf("2) -3 ATK / +3 DEF\n");
    if (!undici_scelto)
      printf("3) Diventa UndiciVirgolaCinque (+4 ATK/DEF, -7 LUCK)\n");
    printf("Scelta: ");
    scanf("%d", &scelta);
    while (getchar() != '\n')
      ;

    if (scelta == 1) {
      players[i]->attacco_psichico += 3;
      players[i]->difesa_psichica -= 3;
    } else if (scelta == 2) {
      players[i]->attacco_psichico -= 3;
      players[i]->difesa_psichica += 3;
    } else if (scelta == 3 && !undici_scelto) {
      players[i]->attacco_psichico += 4;
      players[i]->difesa_psichica += 4;
      players[i]->fortuna -= 7;
      strcpy(players[i]->nome, "UndiciVirgolaCinque");
      undici_scelto = 1;
    }

    // Inizializzazione parametri iniziali
    players[i]->mondo = 0;
    for (int k = 0; k < 3; k++)
      players[i]->zaino[k] = nessun_oggetto;
  }

  menu_mappa();

  printf("\n" GRN "Configurazione completata!" RESET "\n");
  printf("Premere INVIO per continuare...");
  getchar();
}
