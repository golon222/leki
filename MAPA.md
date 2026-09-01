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

## `index.html` — 8202 linii, ~130 tys. tokenow

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
| 1390 | 1490 | tab-diag |
| 1491 | 1766 | tab-help |
| 1767 | 1788 | tab-ev |
| 1789 | 1879 | tab-hist |
| 1880 | 1888 | JS — poczatek |
| 1889 | 1942 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 1943 | 2017 | INFORMACJA ZWROTNA |
| 2018 | 2112 | STREFY CZASOWE |
| 2113 | 2140 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2141 | 2207 | TABLETKA JAKO BRYŁA |
| 2208 | 2222 | LOGOWANIE |
| 2223 | 2362 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2363 | 2727 | START |
| 2728 | 2788 | OSŁONA RYSOWANIA |
| 2789 | 2847 | REKONCYLIACJA |
| 2848 | 3052 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3053 | 3111 | KALENDARZ |
| 3112 | 3378 | HISTORIA ROZPISANIA DAWKI |
| 3379 | 3560 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3561 | 3712 | ARKUSZ DNIA |
| 3713 | 3812 | WZIĄŁEM TERAZ |
| 3813 | 3927 | INR |
| 3928 | 4067 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4068 | 4080 | STATUS PUDEŁKA |
| 4081 | 4230 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4231 | 4392 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4393 | 4675 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4676 | 4716 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4717 | 4813 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 4814 | 5221 | EKRAN ZDARZEN |
| 5222 | 5340 | ZAPAS TABLETEK |
| 5341 | 5628 | USTAWIENIA |
| 5629 | 5936 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 5937 | 6393 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6394 | 6764 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 6765 | 6975 | ANALIZA |
| 6976 | 7361 | WYKRESY ANALIZY |
| 7362 | 7518 | RAPORT |
| 7519 | 7580 | KONTEKST DNIA (TAGI) |
| 7581 | 7626 | KOPIA ZAPASOWA |
| 7627 | 7840 | KOPIA NA TELEGRAM |
| 7841 | 7906 | WIEK KOPII |
| 7907 | 8004 | ODTWARZANIE Z KOPII |
| 8005 | 8079 | KOPIE Z BAZY |
| 8080 | 8160 | NAWIGACJA |
| 8161 | 8202 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (208) — nazwa i linia deklaracji:

*INFORMACJA ZWROTNA* — `toast`&nbsp;1964, `busy`&nbsp;1980, `todayKey`&nbsp;2007, `dzisiajKey`&nbsp;2011, `inNightWindow`&nbsp;2014

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2069, `tzName`&nbsp;2085, `tzLabel`&nbsp;2086, `tzOffsetTxt`&nbsp;2087, `devDate`&nbsp;2093, `devKey`&nbsp;2098, `devHM`&nbsp;2103, `slotMin`&nbsp;2104, `pillColors`&nbsp;2106

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2117

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2154, `cieniuj`&nbsp;2181, `doseGraphic`&nbsp;2198

*LOGOWANIE* — `doLogin`&nbsp;2212

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2232, `wyczyscCache`&nbsp;2327, `fbSignOut`&nbsp;2346

*START* — `boot`&nbsp;2364

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;2755, `rysujWszystkie`&nbsp;2768, `renderAll`&nbsp;2772

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;2795, `reconcileDecyzja`&nbsp;2834

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;2869, `zapiszReconcile`&nbsp;2881, `doReconcile`&nbsp;2920, `doReconcileWewn`&nbsp;2930, `reconcile`&nbsp;3051

*KALENDARZ* — `tydzienDawek`&nbsp;3091

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3137, `dawkaNaDzien`&nbsp;3148, `dzienBezLeku`&nbsp;3169, `wyjatekNaDzien`&nbsp;3174, `opisDawkowania`&nbsp;3180, `dayDose`&nbsp;3190, `dzienZamkniety`&nbsp;3224, `trackingSince`&nbsp;3230, `beforeTracking`&nbsp;3231, `dayStatus`&nbsp;3233, `renderCalendar`&nbsp;3274, `seriaDni`&nbsp;3339, `doNastepnej`&nbsp;3357, `opisCzasu`&nbsp;3372

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3387, `trwanieTxt`&nbsp;3402, `kiedyDawkaTxt`&nbsp;3413, `odswiezOdDawki`&nbsp;3421, `startTikOdDawki`&nbsp;3435, `renderToday`&nbsp;3445

