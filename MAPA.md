# Mapa kodu — gdzie co stoi

**Plik jest generowany** (`python3 tests/mapa.py`, robi to tez `tests/run_all.sh`).
Nie poprawiaj recznie — zmiany przepadna przy najblizszym przebiegu testow.

Po co: `index.html` i `PillBox.ino` maja razem ~220 tys. tokenow. Wczytanie
ktoregokolwiek w calosci kosztuje wiecej niz cale zadanie, ktore go dotyczy.
Majac zakres linii czyta sie fragment:

```bash
sed -n '2800,2960p' index.html          # jeden obszar
grep -n "nazwaFunkcji" index.html       # gdy znasz nazwe
```

## `index.html` — 8002 linii, ~130 tys. tokenow

Ekrany (`<section>`) i dwa duze bloki. Zakladki `tab-*` odpowiadaja
pozycjom w pasku nawigacji i podekranom Ustawien.

| od | do | co |
|---|---|---|
| 21 | 21 | CSS — poczatek |
| 22 | 222 | SYSTEM WIZUALNY PillBox |
| 223 | 319 | EKRAN GŁÓWNY — KARTA DNIA |
| 320 | 392 | INFORMACJA ZWROTNA |
| 393 | 769 | TABLETKA W 3D |
| 770 | 929 | tab-cal |
| 930 | 978 | tab-inr |
| 979 | 1051 | tab-ana |
| 1052 | 1127 | tab-set |
| 1128 | 1194 | tab-lek |
| 1195 | 1214 | tab-sinr |
| 1215 | 1254 | tab-wifi |
| 1255 | 1355 | tab-tg |
| 1356 | 1389 | tab-dev |
| 1390 | 1503 | tab-diag |
| 1504 | 1782 | tab-help |
| 1783 | 1804 | tab-ev |
| 1805 | 1821 | tab-hist |
| 1822 | 1905 | tab-lid |
| 1906 | 1914 | JS — poczatek |
| 1915 | 1970 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 1971 | 2045 | INFORMACJA ZWROTNA |
| 2046 | 2140 | STREFY CZASOWE |
| 2141 | 2168 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2169 | 2235 | TABLETKA JAKO BRYŁA |
| 2236 | 2250 | LOGOWANIE |
| 2251 | 2390 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2391 | 2749 | START |
| 2750 | 2810 | OSŁONA RYSOWANIA |
| 2811 | 3007 | REKONCYLIACJA |
| 3008 | 3066 | KALENDARZ |
| 3067 | 3333 | HISTORIA ROZPISANIA DAWKI |
| 3334 | 3515 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3516 | 3667 | ARKUSZ DNIA |
| 3668 | 3763 | WZIĄŁEM TERAZ |
| 3764 | 3863 | INR |
| 3864 | 4003 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4004 | 4016 | STATUS PUDEŁKA |
| 4017 | 4167 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4168 | 4272 | DZIENNIK WIECZKA — narzędzie na czas testu terenowego. |
| 4273 | 4442 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4443 | 4676 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4677 | 4717 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4718 | 4811 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 4812 | 5117 | EKRAN ZDARZEN |
| 5118 | 5236 | ZAPAS TABLETEK |
| 5237 | 5466 | USTAWIENIA |
| 5467 | 5774 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 5775 | 6231 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6232 | 6593 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 6594 | 6793 | ANALIZA |
| 6794 | 7179 | WYKRESY ANALIZY |
| 7180 | 7331 | RAPORT |
| 7332 | 7393 | KONTEKST DNIA (TAGI) |
| 7394 | 7429 | KOPIA ZAPASOWA |
| 7430 | 7643 | KOPIA NA TELEGRAM |
| 7644 | 7709 | WIEK KOPII |
| 7710 | 7807 | ODTWARZANIE Z KOPII |
| 7808 | 7877 | KOPIE Z BAZY |
| 7878 | 7960 | NAWIGACJA |
| 7961 | 8002 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (209) — nazwa i linia deklaracji:

*INFORMACJA ZWROTNA* — `toast`&nbsp;1992, `busy`&nbsp;2008, `todayKey`&nbsp;2035, `dzisiajKey`&nbsp;2039, `inNightWindow`&nbsp;2042

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2097, `tzName`&nbsp;2113, `tzLabel`&nbsp;2114, `tzOffsetTxt`&nbsp;2115, `devDate`&nbsp;2121, `devKey`&nbsp;2126, `devHM`&nbsp;2131, `slotMin`&nbsp;2132, `pillColors`&nbsp;2134

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2145

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2182, `cieniuj`&nbsp;2209, `doseGraphic`&nbsp;2226

