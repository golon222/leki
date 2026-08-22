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

## `index.html` — 7853 linii, ~130 tys. tokenow

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
| 1501 | 1755 | tab-help |
| 1756 | 1777 | tab-ev |
| 1778 | 1794 | tab-hist |
| 1795 | 1878 | tab-lid |
| 1879 | 1887 | JS — poczatek |
| 1888 | 1943 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 1944 | 2018 | INFORMACJA ZWROTNA |
| 2019 | 2113 | STREFY CZASOWE |
| 2114 | 2141 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2142 | 2208 | TABLETKA JAKO BRYŁA |
| 2209 | 2223 | LOGOWANIE |
| 2224 | 2363 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2364 | 2722 | START |
| 2723 | 2783 | OSŁONA RYSOWANIA |
| 2784 | 2980 | REKONCYLIACJA |
| 2981 | 3039 | KALENDARZ |
| 3040 | 3306 | HISTORIA ROZPISANIA DAWKI |
| 3307 | 3488 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3489 | 3640 | ARKUSZ DNIA |
| 3641 | 3736 | WZIĄŁEM TERAZ |
| 3737 | 3836 | INR |
| 3837 | 3976 | ODSTĘP MIĘDZY POMIARAMI INR |
| 3977 | 3989 | STATUS PUDEŁKA |
| 3990 | 4140 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4141 | 4245 | DZIENNIK WIECZKA — narzędzie na czas testu terenowego. |
| 4246 | 4415 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4416 | 4649 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4650 | 4690 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4691 | 4784 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 4785 | 5090 | EKRAN ZDARZEN |
| 5091 | 5209 | ZAPAS TABLETEK |
| 5210 | 5439 | USTAWIENIA |
| 5440 | 5737 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 5738 | 6194 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6195 | 6556 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 6557 | 6756 | ANALIZA |
| 6757 | 7142 | WYKRESY ANALIZY |
| 7143 | 7294 | RAPORT |
| 7295 | 7356 | KONTEKST DNIA (TAGI) |
| 7357 | 7392 | KOPIA ZAPASOWA |
| 7393 | 7631 | KOPIA NA TELEGRAM |
| 7632 | 7728 | ODTWARZANIE Z KOPII |
| 7729 | 7811 | NAWIGACJA |
| 7812 | 7853 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (202) — nazwa i linia deklaracji:

*INFORMACJA ZWROTNA* — `toast`&nbsp;1965, `busy`&nbsp;1981, `todayKey`&nbsp;2008, `dzisiajKey`&nbsp;2012, `inNightWindow`&nbsp;2015

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2070, `tzName`&nbsp;2086, `tzLabel`&nbsp;2087, `tzOffsetTxt`&nbsp;2088, `devDate`&nbsp;2094, `devKey`&nbsp;2099, `devHM`&nbsp;2104, `slotMin`&nbsp;2105, `pillColors`&nbsp;2107

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2118

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2155, `cieniuj`&nbsp;2182, `doseGraphic`&nbsp;2199

*LOGOWANIE* — `doLogin`&nbsp;2213

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2233, `wyczyscCache`&nbsp;2328, `fbSignOut`&nbsp;2347

*START* — `boot`&nbsp;2365

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;2750, `rysujWszystkie`&nbsp;2763, `renderAll`&nbsp;2767

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;2790, `reconcileDecyzja`&nbsp;2829, `zapiszReconcile`&nbsp;2846, `doReconcile`&nbsp;2863, `reconcile`&nbsp;2979

*KALENDARZ* — `tydzienDawek`&nbsp;3019

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3065, `dawkaNaDzien`&nbsp;3076, `dzienBezLeku`&nbsp;3097, `wyjatekNaDzien`&nbsp;3102, `opisDawkowania`&nbsp;3108, `dayDose`&nbsp;3118, `dzienZamkniety`&nbsp;3152, `trackingSince`&nbsp;3158, `beforeTracking`&nbsp;3159, `dayStatus`&nbsp;3161, `renderCalendar`&nbsp;3202, `seriaDni`&nbsp;3267, `doNastepnej`&nbsp;3285, `opisCzasu`&nbsp;3300

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3315, `trwanieTxt`&nbsp;3330, `kiedyDawkaTxt`&nbsp;3341, `odswiezOdDawki`&nbsp;3349, `startTikOdDawki`&nbsp;3363, `renderToday`&nbsp;3373

*ARKUSZ DNIA* — `closeSheet`&nbsp;3500, `renderSheet`&nbsp;3502, `resetDose`&nbsp;3580, `resetPlan`&nbsp;3587, `commitPlan`&nbsp;3592, `clearPlan`&nbsp;3608, `commitDose`&nbsp;3620

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3661, `askConfirm`&nbsp;3726

*INR* — `inrState`&nbsp;3738, `odswiezTerminInr`&nbsp;3751, `addInr`&nbsp;3760, `inrKeysOk`&nbsp;3831

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;3846, `inrTerminKey`&nbsp;3854, `inrDoTerminu`&nbsp;3866, `dniTxt`&nbsp;3875, `renderInr`&nbsp;3877, `inrChart`&nbsp;3949

*STATUS PUDEŁKA* — `relTime`&nbsp;3978, `devDayMon`&nbsp;3987

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4022, `renderBoxLog`&nbsp;4068, `logPrzelacz`&nbsp;4104, `renderNvsFailLog`&nbsp;4113

