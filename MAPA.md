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

## `index.html` — 7923 linii, ~130 tys. tokenow

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
| 1390 | 1500 | tab-diag |
| 1501 | 1774 | tab-help |
| 1775 | 1796 | tab-ev |
| 1797 | 1813 | tab-hist |
| 1814 | 1897 | tab-lid |
| 1898 | 1906 | JS — poczatek |
| 1907 | 1962 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 1963 | 2037 | INFORMACJA ZWROTNA |
| 2038 | 2132 | STREFY CZASOWE |
| 2133 | 2160 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2161 | 2227 | TABLETKA JAKO BRYŁA |
| 2228 | 2242 | LOGOWANIE |
| 2243 | 2382 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2383 | 2741 | START |
| 2742 | 2802 | OSŁONA RYSOWANIA |
| 2803 | 2999 | REKONCYLIACJA |
| 3000 | 3058 | KALENDARZ |
| 3059 | 3325 | HISTORIA ROZPISANIA DAWKI |
| 3326 | 3507 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3508 | 3659 | ARKUSZ DNIA |
| 3660 | 3755 | WZIĄŁEM TERAZ |
| 3756 | 3855 | INR |
| 3856 | 3995 | ODSTĘP MIĘDZY POMIARAMI INR |
| 3996 | 4008 | STATUS PUDEŁKA |
| 4009 | 4159 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4160 | 4264 | DZIENNIK WIECZKA — narzędzie na czas testu terenowego. |
| 4265 | 4434 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4435 | 4668 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4669 | 4709 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4710 | 4803 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 4804 | 5109 | EKRAN ZDARZEN |
| 5110 | 5228 | ZAPAS TABLETEK |
| 5229 | 5458 | USTAWIENIA |
| 5459 | 5766 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 5767 | 6223 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6224 | 6585 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 6586 | 6785 | ANALIZA |
| 6786 | 7171 | WYKRESY ANALIZY |
| 7172 | 7323 | RAPORT |
| 7324 | 7385 | KONTEKST DNIA (TAGI) |
| 7386 | 7421 | KOPIA ZAPASOWA |
| 7422 | 7635 | KOPIA NA TELEGRAM |
| 7636 | 7701 | WIEK KOPII |
| 7702 | 7798 | ODTWARZANIE Z KOPII |
| 7799 | 7881 | NAWIGACJA |
| 7882 | 7923 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (205) — nazwa i linia deklaracji:

*INFORMACJA ZWROTNA* — `toast`&nbsp;1984, `busy`&nbsp;2000, `todayKey`&nbsp;2027, `dzisiajKey`&nbsp;2031, `inNightWindow`&nbsp;2034

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2089, `tzName`&nbsp;2105, `tzLabel`&nbsp;2106, `tzOffsetTxt`&nbsp;2107, `devDate`&nbsp;2113, `devKey`&nbsp;2118, `devHM`&nbsp;2123, `slotMin`&nbsp;2124, `pillColors`&nbsp;2126

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2137

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2174, `cieniuj`&nbsp;2201, `doseGraphic`&nbsp;2218

*LOGOWANIE* — `doLogin`&nbsp;2232

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2252, `wyczyscCache`&nbsp;2347, `fbSignOut`&nbsp;2366

*START* — `boot`&nbsp;2384

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;2769, `rysujWszystkie`&nbsp;2782, `renderAll`&nbsp;2786

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;2809, `reconcileDecyzja`&nbsp;2848, `zapiszReconcile`&nbsp;2865, `doReconcile`&nbsp;2882, `reconcile`&nbsp;2998

*KALENDARZ* — `tydzienDawek`&nbsp;3038

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3084, `dawkaNaDzien`&nbsp;3095, `dzienBezLeku`&nbsp;3116, `wyjatekNaDzien`&nbsp;3121, `opisDawkowania`&nbsp;3127, `dayDose`&nbsp;3137, `dzienZamkniety`&nbsp;3171, `trackingSince`&nbsp;3177, `beforeTracking`&nbsp;3178, `dayStatus`&nbsp;3180, `renderCalendar`&nbsp;3221, `seriaDni`&nbsp;3286, `doNastepnej`&nbsp;3304, `opisCzasu`&nbsp;3319

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3334, `trwanieTxt`&nbsp;3349, `kiedyDawkaTxt`&nbsp;3360, `odswiezOdDawki`&nbsp;3368, `startTikOdDawki`&nbsp;3382, `renderToday`&nbsp;3392

