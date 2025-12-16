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
char getSimboloOggetto(enum TipoOggetto o) {
    switch(o) {
        case bicicletta: return 'b';
        case maglietta_fuocoinferno: return 'm';
        case bussola: return 'o';
        case schitarrata_metallica: return 'j';
        default: return ' '; // Nessun oggetto
    }
}
char getSimboloNemico(enum TipoNemico n) {
    switch(n) {
        case billi: return 'W';
        case democane: return 'C';
        case demotorzone: return 'D';
        default: return ' '; // Nessun nemico
    }
}

void printCredits() {
  printf("\n" RED "==========================================" RESET "\n");
  printf(RED "=                CREDITI                 =" RESET "\n");
  printf(RED "==========================================" RESET "\n");
  printf("\n");
  printf("  " GRN "Sviluppatore:" RESET " Alessio Bragetti\n");
  printf("  " GRN "Progetto:" RESET " Cose Strane\n");
  printf("  " GRN "Versione:" RESET " 1.0.0\n");
  printf("  " GRN "Anno:" RESET " 2025\n");
  printf("\n");
  printf("  --------------------------------------\n");
  printf("  Realizzato per l'esame di:\n");
  printf("  Programmazione I / Laboratorio\n");
  printf("  --------------------------------------\n");
  printf("\n" GRN "Premere INVIO per tornare al menu..." RESET "\n");

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

        // 1. MONDO REALE: Tetto
        for (int j = 0; j < 5 && temp_mr != NULL; j++) {
            printf("  ___   ");
            temp_mr = temp_mr->avanti;
        }
        printf("\n");

        // 2. MONDO REALE: Contenuto (Nemico e Oggetto)
        temp_mr = blocco_mr;
        for (int j = 0; j < 5 && temp_mr != NULL; j++) {
            printf(" |" RED "%c" RESET " " YEL "%c" RESET "|  ", 
                   getSimboloNemico(temp_mr->nemico), getSimboloOggetto(temp_mr->oggetto));
            temp_mr = temp_mr->avanti;
        }
        printf(" (MR)\n");

        // 3. MONDO REALE: Base
        temp_mr = blocco_mr;
        for (int j = 0; j < 5 && temp_mr != NULL; j++) {
            printf(" |___|  ");
            temp_mr = temp_mr->avanti;
        }
        printf("\n");

        // --- COLLEGAMENTO DIMENSIONALE ---
        temp_mr = blocco_mr;
        for (int j = 0; j < 5 && temp_mr != NULL; j++) {
            printf("   V    ");
            temp_mr = temp_mr->avanti;
        }
        printf("\n");

        // 4. SOPRASOTTO: Tetto (Rosso)
        temp_ss = blocco_ss;
        for (int j = 0; j < 5 && temp_ss != NULL; j++) {
            printf(RED "  ___   " RESET);
            temp_ss = temp_ss->avanti;
        }
        printf("\n");

        // 5. SOPRASOTTO: Contenuto (Solo Nemico)
        temp_ss = blocco_ss;
        for (int j = 0; j < 5 && temp_ss != NULL; j++) {
            printf(RED " |" RESET RED "%c" RESET "  " RED "|  " RESET, getSimboloNemico(temp_ss->nemico));
            temp_ss = temp_ss->avanti;
        }
        printf(RED " (SS)\n" RESET);

        // 6. SOPRASOTTO: Base (Rosso)
        temp_ss = blocco_ss;
        for (int j = 0; j < 5 && temp_ss != NULL; j++) {
            printf(RED " |___|  " RESET);
            temp_ss = temp_ss->avanti;
        }
        printf("\n\n");

        // Avanzamento al blocco successivo (5 zone avanti)
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
  
  // Chiediamo la posizione i
  printf("Inserisci la posizione della zona da stampare: ");
  if (scanf("%d", &pos) != 1 || pos < 0) {
    printf(RED "Errore: inserire un numero valido." RESET "\n");
    while (getchar() != '\n');
    return;
  }
  while (getchar() != '\n');

  // Usiamo il puntatore del Mondo Reale come base per la ricerca
  struct Zona_MondoReale *curr = prima_zona_mondoreale;

  // Scorriamo la lista fino alla posizione i
  while (curr != NULL && attuale < pos) {
    curr = curr->avanti;
    attuale++;
  }

  // Se curr è NULL, la posizione i non esiste nella mappa
  if (curr == NULL) {
    printf(RED "Errore: la posizione %d non esiste nella mappa attuale." RESET "\n", pos);
    return;
  }

  // Stampiamo i dati della zona nel Mondo Reale
  printf(RED "\n--- INFO ZONA %d ---" RESET "\n", pos);
  printf(YEL "MONDO REALE:" RESET "\n");
  printf("  Tipo: %s\n  Nemico: %s\n  Oggetto: %s\n", 
         getNomeZona(curr->tipo), 
         getNomeNemico(curr->nemico), 
         getNomeOggetto(curr->oggetto));

  // Stampiamo i dati della zona corrispondente nel Soprasotto
  struct Zona_SopraSotto *speculare = curr->link_soprasotto;
  printf(RED "\nSOPRASOTTO (Speculare):" RESET "\n");
  printf("  Tipo: %s\n  Nemico: %s\n", 
         getNomeZona(speculare->tipo), 
         getNomeNemico(speculare->nemico));
  
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

    struct Zona_MondoReale *nuova_mr = (struct Zona_MondoReale *)malloc(sizeof(struct Zona_MondoReale));
    struct Zona_SopraSotto *nuova_ss = (struct Zona_SopraSotto *)malloc(sizeof(struct Zona_SopraSotto));

    printf(RED "\n--- CONFIGURAZIONE NUOVA ZONA ---\n" RESET);

    // Tipo Zona
    printf(YEL "Scegli il TIPO della zona:\n" RESET);
    printf("0) Bosco\t1) Scuola\t2) Laboratorio\n3) Caverna\t4) Strada\t5) Giardino\n6) Supermercato\t7) Centrale Elettrica\t8) Deposito Abbandonato\t9) Stazione di Polizia\n");
    printf("Scelta tipo: ");
    int t; scanf("%d", &t); 
    nuova_mr->tipo = (enum TipoZona)(t % 10);
    nuova_ss->tipo = nuova_mr->tipo;

    // Nemico Mondo Reale
    printf(YEL "\nScegli il NEMICO (Mondo Reale):\n" RESET);
    printf("0) Nessuno\t1) Billi\t2) Democane\n");
    printf("Scelta nemico MR: ");
    int n_mr; scanf("%d", &n_mr); 
    nuova_mr->nemico = (enum TipoNemico)(n_mr % 3);

    // Nemico Soprasotto con controllo Demotorzone
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
    printf("0) Nessuno\t2) Democane"); // Usiamo gli indici corretti del tuo enum
    if (!demotorzone_presente) {
        printf("\t3) Demotorzone");
    } else {
        printf(RED "\t[3) Demotorzone già presente]" RESET);
    }
    printf("\nScelta nemico SS: ");
    int n_ss; scanf("%d", &n_ss);
    
    if (n_ss == 3 && demotorzone_presente) {
        printf(RED "Azione non permessa. Impostato Nessun Nemico.\n" RESET);
        nuova_ss->nemico = nessun_nemico;
    } else {
        nuova_ss->nemico = (enum TipoNemico)n_ss;
    }

    // Oggetto Mondo Reale
    printf(YEL "\nScegli l'OGGETTO (Mondo Reale):\n" RESET);
    printf("0) Nessuno\t1) Bicicletta\t2) Maglietta\n3) Bussola\t4) Chitarra\n");
    printf("Scelta oggetto: ");
    int o; scanf("%d", &o); 
    nuova_mr->oggetto = (enum TipoOggetto)(o % 5);
    while (getchar() != '\n'); 

    nuova_mr->link_soprasotto = nuova_ss;
    nuova_ss->link_soprasotto = nuova_mr;

    // Logica di inserimento in lista doppiamente collegata
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
    } 
    else {
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

    mappa_pronta = 0; // Importante: forza il GM a ri-validare al punto 6
    printf(GRN "\nZona inserita con successo! Ricorda di chiudere la mappa (opzione 6) per salvare.\n" RESET);
}