*LOGOWANIE* — `doLogin`&nbsp;2240

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2260, `wyczyscCache`&nbsp;2355, `fbSignOut`&nbsp;2374

*START* — `boot`&nbsp;2392

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;2777, `rysujWszystkie`&nbsp;2790, `renderAll`&nbsp;2794

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;2817, `reconcileDecyzja`&nbsp;2856, `zapiszReconcile`&nbsp;2873, `doReconcile`&nbsp;2890, `reconcile`&nbsp;3006

*KALENDARZ* — `tydzienDawek`&nbsp;3046

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3092, `dawkaNaDzien`&nbsp;3103, `dzienBezLeku`&nbsp;3124, `wyjatekNaDzien`&nbsp;3129, `opisDawkowania`&nbsp;3135, `dayDose`&nbsp;3145, `dzienZamkniety`&nbsp;3179, `trackingSince`&nbsp;3185, `beforeTracking`&nbsp;3186, `dayStatus`&nbsp;3188, `renderCalendar`&nbsp;3229, `seriaDni`&nbsp;3294, `doNastepnej`&nbsp;3312, `opisCzasu`&nbsp;3327

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3342, `trwanieTxt`&nbsp;3357, `kiedyDawkaTxt`&nbsp;3368, `odswiezOdDawki`&nbsp;3376, `startTikOdDawki`&nbsp;3390, `renderToday`&nbsp;3400

*ARKUSZ DNIA* — `closeSheet`&nbsp;3527, `renderSheet`&nbsp;3529, `resetDose`&nbsp;3607, `resetPlan`&nbsp;3614, `commitPlan`&nbsp;3619, `clearPlan`&nbsp;3635, `commitDose`&nbsp;3647

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3688, `askConfirm`&nbsp;3753

*INR* — `inrState`&nbsp;3765, `odswiezTerminInr`&nbsp;3778, `addInr`&nbsp;3787, `inrKeysOk`&nbsp;3858

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;3873, `inrTerminKey`&nbsp;3881, `inrDoTerminu`&nbsp;3893, `dniTxt`&nbsp;3902, `renderInr`&nbsp;3904, `inrChart`&nbsp;3976

*STATUS PUDEŁKA* — `relTime`&nbsp;4005, `devDayMon`&nbsp;4014

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4049, `renderBoxLog`&nbsp;4095, `logPrzelacz`&nbsp;4131, `renderNvsFailLog`&nbsp;4140

*DZIENNIK WIECZKA — narzędzie na czas testu terenowego.* — `lidPaczki`&nbsp;4189, `lidWpisy`&nbsp;4195, `renderLidLog`&nbsp;4208

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4290, `oczekWczytaj`&nbsp;4298, `oczekZapisz`&nbsp;4303, `oczekIle`&nbsp;4306, `zapiszPewnie`&nbsp;4316, `zapiszCfg`&nbsp;4358, `bazaOdmowila`&nbsp;4376, `oczekWyslij`&nbsp;4400, `oczekOdmowy`&nbsp;4441

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4457, `ostrzReguly`&nbsp;4479, `lm`&nbsp;4518, `ostrzMilczy`&nbsp;4524, `nvsMalo`&nbsp;4571, `opisNvsFailKey`&nbsp;4583, `stratyDotyczaLeku`&nbsp;4620, `ostrzStraty`&nbsp;4632, `stratyCicho`&nbsp;4666

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4698

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4728, `renderOstrzezenia`&nbsp;4743, `bezPokrycia`&nbsp;4753, `wierszZdarzenia`&nbsp;4759, `renderDiag`&nbsp;4776

*EKRAN ZDARZEN* — `evFiltr`&nbsp;4828, `evPasuje`&nbsp;4833, `renderEvents`&nbsp;4845, `renderOpenWarn`&nbsp;4885, `minutyDoPelna`&nbsp;4938, `opisLadowania`&nbsp;4950, `dni`&nbsp;4970, `opisLadowan`&nbsp;4973, `renderStatus`&nbsp;4990

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5121, `dayAfter`&nbsp;5124, `pillsBaseInfo`&nbsp;5135, `settlePills`&nbsp;5145, `dniZapasu`&nbsp;5185, `renderPills`&nbsp;5198, `savePills`&nbsp;5219, `setPills`&nbsp;5230