*ARKUSZ DNIA* — `closeSheet`&nbsp;3572, `renderSheet`&nbsp;3574, `resetDose`&nbsp;3652, `resetPlan`&nbsp;3659, `commitPlan`&nbsp;3664, `clearPlan`&nbsp;3680, `commitDose`&nbsp;3692

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3733, `askConfirm`&nbsp;3802

*INR* — `inrState`&nbsp;3814, `odswiezTerminInr`&nbsp;3827, `addInr`&nbsp;3836, `inrKeysOk`&nbsp;3922

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;3937, `inrTerminKey`&nbsp;3945, `inrDoTerminu`&nbsp;3957, `dniTxt`&nbsp;3966, `renderInr`&nbsp;3968, `inrChart`&nbsp;4040

*STATUS PUDEŁKA* — `relTime`&nbsp;4069, `devDayMon`&nbsp;4078

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4113, `renderBoxLog`&nbsp;4159, `logPrzelacz`&nbsp;4195, `renderNvsFailLog`&nbsp;4203

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4248, `oczekWczytaj`&nbsp;4256, `oczekZapisz`&nbsp;4261, `oczekIle`&nbsp;4264, `zapiszPewnie`&nbsp;4274, `zapiszCfg`&nbsp;4312, `bazaOdmowila`&nbsp;4330, `oczekWyslij`&nbsp;4354, `oczekOdmowy`&nbsp;4391

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4407, `ostrzReguly`&nbsp;4429, `lm`&nbsp;4468, `ostrzMilczy`&nbsp;4474, `nvsMalo`&nbsp;4554, `opisNvsFailKey`&nbsp;4566, `stratyDotyczaLeku`&nbsp;4619, `ostrzStraty`&nbsp;4631, `stratyCicho`&nbsp;4665

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4697

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4727, `renderOstrzezenia`&nbsp;4742, `bezPokrycia`&nbsp;4752, `wierszZdarzenia`&nbsp;4758, `renderDiag`&nbsp;4775

*EKRAN ZDARZEN* — `evFiltr`&nbsp;4830, `evPasuje`&nbsp;4835, `renderEvents`&nbsp;4847, `renderOpenWarn`&nbsp;4887, `minutyDoPelna`&nbsp;4940, `opisLadowania`&nbsp;4952, `dni`&nbsp;4972, `opisLadowan`&nbsp;4975, `renderStatus`&nbsp;4992

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5225, `dayAfter`&nbsp;5228, `pillsBaseInfo`&nbsp;5239, `settlePills`&nbsp;5249, `dniZapasu`&nbsp;5289, `renderPills`&nbsp;5302, `savePills`&nbsp;5323, `setPills`&nbsp;5334

*USTAWIENIA* — `renderKafelki`&nbsp;5345, `renderSettings`&nbsp;5372, `tydzienZPol`&nbsp;5427, `renderWeekEditor`&nbsp;5439, `odswiezPodpowiedzTygodnia`&nbsp;5456, `tydzienZmieniony`&nbsp;5470, `rownajTydzien`&nbsp;5471, `renderPlanList`&nbsp;5519, `renderExceptions`&nbsp;5546, `wyslijSiec`&nbsp;5604

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5646, `tgZapytaj`&nbsp;5656, `tgKodParowania`&nbsp;5696, `tgZnajdzCzat`&nbsp;5712, `tgPolacz`&nbsp;5780, `tgProbna`&nbsp;5813, `tgOdlacz`&nbsp;5820, `renderTgStan`&nbsp;5842

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;5957, `pobierzOpisFirmware`&nbsp;5963, `wyslijAktualizacje`&nbsp;5984, `anulujAktualizacje`&nbsp;6029, `renderOta`&nbsp;6035, `renderNetStan`&nbsp;6320

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6405, `renderSkan`&nbsp;6411, `szukajSieci`&nbsp;6463, `wybierzSiec`&nbsp;6471, `wyslijPolecenieSieci`&nbsp;6489, `siecZIndeksu`&nbsp;6499, `tzChanged`&nbsp;6533, `cfgTime`&nbsp;6538, `addSlot`&nbsp;6546, `zapiszPlanDnia`&nbsp;6559, `saveConfig`&nbsp;6577, `inrKrokiZakresu`&nbsp;6647, `opcjeInr`&nbsp;6654, `inrZakresZmieniony`&nbsp;6665, `wypelnijListyZakresu`&nbsp;6678, `saveInrRange`&nbsp;6689, `wypelnijListeOdstepu`&nbsp;6723, `saveInrEvery`&nbsp;6736, `odswiezPodpowiedzInr`&nbsp;6746

