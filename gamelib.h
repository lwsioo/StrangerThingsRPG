// COLORI PER I PRINT IN CONSOLE
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define RESET "\x1B[0m"

// ENUM PER I VARI TIPI
enum TipoOggetto {
  nessun_oggetto,
  bicicletta,
  maglietta_fuocoinferno,
  bussola,
  schitarrata_metallica
};
enum TipoZona {
  bosco,
  scuola,
  laboratorio,
  caverna,
  strada,
  giardino,
  supermercato,
  centrale_elettrica,
  deposito_abbandonato,
  stazione_polizia
};
enum TipoNemico { 
    nessun_nemico, 
    billi, 
    democane, 
    demotorzone 
};
enum Mondo {
  reale,
  soprasotto
};

// DICHIARAZIONI STRUCT
struct Zona_SopraSotto;
struct Zona_MondoReale {
    enum TipoZona tipo;
    enum TipoNemico nemico;
    enum TipoOggetto oggetto;
    struct Zona_MondoReale * avanti;
    struct Zona_MondoReale *indietro;
    struct Zona_SopraSotto *link_soprasotto;
};
struct Zona_SopraSotto {
    enum TipoZona tipo;
    enum TipoNemico nemico;
    struct Zona_SopraSotto *avanti;
    struct Zona_SopraSotto *indietro;
    struct Zona_MondoReale *link_soprasotto;
};
struct Giocatore {
  char nome[26];
  enum Mondo mondo;

  void *posizione;

  int ha_mosso; //0 se si può ancora muovere, 1 se ha già usato una funzione di spostamento

  // abilità del player da 1 a 20
  int attacco_psichico;
  int difesa_psichica;
  int fortuna;

  enum TipoOggetto zaino[3];
};

// DICHIARAZIONI VARIE FUNZIONI GAMELIB.C
void printCredits();
void impostaGioco();
void printRules();
void gioca();
void pulisci_risorse();