*USTAWIENIA* — `renderKafelki`&nbsp;5241, `renderSettings`&nbsp;5268, `tydzienZPol`&nbsp;5312, `renderWeekEditor`&nbsp;5324, `odswiezPodpowiedzTygodnia`&nbsp;5341, `tydzienZmieniony`&nbsp;5355, `rownajTydzien`&nbsp;5356, `renderPlanList`&nbsp;5369, `renderExceptions`&nbsp;5396, `wyslijSiec`&nbsp;5442

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5484, `tgZapytaj`&nbsp;5494, `tgKodParowania`&nbsp;5534, `tgZnajdzCzat`&nbsp;5550, `tgPolacz`&nbsp;5618, `tgProbna`&nbsp;5651, `tgOdlacz`&nbsp;5658, `renderTgStan`&nbsp;5680

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;5795, `pobierzOpisFirmware`&nbsp;5801, `wyslijAktualizacje`&nbsp;5822, `anulujAktualizacje`&nbsp;5867, `renderOta`&nbsp;5873, `renderNetStan`&nbsp;6158

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6243, `renderSkan`&nbsp;6249, `szukajSieci`&nbsp;6301, `wybierzSiec`&nbsp;6309, `wyslijPolecenieSieci`&nbsp;6327, `siecZIndeksu`&nbsp;6337, `tzChanged`&nbsp;6371, `cfgTime`&nbsp;6376, `addSlot`&nbsp;6384, `zapiszPlanDnia`&nbsp;6397, `saveConfig`&nbsp;6415, `inrKrokiZakresu`&nbsp;6476, `opcjeInr`&nbsp;6483, `inrZakresZmieniony`&nbsp;6494, `wypelnijListyZakresu`&nbsp;6507, `saveInrRange`&nbsp;6518, `wypelnijListeOdstepu`&nbsp;6552, `saveInrEvery`&nbsp;6565, `odswiezPodpowiedzInr`&nbsp;6575

*ANALIZA* — `openTimeOf`&nbsp;6600, `openMinutes`&nbsp;6606, `sredniaPora`&nbsp;6630, `kwantyl`&nbsp;6638, `dniMiedzy`&nbsp;6646, `odstepyZPunktow`&nbsp;6660, `analyze`&nbsp;6669, `inrContext`&nbsp;6760

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;6819, `rytmSVG`&nbsp;6831, `poryWCzasieSVG`&nbsp;6893, `iskraSVG`&nbsp;6961, `dowSVG`&nbsp;6989, `dniRytmu`&nbsp;7025, `skutecznoscTygodniami`&nbsp;7046, `renderAnalysis`&nbsp;7074

*RAPORT* — `collectRows`&nbsp;7181, `makeReport`&nbsp;7218

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7364, `tagiPrzed`&nbsp;7372, `tagPrzelacz`&nbsp;7381

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7414, `opisKopii`&nbsp;7424

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7464, `tgCzatKopii`&nbsp;7471, `odswiezKopie`&nbsp;7478, `tgKopiaCzatZapisz`&nbsp;7486, `tgKopiaCzatZnajdz`&nbsp;7506, `tgKopiaWlacz`&nbsp;7536, `tgKopiaWylacz`&nbsp;7555, `kopiaNaTelegram`&nbsp;7564, `kopiaAutomat`&nbsp;7615

*WIEK KOPII* — `dniOdDaty`&nbsp;7662, `wiekKopiiTxt`&nbsp;7668, `renderKopiaStan`&nbsp;7676, `zapiszKopie`&nbsp;7691

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;7725, `wczytajKopie`&nbsp;7755, `kopiaCzytelna`&nbsp;7760, `odtworzKopie`&nbsp;7770, `kopiaWybrana`&nbsp;7793

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;7817, `odtworzZBazy`&nbsp;7849, `exportCsv`&nbsp;7861

*NAWIGACJA* — `wrocZEkranu`&nbsp;7959


---

## `firmware/PillBox/PillBox.ino` — 6211 linii