*ARKUSZ DNIA* — `closeSheet`&nbsp;3519, `renderSheet`&nbsp;3521, `resetDose`&nbsp;3599, `resetPlan`&nbsp;3606, `commitPlan`&nbsp;3611, `clearPlan`&nbsp;3627, `commitDose`&nbsp;3639

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3680, `askConfirm`&nbsp;3745

*INR* — `inrState`&nbsp;3757, `odswiezTerminInr`&nbsp;3770, `addInr`&nbsp;3779, `inrKeysOk`&nbsp;3850

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;3865, `inrTerminKey`&nbsp;3873, `inrDoTerminu`&nbsp;3885, `dniTxt`&nbsp;3894, `renderInr`&nbsp;3896, `inrChart`&nbsp;3968

*STATUS PUDEŁKA* — `relTime`&nbsp;3997, `devDayMon`&nbsp;4006

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4041, `renderBoxLog`&nbsp;4087, `logPrzelacz`&nbsp;4123, `renderNvsFailLog`&nbsp;4132

*DZIENNIK WIECZKA — narzędzie na czas testu terenowego.* — `lidPaczki`&nbsp;4181, `lidWpisy`&nbsp;4187, `renderLidLog`&nbsp;4200

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4282, `oczekWczytaj`&nbsp;4290, `oczekZapisz`&nbsp;4295, `oczekIle`&nbsp;4298, `zapiszPewnie`&nbsp;4308, `zapiszCfg`&nbsp;4350, `bazaOdmowila`&nbsp;4368, `oczekWyslij`&nbsp;4392, `oczekOdmowy`&nbsp;4433

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4449, `ostrzReguly`&nbsp;4471, `lm`&nbsp;4510, `ostrzMilczy`&nbsp;4516, `nvsMalo`&nbsp;4563, `opisNvsFailKey`&nbsp;4575, `stratyDotyczaLeku`&nbsp;4612, `ostrzStraty`&nbsp;4624, `stratyCicho`&nbsp;4658

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4690

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4720, `renderOstrzezenia`&nbsp;4735, `bezPokrycia`&nbsp;4745, `wierszZdarzenia`&nbsp;4751, `renderDiag`&nbsp;4768

*EKRAN ZDARZEN* — `evFiltr`&nbsp;4820, `evPasuje`&nbsp;4825, `renderEvents`&nbsp;4837, `renderOpenWarn`&nbsp;4877, `minutyDoPelna`&nbsp;4930, `opisLadowania`&nbsp;4942, `dni`&nbsp;4962, `opisLadowan`&nbsp;4965, `renderStatus`&nbsp;4982

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5113, `dayAfter`&nbsp;5116, `pillsBaseInfo`&nbsp;5127, `settlePills`&nbsp;5137, `dniZapasu`&nbsp;5177, `renderPills`&nbsp;5190, `savePills`&nbsp;5211, `setPills`&nbsp;5222

