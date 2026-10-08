/* =====================================================================
 *  Testy logiki pudelka TYGODNIOWEGO. Kompiluja PRAWDZIWE funkcje
 *  wyciete z PillBoxWeek.ino (logic_tydzien.inc) na atrapach Arduino.
 *
 *      python3 extract_tydzien.py && g++ -O0 -std=c++17 test_tydzien.cpp -o t && ./t
 *
 *  DLACZEGO TO W OGOLE POWSTALO (D145)
 *
 *  Do 0.15.0 pudelko tygodniowe nie mialo ani jednego testu jednostkowego.
 *  Stala za nim kontrola statyczna - a ta czyta kod, nie uruchamia go.
 *  Kosztowalo to dwa bledy w rzeczach, ktore sprawdza sie na biurku
 *  w milisekundy: meldunek o otwarciu wychodzil tylko przy nowej dawce
 *  (B31), a drugie przypomnienie w ciagu dnia nie dzwonilo nigdy (D145).
 *  Prosba Kuby brzmiala wprost: "zobacz, czy przypomnienia faktycznie
 *  dzialaja, i pls sprawdz to dobrze".
 *
 *  STREFA CZASU: testy ustawiaja TZ=UTC, zeby wynik nie zalezal od
 *  maszyny. Na plytce strefe ustawia configTzTime(); tu liczy sie
 *  wylacznie to, czy arytmetyka godzin i doby jest poprawna.
 * ===================================================================== */
#include "arduino_shim.h"
#include "../firmware/PillBoxWeek/config.h"

time_t FAKE_NOW = 0;
unsigned long FAKE_MILLIS = 0;
FakeSerial Serial;

/* ---------- globalne, ktorych uzywa wycieta logika ---------- */
Preferences nvs;
static const uint16_t PROGI[8] = PROGI_KLAPEK;

int   slotyMin[SLOTOW_MAX] = { 20 * 60 };
int   slotowIle   = 1;
bool  rtcCzasPewny = true;
bool  ponowAlarm   = false;
bool  battLaduje   = false;

#include "logic_tydzien.inc"