*ANALIZA* — `openTimeOf`&nbsp;6771, `openMinutes`&nbsp;6777, `sredniaPora`&nbsp;6801, `kwantyl`&nbsp;6809, `dniMiedzy`&nbsp;6817, `odstepyZPunktow`&nbsp;6831, `analyze`&nbsp;6840, `inrContext`&nbsp;6942

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7001, `rytmSVG`&nbsp;7013, `poryWCzasieSVG`&nbsp;7075, `iskraSVG`&nbsp;7143, `dowSVG`&nbsp;7171, `dniRytmu`&nbsp;7207, `skutecznoscTygodniami`&nbsp;7228, `renderAnalysis`&nbsp;7256

*RAPORT* — `collectRows`&nbsp;7363, `makeReport`&nbsp;7400

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7551, `tagiPrzed`&nbsp;7559, `tagPrzelacz`&nbsp;7568

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7611, `opisKopii`&nbsp;7621

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7661, `tgCzatKopii`&nbsp;7668, `odswiezKopie`&nbsp;7675, `tgKopiaCzatZapisz`&nbsp;7683, `tgKopiaCzatZnajdz`&nbsp;7703, `tgKopiaWlacz`&nbsp;7733, `tgKopiaWylacz`&nbsp;7752, `kopiaNaTelegram`&nbsp;7761, `kopiaAutomat`&nbsp;7812

*WIEK KOPII* — `dniOdDaty`&nbsp;7859, `wiekKopiiTxt`&nbsp;7865, `renderKopiaStan`&nbsp;7873, `zapiszKopie`&nbsp;7888

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;7922, `wczytajKopie`&nbsp;7952, `kopiaCzytelna`&nbsp;7957, `odtworzKopie`&nbsp;7967, `kopiaWybrana`&nbsp;7990

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8014, `odtworzZBazy`&nbsp;8046, `exportCsv`&nbsp;8058

*NAWIGACJA* — `wrocZEkranu`&nbsp;8159


---

## `firmware/PillBox/PillBox.ino` — 6857 linii

| od | do | blok |
|---|---|---|
| 1 | 64 | PillBox.ino  -  Inteligentne pudelko na leki / IoT Pill Reminder |
| 65 | 320 | PAMIEC RTC  (przezywa deep sleep, ginie po odlaczeniu zasilania) |
| 321 | 583 | STAN GLOBALNY |
| 584 | 804 | 1.  POMIAR BATERII |
| 805 | 967 | 2.  BUZZER  (pasywny piezo -> PWM przez LEDC) |
| 968 | 1155 | 3.  GPIO / WYBUDZANIE |
| 1156 | 1270 | 4.  HARMONOGRAM |
| 1271 | 1396 | 4a.  DNI BEZ LEKU |
| 1397 | 1418 | 4b.  PUDELKO ZOSTAWIONE OTWARTE |
| 1419 | 1551 | 4c.  DZIENNIK WIECZKA - USUNIETY (D109) |
| 1552 | 1803 | 5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien) |
| 1804 | 2317 | 6.  WiFi |
| 2318 | 3398 | 7.  FIREBASE  (REST: Auth email/haslo + Realtime Database) |
| 3399 | 3783 | 8.  ZDARZENIA |
| 3784 | 3839 | 9.  ALARM |
| 3840 | 4058 | 10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth) |
| 4059 | 4550 | 10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59 |
| 4551 | 4670 | 10b. CZARNA SKRZYNKA |
| 4671 | 5067 | 10c. GESTY SERWISOWE I AUTOTEST |
| 5068 | 5490 | 10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67) |
| 5491 | 6088 | 11.  DEEP SLEEP |
| 6089 | 6857 | 12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany) |

