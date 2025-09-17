#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h> // Za korištenje bool, true, false


typedef struct {
    char naziv[50];
    int cijenaEur;
    int cevapciciGr;
    int piletinaGr;
    int kobasiceGr;
    int kotletiGr;
} RostiljPaket;

typedef struct {
    int male;
    int female;
    int kids;
} Korisnici;

typedef struct {
    char type[50];
    int eur400g;
    int eur500g;
    int eur800g;
    int eur1000g;
    int eur2000g;
    int boolMale;
    int boolFemale;
    int boolKids;
    double qGr; // Potrebna kolièina u gramima
    int qKom;   // Ukupno kupljena pojedinaèna kolièina
} Meso;

// Enum za opcije u izborniku
typedef enum {
    UNOS_CLANOVA = 1,
    ODABIR_MESA,
    IZRACUN,
    UPRAVLJANJE_PAKETIMA,
    KRAJ_RADA
} MeniOpcija;

// Prototipovi funkcija
void ucitajPodatke(Meso* meso, RostiljPaket** paketi, int* brojPaketa);
void spremiCijene(const Meso* meso, const RostiljPaket* paketi, int brojPaketa);
void prikaziMeni();

void unosClanova(Korisnici* korisnici, int* provjeraUnosa);
void odabirMesa(Meso* meso, int* provjeraOdabira);
void izracunajCijenu(Korisnici* korisnici, Meso* meso, RostiljPaket* paketi, int brojPaketa, int* provjeraUnosa, int* provjeraOdabira);

// CRUID funkcije za pakete
void upravljanjePaketima(Meso* meso, RostiljPaket** paketi, int* brojPaketa);
void ispisiSvePakete(const RostiljPaket* paketi, int brojPaketa);
void dodajPaket(RostiljPaket** paketi, int* brojPaketa);
void azurirajPaket(RostiljPaket* paketi, int brojPaketa);
void obrisiPaket(RostiljPaket** paketi, int* brojPaketa);

static inline void resetirajKolicineMesa(Meso* meso);
void postaviPocetneVrijednostiMesa(Meso* meso);