/* ---------- mikro-framework ---------- */
int PASS = 0, FAIL = 0;
#define CHECK(cond, ...) do{ if(cond){PASS++;} else {FAIL++; \
  printf("  FAIL  %s:%d  ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } }while(0)
void head(const char* t){ printf("\n=== %s ===\n", t); }

time_t utc(int y,int mo,int d,int h,int mi,int s=0){
  struct tm t{}; t.tm_year=y-1900; t.tm_mon=mo-1; t.tm_mday=d;
  t.tm_hour=h; t.tm_min=mi; t.tm_sec=s;
  return timegm(&t);
}
/* Ustawia zegar pudelka na podana godzine. */
void ozegarze(int h, int mi){ FAKE_NOW = utc(2026,10,8,h,mi,0); }

int main(){
  setenv("TZ", "UTC", 1); tzset();

/* ================= 1. DRABINKA ================= */
head("Drabinka - ktora klapka");
{
  /* Progi pochodza Z POMIARU na plytce (config.h). Sprawdzamy srodki
     pasm, a nie same progi: na plytce napiecie nie jest idealne.     */
  const uint16_t srodki[7] = { 105, 170, 300, 388, 557, 633, 761 };
  for (int i = 0; i < 7; i++)
    CHECK(ktoraKomora(srodki[i]) == i,
          "%u mV to komora %d, a wyszlo %d", srodki[i], i, ktoraKomora(srodki[i]));

  CHECK(ktoraKomora(1125) == -1, "wszystko zamkniete = -1");
  CHECK(ktoraKomora(2000) == -1, "napiecie wyzsze niz zamkniete tez jest zamkniete");

  /*  KILKA KLAPEK NARAZ TO NAPELNIANIE, NIE DAWKA. Zapisane jako wziecie
      zmyliloby kalendarz na caly tydzien w przod.                      */
  CHECK(ktoraKomora(30) == -2, "ponizej najnizszego progu = kilka naraz");
  CHECK(ktoraKomora(0)  == -2, "zwarcie do masy tez");

  /*  GRANICE PASM. Prog nalezy do komory, ktora zaczyna - inaczej jedno
      drgniecie napiecia przerzuca dzien na sasiednia przegrodke.      */
  CHECK(ktoraKomora(72)  == 0, "dokladnie prog[0] to juz komora 1");
  CHECK(ktoraKomora(71)  == -2, "miliwolt nizej to kilka naraz");
  CHECK(ktoraKomora(943) == 6, "dokladnie prog[7] to jeszcze komora 7");
  CHECK(ktoraKomora(944) == -1, "miliwolt wyzej to zamkniete");
}

head("Komora nie ma dnia tygodnia");
{
  /*  D140: numer komory zostaje, jego tlumaczenie na dzien odpada -
      przypisanie zylo w kolejnosci rezystorow, czyli w niczym.       */
  CHECK(strcmp(opisKomory(0), "komora 1") == 0, "komory liczymy od jedynki: %s", opisKomory(0));
  CHECK(strcmp(opisKomory(6), "komora 7") == 0, "...do siodemki: %s", opisKomory(6));
  CHECK(strcmp(opisKomory(-1), "zamkniete") == 0, "zamkniete mowimy slowem");
  CHECK(strcmp(opisKomory(-2), "kilka naraz") == 0, "napelnianie tez");
  for (const char* d : { "PON","WT","SR","CZW","PT","SOB","ND" })
    for (int k = -2; k < 8; k++)
      CHECK(strcmp(opisKomory(k), d) != 0, "zaden opis komory nie jest dniem tygodnia");
}

/* ================= 2. PRZYPOMNIENIA ================= */
head("Ktore przypomnienie wypada teraz");
{
  rtcCzasPewny = true;
  slotowIle = 1; slotyMin[0] = 20*60;

  ozegarze(20, 0);
  CHECK(slotTeraz() == 0, "o 20:00 wypada przypomnienie z 20:00");
  ozegarze(20, 29);
  CHECK(slotTeraz() == 0, "29 minut po - jeszcze to samo (spoznione wybudzenie)");
  ozegarze(20, 31);
  CHECK(slotTeraz() == -1, "31 minut po - juz nie, okno sie zamknelo");

  /*  NIE DZWONIMY PRZED CZASEM (D145).

      Stara wersja liczyla `abs()`, czyli okno w OBIE strony. Wybudzenie
      o 19:35 - a takie sie zdarza, np. po zaleglym wpisie w kolejce -
      odpalalo przypomnienie o 20:00 dwadziescia piec minut za wczesnie. */
  ozegarze(19, 35);
  CHECK(slotTeraz() == -1, "25 minut PRZED godzina to nie jest ta godzina");
  ozegarze(19, 59);
  CHECK(slotTeraz() == -1, "minute przed tez nie");

  /*  DWA PRZYPOMNIENIA BLISKO SIEBIE (D145).

      To jest ten blad, przez ktory "wiecej niz jedna godzina" nie
      dzialalo: stara petla brala PIERWSZY slot w oknie, wiec o 20:15
      trafiala w slot 20:00 - a ten byl juz wyczerpany, wiec drugie
      przypomnienie nie dzwonilo NIGDY.                                */
  slotowIle = 2; slotyMin[0] = 20*60; slotyMin[1] = 20*60 + 15;
  ozegarze(20, 0);
  CHECK(slotTeraz() == 0, "o 20:00 - pierwsze");
  ozegarze(20, 10);
  CHECK(slotTeraz() == 0, "o 20:10 nadal pierwsze - drugie jeszcze nie nadeszlo");
  ozegarze(20, 15);
  CHECK(slotTeraz() == 1, "o 20:15 DRUGIE, nie pierwsze");
  ozegarze(20, 20);
  CHECK(slotTeraz() == 1, "o 20:20 blizej jest drugie");

  /*  PRZEZ POLNOC. Przypomnienie o 23:50 ogladane o 00:05 to 15 minut
      po nim, a nie 1425.                                             */
  slotowIle = 1; slotyMin[0] = 23*60 + 50;
  ozegarze(0, 5);
  CHECK(slotTeraz() == 0, "23:50 ogladane o 00:05 to wciaz to przypomnienie");
  ozegarze(0, 21);
  CHECK(slotTeraz() == -1, "ale 31 minut po polnocy juz nie");

  /*  BEZ ZEGARA ANI SLOWA. Przypomnienie o losowej porze uczy ignorowac
      pudelko - co jest gorsze niz jego brak.                          */
  rtcCzasPewny = false;
  slotowIle = 1; slotyMin[0] = 20*60;
  ozegarze(20, 0);
  CHECK(slotTeraz() == -1, "bez pewnego zegara nie dzwonimy wcale");
  rtcCzasPewny = true;

  /*  PUSTY HARMONOGRAM to nie jest godzina 00:00. */
  slotowIle = 0;
  ozegarze(0, 0);
  CHECK(slotTeraz() == -1, "brak przypomnien to brak przypomnien");
  slotowIle = 1;
}

head("Dwanascie przypomnien miesci sie w pudelku");
{
  /*  Aplikacja pozwala ustawic do 12 godzin (MAX_PRZYPOMNIEN). Do 0.14.0
      pudelko tygodniowe mialo SLOTOW_MAX 4 i po cichu ucinalo reszte -
      aplikacja pokazywala szesc przypomnien, a pudelko znalo cztery.  */
  CHECK(SLOTOW_MAX >= 12, "pudelko miesci tyle przypomnien, ile daje ustawic aplikacja (%d)",
        SLOTOW_MAX);

  rtcCzasPewny = true;
  slotowIle = 12;
  for (int i = 0; i < 12; i++) slotyMin[i] = (8 + i) * 60;   // 8:00 ... 19:00
  ozegarze(19, 0);
  CHECK(slotTeraz() == 11, "dwunaste przypomnienie tez da sie trafic (%d)", slotTeraz());
  ozegarze(8, 0);
  CHECK(slotTeraz() == 0, "i pierwsze nadal dziala");

  /*  Maska wyczerpanych slotow musi miescic KAZDY z nich. Przy uint8_t
      dwunasty bit wypadal poza typ i slot nigdy sie nie zamykal.      */
  uint16_t maska = 0;
  for (int i = 0; i < SLOTOW_MAX; i++) maska |= (uint16_t)(1u << i);
  for (int i = 0; i < SLOTOW_MAX; i++)
    CHECK(maska & (uint16_t)(1u << i), "slot %d miesci sie w masce", i);

  slotowIle = 1; slotyMin[0] = 20*60;
}

/* ================= 3. KIEDY SIE OBUDZIC ================= */
head("Sen do nastepnego przypomnienia");
{
  rtcCzasPewny = true; ponowAlarm = false; battLaduje = false;
  nvs.in.clear();
  slotowIle = 1; slotyMin[0] = 20*60;

  ozegarze(12, 0);
  CHECK(sekundDoNastepnego() == 8*3600, "o 12:00 spimy 8 h do 20:00 (%llu)",
        (unsigned long long)sekundDoNastepnego());

  /*  GRANICA DOBY TEZ MUSI OBUDZIC - tam zapada decyzja "missed" (D64). */
  ozegarze(21, 0);
  CHECK(sekundDoNastepnego() == 6*3600, "po 20:00 najblizsza jest granica doby o 3:00 (%llu)",
        (unsigned long long)sekundDoNastepnego());

  /*  Z DWOCH GODZIN budzimy sie na blizsza. */
  slotowIle = 2; slotyMin[0] = 8*60; slotyMin[1] = 20*60;
  ozegarze(19, 30);
  CHECK(sekundDoNastepnego() == 30*60, "o 19:30 do 20:00 jest pol godziny (%llu)",
        (unsigned long long)sekundDoNastepnego());
  ozegarze(20, 30);
  CHECK(sekundDoNastepnego() == (6*60+30)*60, "a po 20:30 - do 3:00 (%llu)",
        (unsigned long long)sekundDoNastepnego());

  /*  ZALEGLE ZDARZENIA skracaja sen do 30 minut - dawka nie moze czekac
      w pamieci pudelka dluzej, niz musi.                              */
  slotowIle = 1; slotyMin[0] = 20*60;
  ozegarze(12, 0);
  nvs.putInt("q_ile", 3);
  CHECK(sekundDoNastepnego() == 30*60, "z kolejka wracamy po 30 minutach (%llu)",
        (unsigned long long)sekundDoNastepnego());
  nvs.putInt("q_ile", 0);

  /*  BEZ ZEGARA pudelko probuje co 15 minut - nie wie, kiedy jest 20:00. */
  rtcCzasPewny = false;
  CHECK(sekundDoNastepnego() == 15*60, "bez zegara co 15 minut (%llu)",
        (unsigned long long)sekundDoNastepnego());
  rtcCzasPewny = true;

  /*  PONOWIENIE ALARMU ma pierwszenstwo przed wszystkim - przypomnienie,
      ktore nikogo nie zastalo, wraca za chwile i w tym jest cala jego
      wartosc.                                                          */
  ponowAlarm = true;
  nvs.putInt("q_ile", 5);
  CHECK(sekundDoNastepnego() == (uint64_t)ALARM_PONOW_MIN*60,
        "ponowienie alarmu wygrywa z kolejka (%llu)", (unsigned long long)sekundDoNastepnego());
  ponowAlarm = false;
  nvs.putInt("q_ile", 0);

  /*  NA LADOWARCE budzimy sie co minute, zeby bylo widac, jak rosnie
      procent (D143). Prad plynie z kabla, wiec nic to nie kosztuje.   */
  battLaduje = true;
  ozegarze(12, 0);
  CHECK(sekundDoNastepnego() == 60, "na ladowarce meldujemy sie co minute (%llu)",
        (unsigned long long)sekundDoNastepnego());
  battLaduje = false;

  /*  ...ale alarm jest wazniejszy niz podglad ladowania. */
  battLaduje = true; ponowAlarm = true;
  CHECK(sekundDoNastepnego() == (uint64_t)ALARM_PONOW_MIN*60,
        "alarm wygrywa takze z ladowarka (%llu)", (unsigned long long)sekundDoNastepnego());
  battLaduje = false; ponowAlarm = false;
}

/* ================= 4. DOBA LEKOWA ================= */
head("Doba lekowa konczy sie o 3:00, nie o polnocy");
{
  /*  DAY_START_HOUR = 3, identycznie jak w aplikacji (ograniczenie 4).
      Klapka otwarta o 1:30 nalezy do dnia POPRZEDNIEGO.              */
  FAKE_NOW = utc(2026,10,8, 1,30,0);
  int32_t noca = numerDoby(FAKE_NOW);
  FAKE_NOW = utc(2026,10,7,23,30,0);
  CHECK(numerDoby(FAKE_NOW) == noca, "1:30 nalezy do doby poprzedniego dnia");

  FAKE_NOW = utc(2026,10,8, 3,30,0);
  CHECK(numerDoby(FAKE_NOW) == noca + 1, "3:30 to juz nastepna doba");

  FAKE_NOW = utc(2026,10,8, 2,59,0);
  CHECK(numerDoby(FAKE_NOW) == noca, "2:59 jeszcze nie");
  FAKE_NOW = utc(2026,10,8, 3, 0,0);
  CHECK(numerDoby(FAKE_NOW) == noca + 1, "3:00 juz tak");

  /*  BEZ ZEGARA numeru doby nie zgadujemy - zgadniety skasowalby ten
      prawdziwy w pamieci RTC.                                        */
  CHECK(numerDoby(1000) == -1, "zegar sprzed 2020 roku to brak zegara");

  /*  Poczatek doby lekowej zawierajacej dany moment. */
  FAKE_NOW = utc(2026,10,8,20,0,0);
  CHECK(poczatekDoby(FAKE_NOW) == utc(2026,10,8,3,0,0),
        "doba zawierajaca 20:00 zaczela sie tego dnia o 3:00");
  FAKE_NOW = utc(2026,10,8,1,0,0);
  CHECK(poczatekDoby(FAKE_NOW) == utc(2026,10,7,3,0,0),
        "a doba zawierajaca 1:00 - poprzedniego dnia o 3:00");
}

printf("\n──────────────────────────────────────\n");
printf("  ZALICZONE: %d    BLEDY: %d\n", PASS, FAIL);
printf("──────────────────────────────────────\n");
return FAIL ? 1 : 0;
}
