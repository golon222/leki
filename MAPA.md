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

## `index.html` — 7877 linii, ~130 tys. tokenow

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
| 1501 | 1765 | tab-help |
| 1766 | 1787 | tab-ev |
| 1788 | 1804 | tab-hist |
| 1805 | 1888 | tab-lid |
| 1889 | 1897 | JS — poczatek |
| 1898 | 1953 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 1954 | 2028 | INFORMACJA ZWROTNA |
| 2029 | 2123 | STREFY CZASOWE |
| 2124 | 2151 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2152 | 2218 | TABLETKA JAKO BRYŁA |
| 2219 | 2233 | LOGOWANIE |
| 2234 | 2373 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2374 | 2732 | START |
| 2733 | 2793 | OSŁONA RYSOWANIA |
| 2794 | 2990 | REKONCYLIACJA |
| 2991 | 3049 | KALENDARZ |
| 3050 | 3316 | HISTORIA ROZPISANIA DAWKI |
| 3317 | 3498 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3499 | 3650 | ARKUSZ DNIA |
| 3651 | 3746 | WZIĄŁEM TERAZ |
| 3747 | 3846 | INR |
| 3847 | 3986 | ODSTĘP MIĘDZY POMIARAMI INR |
| 3987 | 3999 | STATUS PUDEŁKA |
| 4000 | 4150 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4151 | 4255 | DZIENNIK WIECZKA — narzędzie na czas testu terenowego. |
| 4256 | 4425 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4426 | 4659 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4660 | 4700 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4701 | 4794 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 4795 | 5100 | EKRAN ZDARZEN |
| 5101 | 5219 | ZAPAS TABLETEK |
| 5220 | 5449 | USTAWIENIA |
| 5450 | 5754 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 5755 | 6211 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6212 | 6573 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 6574 | 6773 | ANALIZA |
| 6774 | 7159 | WYKRESY ANALIZY |
| 7160 | 7311 | RAPORT |
| 7312 | 7373 | KONTEKST DNIA (TAGI) |
| 7374 | 7409 | KOPIA ZAPASOWA |
| 7410 | 7655 | KOPIA NA TELEGRAM |
| 7656 | 7752 | ODTWARZANIE Z KOPII |
| 7753 | 7835 | NAWIGACJA |
| 7836 | 7877 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (203) — nazwa i linia deklaracji:

*INFORMACJA ZWROTNA* — `toast`&nbsp;1975, `busy`&nbsp;1991, `todayKey`&nbsp;2018, `dzisiajKey`&nbsp;2022, `inNightWindow`&nbsp;2025

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2080, `tzName`&nbsp;2096, `tzLabel`&nbsp;2097, `tzOffsetTxt`&nbsp;2098, `devDate`&nbsp;2104, `devKey`&nbsp;2109, `devHM`&nbsp;2114, `slotMin`&nbsp;2115, `pillColors`&nbsp;2117

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2128

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2165, `cieniuj`&nbsp;2192, `doseGraphic`&nbsp;2209

*LOGOWANIE* — `doLogin`&nbsp;2223

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2243, `wyczyscCache`&nbsp;2338, `fbSignOut`&nbsp;2357

*START* — `boot`&nbsp;2375

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;2760, `rysujWszystkie`&nbsp;2773, `renderAll`&nbsp;2777

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;2800, `reconcileDecyzja`&nbsp;2839, `zapiszReconcile`&nbsp;2856, `doReconcile`&nbsp;2873, `reconcile`&nbsp;2989

*KALENDARZ* — `tydzienDawek`&nbsp;3029

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3075, `dawkaNaDzien`&nbsp;3086, `dzienBezLeku`&nbsp;3107, `wyjatekNaDzien`&nbsp;3112, `opisDawkowania`&nbsp;3118, `dayDose`&nbsp;3128, `dzienZamkniety`&nbsp;3162, `trackingSince`&nbsp;3168, `beforeTracking`&nbsp;3169, `dayStatus`&nbsp;3171, `renderCalendar`&nbsp;3212, `seriaDni`&nbsp;3277, `doNastepnej`&nbsp;3295, `opisCzasu`&nbsp;3310

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3325, `trwanieTxt`&nbsp;3340, `kiedyDawkaTxt`&nbsp;3351, `odswiezOdDawki`&nbsp;3359, `startTikOdDawki`&nbsp;3373, `renderToday`&nbsp;3383

*ARKUSZ DNIA* — `closeSheet`&nbsp;3510, `renderSheet`&nbsp;3512, `resetDose`&nbsp;3590, `resetPlan`&nbsp;3597, `commitPlan`&nbsp;3602, `clearPlan`&nbsp;3618, `commitDose`&nbsp;3630

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3671, `askConfirm`&nbsp;3736

*INR* — `inrState`&nbsp;3748, `odswiezTerminInr`&nbsp;3761, `addInr`&nbsp;3770, `inrKeysOk`&nbsp;3841

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;3856, `inrTerminKey`&nbsp;3864, `inrDoTerminu`&nbsp;3876, `dniTxt`&nbsp;3885, `renderInr`&nbsp;3887, `inrChart`&nbsp;3959

*STATUS PUDEŁKA* — `relTime`&nbsp;3988, `devDayMon`&nbsp;3997

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4032, `renderBoxLog`&nbsp;4078, `logPrzelacz`&nbsp;4114, `renderNvsFailLog`&nbsp;4123

