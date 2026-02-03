#include "gamelib.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Giocatore *players[4] = {NULL, NULL, NULL, NULL};

static int usa_bici = 0;

// GESTIONE VINCITORI
static char ultimi_vincitori[3][26] = {"Nessuno", "Nessuno", "Nessuno"};
static int indice_vincitore = 0;

// shuffle dei giocatori
static void shuffle_indices(int *array, int size) {
  for (int i = size - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    int temp = array[i];
    array[i] = array[j];
    array[j] = temp;
  }
}

// GESTIONE MAPPA
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
char getSimboloOggetto(enum TipoOggetto o) {
  switch (o) {
  case bicicletta:
    return 'b';
  case maglietta_fuocoinferno:
    return 'm';
  case bussola:
    return 'o';
  case schitarrata_metallica:
    return 'j';
  default:
    return ' '; // Nessun oggetto
  }
}
char getSimboloNemico(enum TipoNemico n) {
  switch (n) {
  case billi:
    return 'W';
  case democane:
    return 'C';
  case demotorzone:
    return 'D';
  default:
    return ' '; // Nessun nemico
  }
}

void svuota_mappa() {
  struct Zona_MondoReale *temp_mr = prima_zona_mondoreale;
  while (temp_mr != NULL) {
    struct Zona_MondoReale *prossimo_mr = temp_mr->avanti;
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

    int r_nemico = rand() % 20;
    if (r_nemico < 10)
      nuova_mondoreale->nemico = nessun_nemico; // 50% probabilità
    else if (r_nemico < 17)
      nuova_mondoreale->nemico = democane; // 35% probabilità
    else
      nuova_mondoreale->nemico = billi; // 15% probabilità

    if (i == stanza_demotorzone && !demotorzone_inserito) {
      nuova_soprasotto->nemico = demotorzone;
    } else {
      nuova_soprasotto->nemico = (rand() % 3 == 0) ? democane : nessun_nemico;
    }

    int r_oggetto = rand() % 20;
    if (r_oggetto < 3) {
      nuova_mondoreale->oggetto = bicicletta; // 15% probabilità
    } else if (r_oggetto < 8) {
      nuova_mondoreale->oggetto = bussola; // 25% probabilità
    } else if (r_oggetto < 13) {
      nuova_mondoreale->oggetto = maglietta_fuocoinferno; // 25% probabilità
    } else if (r_oggetto < 16) {
      nuova_mondoreale->oggetto = schitarrata_metallica; // 15% probabilità
    } else {
      nuova_mondoreale->oggetto = nessun_oggetto; // 20% probabilità
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
  printf("\nQuale mappa vuoi stampare?\n1) Mondo Reale\n2) Soprasotto\nScelta: ");
  scanf("%d", &scelta);
  while (getchar() != '\n');

  if (scelta == 1) {
    struct Zona_MondoReale *curr = prima_zona_mondoreale;
    int i = 0;
    printf(RED "\n--- MAPPA MONDO REALE ---" RESET "\n");
    while (curr != NULL) {
      printf("[%d] Tipo: %s | Nemico: %s | Oggetto: %s\n", i++, getNomeZona(curr->tipo), getNomeNemico(curr->nemico), getNomeOggetto(curr->oggetto));
      curr = curr->avanti;
    }
  } else {
    struct Zona_SopraSotto *curr = prima_zona_soprasotto;
    int i = 0;
    printf(RED "\n--- MAPPA SOPRASOTTO ---" RESET "\n");
    while (curr != NULL) {
      printf("[%d] Tipo: %s | Nemico: %s\n", i++, getNomeZona(curr->tipo), getNomeNemico(curr->nemico));
      curr = curr->avanti;
    }
  }
}

void stampa_mappa_grafica() {
  if (prima_zona_mondoreale == NULL) {
    printf(RED "Errore: Mappa non generata!" RESET "\n");
    return;
  }

  struct Zona_MondoReale *blocco_mr = prima_zona_mondoreale;
  struct Zona_SopraSotto *blocco_ss = prima_zona_soprasotto;
  int indice_inizio = 0;

  while (blocco_mr != NULL) {
    struct Zona_MondoReale *temp_mr = blocco_mr;
    struct Zona_SopraSotto *temp_ss = blocco_ss;

    printf(RED "\n--- BLOCCO ZONE %d - %d ---" RESET "\n", indice_inizio, indice_inizio + 4);

    // tetto della stanza
    for (int j = 0; j < 5 && temp_mr != NULL; j++) {
      printf("  ___   ");
      temp_mr = temp_mr->avanti;
    }
    printf("\n");

    // pareti e contenuto
    temp_mr = blocco_mr;
    for (int j = 0; j < 5 && temp_mr != NULL; j++) {
      printf(" |" RED "%c" RESET " " YEL "%c" RESET "|  ", getSimboloNemico(temp_mr->nemico), getSimboloOggetto(temp_mr->oggetto));
      temp_mr = temp_mr->avanti;
    }
    printf(" (MR)\n");

    // pavimento e pareti di fondo
    temp_mr = blocco_mr;
    for (int j = 0; j < 5 && temp_mr != NULL; j++) {
      printf(" |___|  ");
      temp_mr = temp_mr->avanti;
    }
    printf("\n");

    // collegamento tra dimensioni
    temp_mr = blocco_mr;
    for (int j = 0; j < 5 && temp_mr != NULL; j++) {
      printf("   V    ");
      temp_mr = temp_mr->avanti;
    }
    printf("\n");

    // tetto rosso SS
    temp_ss = blocco_ss;
    for (int j = 0; j < 5 && temp_ss != NULL; j++) {
      printf(RED "  ___   " RESET);
      temp_ss = temp_ss->avanti;
    }
    printf("\n");

    // contenuto SS (solo nemico)
    temp_ss = blocco_ss;
    for (int j = 0; j < 5 && temp_ss != NULL; j++) {
      printf(RED " |" RESET RED "%c" RESET "  " RED "|  " RESET, getSimboloNemico(temp_ss->nemico));
      temp_ss = temp_ss->avanti;
    }
    printf(RED " (SS)\n" RESET);

    // pavimento e pareti di fondo SS
    temp_ss = blocco_ss;
    for (int j = 0; j < 5 && temp_ss != NULL; j++) {
      printf(RED " |___|  " RESET);
      temp_ss = temp_ss->avanti;
    }
    printf("\n\n");

    // avanzamento al blocco successivo (5 zone avanti)
    for (int j = 0; j < 5 && blocco_mr != NULL; j++) {
      blocco_mr = blocco_mr->avanti;
      blocco_ss = blocco_ss->avanti;
      indice_inizio++;
    }
  }

  printf(YEL "OGGETTI: " RESET "b:Bici, m:Maglia, o:Bussola, j:Chitarra\n");
  printf(RED "NEMICI:  " RESET "W:Billi, C:Democane, D:Demotorzone\n");
  printf(GRN "\nPremere INVIO per continuare...");
  getchar();
}

void stampa_zona() {
  int pos, attuale = 0;

  printf("Inserisci la posizione della zona da stampare: ");
  if (scanf("%d", &pos) != 1 || pos < 0) {
    printf(RED "Errore: inserire un numero valido." RESET "\n");
    while (getchar() != '\n');
    return;
  }
  while (getchar() != '\n');

  // puntatore del MR come base per la ricerca
  struct Zona_MondoReale *curr = prima_zona_mondoreale;

  // scorrere la lista fino alla pos i
  while (curr != NULL && attuale < pos) {
    curr = curr->avanti;
    attuale++;
  }

  // se curr è NULL la pos i non esiste nella mappa
  if (curr == NULL) {
    printf(RED "Errore: la posizione %d non esiste nella mappa attuale." RESET "\n", pos);
    return;
  }

  // stampa dei dati della zona nel MR
  printf(RED "\n--- INFO ZONA %d ---" RESET "\n", pos);
  printf(YEL "MONDO REALE:" RESET "\n");
  printf("  Tipo: %s\n  Nemico: %s\n  Oggetto: %s\n", getNomeZona(curr->tipo), getNomeNemico(curr->nemico), getNomeOggetto(curr->oggetto));

  // stampa corrispondente SS
  struct Zona_SopraSotto *speculare = curr->link_soprasotto;
  printf(RED "\nSOPRASOTTO (Speculare):" RESET "\n");
  printf("  Tipo: %s\n  Nemico: %s\n", getNomeZona(speculare->tipo), getNomeNemico(speculare->nemico));

  printf("\nPremere INVIO per continuare...");
  getchar();
}

void inserisci_zona() {
  int pos, attuale = 0;
  printf("In quale posizione vuoi inserire la nuova zona? (0 per la testa): ");
  if (scanf("%d", &pos) != 1 || pos < 0) {
    printf(RED "Errore: posizione non valida.\n" RESET);
    while (getchar() != '\n');
    return;
  }
  while (getchar() != '\n');

  struct Zona_MondoReale *nuova_mr =(struct Zona_MondoReale *)malloc(sizeof(struct Zona_MondoReale));
  struct Zona_SopraSotto *nuova_ss =(struct Zona_SopraSotto *)malloc(sizeof(struct Zona_SopraSotto));

  printf(RED "\n--- CONFIGURAZIONE NUOVA ZONA ---\n" RESET);

  // tipo zona
  printf(YEL "Scegli il TIPO della zona:\n" RESET);
  printf("0) Bosco\t1) Scuola\t2) Laboratorio\n3) Caverna\t4) Strada\t5) Giardino\n6) Supermercato\t7) Centrale Elettrica\t8) Deposito Abbandonato\t9) Stazione di Polizia\n");
  printf("Scelta tipo: ");
  int t;
  scanf("%d", &t);
  nuova_mr->tipo = (enum TipoZona)(t % 10);
  nuova_ss->tipo = nuova_mr->tipo;

  // nemico mondo reale
  printf(YEL "\nScegli il NEMICO (Mondo Reale):\n" RESET);
  printf("0) Nessuno\t1) Billi\t2) Democane\n");
  printf("Scelta nemico MR: ");
  int n_mr;
  scanf("%d", &n_mr);
  nuova_mr->nemico = (enum TipoNemico)(n_mr % 3);

  // nemico SS con controllo demotorzone
  int demotorzone_presente = 0;
  struct Zona_SopraSotto *temp_check = prima_zona_soprasotto;
  while (temp_check != NULL) {
    if (temp_check->nemico == demotorzone) {
      demotorzone_presente = 1;
      break;
    }
    temp_check = temp_check->avanti;
  }

  printf(YEL "\nScegli il NEMICO (Soprasotto):\n" RESET);
  printf("0) Nessuno\t2) Democane");
  if (!demotorzone_presente) {
    printf("\t3) Demotorzone");
  } else {
    printf(RED "\t[3) Demotorzone già presente]" RESET);
  }
  printf("\nScelta nemico SS: ");
  int n_ss;
  scanf("%d", &n_ss);

  if (n_ss == 3 && demotorzone_presente) {
    printf(RED "Azione non permessa. Impostato Nessun Nemico.\n" RESET);
    nuova_ss->nemico = nessun_nemico;
  } else {
    nuova_ss->nemico = (enum TipoNemico)n_ss;
  }

  // oggetto per il MR
  printf(YEL "\nScegli l'OGGETTO (Mondo Reale):\n" RESET);
  printf("0) Nessuno\t1) Bicicletta\t2) Maglietta\n3) Bussola\t4) Chitarra\n");
  printf("Scelta oggetto: ");
  int o;
  scanf("%d", &o);
  nuova_mr->oggetto = (enum TipoOggetto)(o % 5);
  while (getchar() != '\n');

  nuova_mr->link_soprasotto = nuova_ss;
  nuova_ss->link_soprasotto = nuova_mr;

  // inserimento in lista doppiamente collegata
  if (pos == 0 || prima_zona_mondoreale == NULL) {
    nuova_mr->avanti = prima_zona_mondoreale;
    nuova_mr->indietro = NULL;
    nuova_ss->avanti = prima_zona_soprasotto;
    nuova_ss->indietro = NULL;
    if (prima_zona_mondoreale != NULL) {
      prima_zona_mondoreale->indietro = nuova_mr;
      prima_zona_soprasotto->indietro = nuova_ss;
    }
    prima_zona_mondoreale = nuova_mr;
    prima_zona_soprasotto = nuova_ss;
  } else {
    struct Zona_MondoReale *prec_mr = prima_zona_mondoreale;
    struct Zona_SopraSotto *prec_ss = prima_zona_soprasotto;
    while (prec_mr->avanti != NULL && attuale < pos - 1) {
      prec_mr = prec_mr->avanti;
      prec_ss = prec_ss->avanti;
      attuale++;
    }
    nuova_mr->avanti = prec_mr->avanti;
    nuova_mr->indietro = prec_mr;
    nuova_ss->avanti = prec_ss->avanti;
    nuova_ss->indietro = prec_ss;
    if (prec_mr->avanti != NULL) {
      prec_mr->avanti->indietro = nuova_mr;
      prec_ss->avanti->indietro = nuova_ss;
    }
    prec_mr->avanti = nuova_mr;
    prec_ss->avanti = nuova_ss;
  }

  mappa_pronta = 0;
  printf(GRN "\nZona inserita con successo! Ricorda di chiudere la mappa (opzione 7) per salvare.\n" RESET);
}

void cancella_zona() {
  // controllo se la mappa esiste
  if (prima_zona_mondoreale == NULL) {
    printf(RED"Errore: La mappa è vuota, non c'è nulla da cancellare.\n" RESET);
    return;
  }

  int pos, attuale = 0;
  printf("Inserisci la posizione della zona da cancellare (0, 1, 2...): ");
  if (scanf("%d", &pos) != 1 || pos < 0) {
    printf(RED "Errore: Inserimento non valido.\n" RESET);
    while (getchar() != '\n');
    return;
  }
  while (getchar() != '\n');

  // ricerca del nodo in pos
  struct Zona_MondoReale *curr_mr = prima_zona_mondoreale;
  while (curr_mr != NULL && attuale < pos) {
    curr_mr = curr_mr->avanti;
    attuale++;
  }

  // verifica se esiste la posizione
  if (curr_mr == NULL) {
    printf(RED "Errore: La posizione %d non esiste nella mappa.\n" RESET, pos);
    return;
  }

  // scollegamento nel MR
  if (curr_mr->indietro != NULL) {
    curr_mr->indietro->avanti = curr_mr->avanti;
  } else {
    prima_zona_mondoreale = curr_mr->avanti;
  }

  if (curr_mr->avanti != NULL) {
    curr_mr->avanti->indietro = curr_mr->indietro;
  }

  // scollegamento nel SS
  struct Zona_SopraSotto *curr_ss = curr_mr->link_soprasotto;

  if (curr_ss->indietro != NULL) {
    curr_ss->indietro->avanti = curr_ss->avanti;
  } else {
    prima_zona_soprasotto = curr_ss->avanti;
  }

  if (curr_ss->avanti != NULL) {
    curr_ss->avanti->indietro = curr_ss->indietro;
  }

  // pulizia memoria
  free(curr_ss);
  free(curr_mr);

  // reset dello stato mappa che deve essere rivalidata
  mappa_pronta = 0;
  printf(GRN "Zona in posizione %d e la sua speculare rimosse con successo.\n" RESET, pos);
}

void menu_mappa() {
  int scelta_mappa;
  do {
    printf(RED "\n--- GESTIONE MAPPA ---\n" RESET);
    printf("1) Genera Mappa (15 zone)\n");
    printf("2) Inserisci zona\n");
    printf("3) Cancella zona\n");
    printf("4) Stampa mappa\n");
    printf("5) Stampa mappa grafica\n");
    printf("6) Stampa singola zona\n");
    printf("7) Chiudi mappa ed esci\n");
    printf("Scelta: ");
    scanf("%d", &scelta_mappa);
    while (getchar() != '\n');

    switch (scelta_mappa) {
    case 1:
      genera_mappa();
      break;
    case 2:
      inserisci_zona();
      break;
    case 3:
      cancella_zona();
      break;
    case 4:
      stampa_mappa();
      break;
    case 5:
      stampa_mappa_grafica();
      break;
    case 6:
      stampa_zona();
      break;
    case 7: {
      int contatore = 0;
      int ha_demotorzone = 0;
      struct Zona_SopraSotto *temp = prima_zona_soprasotto;

      // conta le zone e verifica la presenza del Demotorzone
      while (temp != NULL) {
        contatore++;
        if (temp->nemico == demotorzone) {
          ha_demotorzone++;
        }
        temp = temp->avanti;
      }

      if (contatore >= 15 && ha_demotorzone == 1) {
        mappa_pronta = 1; // stato mappa pronta
        printf(GRN "Mappa validata e chiusa con successo!" RESET "\n");
      } else {
        printf(RED "Errore: la mappa deve avere almeno 15 zone e UN demotorzone (attualmente: %d zone, %d demotorzone)." RESET "\n", contatore, ha_demotorzone);
        scelta_mappa = 0; // RESETTA scelta_mappa per non uscire dal ciclo
      }
      break;
    }
    default:
      printf(RED "Numero non valido\n");
    }
  } while (scelta_mappa != 7);
}

void impostaGioco() {
  int nPlayers = 0;
  svuota_mappa();

  // pulizia memoria se il gioco viene impostato più volte
  for (int i = 0; i < 4; i++) {
    if (players[i] != NULL) {
      free(players[i]);
      players[i] = NULL;
    }
  }

  do {
    printf("Inserire il numero dei giocatori (1-4): ");
    if (scanf("%d", &nPlayers) != 1) {
      while (getchar() != '\n');
      continue;
    }
  } while (nPlayers < 1 || nPlayers > 4);

  while (getchar() != '\n');

  for (int i = 0; i < nPlayers; i++) {
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

    // il minimo è impostato in base al malus massimo possibile per ogni skill
    // ATK e DEF possono subire -3, quindi minimo 8 (8-3 = 5)
    // LUCK può subire -7, quindi minimo 8 (8-7 = 1)
    players[i]->attacco_psichico = (rand() % 17) + 4; // range 4-20
    players[i]->difesa_psichica = (rand() % 17) + 8;  // range 8-24 (aumentato per migliorare giocabilità)
    players[i]->fortuna = (rand() % 13) + 8;          // range 8-20

    printf(GRN "Statistiche base: ATK:%d, DEF:%d, LUCK:%d\n" RESET, 
           players[i]->attacco_psichico, players[i]->difesa_psichica, players[i]->fortuna);

    int scelta;
    int scelta_valida = 0;
    do {
      printf("\nScegli un bonus:\n");
      printf("1) +3 ATK / -3 DEF\n");
      printf("2) -3 ATK / +3 DEF\n");
      printf("3) -3 ATK/DEF +4 LUCK\n");
      if (!undici_scelto)
        printf("4) Diventa UndiciVirgolaCinque (+4 ATK/DEF, -7 LUCK)\n");
      
      printf("Scelta: ");
      if (scanf("%d", &scelta) != 1) {
        printf(RED "Errore: Inserire un numero.\n" RESET);
        while (getchar() != '\n');
        continue;
      }
      while (getchar() != '\n');

      if (scelta >= 1 && scelta <= 3) {
        scelta_valida = 1;
      } else if (scelta == 4 && !undici_scelto) {
        scelta_valida = 1;
      } else {
        printf(RED "Scelta non valida! Riprova.\n" RESET);
      }
    } while (!scelta_valida);

    // applicazione bonus con controllo underflow
    switch (scelta) {
      case 1:
        players[i]->attacco_psichico += 3;
        players[i]->difesa_psichica = (players[i]->difesa_psichica > 3) ? players[i]->difesa_psichica - 3 : 0;
        break;
      case 2:
        players[i]->attacco_psichico = (players[i]->attacco_psichico > 3) ? players[i]->attacco_psichico - 3 : 0;
        players[i]->difesa_psichica += 3;
        break;
      case 3:
        players[i]->attacco_psichico = (players[i]->attacco_psichico > 3) ? players[i]->attacco_psichico - 3 : 0;
        players[i]->difesa_psichica = (players[i]->difesa_psichica > 3) ? players[i]->difesa_psichica - 3 : 0;
        players[i]->fortuna += 4;
        break;
      case 4:
        if (!undici_scelto) {
          players[i]->attacco_psichico += 4;
          players[i]->difesa_psichica += 4;
          players[i]->fortuna = (players[i]->fortuna > 7) ? players[i]->fortuna - 7 : 0;
          strcpy(players[i]->nome, "UndiciVirgolaCinque");
          undici_scelto = 1;
        }
        break;
    }

    printf(YEL "Statistiche finali: ATK:%d, DEF:%d, LUCK:%d\n" RESET, 
           players[i]->attacco_psichico, players[i]->difesa_psichica, players[i]->fortuna);

    players[i]->mondo = 0;
    for (int k = 0; k < 3; k++) players[i]->zaino[k] = nessun_oggetto;
    
    printf("\nPremere INVIO per passare al prossimo giocatore...");
    getchar();
    system("clear");
  }

  menu_mappa();
}

// STAMPA CREDITI
void printCredits() {
  printf("\n" RED "==========================================" RESET "\n");
  printf(RED "=                CREDITI                 =" RESET "\n");
  printf(RED "==========================================" RESET "\n");
  printf("\n  " GRN "Sviluppatore:" RESET " Alessio Bragetti\n");
  printf("  " GRN "Progetto:" RESET " Cose Strane\n");
  printf("  " GRN "Anno:" RESET " 2025\n");

  printf("\n" YEL "=== ULTIMI 3 VINCITORI (Hall of Fame) ===" RESET "\n");
  for (int i = 0; i < 3; i++) {
      printf("  %d. %s\n", i + 1, ultimi_vincitori[i]);
  }

  printf("\n" GRN "Premere INVIO per tornare al menu..." RESET "\n");
  getchar();
}

// STAMPA REGOLE
void printRules() {
  system("clear");
  printf(RED "========================================================\n" RESET);
  printf(RED "               REGOLAMENTO UFFICIALE: COSE STRANE       \n" RESET);
  printf(RED "========================================================\n" RESET);

  printf(YEL "\n1. CREAZIONE PERSONAGGIO" RESET "\n");
  printf("   - Le statistiche (ATK, DEF, LUCK) sono generate con un D20 (tranne la difesa che arriva massimo a 24 per aumentare la giocabilità).\n");
  printf("   - La tua " GRN "DIFESA" RESET " rappresenta anche i tuoi " GRN "Punti Vita (HP)" RESET ".\n");
  printf("   - I Bonus permettono di specializzare il personaggio in base allo stile.\n");

  printf(YEL "\n2. ESPLORAZIONE E VINCOLI" RESET "\n");
  printf("   - Puoi effettuare " YEL "un solo spostamento" RESET " per turno (Avanza/Indietro/Varco).\n");
  printf("   - " RED "BLOCCO NEMICO:" RESET " Se un nemico è vivo nella tua zona, non puoi\n");
  printf("     raccogliere oggetti o muoverti. Devi combattere o usare la Bici.\n");

  printf(YEL "\n3. SISTEMA DI COMBATTIMENTO" RESET "\n");
  printf("   - " YEL "Attacco:" RESET " Lanci un D20; colpisci se il risultato è < della tua LUCK.\n");
  printf("   - " RED "Danni Nemici:" RESET " Il danno subito viene sottratto dalla tua Difesa.\n");
  printf("   - " GRN "RECUPERO:" RESET " Al termine di ogni scontro vinto, la tua Difesa (HP)\n");
  printf("     viene " GRN "completamente ripristinata" RESET " al valore originale.\n");
  printf("   - Se i tuoi HP arrivano a 0 durante lo scontro, sarai eliminato.\n");

  printf(YEL "\n4. EQUIPAGGIAMENTO (ZAINO MAX 3 SLOT)" RESET "\n");
  printf("   - " GRN "BICICLETTA:" RESET " Permette di fuggire dal combattimento o muoversi due volte.\n");
  printf("   - " GRN "BUSSOLA:" RESET " Indica la distanza precisa dal Demotorzone nel Soprasotto.\n");
  printf("   - " GRN "MAGLIETTA:" RESET " Bonus massiccio di " YEL "+10 HP" RESET " temporanei in battaglia.\n");
  printf("   - " GRN "CHITARRA:" RESET " Bonus massiccio di " YEL "+10 ATK" RESET " temporaneo in battaglia.\n");

  printf(YEL "\n5. VITTORIA" RESET "\n");
  printf("   - La partita termina quando il primo giocatore sconfigge il\n");
  printf("     temibile " RED "Demotorzone (45 HP / 12 Forza)" RESET " nel Soprasotto.\n");

  printf(RED "\n========================================================\n" RESET);
  printf(GRN "Premere INVIO per tornare al menu principale...");
  
  // pulizia buffer e attesa
  getchar();
}

// GESTIONE GIOCO
void stampa_cella_attuale(struct Giocatore *p) {
  char nem = ' ', ogg = ' ';

  if (p->mondo == reale) {
    struct Zona_MondoReale *z = (struct Zona_MondoReale *)p->posizione;
    nem = getSimboloNemico(z->nemico);
    ogg = getSimboloOggetto(z->oggetto);
    printf("\nTi trovi in: " GRN "%s (Mondo Reale)" RESET "\n", getNomeZona(z->tipo));

    // stampa grafica MR
    printf("       _______\n");
    printf("      | " RED "%c" RESET "   " YEL "%c" RESET " |\n", nem, ogg);
    printf("      |_______|\n\n");
  } else {
    struct Zona_SopraSotto *z = (struct Zona_SopraSotto *)p->posizione;
    nem = getSimboloNemico(z->nemico);
    printf("\nTi trovi in: " RED "%s (Soprasotto)" RESET "\n",
           getNomeZona(z->tipo));

    // stampa grafica SS
    printf(RED "       _______\n" RESET);
    printf(RED "      |" RESET " " RED "%c" RESET "     " RED "|\n" RESET, nem);
    printf(RED "      |_______|\n\n" RESET);
  }
}

int nemico_vivo(struct Giocatore *p) {
  if (p->mondo == reale) {
    return (((struct Zona_MondoReale *)p->posizione)->nemico != nessun_nemico);
  } else {
    return (((struct Zona_SopraSotto *)p->posizione)->nemico != nessun_nemico);
  }
}

void raccogli_oggetto(struct Giocatore *p) {
  if (p->mondo == soprasotto) {
    printf(RED "Non ci sono oggetti nel Soprasotto!\n" RESET);
    return;
  }

  if (nemico_vivo(p)) {
    printf(RED "Devi prima sconfiggere il nemico per raccogliere l'oggetto!\n" RESET);
    return;
  }

  struct Zona_MondoReale *z = (struct Zona_MondoReale *)p->posizione;
  if (z->oggetto == nessun_oggetto) {
    printf(YEL "Non c'e' nulla da raccogliere qui.\n" RESET);
    return;
  }

  // ricerca spazio nello zaino
  for (int i = 0; i < 3; i++) {
    if (p->zaino[i] == nessun_oggetto) {
      p->zaino[i] = z->oggetto;
      printf(GRN "Hai raccolto: %s!\n" RESET, getNomeOggetto(z->oggetto));
      z->oggetto = nessun_oggetto; // rimosso dalla mappa
      return;
    }
  }
  printf(RED "Lo zaino e' pieno!\n" RESET);
}

void avanza(struct Giocatore *p) {
  if (p->ha_mosso) {
    printf(RED "Hai gia' effettuato uno spostamento in questo turno!\n" RESET);
    return;
  }
  // Controllo bypass nemico tramite bici
  if (nemico_vivo(p) && !usa_bici) {
    printf(RED "Non puoi avanzare! C'e' un nemico che ti sbarra la strada!\n" RESET);
    return;
  }

  void *prossima = (p->mondo == reale)
          ? (void *)((struct Zona_MondoReale *)p->posizione)->avanti
          : (void *)((struct Zona_SopraSotto *)p->posizione)->avanti;
  
  if (prossima == NULL) {
    printf("Fine della mappa raggiunta!\n");
    return;
  }

  p->posizione = prossima;
  p->ha_mosso = 1;
  usa_bici = 0; // Reset dell'effetto bici
  printf(GRN "Ti sei spostato in avanti.\n" RESET);
}

void indietreggia(struct Giocatore *p) {
  if (p->ha_mosso) {
    printf(RED "Hai gia' effettuato uno spostamento!\n" RESET);
    return;
  }
  // controllo bypass nemico tramite bici
  if (nemico_vivo(p) && !usa_bici) {
    printf(RED "Il nemico non ti permette di scappare!\n" RESET);
    return;
  }

  void *precedente = (p->mondo == reale)
          ? (void *)((struct Zona_MondoReale *)p->posizione)->indietro
          : (void *)((struct Zona_SopraSotto *)p->posizione)->indietro;
  
  if (precedente == NULL) {
    printf("Sei gia' all'inizio della mappa!\n");
    return;
  }

  p->posizione = precedente;
  p->ha_mosso = 1;
  usa_bici = 0; // reset dell'effetto bici
  printf(GRN "Sei tornato alla zona precedente.\n" RESET);
}

void cambia_mondo(struct Giocatore *p) {
  if (p->ha_mosso) {
    printf(RED "Hai gia' effettuato uno spostamento!\n" RESET);
    return;
  }
  // controllo bypass nemico tramite bici
  if (nemico_vivo(p) && !usa_bici) {
    printf(RED "Il nemico ti impedisce di aprire un varco!\n" RESET);
    return;
  }

  if (p->mondo == reale) {
    p->posizione = (void *)((struct Zona_MondoReale *)p->posizione)->link_soprasotto;
    p->mondo = soprasotto;
  } else {
    int roll = (rand() % 20) + 1;
    printf(YEL "Tiri un d20 per uscire dal Soprasotto... Risultato: %d (Fortuna: %d)\n" RESET, roll, p->fortuna);
    if (roll < p->fortuna) {
      p->posizione = (void *)((struct Zona_SopraSotto *)p->posizione)->link_soprasotto;
      p->mondo = reale;
    } else {
      printf(RED "Il varco non si apre! Sei ancora intrappolato nel Soprasotto.\n" RESET);
      return;
    }
  }
  p->ha_mosso = 1;
  usa_bici = 0; // reset dell'effetto bici
  printf(RED "VARCO APERTO! Hai cambiato dimensione.\n" RESET);
}

void stampa_giocatore(struct Giocatore *p) {
  printf(YEL "\n=== STATISTICHE %s ===\n" RESET, p->nome);
  printf("Posizione: %s (%s)\n", getNomeZona((p->mondo == reale) ? ((struct Zona_MondoReale *)p->posizione)->tipo : ((struct Zona_SopraSotto *)p->posizione)->tipo), (p->mondo == reale) ? "Mondo Reale" : "Soprasotto");
  printf("ATK: %d | DEF: %d | LUCK: %d\n", p->attacco_psichico, p->difesa_psichica, p->fortuna);
  printf("ZAINO: [%s] [%s] [%s]\n", getNomeOggetto(p->zaino[0]), getNomeOggetto(p->zaino[1]), getNomeOggetto(p->zaino[2]));
  printf("---------------------------\n");
}

int sconfiggi_nemico(struct Giocatore *p) {
    enum TipoNemico tipo;
    int esito = rand() % 2 + 1;
    
    if (p->mondo == reale) {
        struct Zona_MondoReale *z = (struct Zona_MondoReale*)p->posizione;
        tipo = z->nemico;

        // vittoria
        if (tipo == demotorzone) {
            strcpy(ultimi_vincitori[indice_vincitore], p->nome);
            indice_vincitore = (indice_vincitore + 1) % 3;
            return 1; 
        }

        // calcolo prob scomparsa

        
        // feedback per il MR
        if (esito == 1) {
            z->nemico = nessun_nemico;
            printf(GRN "\nIl nemico è stato sconfitto e scompare dalla zona!" RESET "\n");
        } else {
            printf(YEL "\nHai messo in fuga il nemico, ma la sua presenza infesta ancora la zona..." RESET "\n");
            printf("Dovrai combattere ancora se vuoi liberare il passaggio.\n");
        }
    } else {
        struct Zona_SopraSotto *z = (struct Zona_SopraSotto*)p->posizione;
        tipo = z->nemico;

        if (tipo == demotorzone) {
            strcpy(ultimi_vincitori[indice_vincitore], p->nome);
            indice_vincitore = (indice_vincitore + 1) % 3;
            return 1;
        }

        // feedback per il SS
        if (esito == 1) {
            z->nemico = nessun_nemico;
            printf(GRN "\nL'ombra del nemico si dissolve nel nulla!" RESET "\n");
        } else {
            printf(YEL "\nIl nemico è ferito, ma non accenna ad andarsene da questa zona..." RESET "\n");
        }
    }
    return 0;
}

void trova_demotorzone(struct Giocatore *p) {
    struct Zona_SopraSotto *temp = prima_zona_soprasotto;
    int pos_boss = 0;
    int trovata = 0;

    while (temp != NULL) {
        if (temp->nemico == demotorzone) {
            trovata = 1;
            // nome della zona
            const char *name_pos_boss = getNomeZona(temp->tipo); 
            
            if (p->mondo == soprasotto) {
                printf(YEL "La Bussola brilla! Il Demotorzone è nella zona %d: %s.\n" RESET, pos_boss, name_pos_boss);
            } else {
                printf(YEL "La Bussola punta in basso... Il boss è nel Soprasotto, zona %d (%s).\n" RESET, pos_boss, name_pos_boss);
            }
            break;
        }
        temp = temp->avanti;
        pos_boss++;
    }

    if (!trovata) {
        printf(YEL "La bussola impazzisce... Il Demotorzone non è in questa dimensione.\n" RESET);
    }
}

int gestisci_zaino(struct Giocatore *p, int *hp_attuale, int isFighting) {
  int o_sc;
  do {
    printf("\n--- ZAINO (%s) ---\n", isFighting ? "COMBATTIMENTO" : "ESPLORAZIONE");
    for (int i = 0; i < 3; i++) {
      printf("%d) %s\n", i + 1, getNomeOggetto(p->zaino[i]));
    }
    printf("Scegli (0 per tornare): ");

    if (scanf("%d", &o_sc) != 1) {
      printf(RED "Inserire un numero valido.\n" RESET);
      while (getchar() != '\n');
      o_sc = -1;
      continue;
    }
    while (getchar() != '\n');

    if (o_sc == 0) return 0;

    if (o_sc < 1 || o_sc > 3 || p->zaino[o_sc - 1] == nessun_oggetto) {
      printf(RED "Slot vuoto o scelta non valida!\n" RESET);
      continue;
    }

    enum TipoOggetto obj = p->zaino[o_sc - 1];
        
    // logica specifica per oggetto
    switch (obj) {
      case bicicletta:
        usa_bici = 1;
        p->zaino[o_sc - 1] = nessun_oggetto;
        if (isFighting) {
          printf(GRN "Fuga riuscita! Usi la bici per seminare il nemico.\n" RESET);
          return 1; // fine scontro
        } else {
          p->ha_mosso = 0;
          printf(GRN "Hai usato la Bici! Ora puoi ignorare i nemici per un movimento.\n" RESET);
          return 0;
        }

      case maglietta_fuocoinferno:
        if (isFighting) {
          *hp_attuale += 10;
          p->zaino[o_sc - 1] = nessun_oggetto;
          printf(GRN "La maglietta ti infonde coraggio! +10 HP temporanei.\n" RESET);
          return 0;
        } else {
          printf(YEL "Meglio conservarla per il combattimento!\n" RESET);
        }
        break;

        case bussola:
          trova_demotorzone(p);
          p->zaino[o_sc - 1] = nessun_oggetto;
          break;

        case schitarrata_metallica:
          if (isFighting) {
            p->attacco_psichico += 10;
            p->zaino[o_sc - 1] = nessun_oggetto;
            printf(GRN "L'assolo aumenta il tuo potere! +10 ATK per questo scontro.\n" RESET);
            return 0;
          } else {
            printf(YEL "Senza nemici è solo rumore. Conservala!\n" RESET);
          }
          break;

        default:
         break;
    }
  } while (o_sc != 0);

  return 0;
}

void combatti(struct Giocatore *p) {
    enum TipoNemico tipo;
    if (p->mondo == reale) tipo = ((struct Zona_MondoReale*)p->posizione)->nemico;
    else tipo = ((struct Zona_SopraSotto*)p->posizione)->nemico;

    // vita e forza dei nemici per rendere giocabile
    int hp_n, forza_n;
    if (tipo == democane) { hp_n = 20; forza_n = 6; }
    else if (tipo == billi) { hp_n = 30; forza_n = 10; }
    else if (tipo == demotorzone) { hp_n = 60; forza_n = 15; }
    else return;

    // statistiche originali
    int atk_base = p->attacco_psichico;
    int def_base = p->difesa_psichica;
    int hp_p = p->difesa_psichica; // vita attuale del giocatore

    int fine_scontro = 0;
    while (hp_p > 0 && hp_n > 0 && !fine_scontro) {
        system("clear");
        printf(RED "--- COMBATTIMENTO IN CORSO ---" RESET "\n");
        stampa_cella_attuale(p);
        printf(YEL "%s" RESET " (HP: %d) VS " RED "%s" RESET " (HP: %d)\n", p->nome, hp_p, getNomeNemico(tipo), hp_n);
        
        printf("\n1) Attacca\n2) Apri Zaino\nScelta: ");
        int c_scelta;
        if (scanf("%d", &c_scelta) != 1) {
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        if (c_scelta == 1) {
            // turno player
            if ((rand() % 20) + 1 < p->fortuna) {
                printf(GRN "COLPITO! Infliggi %d danni!\n" RESET, p->attacco_psichico);
                hp_n -= p->attacco_psichico;
            } else {
                printf(RED "MANCATO!\n" RESET);
            }

            if (hp_n <= 0) {
              int boss_ucciso = sconfiggi_nemico(p); 
              if (boss_ucciso) {
                printf(GRN "\nIL DEMOTORZONE E' STATO DISINTEGRATO!\n" RESET);
                printf("Premere INVIO per i titoli di coda...");
                getchar();
                exit(0);
              }
            break; 
            }
            // turno nemico
            int danno_n = (rand() % forza_n) + 1;
            printf(RED "Il nemico attacca e ti toglie %d HP!\n" RESET, danno_n);
            hp_p -= danno_n;
        } 
        else if (c_scelta == 2) {
            fine_scontro = gestisci_zaino(p, &hp_p, 1);
        }
        
        if (hp_p > 0 && !fine_scontro) {
            printf("\nPremere INVIO per il prossimo round...");
            getchar();
        }
    }

    if (hp_p <= 0) {
        printf(RED "\n%s E' MORTO...\n" RESET, p->nome);
        p->difesa_psichica = 0; // segnala la morte del giocatore
    } else {
        // reset statistiche
        p->attacco_psichico = atk_base;
        p->difesa_psichica = def_base; 
        printf(GRN "\nScontro terminato. Le tue ferite si rimarginano.\n" RESET);
    }
}

void passa(struct Giocatore *p) {
    printf("\n" YEL "==========================================" RESET);
    printf("\n  Il giocatore " GRN "%s" RESET " ha deciso di passare.", p->nome);
    printf("\n  Il turno termina qui.");
    printf("\n" YEL "==========================================" RESET "\n");
    printf("\nPremere INVIO per cedere il controllo al prossimo giocatore...");
    getchar();
}

void gioca() {
  if (mappa_pronta == 0) {
    printf(RED "Errore: La mappa non è pronta o non è stata validata (Opzione 7 del menu mappa).\n" RESET);
    return;
  }

  // inizializzazione giocatori
  int num_attivi = 0;
  for (int i = 0; i < 4; i++) {
    if (players[i] != NULL) {
      players[i]->mondo = reale;
      players[i]->posizione = (void *)prima_zona_mondoreale;
      players[i]->ha_mosso = 0;
      num_attivi++;
    }
  }

  if (num_attivi == 0) {
    printf(RED "Errore: Non ci sono giocatori impostati.\n" RESET);
    return;
  }

  int vittoria = 0;

  // continua finché qualcuno non vince o tutti muoiono
  while (!vittoria && num_attivi > 0) {
    // creazione array indici giocatori attivi per turno casuale
    int attivi_indices[4];
    int num_attivi_turno = 0;
    for (int i = 0; i < 4; i++) {
      if (players[i] != NULL) {
        attivi_indices[num_attivi_turno] = i;
        num_attivi_turno++;
      }
    }

    // mescola l'ordine dei giocatori per questo turno
    shuffle_indices(attivi_indices, num_attivi_turno);

    // itera attraverso i giocatori in ordine casuale
    for (int turn_idx = 0; turn_idx < num_attivi_turno; turn_idx++) {
      int i = attivi_indices[turn_idx];
      
      // Controllo di sicurezza: se il player è stato rimosso (NULL), salta
      if (players[i] == NULL) continue; 

      if (vittoria) break;

      struct Giocatore *p = players[i];
      p->ha_mosso = 0; // reset flag movimento all'inizio di ogni turno
      usa_bici = 0;
      int fine_turno = 0;

      while (!fine_turno && !vittoria) {
        system("clear");
        printf(YEL "--- TURNO DI %s ---\n" RESET, p->nome);

        // visualizzazione grafica della stanza
        stampa_cella_attuale(p);

        // controllo e segnalazione del nemico bloccante
        if (nemico_vivo(p)) {
          printf(RED "!!! ATTENZIONE: C'è un %s in questa zona !!!\n" RESET, getNomeNemico((p->mondo == reale) ? ((struct Zona_MondoReale *)p->posizione)->nemico : ((struct Zona_SopraSotto *)p->posizione)->nemico));
        }

        // menu azioni
        printf(YEL "AZIONI DISPONIBILI:\n" RESET);
        printf("  [1] Avanza            [5] Stampa Giocatore\n");
        printf("  [2] Indietreggia      [6] Raccogli Oggetto\n");
        printf("  [3] Cambia Mondo      [7] Utilizza Oggetto\n");
        printf("  [4] Combatti          [8] Passa Turno\n");
        printf(YEL "------------------------------------------------\n" RESET);
        printf("Scelta: ");

        int scelta_azione;
        if (scanf("%d", &scelta_azione) != 1) {
          while (getchar() != '\n'); // pulizia buffer
          continue;
        }
        while (getchar() != '\n');

        switch (scelta_azione) {
        case 1:
          avanza(p);
          break;
        case 2:
          indietreggia(p);
          break;
        case 3:
          cambia_mondo(p);
          break;
        case 4:
          if (!nemico_vivo(p)) {
            printf(GRN "Non c'è nessuno con cui combattere qui.\n" RESET);
          } else {
            combatti(p);

            // controllo se il giocatore è morto
            if (p->difesa_psichica <= 0) {
              printf(RED "\n%s è morto. Le sue spoglie restano nel %s...\n" RESET, p->nome, (p->mondo == reale) ? "Mondo Reale" : "Soprasotto");
              printf(GRN "\nPremere INVIO per passare al prossimo giocatore..." RESET);
              getchar(); 
              // ----------------------------------------------------

              free(players[i]);  // libera la memoria allocata
              players[i] = NULL; // segna lo slot come vuoto
              num_attivi--;
              fine_turno = 1; // forza il passaggio al prossimo giocatore
            } 
          }
          break;
        case 5:
          stampa_giocatore(p);
          break;
        case 6:
          raccogli_oggetto(p);
          break;
        case 7:
          gestisci_zaino(p, NULL, 0);
          break;
        case 8:
          passa(p);
          fine_turno = 1;
          break;
        default:
          printf(RED "Scelta non valida.\n" RESET);
        }

        // controllo se tutti i giocatori sono morti dopo l'azione
        if (num_attivi == 0) {
          printf(RED "\nPARTITA FINITA: Tutti i giocatori sono stati sconfitti.\n" RESET);
          vittoria = 0; // nessun vincitore
          break;
        }

        if (!fine_turno && !vittoria) {
          printf("\nPremere INVIO per continuare...");
          getchar();
        }
      }
    }
  }

  printf(GRN "\n--- FINE DELLA PARTITA ---\n" RESET);
  printf("Premere INVIO per tornare al menu principale...");
  getchar();
}

// PULIZIA DELLA MEMORIA
void pulisci_risorse() {
  printf(YEL "\nPulizia della memoria in corso..." RESET "\n");

  // svuotare mappa
  struct Zona_MondoReale *curr_mr = prima_zona_mondoreale;
  while (curr_mr != NULL) {
    struct Zona_MondoReale *temp_mr = curr_mr;

    // eliminare prima MR e poi speculare SS
    if (curr_mr->link_soprasotto != NULL) {
      free(curr_mr->link_soprasotto);
    }

    curr_mr = curr_mr->avanti;
    free(temp_mr);
  }
  prima_zona_mondoreale = NULL;
  prima_zona_soprasotto = NULL;

  // svuotare giocatori
  for (int i = 0; i < 4; i++) {
    if (players[i] != NULL) {
      free(players[i]);
      players[i] = NULL;
    }
  }

  printf(GRN "Memoria pulita correttamente. Arrivederci!" RESET "\n");
}