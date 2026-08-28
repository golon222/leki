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

## `index.html` — 8100 linii, ~130 tys. tokenow

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
| 2391 | 2763 | START |
| 2764 | 2824 | OSŁONA RYSOWANIA |
| 2825 | 3021 | REKONCYLIACJA |
| 3022 | 3080 | KALENDARZ |
| 3081 | 3347 | HISTORIA ROZPISANIA DAWKI |
| 3348 | 3529 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3530 | 3681 | ARKUSZ DNIA |
| 3682 | 3777 | WZIĄŁEM TERAZ |
| 3778 | 3892 | INR |
| 3893 | 4032 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4033 | 4045 | STATUS PUDEŁKA |
| 4046 | 4196 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4197 | 4301 | DZIENNIK WIECZKA — narzędzie na czas testu terenowego. |
| 4302 | 4471 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4472 | 4728 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4729 | 4769 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4770 | 4863 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 4864 | 5184 | EKRAN ZDARZEN |
| 5185 | 5303 | ZAPAS TABLETEK |
| 5304 | 5533 | USTAWIENIA |
| 5534 | 5841 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 5842 | 6298 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6299 | 6660 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 6661 | 6871 | ANALIZA |
| 6872 | 7257 | WYKRESY ANALIZY |
| 7258 | 7414 | RAPORT |
| 7415 | 7476 | KONTEKST DNIA (TAGI) |
| 7477 | 7522 | KOPIA ZAPASOWA |
| 7523 | 7736 | KOPIA NA TELEGRAM |
| 7737 | 7802 | WIEK KOPII |
| 7803 | 7900 | ODTWARZANIE Z KOPII |
| 7901 | 7975 | KOPIE Z BAZY |
| 7976 | 8058 | NAWIGACJA |
| 8059 | 8100 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (209) — nazwa i linia deklaracji:

*INFORMACJA ZWROTNA* — `toast`&nbsp;1992, `busy`&nbsp;2008, `todayKey`&nbsp;2035, `dzisiajKey`&nbsp;2039, `inNightWindow`&nbsp;2042

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2097, `tzName`&nbsp;2113, `tzLabel`&nbsp;2114, `tzOffsetTxt`&nbsp;2115, `devDate`&nbsp;2121, `devKey`&nbsp;2126, `devHM`&nbsp;2131, `slotMin`&nbsp;2132, `pillColors`&nbsp;2134

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2145

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2182, `cieniuj`&nbsp;2209, `doseGraphic`&nbsp;2226

*LOGOWANIE* — `doLogin`&nbsp;2240

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2260, `wyczyscCache`&nbsp;2355, `fbSignOut`&nbsp;2374

*START* — `boot`&nbsp;2392

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;2791, `rysujWszystkie`&nbsp;2804, `renderAll`&nbsp;2808

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;2831, `reconcileDecyzja`&nbsp;2870, `zapiszReconcile`&nbsp;2887, `doReconcile`&nbsp;2904, `reconcile`&nbsp;3020

*KALENDARZ* — `tydzienDawek`&nbsp;3060

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3106, `dawkaNaDzien`&nbsp;3117, `dzienBezLeku`&nbsp;3138, `wyjatekNaDzien`&nbsp;3143, `opisDawkowania`&nbsp;3149, `dayDose`&nbsp;3159, `dzienZamkniety`&nbsp;3193, `trackingSince`&nbsp;3199, `beforeTracking`&nbsp;3200, `dayStatus`&nbsp;3202, `renderCalendar`&nbsp;3243, `seriaDni`&nbsp;3308, `doNastepnej`&nbsp;3326, `opisCzasu`&nbsp;3341

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3356, `trwanieTxt`&nbsp;3371, `kiedyDawkaTxt`&nbsp;3382, `odswiezOdDawki`&nbsp;3390, `startTikOdDawki`&nbsp;3404, `renderToday`&nbsp;3414

*ARKUSZ DNIA* — `closeSheet`&nbsp;3541, `renderSheet`&nbsp;3543, `resetDose`&nbsp;3621, `resetPlan`&nbsp;3628, `commitPlan`&nbsp;3633, `clearPlan`&nbsp;3649, `commitDose`&nbsp;3661

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3702, `askConfirm`&nbsp;3767

*INR* — `inrState`&nbsp;3779, `odswiezTerminInr`&nbsp;3792, `addInr`&nbsp;3801, `inrKeysOk`&nbsp;3887

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;3902, `inrTerminKey`&nbsp;3910, `inrDoTerminu`&nbsp;3922, `dniTxt`&nbsp;3931, `renderInr`&nbsp;3933, `inrChart`&nbsp;4005