**Funkcje** (185):

*STAN GLOBALNY* — `zanotujNvsFail`&nbsp;399, `nvsPutStr`&nbsp;418, `nvsPutU16`&nbsp;441, `nvsPutU32`&nbsp;464, `nvsPutI16`&nbsp;490, `nvsPutU8`&nbsp;497, `nvsWolneWpisy`&nbsp;512, `syncTimeNTP`&nbsp;555, `logbookJson`&nbsp;556, `setTakenDay`&nbsp;561, `note`&nbsp;563, `awakeTooLong`&nbsp;577, `extendAwake`&nbsp;579

*1.  POMIAR BATERII* — `readBatteryRaw`&nbsp;592, `battPercentFromCurve`&nbsp;632, `resetBatteryFilter`&nbsp;664, `zapiszKoniecLadowania`&nbsp;687, `trackCharging`&nbsp;697, `battSmooth`&nbsp;751, `readBattery`&nbsp;782

*2.  BUZZER  (pasywny piezo -> PWM przez LEDC)* — `buzzerInit`&nbsp;808, `buzzerTone`&nbsp;817, `buzzerTonCicho`&nbsp;828, `buzzerOff`&nbsp;837, `beepAck`&nbsp;849, `beepErr`&nbsp;873, `beepQueued`&nbsp;883, `beepAlreadyTaken`&nbsp;893, `beepNowaWersja`&nbsp;918, `beepLowStock`&nbsp;928, `beepLowBattery`&nbsp;937, `beepBoxOpen`&nbsp;953, `beepCharging`&nbsp;961

*3.  GPIO / WYBUDZANIE* — `configureInputs`&nbsp;971, `powodResetuOpis`&nbsp;993, `zanotujReset`&nbsp;1011, `reedPoziomStabilny`&nbsp;1056, `boxIsOpen`&nbsp;1080, `boxIsOpenPewnie`&nbsp;1099, `buttonPressed`&nbsp;1103, `wakeName`&nbsp;1105

*4.  HARMONOGRAM* — `godzinaPoprawna`&nbsp;1169, `parseSchedule`&nbsp;1178, `loadSchedule`&nbsp;1191, `saveSchedule`&nbsp;1214, `localMinutesOfDay`&nbsp;1225, `slotMinutes`&nbsp;1232, `localDayNumber`&nbsp;1241, `matchSlot`&nbsp;1249, `secondsToDayBoundary`&nbsp;1264

*4a.  DNI BEZ LEKU* — `localWeekday`&nbsp;1291, `dateKeyToNum`&nbsp;1299, `dawkaNaDobe`&nbsp;1312, `dzisBezLeku`&nbsp;1322, `parseDoseWeek`&nbsp;1331, `parseDoseEx`&nbsp;1349, `saveDosing`&nbsp;1371, `loadDosing`&nbsp;1384

*4b.  PUDELKO ZOSTAWIONE OTWARTE* — `openWarnSecondsLeft`&nbsp;1408

*4c.  DZIENNIK WIECZKA - USUNIETY (D109)* — `jsonEscape`&nbsp;1442, `nvsFailLogDoWyslania`&nbsp;1458, `nvsFailLogJson`&nbsp;1468, `nvsFailLogOznaczWyslany`&nbsp;1486, `trackBoxOpen`&nbsp;1490, `secondsToNextSlot`&nbsp;1537

*5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien)* — `rekordTs`&nbsp;1568, `rekordBezDaty`&nbsp;1575, `tsDoBazy`&nbsp;1584, `queuePush`&nbsp;1588, `queueCount`&nbsp;1611, `queuePeek`&nbsp;1618, `queuePop`&nbsp;1633, `queueDrop`&nbsp;1652, `przesunZnaczniki`&nbsp;1676, `queueShiftTimestamps`&nbsp;1691, `queueNadajCzas`&nbsp;1739, `queueEpokaSkasuj`&nbsp;1782