*DZIENNIK WIECZKA — narzędzie na czas testu terenowego.* — `lidPaczki`&nbsp;4162, `lidWpisy`&nbsp;4168, `renderLidLog`&nbsp;4181

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4263, `oczekWczytaj`&nbsp;4271, `oczekZapisz`&nbsp;4276, `oczekIle`&nbsp;4279, `zapiszPewnie`&nbsp;4289, `zapiszCfg`&nbsp;4331, `bazaOdmowila`&nbsp;4349, `oczekWyslij`&nbsp;4373, `oczekOdmowy`&nbsp;4414

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4430, `ostrzReguly`&nbsp;4452, `lm`&nbsp;4491, `ostrzMilczy`&nbsp;4497, `nvsMalo`&nbsp;4544, `opisNvsFailKey`&nbsp;4556, `stratyDotyczaLeku`&nbsp;4593, `ostrzStraty`&nbsp;4605, `stratyCicho`&nbsp;4639

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4671

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4701, `renderOstrzezenia`&nbsp;4716, `bezPokrycia`&nbsp;4726, `wierszZdarzenia`&nbsp;4732, `renderDiag`&nbsp;4749

*EKRAN ZDARZEN* — `evFiltr`&nbsp;4801, `evPasuje`&nbsp;4806, `renderEvents`&nbsp;4818, `renderOpenWarn`&nbsp;4858, `minutyDoPelna`&nbsp;4911, `opisLadowania`&nbsp;4923, `dni`&nbsp;4943, `opisLadowan`&nbsp;4946, `renderStatus`&nbsp;4963

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5094, `dayAfter`&nbsp;5097, `pillsBaseInfo`&nbsp;5108, `settlePills`&nbsp;5118, `dniZapasu`&nbsp;5158, `renderPills`&nbsp;5171, `savePills`&nbsp;5192, `setPills`&nbsp;5203

*USTAWIENIA* — `renderKafelki`&nbsp;5214, `renderSettings`&nbsp;5241, `tydzienZPol`&nbsp;5285, `renderWeekEditor`&nbsp;5297, `odswiezPodpowiedzTygodnia`&nbsp;5314, `tydzienZmieniony`&nbsp;5328, `rownajTydzien`&nbsp;5329, `renderPlanList`&nbsp;5342, `renderExceptions`&nbsp;5369, `wyslijSiec`&nbsp;5415

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5457, `tgZapytaj`&nbsp;5467, `tgKodParowania`&nbsp;5507, `tgZnajdzCzat`&nbsp;5523, `tgPolacz`&nbsp;5591, `tgProbna`&nbsp;5624, `tgOdlacz`&nbsp;5631, `renderTgStan`&nbsp;5653

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;5758, `pobierzOpisFirmware`&nbsp;5764, `wyslijAktualizacje`&nbsp;5785, `anulujAktualizacje`&nbsp;5830, `renderOta`&nbsp;5836, `renderNetStan`&nbsp;6121

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6206, `renderSkan`&nbsp;6212, `szukajSieci`&nbsp;6264, `wybierzSiec`&nbsp;6272, `wyslijPolecenieSieci`&nbsp;6290, `siecZIndeksu`&nbsp;6300, `tzChanged`&nbsp;6334, `cfgTime`&nbsp;6339, `addSlot`&nbsp;6347, `zapiszPlanDnia`&nbsp;6360, `saveConfig`&nbsp;6378, `inrKrokiZakresu`&nbsp;6439, `opcjeInr`&nbsp;6446, `inrZakresZmieniony`&nbsp;6457, `wypelnijListyZakresu`&nbsp;6470, `saveInrRange`&nbsp;6481, `wypelnijListeOdstepu`&nbsp;6515, `saveInrEvery`&nbsp;6528, `odswiezPodpowiedzInr`&nbsp;6538

*ANALIZA* — `openTimeOf`&nbsp;6563, `openMinutes`&nbsp;6569, `sredniaPora`&nbsp;6593, `kwantyl`&nbsp;6601, `dniMiedzy`&nbsp;6609, `odstepyZPunktow`&nbsp;6623, `analyze`&nbsp;6632, `inrContext`&nbsp;6723

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;6782, `rytmSVG`&nbsp;6794, `poryWCzasieSVG`&nbsp;6856, `iskraSVG`&nbsp;6924, `dowSVG`&nbsp;6952, `dniRytmu`&nbsp;6988, `skutecznoscTygodniami`&nbsp;7009, `renderAnalysis`&nbsp;7037

*RAPORT* — `collectRows`&nbsp;7144, `makeReport`&nbsp;7181

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7327, `tagiPrzed`&nbsp;7335, `tagPrzelacz`&nbsp;7344

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7377, `opisKopii`&nbsp;7387

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7427, `tgCzatKopii`&nbsp;7434, `tgKopiaCzatZapisz`&nbsp;7442, `tgKopiaCzatZnajdz`&nbsp;7462, `tgKopiaWlacz`&nbsp;7492, `tgKopiaWylacz`&nbsp;7511, `kopiaNaTelegram`&nbsp;7520, `kopiaAutomat`&nbsp;7571, `renderKopiaStan`&nbsp;7602, `zapiszKopie`&nbsp;7613

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;7647, `wczytajKopie`&nbsp;7677, `kopiaWybrana`&nbsp;7679, `exportCsv`&nbsp;7712

*NAWIGACJA* — `wrocZEkranu`&nbsp;7810


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