*STATUS PUDEŁKA* — `relTime`&nbsp;4034, `devDayMon`&nbsp;4043

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4078, `renderBoxLog`&nbsp;4124, `logPrzelacz`&nbsp;4160, `renderNvsFailLog`&nbsp;4169

*DZIENNIK WIECZKA — narzędzie na czas testu terenowego.* — `lidPaczki`&nbsp;4218, `lidWpisy`&nbsp;4224, `renderLidLog`&nbsp;4237

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4319, `oczekWczytaj`&nbsp;4327, `oczekZapisz`&nbsp;4332, `oczekIle`&nbsp;4335, `zapiszPewnie`&nbsp;4345, `zapiszCfg`&nbsp;4387, `bazaOdmowila`&nbsp;4405, `oczekWyslij`&nbsp;4429, `oczekOdmowy`&nbsp;4470

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4486, `ostrzReguly`&nbsp;4508, `lm`&nbsp;4547, `ostrzMilczy`&nbsp;4553, `nvsMalo`&nbsp;4610, `opisNvsFailKey`&nbsp;4622, `stratyDotyczaLeku`&nbsp;4672, `ostrzStraty`&nbsp;4684, `stratyCicho`&nbsp;4718

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4750

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4780, `renderOstrzezenia`&nbsp;4795, `bezPokrycia`&nbsp;4805, `wierszZdarzenia`&nbsp;4811, `renderDiag`&nbsp;4828

*EKRAN ZDARZEN* — `evFiltr`&nbsp;4880, `evPasuje`&nbsp;4885, `renderEvents`&nbsp;4897, `renderOpenWarn`&nbsp;4937, `minutyDoPelna`&nbsp;4990, `opisLadowania`&nbsp;5002, `dni`&nbsp;5022, `opisLadowan`&nbsp;5025, `renderStatus`&nbsp;5042

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5188, `dayAfter`&nbsp;5191, `pillsBaseInfo`&nbsp;5202, `settlePills`&nbsp;5212, `dniZapasu`&nbsp;5252, `renderPills`&nbsp;5265, `savePills`&nbsp;5286, `setPills`&nbsp;5297

*USTAWIENIA* — `renderKafelki`&nbsp;5308, `renderSettings`&nbsp;5335, `tydzienZPol`&nbsp;5379, `renderWeekEditor`&nbsp;5391, `odswiezPodpowiedzTygodnia`&nbsp;5408, `tydzienZmieniony`&nbsp;5422, `rownajTydzien`&nbsp;5423, `renderPlanList`&nbsp;5436, `renderExceptions`&nbsp;5463, `wyslijSiec`&nbsp;5509

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5551, `tgZapytaj`&nbsp;5561, `tgKodParowania`&nbsp;5601, `tgZnajdzCzat`&nbsp;5617, `tgPolacz`&nbsp;5685, `tgProbna`&nbsp;5718, `tgOdlacz`&nbsp;5725, `renderTgStan`&nbsp;5747

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;5862, `pobierzOpisFirmware`&nbsp;5868, `wyslijAktualizacje`&nbsp;5889, `anulujAktualizacje`&nbsp;5934, `renderOta`&nbsp;5940, `renderNetStan`&nbsp;6225

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6310, `renderSkan`&nbsp;6316, `szukajSieci`&nbsp;6368, `wybierzSiec`&nbsp;6376, `wyslijPolecenieSieci`&nbsp;6394, `siecZIndeksu`&nbsp;6404, `tzChanged`&nbsp;6438, `cfgTime`&nbsp;6443, `addSlot`&nbsp;6451, `zapiszPlanDnia`&nbsp;6464, `saveConfig`&nbsp;6482, `inrKrokiZakresu`&nbsp;6543, `opcjeInr`&nbsp;6550, `inrZakresZmieniony`&nbsp;6561, `wypelnijListyZakresu`&nbsp;6574, `saveInrRange`&nbsp;6585, `wypelnijListeOdstepu`&nbsp;6619, `saveInrEvery`&nbsp;6632, `odswiezPodpowiedzInr`&nbsp;6642

*ANALIZA* — `openTimeOf`&nbsp;6667, `openMinutes`&nbsp;6673, `sredniaPora`&nbsp;6697, `kwantyl`&nbsp;6705, `dniMiedzy`&nbsp;6713, `odstepyZPunktow`&nbsp;6727, `analyze`&nbsp;6736, `inrContext`&nbsp;6838

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;6897, `rytmSVG`&nbsp;6909, `poryWCzasieSVG`&nbsp;6971, `iskraSVG`&nbsp;7039, `dowSVG`&nbsp;7067, `dniRytmu`&nbsp;7103, `skutecznoscTygodniami`&nbsp;7124, `renderAnalysis`&nbsp;7152