| od | do | blok |
|---|---|---|
| 1 | 63 | PillBox.ino  -  Inteligentne pudelko na leki / IoT Pill Reminder |
| 64 | 243 | PAMIEC RTC  (przezywa deep sleep, ginie po odlaczeniu zasilania) |
| 244 | 446 | STAN GLOBALNY |
| 447 | 667 | 1.  POMIAR BATERII |
| 668 | 830 | 2.  BUZZER  (pasywny piezo -> PWM przez LEDC) |
| 831 | 892 | 3.  GPIO / WYBUDZANIE |
| 893 | 978 | 4.  HARMONOGRAM |
| 979 | 1104 | 4a.  DNI BEZ LEKU |
| 1105 | 1125 | 4b.  PUDELKO ZOSTAWIONE OTWARTE |
| 1126 | 1349 | 4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego |
| 1350 | 1601 | 5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien) |
| 1602 | 1950 | 6.  WiFi |
| 1951 | 2953 | 7.  FIREBASE  (REST: Auth email/haslo + Realtime Database) |
| 2954 | 3293 | 8.  ZDARZENIA |
| 3294 | 3349 | 9.  ALARM |
| 3350 | 3568 | 10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth) |
| 3569 | 4060 | 10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59 |
| 4061 | 4180 | 10b. CZARNA SKRZYNKA |
| 4181 | 4483 | 10c. GESTY SERWISOWE I AUTOTEST |
| 4484 | 4906 | 10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67) |
| 4907 | 5477 | 11.  DEEP SLEEP |
| 5478 | 6211 | 12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany) |

**Funkcje** (172):

*STAN GLOBALNY* — `zanotujNvsFail`&nbsp;322, `nvsPutStr`&nbsp;341, `nvsPutU16`&nbsp;364, `nvsWolneWpisy`&nbsp;379, `syncTimeNTP`&nbsp;422, `logbookJson`&nbsp;423, `setTakenDay`&nbsp;424, `note`&nbsp;426, `awakeTooLong`&nbsp;440, `extendAwake`&nbsp;442

*1.  POMIAR BATERII* — `readBatteryRaw`&nbsp;455, `battPercentFromCurve`&nbsp;495, `resetBatteryFilter`&nbsp;527, `zapiszKoniecLadowania`&nbsp;550, `trackCharging`&nbsp;560, `battSmooth`&nbsp;614, `readBattery`&nbsp;645

*2.  BUZZER  (pasywny piezo -> PWM przez LEDC)* — `buzzerInit`&nbsp;671, `buzzerTone`&nbsp;680, `buzzerTonCicho`&nbsp;691, `buzzerOff`&nbsp;700, `beepAck`&nbsp;712, `beepErr`&nbsp;736, `beepQueued`&nbsp;746, `beepAlreadyTaken`&nbsp;756, `beepNowaWersja`&nbsp;781, `beepLowStock`&nbsp;791, `beepLowBattery`&nbsp;800, `beepBoxOpen`&nbsp;816, `beepCharging`&nbsp;824

*3.  GPIO / WYBUDZANIE* — `configureInputs`&nbsp;834, `boxIsOpen`&nbsp;839, `buttonPressed`&nbsp;840, `wakeName`&nbsp;842

*4.  HARMONOGRAM* — `parseSchedule`&nbsp;896, `loadSchedule`&nbsp;909, `saveSchedule`&nbsp;922, `localMinutesOfDay`&nbsp;933, `slotMinutes`&nbsp;940, `localDayNumber`&nbsp;949, `matchSlot`&nbsp;957, `secondsToDayBoundary`&nbsp;972

*4a.  DNI BEZ LEKU* — `localWeekday`&nbsp;999, `dateKeyToNum`&nbsp;1007, `dawkaNaDobe`&nbsp;1020, `dzisBezLeku`&nbsp;1030, `parseDoseWeek`&nbsp;1039, `parseDoseEx`&nbsp;1057, `saveDosing`&nbsp;1079, `loadDosing`&nbsp;1092

*4b.  PUDELKO ZOSTAWIONE OTWARTE* — `openWarnSecondsLeft`&nbsp;1116

*4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego* — `lidLogAdd`&nbsp;1159, `lidLogCount`&nbsp;1183, `jsonEscape`&nbsp;1193, `lidLogJson`&nbsp;1209, `lidLogClear`&nbsp;1239, `nvsFailLogDoWyslania`&nbsp;1254, `nvsFailLogJson`&nbsp;1264, `nvsFailLogOznaczWyslany`&nbsp;1282, `trackBoxOpen`&nbsp;1286, `secondsToNextSlot`&nbsp;1335

