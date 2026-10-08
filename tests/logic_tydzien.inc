/* WYGENEROWANE przez extract_tydzien.py - nie edytowac recznie */

int ktoraKomora(uint16_t mV) {
  if (mV > PROGI[7]) return -1;
  if (mV < PROGI[0]) return -2;
  for (int i = 6; i >= 0; i--) if (mV >= PROGI[i]) return i;
  return -2;
}

const char* opisKomory(int k) {
  if (k == -1) return "zamkniete";
  if (k == -2) return "kilka naraz";
  if (k < 0 || k >= 7) return "?";
  static char buf[12];
  snprintf(buf, sizeof(buf), "komora %d", k + 1);
  return buf;
}

static int32_t dniOdEry(int r, int m, int d) {        // algorytm Hinnanta
  r -= (m <= 2);
  const int32_t  era = (r >= 0 ? r : r - 399) / 400;
  const uint32_t yoe = (uint32_t)(r - era * 400);
  const uint32_t doy = (153u * (uint32_t)(m + (m > 2 ? -3 : 9)) + 2u) / 5u + (uint32_t)d - 1u;
  const uint32_t doe = yoe * 365u + yoe/4u - yoe/100u + doy;
  return era * 146097 + (int32_t)doe - 719468;
}

int32_t numerDoby(time_t t) {
  if (t < 1600000000) return -1;               // zegar jeszcze nie ustawiony
  time_t przesuniety = t - (time_t)DAY_START_HOUR * 3600;
  struct tm lt;
  localtime_r(&przesuniety, &lt);
  return dniOdEry(lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday);
}

time_t poczatekDoby(time_t t) {
  time_t przesuniety = t - (time_t)DAY_START_HOUR * 3600;
  struct tm lt;
  localtime_r(&przesuniety, &lt);
  return t - (time_t)(lt.tm_hour * 3600L + lt.tm_min * 60L + lt.tm_sec);
}

int minutyDnia() {
  time_t t = time(nullptr);
  struct tm lt;
  localtime_r(&t, &lt);
  return lt.tm_hour * 60 + lt.tm_min;
}

int slotTeraz() {
  if (!rtcCzasPewny || slotowIle <= 0) return -1;
  int m = minutyDnia();
  int best = -1, najblizej = 100000;
  for (int i = 0; i < slotowIle; i++) {
    int po = (m - slotyMin[i] + 1440) % 1440;     // ile minut PO godzinie
    if (po <= SLOT_OKNO_MIN && po < najblizej) { najblizej = po; best = i; }
  }
  return best;
}

int kolejkaIle() { return nvs.getInt("q_ile", 0); }

uint64_t sekundDoNastepnego() {
  /*  Przypomnienie, ktore nikogo nie zastalo, wraca za chwile - to jest
      cala jego wartosc. Dopiero po ALARM_PONOWIEN probach slot milknie
      do konca doby.                                                   */
  if (ponowAlarm) return (uint64_t)ALARM_PONOW_MIN * 60;
  if (!rtcCzasPewny) return 15 * 60;              // bez zegara: co 15 minut

  int teraz = minutyDnia();
  int najblizej = 24 * 60;
  for (int i = 0; i < slotowIle; i++) {
    int d = slotyMin[i] - teraz;
    if (d <= 0) d += 24 * 60;
    if (d < najblizej) najblizej = d;
  }
  /* Granica doby tez musi nas obudzic - tam zapada decyzja "missed". */
  int doGranicy = DAY_START_HOUR * 60 - teraz;
  if (doGranicy <= 0) doGranicy += 24 * 60;
  if (doGranicy < najblizej) najblizej = doGranicy;

  if (kolejkaIle() > 0 && najblizej > 30) najblizej = 30;   // zalegle zdarzenia

  /*  NA LADOWARCE MELDUJEMY SIE CZESCIEJ  (D143).

      Bez tego Kuba podlaczylby kabel i nie zobaczyl w aplikacji nic az
      do nastepnego wybudzenia - czyli nawet kilka godzin. Pytanie
      brzmialo wprost: "jak bedzie mi sie ladowalo, to sie pokaze, ze sie
      laduje". Pokaze sie, jesli pudelko ma kiedy o tym powiedziec.

      Prad plynie z kabla, wiec czestsze wybudzenia nic nie kosztuja -
      i same sie koncza: tempo mierzymy przy kazdym wybudzeniu, a po
      odlaczeniu ladowarki schodzi ponizej progu i wracamy do zwyklego
      rytmu. Pudelko dzienne robi dokladnie to samo, tylko ladowanie
      zgaduje z napiecia zamiast je mierzyc.                          */
  if (battLaduje && najblizej > 1) najblizej = 1;

  return (uint64_t)najblizej * 60;
}