*RAPORT* — `collectRows`&nbsp;7259, `makeReport`&nbsp;7296

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7447, `tagiPrzed`&nbsp;7455, `tagPrzelacz`&nbsp;7464

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7507, `opisKopii`&nbsp;7517

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7557, `tgCzatKopii`&nbsp;7564, `odswiezKopie`&nbsp;7571, `tgKopiaCzatZapisz`&nbsp;7579, `tgKopiaCzatZnajdz`&nbsp;7599, `tgKopiaWlacz`&nbsp;7629, `tgKopiaWylacz`&nbsp;7648, `kopiaNaTelegram`&nbsp;7657, `kopiaAutomat`&nbsp;7708

*WIEK KOPII* — `dniOdDaty`&nbsp;7755, `wiekKopiiTxt`&nbsp;7761, `renderKopiaStan`&nbsp;7769, `zapiszKopie`&nbsp;7784

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;7818, `wczytajKopie`&nbsp;7848, `kopiaCzytelna`&nbsp;7853, `odtworzKopie`&nbsp;7863, `kopiaWybrana`&nbsp;7886

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;7910, `odtworzZBazy`&nbsp;7942, `exportCsv`&nbsp;7954

*NAWIGACJA* — `wrocZEkranu`&nbsp;8057


---

## `firmware/PillBox/PillBox.ino` — 6389 linii

| od | do | blok |
|---|---|---|
| 1 | 63 | PillBox.ino  -  Inteligentne pudelko na leki / IoT Pill Reminder |
| 64 | 244 | PAMIEC RTC  (przezywa deep sleep, ginie po odlaczeniu zasilania) |
| 245 | 470 | STAN GLOBALNY |
| 471 | 691 | 1.  POMIAR BATERII |
| 692 | 854 | 2.  BUZZER  (pasywny piezo -> PWM przez LEDC) |
| 855 | 973 | 3.  GPIO / WYBUDZANIE |
| 974 | 1088 | 4.  HARMONOGRAM |
| 1089 | 1214 | 4a.  DNI BEZ LEKU |
| 1215 | 1235 | 4b.  PUDELKO ZOSTAWIONE OTWARTE |
| 1236 | 1459 | 4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego |
| 1460 | 1711 | 5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien) |
| 1712 | 2060 | 6.  WiFi |
| 2061 | 3084 | 7.  FIREBASE  (REST: Auth email/haslo + Realtime Database) |
| 3085 | 3449 | 8.  ZDARZENIA |
| 3450 | 3505 | 9.  ALARM |
| 3506 | 3724 | 10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth) |
| 3725 | 4216 | 10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59 |
| 4217 | 4336 | 10b. CZARNA SKRZYNKA |
| 4337 | 4639 | 10c. GESTY SERWISOWE I AUTOTEST |
| 4640 | 5062 | 10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67) |
| 5063 | 5655 | 11.  DEEP SLEEP |
| 5656 | 6389 | 12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany) |

**Funkcje** (175):

*STAN GLOBALNY* — `zanotujNvsFail`&nbsp;323, `nvsPutStr`&nbsp;342, `nvsPutU16`&nbsp;365, `nvsPutU32`&nbsp;388, `nvsWolneWpisy`&nbsp;403, `syncTimeNTP`&nbsp;446, `logbookJson`&nbsp;447, `setTakenDay`&nbsp;448, `note`&nbsp;450, `awakeTooLong`&nbsp;464, `extendAwake`&nbsp;466

*1.  POMIAR BATERII* — `readBatteryRaw`&nbsp;479, `battPercentFromCurve`&nbsp;519, `resetBatteryFilter`&nbsp;551, `zapiszKoniecLadowania`&nbsp;574, `trackCharging`&nbsp;584, `battSmooth`&nbsp;638, `readBattery`&nbsp;669

*2.  BUZZER  (pasywny piezo -> PWM przez LEDC)* — `buzzerInit`&nbsp;695, `buzzerTone`&nbsp;704, `buzzerTonCicho`&nbsp;715, `buzzerOff`&nbsp;724, `beepAck`&nbsp;736, `beepErr`&nbsp;760, `beepQueued`&nbsp;770, `beepAlreadyTaken`&nbsp;780, `beepNowaWersja`&nbsp;805, `beepLowStock`&nbsp;815, `beepLowBattery`&nbsp;824, `beepBoxOpen`&nbsp;840, `beepCharging`&nbsp;848