*5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien)* — `rekordTs`&nbsp;1366, `rekordBezDaty`&nbsp;1373, `tsDoBazy`&nbsp;1382, `queuePush`&nbsp;1386, `queueCount`&nbsp;1409, `queuePeek`&nbsp;1416, `queuePop`&nbsp;1431, `queueDrop`&nbsp;1450, `przesunZnaczniki`&nbsp;1474, `queueShiftTimestamps`&nbsp;1489, `queueNadajCzas`&nbsp;1537, `queueEpokaSkasuj`&nbsp;1580

*6.  WiFi* — `netKlucz`&nbsp;1619, `wifiSieciCount`&nbsp;1623, `wifiSiecSsid`&nbsp;1630, `wifiSiecPass`&nbsp;1639, `wifiListeZapisz`&nbsp;1666, `wifiListeCzytaj`&nbsp;1690, `wifiSiecDodaj`&nbsp;1703, `wifiSiecUsun`&nbsp;1734, `wifiSiecPriorytet`&nbsp;1767, `wifiSprobuj`&nbsp;1795, `wifiConnect`&nbsp;1813, `wifiOff`&nbsp;1882, `wifiUspij`&nbsp;1896, `syncTimeNTP`&nbsp;1901

*7.  FIREBASE  (REST: Auth email/haslo + Realtime Database)* — `tokenZPamieci`&nbsp;1968, `zapomnijToken`&nbsp;1977, `hasloJestPrawdziwe`&nbsp;2022, `hasloZPamieci`&nbsp;2027, `hasloWPamieci`&nbsp;2036, `hasloUtrwal`&nbsp;2040, `hasloDoLogowania`&nbsp;2053, `tgTokenZPamieci`&nbsp;2073, `tgChatZPamieci`&nbsp;2080, `tgSkonfigurowany`&nbsp;2089, `tgUtrwal`&nbsp;2096, `tgZapomnij`&nbsp;2108, `firebaseSignIn`&nbsp;2142, `rtdbUrl`&nbsp;2237, `rtdbSend`&nbsp;2259, `rekordKompletny`&nbsp;2286, `pushEventRecord`&nbsp;2295, `pushLidState`&nbsp;2352, `otaSumaZPamieci`&nbsp;2373, `otaSumaWgranej`&nbsp;2395, `pushStatus`&nbsp;2401, `fetchConfig`&nbsp;2596, `trwaleOdrzucony`&nbsp;2911, `flushQueue`&nbsp;2915

*8.  ZDARZENIA* — `makeRecordAt`&nbsp;2957, `makeRecord`&nbsp;2979, `loadDayMarkers`&nbsp;2989, `clearDayMarkers`&nbsp;3008, `setTakenDay`&nbsp;3022, `setRolloverDay`&nbsp;3030, `zapiszDawke`&nbsp;3060, `oznaczAlarmObsluzony`&nbsp;3103, `alarmJuzObsluzony`&nbsp;3120, `ostatniSlotDoby`&nbsp;3146, `juzDzisBrane`&nbsp;3156, `checkDayRollover`&nbsp;3163, `reportEvent`&nbsp;3220

*9.  ALARM* — `alarmPotwierdzony`&nbsp;3317, `runAlarmWindow`&nbsp;3322

*10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth)* — `htmlEscape`&nbsp;3363, `portalPage`&nbsp;3377, `startWifiPortal`&nbsp;3421

*10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59* — `otaOpisDecyzji`&nbsp;3693, `otaZanotujProbe`&nbsp;3719, `otaWyzerujLicznik`&nbsp;3727, `otaZlecenieWBazie`&nbsp;3757, `otaPobierzOpis`&nbsp;3772, `otaWgraj`&nbsp;3816, `otaSprawdzPoStarcie`&nbsp;3974, `otaPotwierdzDzialanie`&nbsp;4007

*10b. CZARNA SKRZYNKA* — `note`&nbsp;4081, `wartoZapisac`&nbsp;4088, `logbookAdd`&nbsp;4100, `logbookPrint`&nbsp;4139, `logbookJson`&nbsp;4163

*10c. GESTY SERWISOWE I AUTOTEST* — `pikNumer`&nbsp;4313, `pikKoniecTestu`&nbsp;4325, `pikBrakSieci`&nbsp;4336, `wynikEtapu`&nbsp;4348, `etapTestu`&nbsp;4367, `autoTest`&nbsp;4372

