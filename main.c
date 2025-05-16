#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

typedef struct {
    int id;
    char ime[50];
    char prezime[50];
    int brojOsoba;
    char vrstaMesa[30];
    float kolicinaMesa;  
} Osoba;

typedef struct {
    char naziv[30];
    float cijenaPoKg;
} Meso;


void unosOsobe(Osoba* o) {
    printf("Unesite ID: ");
    scanf("%d", &o->id);
    printf("Ime: ");
    scanf("%s", o->ime);
    printf("Prezime: ");
    scanf("%s", o->prezime);
    printf("Broj osoba: ");
    scanf("%d", &o->brojOsoba);
    printf("Vrsta mesa: ");
    scanf("%s", o->vrstaMesa);

    o->kolicinaMesa = o->brojOsoba * 0.3f;  
}


void unosMesa(Meso* m) {
    printf("Unesite naziv mesa: ");
    scanf("%s", m->naziv);
    printf("Unesite cijenu po kg (u kn): ");
    scanf("%f", &m->cijenaPoKg);
}


void ispisiOsobuIMeso(const Osoba* o, const Meso* m) {
    printf("\nPodaci o osobi:\n");
    printf("ID: %d\n", o->id);
    printf("Ime i prezime: %s %s\n", o->ime, o->prezime);
    printf("Broj osoba: %d\n", o->brojOsoba);
    printf("Vrsta mesa: %s\n", o->vrstaMesa);
    printf("Potrebna kolicina mesa: %.2f kg\n", o->kolicinaMesa);
    printf("\nPodaci o mesu:\n");
    printf("Naziv mesa: %s\n", m->naziv);
    printf("Cijena po kg: %.2f kn\n", m->cijenaPoKg);

    float ukupnaCijena = o->kolicinaMesa * m->cijenaPoKg;
    printf("\nUkupna cijena za %s: %.2f kn\n", o->vrstaMesa, ukupnaCijena);
}

int main() {
    Osoba osoba;
    Meso meso;

    printf("--- Roštilj Kalkulator ---\n");

    unosOsobe(&osoba);
    unosMesa(&meso);

  
    if (strcmp(osoba.vrstaMesa, meso.naziv) != 0) {
        printf("\nUpozorenje: Vrsta mesa osobe i unesena vrsta mesa se razlikuju.\n");
    }

    ispisiOsobuIMeso(&osoba, &meso);

    return 0;
}