int main(void) {
    Meso meso[4];
    Korisnici korisnici = { 0, 0, 0 };

    RostiljPaket* paketi = NULL;
    int brojPaketa = 0;

    int provjeraUnosaClanova = 0;
    int provjeraOdabiraMesa = 0;

    ucitajPodatke(meso, &paketi, &brojPaketa);

    MeniOpcija odabir;
    while (1) {
        prikaziMeni();
        printf("Unesi broj: ");
        if (scanf("%d", (int*)&odabir) != 1) {
            printf("Greska, unesite broj.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (odabir) {
        case UNOS_CLANOVA:
            unosClanova(&korisnici, &provjeraUnosaClanova);
            break;
        case ODABIR_MESA:
            odabirMesa(meso, &provjeraOdabiraMesa);
            break;
        case IZRACUN:
            izracunajCijenu(&korisnici, meso, paketi, brojPaketa, &provjeraUnosaClanova, &provjeraOdabiraMesa);
            break;
        case UPRAVLJANJE_PAKETIMA:
            upravljanjePaketima(meso, &paketi, &brojPaketa);
            break;
        case KRAJ_RADA:
            free(paketi);
            paketi = NULL;
            printf("Kraj programa.\n");
            return 0;
        default:
            printf("Nepoznata opcija. Molimo pokusajte ponovo.\n");
            break;
        }
    }

    return 0;
}

void prikaziMeni() {
    printf("\n______________________________________\n\n");
    printf("1. Unos clanova\n");
    printf("2. Odabir vrste mesa po grupi\n");
    printf("3. Izracun\n");
    printf("4. Upravljanje cijenama i paketima\n");
    printf("5. Kraj rada\n");
    printf("______________________________________\n");
}


void ucitajPodatke(Meso* meso, RostiljPaket** paketi, int* brojPaketa) {
    *brojPaketa = 0;
    *paketi = NULL;

    FILE* fp = fopen("cijene.bin", "rb+");
    if (fp == NULL) {
        printf("Datoteka 'cijene.bin' nije pronadjena. Postavljam pocetne vrijednosti.\n");
        postaviPocetneVrijednostiMesa(meso);
        return;
    }

    if (fread(brojPaketa, sizeof(int), 1, fp) != 1) {
        printf("Upozorenje: Datoteka 'cijene.bin' je prazna ili ostecena. Krece se s 0 paketa.\n");
        postaviPocetneVrijednostiMesa(meso);
        *brojPaketa = 0;
        fclose(fp);
        return;
    }

    if (*brojPaketa < 0 || *brojPaketa > 1000) {
        printf("Greska: Procitan nevjerojatan broj paketa (%d). Datoteka je ostecena.\n", *brojPaketa);
        postaviPocetneVrijednostiMesa(meso);
        *brojPaketa = 0;
        fclose(fp);
        return;
    }

    // Nakon što znamo broj paketa, èitamo fiksne podatke o mesu
    fread(meso, sizeof(Meso), 4, fp);

    // Alociramo memoriju samo ako ima paketa
    if (*brojPaketa > 0) {
        *paketi = (RostiljPaket*)malloc(*brojPaketa * sizeof(RostiljPaket));
        if (*paketi == NULL) {
            perror("Neuspjesna alokacija memorije za pakete");
            *brojPaketa = 0; // Resetiraj na sigurno stanje
            fclose(fp);
            return;
        }
        // Ako je alokacija uspjela, èitamo podatke o paketima
        fread(*paketi, sizeof(RostiljPaket), *brojPaketa, fp);
    }

    printf("Ucitano %d paketa.\n", *brojPaketa);
    fclose(fp);
}

void spremiCijene(const Meso* meso, const RostiljPaket* paketi, int brojPaketa) {
    FILE* fp = fopen("cijene.bin", "wb");
    if (fp == NULL) {
        perror("Greska pri otvaranju datoteke za pisanje");
        return;
    }

    fwrite(&brojPaketa, sizeof(int), 1, fp);
    fwrite(meso, sizeof(Meso), 4, fp);
    if (brojPaketa > 0) {
        fwrite(paketi, sizeof(RostiljPaket), brojPaketa, fp);
    }

    fclose(fp);
    printf("Cijene i %d paketa su uspjesno spremljeni.\n", brojPaketa);
}

void upravljanjePaketima(Meso* meso, RostiljPaket** paketi, int* brojPaketa) {
    int odabir;
    while (1) {
        printf("\n--- UPRAVLJANJE PAKETIMA ---\n");
        printf("1. Ispisi sve pakete\n");
        printf("2. Dodaj novi paket\n");
        printf("3. Azuriraj postojeci paket\n");
        printf("4. Obrisi paket\n");
        printf("5. Spremi promjene i vrati se na glavni meni\n");
        printf("Odabir: ");
        scanf("%d", &odabir);

        switch (odabir) {
        case 1:
            ispisiSvePakete(*paketi, *brojPaketa);
            break;
        case 2:
            dodajPaket(paketi, brojPaketa);
            break;
        case 3:
            azurirajPaket(*paketi, *brojPaketa);
            break;
        case 4:
            obrisiPaket(paketi, brojPaketa);
            break;
        case 5:
            spremiCijene(meso, *paketi, *brojPaketa);
            return;
        default:
            printf("Nepoznat odabir.\n");
        }
    }
}

void ispisiSvePakete(const RostiljPaket* paketi, int brojPaketa) {
    if (brojPaketa == 0) {
        printf("\nNema unesenih paketa.\n");
        return;
    }
    printf("\n--- POPIS ROSTILJ PAKETA ---\n");
    for (int i = 0; i < brojPaketa; i++) {
        printf("ID: %d | Naziv: %s | Cijena: %d EUR\n", i, paketi[i].naziv, paketi[i].cijenaEur);
        printf("       Sastav: Cevapi(%d g), Piletina(%d g), Kobasice(%d g), Kotleti(%d g)\n",
            paketi[i].cevapciciGr, paketi[i].piletinaGr, paketi[i].kobasiceGr, paketi[i].kotletiGr);
    }
}

void dodajPaket(RostiljPaket** paketi, int* brojPaketa) {
    RostiljPaket* noviPaketi = (RostiljPaket*)realloc(*paketi, (*brojPaketa + 1) * sizeof(RostiljPaket));

    if (noviPaketi == NULL) {
        perror("Neuspjesna realokacija memorije za dodavanje paketa");
        return;
    }
    *paketi = noviPaketi;

    RostiljPaket* novi = &(*paketi)[*brojPaketa];
    printf("\n--- UNOS NOVOG PAKETA ---\n");
    printf("Naziv paketa: ");
    scanf(" %[^\n]", novi->naziv);
    printf("Cijena (EUR): ");
    scanf("%d", &novi->cijenaEur);
    printf("Grama cevapcica: ");
    scanf("%d", &novi->cevapciciGr);
    printf("Grama piletine: ");
    scanf("%d", &novi->piletinaGr);
    printf("Grama kobasica: ");
    scanf("%d", &novi->kobasiceGr);
    printf("Grama kotleta: ");
    scanf("%d", &novi->kotletiGr);

    (*brojPaketa)++;
    printf("Novi paket uspjesno dodan.\n");
}


void azurirajPaket(RostiljPaket* paketi, int brojPaketa) {
    if (brojPaketa == 0) {
        printf("Nema paketa za azuriranje.\n");
        return;
    }
    ispisiSvePakete(paketi, brojPaketa);
    int id;
    printf("Unesite ID paketa koji zelite azurirati: ");
    scanf("%d", &id);

    if (id < 0 || id >= brojPaketa) {
        printf("Nevazeci ID.\n");
        return;
    }

    RostiljPaket* p = &paketi[id];
    printf("\n--- AZURIRANJE PAKETA: %s ---\n", p->naziv);
    printf("Novi naziv (trenutni: %s): ", p->naziv);
    scanf(" %[^\n]", p->naziv);
    printf("Nova cijena (EUR) (trenutna: %d): ", p->cijenaEur);
    scanf("%d", &p->cijenaEur);
    printf("Novi grami cevapcica (trenutno: %d): ", p->cevapciciGr);
    scanf("%d", &p->cevapciciGr);
    printf("Novi grami piletine (trenutno: %d): ", p->piletinaGr);
    scanf("%d", &p->piletinaGr);
    printf("Novi grami kobasica (trenutno: %d): ", p->kobasiceGr);
    scanf("%d", &p->kobasiceGr);
    printf("Novi grami kotleta (trenutno: %d): ", p->kotletiGr);
    scanf("%d", &p->kotletiGr);

    printf("Paket uspjesno azuriran.\n");
}

void obrisiPaket(RostiljPaket** paketi, int* brojPaketa) {
    if (*brojPaketa == 0) {
        printf("Nema paketa za brisanje.\n");
        return;
    }
    ispisiSvePakete(*paketi, *brojPaketa);
    int id;
    printf("Unesite ID paketa koji zelite obrisati: ");
    scanf("%d", &id);

    if (id < 0 || id >= *brojPaketa) {
        printf("Nevazeci ID.\n");
        return;
    }

    for (int i = id; i < *brojPaketa - 1; i++) {
        (*paketi)[i] = (*paketi)[i + 1];
    }

    (*brojPaketa)--;

    if (*brojPaketa == 0) {
        free(*paketi);
        *paketi = NULL;
    }
    else {
        RostiljPaket* noviPaketi = (RostiljPaket*)realloc(*paketi, *brojPaketa * sizeof(RostiljPaket));
        if (noviPaketi == NULL) {
            perror("Neuspjesna realokacija nakon brisanja");
            return;
        }
        *paketi = noviPaketi;
    }

    printf("Paket s ID %d je obrisan.\n", id);
}

void unosClanova(Korisnici* korisnici, int* provjeraUnosa) {
    *provjeraUnosa = 1;
    printf("\n--- UNOS CLANOVA ---\n");
    printf("Unesi broj muskaraca: ");
    scanf("%d", &korisnici->male);
    printf("Unesi broj zena: ");
    scanf("%d", &korisnici->female);
    printf("Unesi broj djece: ");
    scanf("%d", &korisnici->kids);
}

void odabirMesa(Meso* meso, int* provjeraOdabira) {
    *provjeraOdabira = 1;
    int i, odabirGrupe, zeliMeso;
    int odabranoZaMuskarce = 0, odabranoZaZene = 0, odabranoZaDjecu = 0;

    for (i = 0; i < 4; i++) {
        meso[i].boolMale = 0;
        meso[i].boolFemale = 0;
        meso[i].boolKids = 0;
    }

    printf("\n--- ODABIR VRSTE MESA PO GRUPI ---\n");
    while (1) {
        printf("\n 1. Muskarci\n 2. Zene\n 3. Djeca\n 4. Kraj odabira\n");
        printf("Odaberite grupu: ");
        scanf("%d", &odabirGrupe);

        if (odabirGrupe == 4) break;

        switch (odabirGrupe) {
        case 1:
            printf("MUSKARCI (1 za DA, 0 za NE):\n");
            odabranoZaMuskarce = 1;
            for (i = 0; i < 4; i++) {
                printf("Zele li muskarci '%s'? ", meso[i].type);
                scanf("%d", &zeliMeso);
                meso[i].boolMale = (zeliMeso == 1);
            }
            break;
        case 2:
            printf("ZENE (1 za DA, 0 za NE):\n");
            odabranoZaZene = 1;
            for (i = 0; i < 4; i++) {
                printf("Zele li zene '%s'? ", meso[i].type);
                scanf("%d", &zeliMeso);
                meso[i].boolFemale = (zeliMeso == 1);
            }
            break;
        case 3:
            printf("DJECA (1 za DA, 0 za NE):\n");
            odabranoZaDjecu = 1;
            for (i = 0; i < 4; i++) {
                printf("Zele li djeca '%s'? ", meso[i].type);
                scanf("%d", &zeliMeso);
                meso[i].boolKids = (zeliMeso == 1);
            }
            break;
        default:
            printf("Nepoznat odabir.\n");
            break;
        }
    }

    if (!odabranoZaMuskarce) for (i = 0; i < 4; i++) meso[i].boolMale = 1;
    if (!odabranoZaZene) for (i = 0; i < 4; i++) meso[i].boolFemale = 1;
    if (!odabranoZaDjecu) for (i = 0; i < 4; i++) meso[i].boolKids = 1;
}

static inline void resetirajKolicineMesa(Meso* meso) {
    for (int i = 0; i < 4; i++) {
        meso[i].qGr = 0;
        meso[i].qKom = 0;
    }
}

void izracunajCijenu(Korisnici* korisnici, Meso* meso, RostiljPaket* paketi, int brojPaketa, int* provjeraUnosa, int* provjeraOdabira) {
    if (*provjeraUnosa == 0) {
        printf("\nKorisnik nije unio clanove, pripisujem jednog clana svakoj grupi!\n");
        korisnici->male = 1;
        korisnici->female = 1;
        korisnici->kids = 1;
    }
    if (*provjeraOdabira == 0) {
        printf("\nKorisnik nije birao meso, svim grupama pripisane sve vrste!\n\n");
        for (int i = 0; i < 4; i++) {
            meso[i].boolMale = 1;
            meso[i].boolFemale = 1;
            meso[i].boolKids = 1;
        }
    }

    resetirajKolicineMesa(meso);

    int faktorM = 0, faktorF = 0, faktorK = 0;
    for (int i = 0; i < 4; i++) {
        if (meso[i].boolMale) faktorM++;
        if (meso[i].boolFemale) faktorF++;
        if (meso[i].boolKids) faktorK++;
    }

    double maleQ = (faktorM > 0) ? (340.0 * korisnici->male) / faktorM : 0;
    double femaleQ = (faktorF > 0) ? (280.0 * korisnici->female) / faktorF : 0;
    double kidsQ = (faktorK > 0) ? (150.0 * korisnici->kids) / faktorK : 0;

    for (int i = 0; i < 4; i++) {
        if (meso[i].boolMale) meso[i].qGr += maleQ;
        if (meso[i].boolFemale) meso[i].qGr += femaleQ;
        if (meso[i].boolKids) meso[i].qGr += kidsQ;
    }

    int ukupnaCijena = 0;
    int* paketiCount = (int*)calloc(brojPaketa, sizeof(int));
    if (paketiCount == NULL && brojPaketa > 0) {
        perror("Greska pri alokaciji memorije za brojac paketa");
        return;
    }

    while (true) {
        bool trebaJosMesa = false;
        for (int i = 0; i < 4; i++) {
            if (meso[i].qGr > 50) {
                trebaJosMesa = true;
                break;
            }
        }
        if (!trebaJosMesa) break;

        for (int i = 0; i < brojPaketa; i++) {
            while (meso[0].qGr >= paketi[i].cevapciciGr &&
                meso[1].qGr >= paketi[i].piletinaGr &&
                meso[2].qGr >= paketi[i].kobasiceGr &&
                meso[3].qGr >= paketi[i].kotletiGr)
            {
                meso[0].qGr -= paketi[i].cevapciciGr;
                meso[1].qGr -= paketi[i].piletinaGr;
                meso[2].qGr -= paketi[i].kobasiceGr;
                meso[3].qGr -= paketi[i].kotletiGr;
                ukupnaCijena += paketi[i].cijenaEur;
                paketiCount[i]++;
            }
        }

        while (meso[0].qGr > 50) {
            if (meso[0].eur2000g > 0 && meso[0].qGr >= 1001) { meso[0].qGr -= 2000; ukupnaCijena += meso[0].eur2000g; meso[0].qKom += 2000; }
            else if (meso[0].eur1000g > 0 && meso[0].qGr >= 501) { meso[0].qGr -= 1000; ukupnaCijena += meso[0].eur1000g; meso[0].qKom += 1000; }
            else if (meso[0].eur500g > 0) { meso[0].qGr -= 500; ukupnaCijena += meso[0].eur500g; meso[0].qKom += 500; }
            else break;
        }

        while (meso[1].qGr > 50) {
            if (meso[1].eur1000g > 0 && meso[1].qGr >= 501) { meso[1].qGr -= 1000; ukupnaCijena += meso[1].eur1000g; meso[1].qKom += 1000; }
            else if (meso[1].eur500g > 0) { meso[1].qGr -= 500; ukupnaCijena += meso[1].eur500g; meso[1].qKom += 500; }
            else break;
        }

        while (meso[2].qGr > 50) {
            if (meso[2].eur800g > 0 && meso[2].qGr >= 401) { meso[2].qGr -= 800; ukupnaCijena += meso[2].eur800g; meso[2].qKom += 800; }
            else if (meso[2].eur400g > 0) { meso[2].qGr -= 400; ukupnaCijena += meso[2].eur400g; meso[2].qKom += 400; }
            else break;
        }

        while (meso[3].qGr > 50) {
            if (meso[3].eur1000g > 0 && meso[3].qGr >= 501) { meso[3].qGr -= 1000; ukupnaCijena += meso[3].eur1000g; meso[3].qKom += 1000; }
            else if (meso[3].eur500g > 0) { meso[3].qGr -= 500; ukupnaCijena += meso[3].eur500g; meso[3].qKom += 500; }
            else break;
        }

        for (int i = 0; i < 4; i++) { if (meso[i].qGr <= 50) meso[i].qGr = 0; }
    }

    printf("\n--- REZULTAT IZRACUNA ---\n");
    printf("UKUPNA CIJENA: %d EUR\n", ukupnaCijena);

    printf("\nARTIKLI:\n");
    for (int i = 0; i < brojPaketa; i++) {
        if (paketiCount[i] > 0) {
            printf("- %s: %d kom\n", paketi[i].naziv, paketiCount[i]);
        }
    }
    if (meso[0].qKom > 0) { printf("- %s (pojedinacno): %d grama\n", meso[0].type, meso[0].qKom); }
    if (meso[1].qKom > 0) { printf("- %s (pojedinacno): %d grama\n", meso[1].type, meso[1].qKom); }
    if (meso[2].qKom > 0) { printf("- %s (pojedinacno): %d grama\n", meso[2].type, meso[2].qKom); }
    if (meso[3].qKom > 0) { printf("- %s (pojedinacno): %d grama\n", meso[3].type, meso[3].qKom); }


    free(paketiCount);

    char nastavak[4];
    printf("\nNova kalkulacija (DA/NE)? ");
    scanf("%3s", nastavak);

    if (strcmp(nastavak, "DA") == 0 || strcmp(nastavak, "da") == 0) {
        *provjeraUnosa = 0;
        *provjeraOdabira = 0;
        return;
    }
    else {
        printf("Vracanje na glavni izbornik...\n");
    }
}




void postaviPocetneVrijednostiMesa(Meso* meso) {
    strcpy(meso[0].type, "Cevapcici");
    meso[0].eur400g = 0; meso[0].eur500g = 3; meso[0].eur800g = 0; meso[0].eur1000g = 5; meso[0].eur2000g = 8;
    strcpy(meso[1].type, "Piletina");
    meso[1].eur400g = 0; meso[1].eur500g = 5; meso[1].eur800g = 0; meso[1].eur1000g = 8; meso[1].eur2000g = 0;
    strcpy(meso[2].type, "Kobasice");
    meso[2].eur400g = 2; meso[2].eur500g = 0; meso[2].eur800g = 4; meso[2].eur1000g = 0; meso[2].eur2000g = 0;
    strcpy(meso[3].type, "Kotleti");
    meso[3].eur400g = 0; meso[3].eur500g = 3; meso[3].eur800g = 0; meso[3].eur1000g = 5; meso[3].eur2000g = 0;
}