*6.  WiFi* — `netKlucz`&nbsp;1821, `wifiSieciCount`&nbsp;1825, `wifiSiecSsid`&nbsp;1832, `wifiSiecPass`&nbsp;1841, `wifiListeZapisz`&nbsp;1868, `wifiListeCzytaj`&nbsp;1892, `wifiSiecDodaj`&nbsp;1905, `wifiSiecUsun`&nbsp;1936, `wifiSiecPriorytet`&nbsp;1969, `zapamietajAp`&nbsp;1999, `apPodpowiedzPasuje`&nbsp;2010, `wifiBeginZPodpowiedzia`&nbsp;2018, `wifiCzekajNaLacze`&nbsp;2051, `wifiSprobuj`&nbsp;2064, `netSkadZnany`&nbsp;2101, `netSkadZapamietaj`&nbsp;2116, `wifiConnect`&nbsp;2128, `wifiOff`&nbsp;2249, `wifiUspij`&nbsp;2263, `syncTimeNTP`&nbsp;2268

*7.  FIREBASE  (REST: Auth email/haslo + Realtime Database)* — `tokenZPamieci`&nbsp;2335, `zapomnijToken`&nbsp;2344, `hasloJestPrawdziwe`&nbsp;2389, `hasloZPamieci`&nbsp;2394, `hasloWPamieci`&nbsp;2403, `hasloUtrwal`&nbsp;2407, `hasloDoLogowania`&nbsp;2420, `tgTokenZPamieci`&nbsp;2440, `tgChatZPamieci`&nbsp;2447, `tgSkonfigurowany`&nbsp;2456, `tgUtrwal`&nbsp;2463, `tgZapomnij`&nbsp;2475, `firebaseSignIn`&nbsp;2509, `rtdbUrl`&nbsp;2604, `rtdbSend`&nbsp;2626, `rekordKompletny`&nbsp;2653, `pushEventRecord`&nbsp;2662, `pushLidState`&nbsp;2719, `otaSumaZPamieci`&nbsp;2770, `otaSumaWgranej`&nbsp;2792, `pushStatus`&nbsp;2815, `fetchConfig`&nbsp;3032, `trwaleOdrzucony`&nbsp;3356, `flushQueue`&nbsp;3360

*8.  ZDARZENIA* — `makeRecordAt`&nbsp;3402, `makeRecord`&nbsp;3424, `loadDayMarkers`&nbsp;3434, `clearDayMarkers`&nbsp;3453, `setTakenDay`&nbsp;3467, `setRolloverDay`&nbsp;3475, `zapiszDawke`&nbsp;3505, `oznaczAlarmObsluzony`&nbsp;3548, `alarmJuzObsluzony`&nbsp;3581, `ostatniSlotDoby`&nbsp;3607, `juzDzisBrane`&nbsp;3617, `checkDayRollover`&nbsp;3624, `reportEvent`&nbsp;3706

*9.  ALARM* — `alarmPotwierdzony`&nbsp;3807, `runAlarmWindow`&nbsp;3812

*10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth)* — `htmlEscape`&nbsp;3853, `portalPage`&nbsp;3867, `startWifiPortal`&nbsp;3911

*10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59* — `otaOpisDecyzji`&nbsp;4183, `otaZanotujProbe`&nbsp;4209, `otaWyzerujLicznik`&nbsp;4217, `otaZlecenieWBazie`&nbsp;4247, `otaPobierzOpis`&nbsp;4262, `otaWgraj`&nbsp;4306, `otaSprawdzPoStarcie`&nbsp;4464, `otaPotwierdzDzialanie`&nbsp;4497

*10b. CZARNA SKRZYNKA* — `note`&nbsp;4571, `wartoZapisac`&nbsp;4578, `logbookAdd`&nbsp;4590, `logbookPrint`&nbsp;4629, `logbookJson`&nbsp;4653

*10c. GESTY SERWISOWE I AUTOTEST* — `netSkadOpis`&nbsp;4696, `lidMeldunek`&nbsp;4704, `dozorKrok`&nbsp;4731, `pikNumer`&nbsp;4897, `pikKoniecTestu`&nbsp;4909, `pikBrakSieci`&nbsp;4920, `wynikEtapu`&nbsp;4932, `etapTestu`&nbsp;4951, `autoTest`&nbsp;4956

