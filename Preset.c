#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BWHT "\e[1;37m"
#define RESET "\e[0m"

// Struktur für ein Lagerregal
typedef struct {
    char id[20]; // Eindeutige Kennung des Fachs
    int belegt;  // 0: nicht belegt, 1: belegt
} Regalfach;

// Struktur für einen Artikel
typedef struct {
    char artikelnummer[20];
    char name[50];
    int breite;
    int hoehe;
    int tiefe;
} Artikel;

// Struktur für ein Lager
typedef struct {
    char name[3];
    Regalfach*** regale;  // 3D-Array für die Regale
    int* belegung;        // Array für die Belegung der Fachhöhen
    Artikel** inventar;   // Array für die inventarisierten Artikel
} Lager;

// Globales Array für die gespeicherten Artikel
Artikel* lagerArtikel = NULL;
int anzahlArtikel = 0;

// Funktionen für Lagerverwaltung
Lager* erstelle_halle_lager(void);
Lager* erstelle_porta_lager(void);
void menu(void);
void clear_screen(void);
void artikel_anlegen(void);
void zeige_belegung_halle(Lager* lager);
void zeige_belegung_porta(Lager* lager);
void belege_zufaellig(Lager* lager, Artikel* artikel);
void erfasse_artikel(Lager* lager);
void auslagern(Lager* lager, char artikelnummer[]);
void zufaellige_bestellung(Lager* lager);


Lager* halleLager = NULL;
Lager* portaLager = NULL;

int main() {
    halleLager = erstelle_halle_lager();
    portaLager = erstelle_porta_lager();

    char option;

    while (1) {
        menu();

        scanf(" %c", &option);

        switch (option) {
            case 'n':
                artikel_anlegen();
                break;
            case 'e':
                clear_screen();
                printf("In welchem Lager soll der Artikel eingelagert werden?\n\n");
                printf("\t1) Halle\n");
                printf("\t2) Porta Westfalica\n\n\t- ");
                char lagerAuswahl;
                scanf(" %c", &lagerAuswahl);
                switch(lagerAuswahl) {
                    case '1':
                        erfasse_artikel(halleLager);
                        break;
                    case '2':
                        erfasse_artikel(portaLager);
                        break;
                }
            case 'a':
                printf("Geben Sie die Artikelnummer ein: ");
                char artikelnummer[20];
                scanf("%s", artikelnummer);
                auslagern(halleLager, artikelnummer);
                auslagern(portaLager, artikelnummer);
                break;
            case 'b':
                zufaellige_bestellung(halleLager);
                zufaellige_bestellung(portaLager);
                break;
            case 'x':
                free(halleLager);
                free(portaLager);
                exit(0);
            default:
                printf("Ungültige Option. Bitte erneut wählen.\n");
        }
    }

    return 0;
}

void menu(void) {
    clear_screen();
    zeige_belegung_halle(halleLager);
    printf("\n");
    zeige_belegung_porta(portaLager);

    printf("\n(n)euen Artikel anlegen\n");
    printf("Artikel (e)rfassen\n");
    printf("Artikel (v)er\x84ndern\n");
    printf("Artikel (l)\x94schen\n");
    printf("Lagerbestand zuf\x84llig be(f)\x81llen\n");
    printf("Lagerbestand man(u)ell bef\x81llen\n");
    printf("zuf\x84llige (B)estellung erzeugen\n");
    printf("(m)anuelle Bestellung erzeugen\n");
    printf("erfasste Bestellung (v)ersenden\n");
    printf("erfasste Bestellung (a)bbrechen\n");
    printf("(x) Beenden\n");
    printf("- ");
    fflush(stdout);
    return;
}

void warte(void) {
    int z;

    getchar();
    printf("\n---> Enter");
    fflush(stdout);
    fflush(stdin);
    z = getchar();
    z++;
    return;
}

void clear_screen(void) {
    system("@cls||clear");
    return;
}