*USTAWIENIA* — `renderKafelki`&nbsp;5233, `renderSettings`&nbsp;5260, `tydzienZPol`&nbsp;5304, `renderWeekEditor`&nbsp;5316, `odswiezPodpowiedzTygodnia`&nbsp;5333, `tydzienZmieniony`&nbsp;5347, `rownajTydzien`&nbsp;5348, `renderPlanList`&nbsp;5361, `renderExceptions`&nbsp;5388, `wyslijSiec`&nbsp;5434

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5476, `tgZapytaj`&nbsp;5486, `tgKodParowania`&nbsp;5526, `tgZnajdzCzat`&nbsp;5542, `tgPolacz`&nbsp;5610, `tgProbna`&nbsp;5643, `tgOdlacz`&nbsp;5650, `renderTgStan`&nbsp;5672

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;5787, `pobierzOpisFirmware`&nbsp;5793, `wyslijAktualizacje`&nbsp;5814, `anulujAktualizacje`&nbsp;5859, `renderOta`&nbsp;5865, `renderNetStan`&nbsp;6150

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6235, `renderSkan`&nbsp;6241, `szukajSieci`&nbsp;6293, `wybierzSiec`&nbsp;6301, `wyslijPolecenieSieci`&nbsp;6319, `siecZIndeksu`&nbsp;6329, `tzChanged`&nbsp;6363, `cfgTime`&nbsp;6368, `addSlot`&nbsp;6376, `zapiszPlanDnia`&nbsp;6389, `saveConfig`&nbsp;6407, `inrKrokiZakresu`&nbsp;6468, `opcjeInr`&nbsp;6475, `inrZakresZmieniony`&nbsp;6486, `wypelnijListyZakresu`&nbsp;6499, `saveInrRange`&nbsp;6510, `wypelnijListeOdstepu`&nbsp;6544, `saveInrEvery`&nbsp;6557, `odswiezPodpowiedzInr`&nbsp;6567

*ANALIZA* — `openTimeOf`&nbsp;6592, `openMinutes`&nbsp;6598, `sredniaPora`&nbsp;6622, `kwantyl`&nbsp;6630, `dniMiedzy`&nbsp;6638, `odstepyZPunktow`&nbsp;6652, `analyze`&nbsp;6661, `inrContext`&nbsp;6752

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;6811, `rytmSVG`&nbsp;6823, `poryWCzasieSVG`&nbsp;6885, `iskraSVG`&nbsp;6953, `dowSVG`&nbsp;6981, `dniRytmu`&nbsp;7017, `skutecznoscTygodniami`&nbsp;7038, `renderAnalysis`&nbsp;7066

*RAPORT* — `collectRows`&nbsp;7173, `makeReport`&nbsp;7210

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7356, `tagiPrzed`&nbsp;7364, `tagPrzelacz`&nbsp;7373

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7406, `opisKopii`&nbsp;7416

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7456, `tgCzatKopii`&nbsp;7463, `odswiezKopie`&nbsp;7470, `tgKopiaCzatZapisz`&nbsp;7478, `tgKopiaCzatZnajdz`&nbsp;7498, `tgKopiaWlacz`&nbsp;7528, `tgKopiaWylacz`&nbsp;7547, `kopiaNaTelegram`&nbsp;7556, `kopiaAutomat`&nbsp;7607

*WIEK KOPII* — `dniOdDaty`&nbsp;7654, `wiekKopiiTxt`&nbsp;7660, `renderKopiaStan`&nbsp;7668, `zapiszKopie`&nbsp;7683

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;7717, `wczytajKopie`&nbsp;7747, `kopiaWybrana`&nbsp;7749, `exportCsv`&nbsp;7782

*NAWIGACJA* — `wrocZEkranu`&nbsp;7880


---

## `firmware/PillBox/PillBox.ino` — 6046 linii

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
| 1350 | 1478 | 5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien) |
| 1479 | 1820 | 6.  WiFi |
| 1821 | 2814 | 7.  FIREBASE  (REST: Auth email/haslo + Realtime Database) |
| 2815 | 3137 | 8.  ZDARZENIA |
| 3138 | 3193 | 9.  ALARM |
| 3194 | 3412 | 10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth) |
| 3413 | 3904 | 10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59 |
| 3905 | 4024 | 10b. CZARNA SKRZYNKA |
| 4025 | 4327 | 10c. GESTY SERWISOWE I AUTOTEST |
| 4328 | 4750 | 10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67) |
| 4751 | 5321 | 11.  DEEP SLEEP |
| 5322 | 6046 | 12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany) |

**Funkcje** (167):

*STAN GLOBALNY* — `zanotujNvsFail`&nbsp;322, `nvsPutStr`&nbsp;341, `nvsPutU16`&nbsp;364, `nvsWolneWpisy`&nbsp;379, `syncTimeNTP`&nbsp;422, `logbookJson`&nbsp;423, `setTakenDay`&nbsp;424, `note`&nbsp;426, `awakeTooLong`&nbsp;440, `extendAwake`&nbsp;442