*10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67)* — `tgWyslijTekst`&nbsp;5103, `tgZglosNieodebrane`&nbsp;5149, `tgSprawdzBaterie`&nbsp;5169, `tgSprawdzZapas`&nbsp;5188, `dniOdEry`&nbsp;5211, `dniDoDaty`&nbsp;5222, `inrPrzypomnienieTeraz`&nbsp;5256, `tgOznaczInrMiniete`&nbsp;5276, `sekundyDoInrPrzypomnienia`&nbsp;5285, `tgSprawdzInr`&nbsp;5307, `tgTekstZapas`&nbsp;5321, `tgTekstInr`&nbsp;5330, `tgTekstNieodebrane`&nbsp;5350, `tgTekstBateria`&nbsp;5359, `tgWyslijZalegle`&nbsp;5378

*11.  DEEP SLEEP* — `otaZglos`&nbsp;5509, `skanujSieci`&nbsp;5537, `otaSprobuj`&nbsp;5586, `kolejnePrzesuniecie`&nbsp;5785, `goToSleep`&nbsp;5790, `planNextSleep`&nbsp;6008

*12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany)* — `petlaLadowania`&nbsp;6101, `setup`&nbsp;6199, `loop`&nbsp;6854


---

## `firmware/PillBoxTest/PillBoxTest.ino` — 744 linii

| od | do | blok |
|---|---|---|
| 1 | 685 | PillBoxTest.ino  -  program DIAGNOSTYCZNY inteligentnego pudelka |
| 686 | 744 | SETUP |

**Funkcje** (28):

*PillBoxTest.ino  -  program DIAGNOSTYCZNY inteligentnego pudelka* — `naglowek`&nbsp;79, `wynik`&nbsp;83, `wynikF`&nbsp;88, `uwagaF`&nbsp;93, `info`&nbsp;99, `adcMediana`&nbsp;106, `napiecieOgniwa`&nbsp;115, `testBaterii`&nbsp;126, `ton`&nbsp;172, `ciszaBuzzera`&nbsp;180, `testBuzzera`&nbsp;191, `pudelkoOtwarte`&nbsp;216, `przyciskWcisniety`&nbsp;217, `testKontaktronu`&nbsp;219, `testPrzycisku`&nbsp;259, `testPamieci`&nbsp;284, `testWifi`&nbsp;314, `testCzasu`&nbsp;363, `wytnijPole`&nbsp;415, `hasloUrzadzenia`&nbsp;454, `firebaseLogowanie`&nbsp;468, `rtdb`&nbsp;535, `testFirebase`&nbsp;549, `testSnuStart`&nbsp;598, `testSnuKoniec`&nbsp;635, `podsumowanie`&nbsp;670

*SETUP* — `setup`&nbsp;689, `loop`&nbsp;743


---

## `firmware/PillBox/config.h` — 633 linii

| linia | grupa |
|---|---|
| 19 | /* --------------------------------------------------------------------- |
| 25 | /* --------------------------------------------------------------------- |
| 52 | /* --------------------------------------------------------------------- |
| 68 | /* --------------------------------------------------------------------- |
| 109 | /* --- ODBICIA STYKU KONTAKTRONU --------------------------------------- |
| 136 | /* --------------------------------------------------------------------- |
| 147 | /* --------------------------------------------------------------------- |
| 154 | /* --------------------------------------------------------------------- |
| 188 | /* --------------------------------------------------------------------- |
| 204 | /* --------------------------------------------------------------------- |
| 223 | /* --------------------------------------------------------------------- |
| 228 | /* --------------------------------------------------------------------- |
| 238 | /* --------------------------------------------------------------------- |
| 291 | /* --------------------------------------------------------------------- |
| 299 | /* --------------------------------------------------------------------- |
| 317 | /* --------------------------------------------------------------------- |
| 325 | /* --------------------------------------------------------------------- |
| 337 | /* --------------------------------------------------------------------- |
| 368 | /* --------------------------------------------------------------------- |
| 378 | /* --------------------------------------------------------------------- |
| 397 | /* --------------------------------------------------------------------- |
| 426 | /* --------------------------------------------------------------------- |
| 473 | /* --------------------------------------------------------------------- |
| 489 | /* --------------------------------------------------------------------- |
| 560 | /* --------------------------------------------------------------------- |
| 602 | /* --------------------------------------------------------------------- |
| 609 | /* --------------------------------------------------------------------- |
| 615 | /* --------------------------------------------------------------------- |