*3.  GPIO / WYBUDZANIE* — `configureInputs`&nbsp;858, `reedPoziomStabilny`&nbsp;897, `boxIsOpen`&nbsp;918, `buttonPressed`&nbsp;921, `wakeName`&nbsp;923

*4.  HARMONOGRAM* — `godzinaPoprawna`&nbsp;987, `parseSchedule`&nbsp;996, `loadSchedule`&nbsp;1009, `saveSchedule`&nbsp;1032, `localMinutesOfDay`&nbsp;1043, `slotMinutes`&nbsp;1050, `localDayNumber`&nbsp;1059, `matchSlot`&nbsp;1067, `secondsToDayBoundary`&nbsp;1082

*4a.  DNI BEZ LEKU* — `localWeekday`&nbsp;1109, `dateKeyToNum`&nbsp;1117, `dawkaNaDobe`&nbsp;1130, `dzisBezLeku`&nbsp;1140, `parseDoseWeek`&nbsp;1149, `parseDoseEx`&nbsp;1167, `saveDosing`&nbsp;1189, `loadDosing`&nbsp;1202

*4b.  PUDELKO ZOSTAWIONE OTWARTE* — `openWarnSecondsLeft`&nbsp;1226

*4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego* — `lidLogAdd`&nbsp;1269, `lidLogCount`&nbsp;1293, `jsonEscape`&nbsp;1303, `lidLogJson`&nbsp;1319, `lidLogClear`&nbsp;1349, `nvsFailLogDoWyslania`&nbsp;1364, `nvsFailLogJson`&nbsp;1374, `nvsFailLogOznaczWyslany`&nbsp;1392, `trackBoxOpen`&nbsp;1396, `secondsToNextSlot`&nbsp;1445

*5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien)* — `rekordTs`&nbsp;1476, `rekordBezDaty`&nbsp;1483, `tsDoBazy`&nbsp;1492, `queuePush`&nbsp;1496, `queueCount`&nbsp;1519, `queuePeek`&nbsp;1526, `queuePop`&nbsp;1541, `queueDrop`&nbsp;1560, `przesunZnaczniki`&nbsp;1584, `queueShiftTimestamps`&nbsp;1599, `queueNadajCzas`&nbsp;1647, `queueEpokaSkasuj`&nbsp;1690

*6.  WiFi* — `netKlucz`&nbsp;1729, `wifiSieciCount`&nbsp;1733, `wifiSiecSsid`&nbsp;1740, `wifiSiecPass`&nbsp;1749, `wifiListeZapisz`&nbsp;1776, `wifiListeCzytaj`&nbsp;1800, `wifiSiecDodaj`&nbsp;1813, `wifiSiecUsun`&nbsp;1844, `wifiSiecPriorytet`&nbsp;1877, `wifiSprobuj`&nbsp;1905, `wifiConnect`&nbsp;1923, `wifiOff`&nbsp;1992, `wifiUspij`&nbsp;2006, `syncTimeNTP`&nbsp;2011

*7.  FIREBASE  (REST: Auth email/haslo + Realtime Database)* — `tokenZPamieci`&nbsp;2078, `zapomnijToken`&nbsp;2087, `hasloJestPrawdziwe`&nbsp;2132, `hasloZPamieci`&nbsp;2137, `hasloWPamieci`&nbsp;2146, `hasloUtrwal`&nbsp;2150, `hasloDoLogowania`&nbsp;2163, `tgTokenZPamieci`&nbsp;2183, `tgChatZPamieci`&nbsp;2190, `tgSkonfigurowany`&nbsp;2199, `tgUtrwal`&nbsp;2206, `tgZapomnij`&nbsp;2218, `firebaseSignIn`&nbsp;2252, `rtdbUrl`&nbsp;2347, `rtdbSend`&nbsp;2369, `rekordKompletny`&nbsp;2396, `pushEventRecord`&nbsp;2405, `pushLidState`&nbsp;2462, `otaSumaZPamieci`&nbsp;2483, `otaSumaWgranej`&nbsp;2505, `pushStatus`&nbsp;2511, `fetchConfig`&nbsp;2718, `trwaleOdrzucony`&nbsp;3042, `flushQueue`&nbsp;3046