void cancella_zona() {
    // 1. Controllo se la mappa esiste
    if (prima_zona_mondoreale == NULL) {
        printf(RED "Errore: La mappa è vuota, non c'è nulla da cancellare.\n" RESET);
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

    // 2. Ricerca del nodo alla posizione pos
    struct Zona_MondoReale *curr_mr = prima_zona_mondoreale;
    while (curr_mr != NULL && attuale < pos) {
        curr_mr = curr_mr->avanti;
        attuale++;
    }

    // 3. Verifica se la posizione esiste
    if (curr_mr == NULL) {
        printf(RED "Errore: La posizione %d non esiste nella mappa.\n" RESET, pos);
        return;
    }

    // 4. Scollegamento dei nodi nel Mondo Reale
    if (curr_mr->indietro != NULL) {
        // Se non è il primo nodo, il precedente deve puntare a quello dopo il corrente
        curr_mr->indietro->avanti = curr_mr->avanti;
    } else {
        // Se è il primo nodo, aggiorniamo il puntatore globale alla testa
        prima_zona_mondoreale = curr_mr->avanti;
    }

    if (curr_mr->avanti != NULL) {
        // Se non è l'ultimo nodo, il successivo deve puntare indietro a quello prima del corrente
        curr_mr->avanti->indietro = curr_mr->indietro;
    }

    // 5. Scollegamento dei nodi nel Soprasotto (Paralleli)
    struct Zona_SopraSotto *curr_ss = curr_mr->link_soprasotto;
    
    if (curr_ss->indietro != NULL) {
        curr_ss->indietro->avanti = curr_ss->avanti;
    } else {
        prima_zona_soprasotto = curr_ss->avanti;
    }

    if (curr_ss->avanti != NULL) {
        curr_ss->avanti->indietro = curr_ss->indietro;
    }

    // 6. Liberazione della memoria
    free(curr_ss);
    free(curr_mr);

    // 7. Reset dello stato della mappa
    mappa_pronta = 0; // La mappa è cambiata, il GM deve ri-validarla (Punto 6)
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
    while (getchar() != '\n')
      ;

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
    default:
      printf(RED "Numero non valido\n");
    }
  } while (scelta_mappa != 7);
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
    for (int k = 0; k < 3; k++) {
      players[i]->zaino[k] = nessun_oggetto;
    }
    system("clear");
  }

  menu_mappa();

  printf("\n" GRN "Configurazione completata!" RESET "\n");
  printf("Premere INVIO per continuare...");
  getchar();
}