*10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67)* — `tgWyslijTekst`&nbsp;4519, `tgZglosNieodebrane`&nbsp;4565, `tgSprawdzBaterie`&nbsp;4585, `tgSprawdzZapas`&nbsp;4604, `dniOdEry`&nbsp;4627, `dniDoDaty`&nbsp;4638, `inrPrzypomnienieTeraz`&nbsp;4672, `tgOznaczInrMiniete`&nbsp;4692, `sekundyDoInrPrzypomnienia`&nbsp;4701, `tgSprawdzInr`&nbsp;4723, `tgTekstZapas`&nbsp;4737, `tgTekstInr`&nbsp;4746, `tgTekstNieodebrane`&nbsp;4766, `tgTekstBateria`&nbsp;4775, `tgWyslijZalegle`&nbsp;4794

*11.  DEEP SLEEP* — `otaZglos`&nbsp;4925, `skanujSieci`&nbsp;4953, `otaSprobuj`&nbsp;5002, `kolejnePrzesuniecie`&nbsp;5201, `goToSleep`&nbsp;5206, `planNextSleep`&nbsp;5402

*12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany)* — `petlaLadowania`&nbsp;5490, `setup`&nbsp;5588, `loop`&nbsp;6208


---

## `firmware/PillBoxTest/PillBoxTest.ino` — 681 linii

| od | do | blok |
|---|---|---|
| 1 | 622 | PillBoxTest.ino  -  program DIAGNOSTYCZNY inteligentnego pudelka |
| 623 | 681 | SETUP |

**Funkcje** (27):

*PillBoxTest.ino  -  program DIAGNOSTYCZNY inteligentnego pudelka* — `naglowek`&nbsp;72, `wynik`&nbsp;76, `wynikF`&nbsp;81, `uwagaF`&nbsp;86, `info`&nbsp;92, `adcMediana`&nbsp;99, `napiecieOgniwa`&nbsp;108, `testBaterii`&nbsp;119, `ton`&nbsp;165, `ciszaBuzzera`&nbsp;173, `testBuzzera`&nbsp;184, `pudelkoOtwarte`&nbsp;209, `przyciskWcisniety`&nbsp;210, `testKontaktronu`&nbsp;212, `testPrzycisku`&nbsp;252, `testPamieci`&nbsp;277, `testWifi`&nbsp;307, `testCzasu`&nbsp;356, `wytnijPole`&nbsp;408, `firebaseLogowanie`&nbsp;429, `rtdb`&nbsp;482, `testFirebase`&nbsp;496, `testSnuStart`&nbsp;535, `testSnuKoniec`&nbsp;572, `podsumowanie`&nbsp;607

*SETUP* — `setup`&nbsp;626, `loop`&nbsp;680


---

## `firmware/PillBox/config.h` — 585 linii

| linia | grupa |
|---|---|
| 19 | /* --------------------------------------------------------------------- |
| 25 | /* --------------------------------------------------------------------- |
| 52 | /* --------------------------------------------------------------------- |
| 68 | /* --------------------------------------------------------------------- |
| 109 | /* --------------------------------------------------------------------- |
| 120 | /* --------------------------------------------------------------------- |
| 127 | /* --------------------------------------------------------------------- |
| 161 | /* --------------------------------------------------------------------- |
| 177 | /* --------------------------------------------------------------------- |
| 196 | /* --------------------------------------------------------------------- |
| 201 | /* --------------------------------------------------------------------- |
| 211 | /* --------------------------------------------------------------------- |
| 242 | /* --------------------------------------------------------------------- |
| 250 | /* --------------------------------------------------------------------- |
| 268 | /* --------------------------------------------------------------------- |
| 276 | /* --------------------------------------------------------------------- |
| 288 | /* --------------------------------------------------------------------- |
| 319 | /* --------------------------------------------------------------------- |
| 329 | /* --------------------------------------------------------------------- |
| 348 | /* --------------------------------------------------------------------- |
| 377 | /* --------------------------------------------------------------------- |
| 424 | /* --------------------------------------------------------------------- |
| 441 | /* --------------------------------------------------------------------- |
| 512 | /* --------------------------------------------------------------------- |
| 554 | /* --------------------------------------------------------------------- |
| 561 | /* --------------------------------------------------------------------- |
| 567 | /* --------------------------------------------------------------------- |