*DZIENNIK WIECZKA — narzędzie na czas testu terenowego.* — `lidPaczki`&nbsp;4172, `lidWpisy`&nbsp;4178, `renderLidLog`&nbsp;4191

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4273, `oczekWczytaj`&nbsp;4281, `oczekZapisz`&nbsp;4286, `oczekIle`&nbsp;4289, `zapiszPewnie`&nbsp;4299, `zapiszCfg`&nbsp;4341, `bazaOdmowila`&nbsp;4359, `oczekWyslij`&nbsp;4383, `oczekOdmowy`&nbsp;4424

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4440, `ostrzReguly`&nbsp;4462, `lm`&nbsp;4501, `ostrzMilczy`&nbsp;4507, `nvsMalo`&nbsp;4554, `opisNvsFailKey`&nbsp;4566, `stratyDotyczaLeku`&nbsp;4603, `ostrzStraty`&nbsp;4615, `stratyCicho`&nbsp;4649

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4681

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4711, `renderOstrzezenia`&nbsp;4726, `bezPokrycia`&nbsp;4736, `wierszZdarzenia`&nbsp;4742, `renderDiag`&nbsp;4759

*EKRAN ZDARZEN* — `evFiltr`&nbsp;4811, `evPasuje`&nbsp;4816, `renderEvents`&nbsp;4828, `renderOpenWarn`&nbsp;4868, `minutyDoPelna`&nbsp;4921, `opisLadowania`&nbsp;4933, `dni`&nbsp;4953, `opisLadowan`&nbsp;4956, `renderStatus`&nbsp;4973

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5104, `dayAfter`&nbsp;5107, `pillsBaseInfo`&nbsp;5118, `settlePills`&nbsp;5128, `dniZapasu`&nbsp;5168, `renderPills`&nbsp;5181, `savePills`&nbsp;5202, `setPills`&nbsp;5213

*USTAWIENIA* — `renderKafelki`&nbsp;5224, `renderSettings`&nbsp;5251, `tydzienZPol`&nbsp;5295, `renderWeekEditor`&nbsp;5307, `odswiezPodpowiedzTygodnia`&nbsp;5324, `tydzienZmieniony`&nbsp;5338, `rownajTydzien`&nbsp;5339, `renderPlanList`&nbsp;5352, `renderExceptions`&nbsp;5379, `wyslijSiec`&nbsp;5425

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5467, `tgZapytaj`&nbsp;5477, `tgKodParowania`&nbsp;5517, `tgZnajdzCzat`&nbsp;5533, `tgPolacz`&nbsp;5601, `tgProbna`&nbsp;5634, `tgOdlacz`&nbsp;5641, `renderTgStan`&nbsp;5663

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;5775, `pobierzOpisFirmware`&nbsp;5781, `wyslijAktualizacje`&nbsp;5802, `anulujAktualizacje`&nbsp;5847, `renderOta`&nbsp;5853, `renderNetStan`&nbsp;6138

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6223, `renderSkan`&nbsp;6229, `szukajSieci`&nbsp;6281, `wybierzSiec`&nbsp;6289, `wyslijPolecenieSieci`&nbsp;6307, `siecZIndeksu`&nbsp;6317, `tzChanged`&nbsp;6351, `cfgTime`&nbsp;6356, `addSlot`&nbsp;6364, `zapiszPlanDnia`&nbsp;6377, `saveConfig`&nbsp;6395, `inrKrokiZakresu`&nbsp;6456, `opcjeInr`&nbsp;6463, `inrZakresZmieniony`&nbsp;6474, `wypelnijListyZakresu`&nbsp;6487, `saveInrRange`&nbsp;6498, `wypelnijListeOdstepu`&nbsp;6532, `saveInrEvery`&nbsp;6545, `odswiezPodpowiedzInr`&nbsp;6555

*ANALIZA* — `openTimeOf`&nbsp;6580, `openMinutes`&nbsp;6586, `sredniaPora`&nbsp;6610, `kwantyl`&nbsp;6618, `dniMiedzy`&nbsp;6626, `odstepyZPunktow`&nbsp;6640, `analyze`&nbsp;6649, `inrContext`&nbsp;6740

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;6799, `rytmSVG`&nbsp;6811, `poryWCzasieSVG`&nbsp;6873, `iskraSVG`&nbsp;6941, `dowSVG`&nbsp;6969, `dniRytmu`&nbsp;7005, `skutecznoscTygodniami`&nbsp;7026, `renderAnalysis`&nbsp;7054

*RAPORT* — `collectRows`&nbsp;7161, `makeReport`&nbsp;7198

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7344, `tagiPrzed`&nbsp;7352, `tagPrzelacz`&nbsp;7361

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7394, `opisKopii`&nbsp;7404

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7444, `tgCzatKopii`&nbsp;7451, `odswiezKopie`&nbsp;7458, `tgKopiaCzatZapisz`&nbsp;7466, `tgKopiaCzatZnajdz`&nbsp;7486, `tgKopiaWlacz`&nbsp;7516, `tgKopiaWylacz`&nbsp;7535, `kopiaNaTelegram`&nbsp;7544, `kopiaAutomat`&nbsp;7595, `renderKopiaStan`&nbsp;7626, `zapiszKopie`&nbsp;7637

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;7671, `wczytajKopie`&nbsp;7701, `kopiaWybrana`&nbsp;7703, `exportCsv`&nbsp;7736

*NAWIGACJA* — `wrocZEkranu`&nbsp;7834


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