*1.  POMIAR BATERII* — `readBatteryRaw`&nbsp;455, `battPercentFromCurve`&nbsp;495, `resetBatteryFilter`&nbsp;527, `zapiszKoniecLadowania`&nbsp;550, `trackCharging`&nbsp;560, `battSmooth`&nbsp;614, `readBattery`&nbsp;645

*2.  BUZZER  (pasywny piezo -> PWM przez LEDC)* — `buzzerInit`&nbsp;671, `buzzerTone`&nbsp;680, `buzzerTonCicho`&nbsp;691, `buzzerOff`&nbsp;700, `beepAck`&nbsp;712, `beepErr`&nbsp;736, `beepQueued`&nbsp;746, `beepAlreadyTaken`&nbsp;756, `beepNowaWersja`&nbsp;781, `beepLowStock`&nbsp;791, `beepLowBattery`&nbsp;800, `beepBoxOpen`&nbsp;816, `beepCharging`&nbsp;824

*3.  GPIO / WYBUDZANIE* — `configureInputs`&nbsp;834, `boxIsOpen`&nbsp;839, `buttonPressed`&nbsp;840, `wakeName`&nbsp;842

*4.  HARMONOGRAM* — `parseSchedule`&nbsp;896, `loadSchedule`&nbsp;909, `saveSchedule`&nbsp;922, `localMinutesOfDay`&nbsp;933, `slotMinutes`&nbsp;940, `localDayNumber`&nbsp;949, `matchSlot`&nbsp;957, `secondsToDayBoundary`&nbsp;972

*4a.  DNI BEZ LEKU* — `localWeekday`&nbsp;999, `dateKeyToNum`&nbsp;1007, `dawkaNaDobe`&nbsp;1020, `dzisBezLeku`&nbsp;1030, `parseDoseWeek`&nbsp;1039, `parseDoseEx`&nbsp;1057, `saveDosing`&nbsp;1079, `loadDosing`&nbsp;1092

*4b.  PUDELKO ZOSTAWIONE OTWARTE* — `openWarnSecondsLeft`&nbsp;1116

*4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego* — `lidLogAdd`&nbsp;1159, `lidLogCount`&nbsp;1183, `jsonEscape`&nbsp;1193, `lidLogJson`&nbsp;1209, `lidLogClear`&nbsp;1239, `nvsFailLogDoWyslania`&nbsp;1254, `nvsFailLogJson`&nbsp;1264, `nvsFailLogOznaczWyslany`&nbsp;1282, `trackBoxOpen`&nbsp;1286, `secondsToNextSlot`&nbsp;1335

*5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien)* — `queuePush`&nbsp;1354, `queueCount`&nbsp;1377, `queuePeek`&nbsp;1384, `queuePop`&nbsp;1399, `queueDrop`&nbsp;1418, `przesunZnaczniki`&nbsp;1442, `queueShiftTimestamps`&nbsp;1457

*6.  WiFi* — `netKlucz`&nbsp;1496, `wifiSieciCount`&nbsp;1500, `wifiSiecSsid`&nbsp;1507, `wifiSiecPass`&nbsp;1516, `wifiListeZapisz`&nbsp;1543, `wifiListeCzytaj`&nbsp;1567, `wifiSiecDodaj`&nbsp;1580, `wifiSiecUsun`&nbsp;1611, `wifiSiecPriorytet`&nbsp;1644, `wifiSprobuj`&nbsp;1672, `wifiConnect`&nbsp;1690, `wifiOff`&nbsp;1759, `wifiUspij`&nbsp;1773, `syncTimeNTP`&nbsp;1778