// Funktion zum Erstellen des Halle-Lagers
Lager* erstelle_halle_lager() {
    Lager* halleLager = (Lager*)malloc(sizeof(Lager));
    strcpy(halleLager->name, "HAL");

    int regalreihen = 40;
    int regalfachbreite = 3;
    int regalfachtiefe = 1.2;  // In Metern

    // Höhen der Ebenen
    int ebenenHoehen[] = {40, 20};

    // Berechnung der maximalen Regalhöhe
    int maxRegalhoehe = 255;  // Maximal zulässige Höhe in cm
    int regalhoehe = 0;
    for (int i = 0; i < sizeof(ebenenHoehen) / sizeof(ebenenHoehen[0]); i++) {
        regalhoehe += ebenenHoehen[i];
        if (regalhoehe > maxRegalhoehe) {
            // Wenn die Höhe die Maximalgrenze überschreitet, brechen wir ab
            printf("Fehler: Die Höhe der Ebenen überschreitet die Maximalhöhe des Regals.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Anzahl der Ebenen im Regal
    int anzahlEbenen = sizeof(ebenenHoehen) / sizeof(ebenenHoehen[0]);

    halleLager->regale = (Regalfach***)malloc(anzahlEbenen * sizeof(Regalfach**));
    for (int i = 0; i < anzahlEbenen; i++) {
        halleLager->regale[i] = (Regalfach**)malloc(regalreihen * sizeof(Regalfach*));
        for (int j = 0; j < regalreihen; j++) {
            int breite = (i == 0) ? 40 : 20;  // Nur die erste Ebene hat 40cm-Fächer

            halleLager->regale[i][j] = (Regalfach*)malloc(breite * sizeof(Regalfach));
            for (int k = 0; k < breite; k++) {
                sprintf(halleLager->regale[i][j][k].id, "%d-%d-%d", i + 1, j + 1, k + 1);
                halleLager->regale[i][j][k].belegt = 0;
            }
        }
    }

    // Initialisiere Artikelanzahl und Belegung
    halleLager->belegung = (int*)calloc(anzahlEbenen, sizeof(int));
    halleLager->inventar = NULL;

    return halleLager;
}

// Funktion zum Erstellen des Porta Westfalica-Lagers
Lager* erstelle_porta_lager() {
    Lager* portaLager = (Lager*)malloc(sizeof(Lager));
    strcpy(portaLager->name, "MI");

    int regalreihen = 30;
    int regalfachbreite = 3;
    int regalfachtiefe = 1.2;  // In Metern

    // Höhen der Ebenen
    int ebenenHoehen[] = {80, 40, 20};

    // Berechnung der maximalen Regalhöhe
    int maxRegalhoehe = 255;  // Maximal zulässige Höhe in cm
    int regalhoehe = 0;
    for (int i = 0; i < sizeof(ebenenHoehen) / sizeof(ebenenHoehen[0]); i++) {
        regalhoehe += ebenenHoehen[i];
        if (regalhoehe > maxRegalhoehe) {
            // Wenn die Höhe die Maximalgrenze überschreitet, brechen wir ab
            printf("Fehler: Die Höhe der Ebenen überschreitet die Maximalhöhe des Regals.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Anzahl der Ebenen im Regal
    int anzahlEbenen = sizeof(ebenenHoehen) / sizeof(ebenenHoehen[0]);

    portaLager->regale = (Regalfach***)malloc(anzahlEbenen * sizeof(Regalfach**));
    for (int i = 0; i < anzahlEbenen; i++) {
        portaLager->regale[i] = (Regalfach**)malloc(regalreihen * sizeof(Regalfach*));
        for (int j = 0; j < regalreihen; j++) {
            int breite;
            if (j < 10) {
                breite = 40;
            } else if (j < 20) {
                breite = 80;
            } else {
                breite = 20;
            }

            portaLager->regale[i][j] = (Regalfach*)malloc(breite * sizeof(Regalfach));
            for (int k = 0; k < breite; k++) {
                sprintf(portaLager->regale[i][j][k].id, "%d-%d-%d", i + 1, j + 1, k + 1);
                portaLager->regale[i][j][k].belegt = 0;
            }
        }
    }

    portaLager->belegung = (int*)calloc(anzahlEbenen, sizeof(int));
    portaLager->inventar = NULL;

    return portaLager;
}

void artikel_anlegen() {
    clear_screen();
    printf("Artikel anlegen:\n\n");

    Artikel neuerArtikel;

    printf("\tArtikelnummer:\t");
    scanf("%s", neuerArtikel.artikelnummer);

    printf("\tName:\t\t");
    scanf("%s", neuerArtikel.name);

    printf("\tBreite:\t\t");
    scanf("%d", &neuerArtikel.breite);

    printf("\tH\x94he:\t\t");
    scanf("%d", &neuerArtikel.hoehe);

    printf("\tTiefe:\t\t");
    scanf("%d", &neuerArtikel.tiefe);

    // Dynamisch Speicherplatz für das Inventar vergrößern
    lagerArtikel = (Artikel*)realloc(lagerArtikel, (anzahlArtikel + 1) * sizeof(Artikel));
    if (lagerArtikel == NULL) {
        printf("Fehler: Speicher konnte nicht allokiert werden.\n");
        exit(EXIT_FAILURE);
    }

    // Artikel hinzufügen
    lagerArtikel[anzahlArtikel++] = neuerArtikel;

    printf("\n\nArtikel erfolgreich angelegt!\n");
    warte();

    return;
}

// Funktion zur Anzeige der Belegung für Halle
void zeige_belegung_halle(Lager* lager) {
    printf(BWHT "%s" RESET "\nArtikel: %d\nBelegung\n", lager->name, lager->belegung[0] + lager->belegung[1]);

    int hoehen[] = {40, 20};  // Höhen der Fächer

    for (int i = 0; i < 2; i++) {
        int belegteFacher = 0;

        for (int j = 0; j < lager->belegung[i]; j++) {
            for (int k = 0; k < hoehen[i]; k++) {
                if (lager->regale[i][j][k].belegt) {
                    belegteFacher++;
                }
            }
        }

        int breite = (i == 0) ? 40 : 20;
        printf("    %dcm: %d%%\n", hoehen[i], (lager->belegung[i] != 0) ? (belegteFacher * 100) / (breite * lager->belegung[i]) : 0);
    }
    printf("-----------------------------------");
}

// Funktion zur Anzeige der Belegung für Porta Westfalica
void zeige_belegung_porta(Lager* lager) {
    printf(BWHT "%s" RESET "\nArtikel: %d\nBelegung\n", lager->name, lager->belegung[0] + lager->belegung[1] + lager->belegung[2]);

    int hoehen[] = {40, 20, 80};  // Höhen der Fächer

    for (int i = 0; i < 3; i++) {
        int belegteFacher = 0;

        for (int j = 0; j < lager->belegung[i]; j++) {
            for (int k = 0; k < hoehen[i]; k++) {
                if (lager->regale[i][j][k].belegt) {
                    belegteFacher++;
                }
            }
        }

        printf("    %dcm: %d%%\n", hoehen[i], (lager->belegung[i] != 0) ? (belegteFacher * 100) / (hoehen[i] * lager->belegung[i]) : 0);
    }
    printf("-----------------------------------");
}

// Funktion zum zufälligen Belegen eines Lagers
void belege_zufaellig(Lager* lager, Artikel* artikel) {
    // Implementierung erforderlich, abhängig von den genauen Anforderungen
}

// Funktion zum Erfassen eines neuen Artikels
void erfasse_artikel(Lager* lager) {
    // Implementierung erforderlich, abhängig von den genauen Anforderungen
}

// Funktion zum Auslagern eines Artikels
void auslagern(Lager* lager, char artikelnummer[]) {
    // Implementierung erforderlich, abhängig von den genauen Anforderungen
}

// Funktion zur Erzeugung einer zufälligen Bestellung
void zufaellige_bestellung(Lager* lager) {
    // Implementierung erforderlich, abhängig von den genauen Anforderungen
}
