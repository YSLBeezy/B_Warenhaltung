#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struktur für die Artikel
typedef struct {
    char artikelnummer[20];
    int fachhoehe;
    int breite;
} Artikel;

// Struktur für die Lager
typedef struct {
    char name[3];
    int regalreihen;
    int inventory[3][3]; // [Fachhöhe][Breite]
} Lager;

// Funktionen
void display_inventory(Lager *lager);
void add_article(Lager *lager, Artikel *artikel);
void generate_random_order(Lager *lager, Artikel *order, int quantity);
void process_order(Lager *lager, Artikel *order, int quantity);

int main() {
    Lager halle_lager = {"HAL", 40, {{0}}};
    Lager mi_lager = {"MI", 30, {{0}}};

    while (1) {
        display_inventory(&halle_lager);
        display_inventory(&mi_lager);

        printf("\nWas möchten Sie tun? (n)euen Artikel anlegen, Artikel (e)rfassen, Lagerbestand zufällig be(f)üllen, "
               "zufällige (B)estellung erzeugen oder (q)uit: ");
        char action;
        scanf(" %c", &action);

        if (action == 'q') {
            break;
        } else if (action == 'n') {
            Artikel new_artikel;
            printf("Artikelnummer: ");
            scanf("%s", new_artikel.artikelnummer);
            printf("Fachhöhe (cm): ");
            scanf("%d", &new_artikel.fachhoehe);
            printf("Breite (cm): ");
            scanf("%d", &new_artikel.breite);
            char lager_name[3];
            printf("In welchem Lager (HAL/MI): ");
            scanf("%s", lager_name);

            if (strcmp(lager_name, "HAL") == 0) {
                add_article(&halle_lager, &new_artikel);
            } else if (strcmp(lager_name, "MI") == 0) {
                add_article(&mi_lager, &new_artikel);
            } else {
                printf("Ungültiges Lager.\n");
            }
        } else if (action == 'f') {
            int quantity;
            printf("Wie viele Artikel sollen eingelagert werden? ");
            scanf("%d", &quantity);
            Artikel order[quantity];
            generate_random_order(&halle_lager, order, quantity);
            generate_random_order(&mi_lager, order, quantity);
        } else if (action == 'B') {
            Artikel order[10];
            int quantity;
            printf("Wie viele Artikel sollen bestellt werden? ");
            scanf("%d", &quantity);
            generate_random_order(&halle_lager, order, quantity);
            generate_random_order(&mi_lager, order, quantity);
            process_order(&halle_lager, order, quantity);
            process_order(&mi_lager, order, quantity);
        } else {
            printf("Ungültige Aktion.\n");
        }
    }

    return 0;
}

void display_inventory(Lager *lager) {
    printf("\n%s\n%-10s%-20s", lager->name, "Artikel", "Belegung");
    for (int i = 0; i < 3; i++) {
        printf("\n%3dcm:", i * 20);
        int total_width = 0;
        for (int j = 0; j < 3; j++) {
            float percentage = (float)lager->inventory[i][j] / lager->regalreihen * 100;
            printf("%3dcm: %3.0f%% | ", j * 20, percentage);
            total_width += j * 20 * lager->inventory[i][j];
        }
        printf("\nGesamtbreite: %dcm", total_width);
    }
    printf("\n");
}

void add_article(Lager *lager, Artikel *artikel) {
    lager->inventory[artikel->fachhoehe / 20][artikel->breite / 20]++;
    printf("Artikel %s eingelagert im %dcm Fach, %dcm Breite.\n", artikel->artikelnummer, artikel->fachhoehe, artikel->breite);
}

void generate_random_order(Lager *lager, Artikel *order, int quantity) {
    // Implementierung für die zufällige Bestellung
}

void process_order(Lager *lager, Artikel *order, int quantity) {
    // Implementierung für die Verarbeitung der Bestellung
}