*7.  FIREBASE  (REST: Auth email/haslo + Realtime Database)* — `tokenZPamieci`&nbsp;1838, `zapomnijToken`&nbsp;1847, `hasloJestPrawdziwe`&nbsp;1892, `hasloZPamieci`&nbsp;1897, `hasloWPamieci`&nbsp;1906, `hasloUtrwal`&nbsp;1910, `hasloDoLogowania`&nbsp;1923, `tgTokenZPamieci`&nbsp;1943, `tgChatZPamieci`&nbsp;1950, `tgSkonfigurowany`&nbsp;1959, `tgUtrwal`&nbsp;1966, `tgZapomnij`&nbsp;1978, `firebaseSignIn`&nbsp;2012, `rtdbUrl`&nbsp;2107, `rtdbSend`&nbsp;2129, `rekordKompletny`&nbsp;2156, `pushEventRecord`&nbsp;2165, `pushLidState`&nbsp;2213, `otaSumaZPamieci`&nbsp;2234, `otaSumaWgranej`&nbsp;2256, `pushStatus`&nbsp;2262, `fetchConfig`&nbsp;2457, `trwaleOdrzucony`&nbsp;2772, `flushQueue`&nbsp;2776

*8.  ZDARZENIA* — `makeRecordAt`&nbsp;2818, `makeRecord`&nbsp;2825, `loadDayMarkers`&nbsp;2835, `clearDayMarkers`&nbsp;2854, `setTakenDay`&nbsp;2868, `setRolloverDay`&nbsp;2876, `zapiszDawke`&nbsp;2906, `oznaczAlarmObsluzony`&nbsp;2949, `alarmJuzObsluzony`&nbsp;2966, `ostatniSlotDoby`&nbsp;2992, `juzDzisBrane`&nbsp;3002, `checkDayRollover`&nbsp;3009, `reportEvent`&nbsp;3066

*9.  ALARM* — `alarmPotwierdzony`&nbsp;3161, `runAlarmWindow`&nbsp;3166

*10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth)* — `htmlEscape`&nbsp;3207, `portalPage`&nbsp;3221, `startWifiPortal`&nbsp;3265

*10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59* — `otaOpisDecyzji`&nbsp;3537, `otaZanotujProbe`&nbsp;3563, `otaWyzerujLicznik`&nbsp;3571, `otaZlecenieWBazie`&nbsp;3601, `otaPobierzOpis`&nbsp;3616, `otaWgraj`&nbsp;3660, `otaSprawdzPoStarcie`&nbsp;3818, `otaPotwierdzDzialanie`&nbsp;3851

*10b. CZARNA SKRZYNKA* — `note`&nbsp;3925, `wartoZapisac`&nbsp;3932, `logbookAdd`&nbsp;3944, `logbookPrint`&nbsp;3983, `logbookJson`&nbsp;4007

*10c. GESTY SERWISOWE I AUTOTEST* — `pikNumer`&nbsp;4157, `pikKoniecTestu`&nbsp;4169, `pikBrakSieci`&nbsp;4180, `wynikEtapu`&nbsp;4192, `etapTestu`&nbsp;4211, `autoTest`&nbsp;4216

*10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67)* — `tgWyslijTekst`&nbsp;4363, `tgZglosNieodebrane`&nbsp;4409, `tgSprawdzBaterie`&nbsp;4429, `tgSprawdzZapas`&nbsp;4448, `dniOdEry`&nbsp;4471, `dniDoDaty`&nbsp;4482, `inrPrzypomnienieTeraz`&nbsp;4516, `tgOznaczInrMiniete`&nbsp;4536, `sekundyDoInrPrzypomnienia`&nbsp;4545, `tgSprawdzInr`&nbsp;4567, `tgTekstZapas`&nbsp;4581, `tgTekstInr`&nbsp;4590, `tgTekstNieodebrane`&nbsp;4610, `tgTekstBateria`&nbsp;4619, `tgWyslijZalegle`&nbsp;4638

*11.  DEEP SLEEP* — `otaZglos`&nbsp;4769, `skanujSieci`&nbsp;4797, `otaSprobuj`&nbsp;4846, `kolejnePrzesuniecie`&nbsp;5045, `goToSleep`&nbsp;5050, `planNextSleep`&nbsp;5246

*12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany)* — `petlaLadowania`&nbsp;5334, `setup`&nbsp;5432, `loop`&nbsp;6043


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