*8.  ZDARZENIA* — `makeRecordAt`&nbsp;3088, `makeRecord`&nbsp;3110, `loadDayMarkers`&nbsp;3120, `clearDayMarkers`&nbsp;3139, `setTakenDay`&nbsp;3153, `setRolloverDay`&nbsp;3161, `zapiszDawke`&nbsp;3191, `oznaczAlarmObsluzony`&nbsp;3234, `alarmJuzObsluzony`&nbsp;3251, `ostatniSlotDoby`&nbsp;3277, `juzDzisBrane`&nbsp;3287, `checkDayRollover`&nbsp;3294, `reportEvent`&nbsp;3376

*9.  ALARM* — `alarmPotwierdzony`&nbsp;3473, `runAlarmWindow`&nbsp;3478

*10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth)* — `htmlEscape`&nbsp;3519, `portalPage`&nbsp;3533, `startWifiPortal`&nbsp;3577

*10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59* — `otaOpisDecyzji`&nbsp;3849, `otaZanotujProbe`&nbsp;3875, `otaWyzerujLicznik`&nbsp;3883, `otaZlecenieWBazie`&nbsp;3913, `otaPobierzOpis`&nbsp;3928, `otaWgraj`&nbsp;3972, `otaSprawdzPoStarcie`&nbsp;4130, `otaPotwierdzDzialanie`&nbsp;4163

*10b. CZARNA SKRZYNKA* — `note`&nbsp;4237, `wartoZapisac`&nbsp;4244, `logbookAdd`&nbsp;4256, `logbookPrint`&nbsp;4295, `logbookJson`&nbsp;4319

*10c. GESTY SERWISOWE I AUTOTEST* — `pikNumer`&nbsp;4469, `pikKoniecTestu`&nbsp;4481, `pikBrakSieci`&nbsp;4492, `wynikEtapu`&nbsp;4504, `etapTestu`&nbsp;4523, `autoTest`&nbsp;4528

*10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67)* — `tgWyslijTekst`&nbsp;4675, `tgZglosNieodebrane`&nbsp;4721, `tgSprawdzBaterie`&nbsp;4741, `tgSprawdzZapas`&nbsp;4760, `dniOdEry`&nbsp;4783, `dniDoDaty`&nbsp;4794, `inrPrzypomnienieTeraz`&nbsp;4828, `tgOznaczInrMiniete`&nbsp;4848, `sekundyDoInrPrzypomnienia`&nbsp;4857, `tgSprawdzInr`&nbsp;4879, `tgTekstZapas`&nbsp;4893, `tgTekstInr`&nbsp;4902, `tgTekstNieodebrane`&nbsp;4922, `tgTekstBateria`&nbsp;4931, `tgWyslijZalegle`&nbsp;4950

*11.  DEEP SLEEP* — `otaZglos`&nbsp;5081, `skanujSieci`&nbsp;5109, `otaSprobuj`&nbsp;5158, `kolejnePrzesuniecie`&nbsp;5357, `goToSleep`&nbsp;5362, `planNextSleep`&nbsp;5580

*12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany)* — `petlaLadowania`&nbsp;5668, `setup`&nbsp;5766, `loop`&nbsp;6386


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

## `firmware/PillBox/config.h` — 611 linii

| linia | grupa |
|---|---|
| 19 | /* --------------------------------------------------------------------- |
| 25 | /* --------------------------------------------------------------------- |
| 52 | /* --------------------------------------------------------------------- |
| 68 | /* --------------------------------------------------------------------- |
| 109 | /* --- ODBICIA STYKU KONTAKTRONU --------------------------------------- |
| 135 | /* --------------------------------------------------------------------- |
| 146 | /* --------------------------------------------------------------------- |
| 153 | /* --------------------------------------------------------------------- |
| 187 | /* --------------------------------------------------------------------- |
| 203 | /* --------------------------------------------------------------------- |
| 222 | /* --------------------------------------------------------------------- |
| 227 | /* --------------------------------------------------------------------- |
| 237 | /* --------------------------------------------------------------------- |
| 268 | /* --------------------------------------------------------------------- |
| 276 | /* --------------------------------------------------------------------- |
| 294 | /* --------------------------------------------------------------------- |
| 302 | /* --------------------------------------------------------------------- |
| 314 | /* --------------------------------------------------------------------- |
| 345 | /* --------------------------------------------------------------------- |
| 355 | /* --------------------------------------------------------------------- |
| 374 | /* --------------------------------------------------------------------- |
| 403 | /* --------------------------------------------------------------------- |
| 450 | /* --------------------------------------------------------------------- |
| 467 | /* --------------------------------------------------------------------- |
| 538 | /* --------------------------------------------------------------------- |
| 580 | /* --------------------------------------------------------------------- |
| 587 | /* --------------------------------------------------------------------- |
| 593 | /* --------------------------------------------------------------------- |
