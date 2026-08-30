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

## `index.html` — 8150 linii, ~130 tys. tokenow

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
| 4393 | 4652 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4653 | 4693 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4694 | 4790 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 4791 | 5169 | EKRAN ZDARZEN |
| 5170 | 5288 | ZAPAS TABLETEK |
| 5289 | 5576 | USTAWIENIA |
| 5577 | 5884 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 5885 | 6341 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6342 | 6712 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 6713 | 6923 | ANALIZA |
| 6924 | 7309 | WYKRESY ANALIZY |
| 7310 | 7466 | RAPORT |
| 7467 | 7528 | KONTEKST DNIA (TAGI) |
| 7529 | 7574 | KOPIA ZAPASOWA |
| 7575 | 7788 | KOPIA NA TELEGRAM |
| 7789 | 7854 | WIEK KOPII |
| 7855 | 7952 | ODTWARZANIE Z KOPII |
| 7953 | 8027 | KOPIE Z BAZY |
| 8028 | 8108 | NAWIGACJA |
| 8109 | 8150 | AUTOMATYCZNA AKTUALIZACJA |

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

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4407, `ostrzReguly`&nbsp;4429, `lm`&nbsp;4468, `ostrzMilczy`&nbsp;4474, `nvsMalo`&nbsp;4531, `opisNvsFailKey`&nbsp;4543, `stratyDotyczaLeku`&nbsp;4596, `ostrzStraty`&nbsp;4608, `stratyCicho`&nbsp;4642

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4674

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4704, `renderOstrzezenia`&nbsp;4719, `bezPokrycia`&nbsp;4729, `wierszZdarzenia`&nbsp;4735, `renderDiag`&nbsp;4752

*EKRAN ZDARZEN* — `evFiltr`&nbsp;4807, `evPasuje`&nbsp;4812, `renderEvents`&nbsp;4824, `renderOpenWarn`&nbsp;4864, `minutyDoPelna`&nbsp;4917, `opisLadowania`&nbsp;4929, `dni`&nbsp;4949, `opisLadowan`&nbsp;4952, `renderStatus`&nbsp;4969

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5173, `dayAfter`&nbsp;5176, `pillsBaseInfo`&nbsp;5187, `settlePills`&nbsp;5197, `dniZapasu`&nbsp;5237, `renderPills`&nbsp;5250, `savePills`&nbsp;5271, `setPills`&nbsp;5282

*USTAWIENIA* — `renderKafelki`&nbsp;5293, `renderSettings`&nbsp;5320, `tydzienZPol`&nbsp;5375, `renderWeekEditor`&nbsp;5387, `odswiezPodpowiedzTygodnia`&nbsp;5404, `tydzienZmieniony`&nbsp;5418, `rownajTydzien`&nbsp;5419, `renderPlanList`&nbsp;5467, `renderExceptions`&nbsp;5494, `wyslijSiec`&nbsp;5552

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5594, `tgZapytaj`&nbsp;5604, `tgKodParowania`&nbsp;5644, `tgZnajdzCzat`&nbsp;5660, `tgPolacz`&nbsp;5728, `tgProbna`&nbsp;5761, `tgOdlacz`&nbsp;5768, `renderTgStan`&nbsp;5790

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;5905, `pobierzOpisFirmware`&nbsp;5911, `wyslijAktualizacje`&nbsp;5932, `anulujAktualizacje`&nbsp;5977, `renderOta`&nbsp;5983, `renderNetStan`&nbsp;6268

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6353, `renderSkan`&nbsp;6359, `szukajSieci`&nbsp;6411, `wybierzSiec`&nbsp;6419, `wyslijPolecenieSieci`&nbsp;6437, `siecZIndeksu`&nbsp;6447, `tzChanged`&nbsp;6481, `cfgTime`&nbsp;6486, `addSlot`&nbsp;6494, `zapiszPlanDnia`&nbsp;6507, `saveConfig`&nbsp;6525, `inrKrokiZakresu`&nbsp;6595, `opcjeInr`&nbsp;6602, `inrZakresZmieniony`&nbsp;6613, `wypelnijListyZakresu`&nbsp;6626, `saveInrRange`&nbsp;6637, `wypelnijListeOdstepu`&nbsp;6671, `saveInrEvery`&nbsp;6684, `odswiezPodpowiedzInr`&nbsp;6694

*ANALIZA* — `openTimeOf`&nbsp;6719, `openMinutes`&nbsp;6725, `sredniaPora`&nbsp;6749, `kwantyl`&nbsp;6757, `dniMiedzy`&nbsp;6765, `odstepyZPunktow`&nbsp;6779, `analyze`&nbsp;6788, `inrContext`&nbsp;6890

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;6949, `rytmSVG`&nbsp;6961, `poryWCzasieSVG`&nbsp;7023, `iskraSVG`&nbsp;7091, `dowSVG`&nbsp;7119, `dniRytmu`&nbsp;7155, `skutecznoscTygodniami`&nbsp;7176, `renderAnalysis`&nbsp;7204

*RAPORT* — `collectRows`&nbsp;7311, `makeReport`&nbsp;7348

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7499, `tagiPrzed`&nbsp;7507, `tagPrzelacz`&nbsp;7516

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7559, `opisKopii`&nbsp;7569

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7609, `tgCzatKopii`&nbsp;7616, `odswiezKopie`&nbsp;7623, `tgKopiaCzatZapisz`&nbsp;7631, `tgKopiaCzatZnajdz`&nbsp;7651, `tgKopiaWlacz`&nbsp;7681, `tgKopiaWylacz`&nbsp;7700, `kopiaNaTelegram`&nbsp;7709, `kopiaAutomat`&nbsp;7760

*WIEK KOPII* — `dniOdDaty`&nbsp;7807, `wiekKopiiTxt`&nbsp;7813, `renderKopiaStan`&nbsp;7821, `zapiszKopie`&nbsp;7836

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;7870, `wczytajKopie`&nbsp;7900, `kopiaCzytelna`&nbsp;7905, `odtworzKopie`&nbsp;7915, `kopiaWybrana`&nbsp;7938

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;7962, `odtworzZBazy`&nbsp;7994, `exportCsv`&nbsp;8006

*NAWIGACJA* — `wrocZEkranu`&nbsp;8107


---

## `firmware/PillBox/PillBox.ino` — 6667 linii

| od | do | blok |
|---|---|---|
| 1 | 64 | PillBox.ino  -  Inteligentne pudelko na leki / IoT Pill Reminder |
| 65 | 273 | PAMIEC RTC  (przezywa deep sleep, ginie po odlaczeniu zasilania) |
| 274 | 532 | STAN GLOBALNY |
| 533 | 753 | 1.  POMIAR BATERII |
| 754 | 916 | 2.  BUZZER  (pasywny piezo -> PWM przez LEDC) |
| 917 | 1081 | 3.  GPIO / WYBUDZANIE |
| 1082 | 1196 | 4.  HARMONOGRAM |
| 1197 | 1322 | 4a.  DNI BEZ LEKU |
| 1323 | 1344 | 4b.  PUDELKO ZOSTAWIONE OTWARTE |
| 1345 | 1477 | 4c.  DZIENNIK WIECZKA - USUNIETY (D109) |
| 1478 | 1729 | 5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien) |
| 1730 | 2192 | 6.  WiFi |
| 2193 | 3246 | 7.  FIREBASE  (REST: Auth email/haslo + Realtime Database) |
| 3247 | 3627 | 8.  ZDARZENIA |
| 3628 | 3683 | 9.  ALARM |
| 3684 | 3902 | 10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth) |
| 3903 | 4394 | 10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59 |
| 4395 | 4514 | 10b. CZARNA SKRZYNKA |
| 4515 | 4901 | 10c. GESTY SERWISOWE I AUTOTEST |
| 4902 | 5324 | 10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67) |
| 5325 | 5922 | 11.  DEEP SLEEP |
| 5923 | 6667 | 12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany) |

**Funkcje** (181):

*STAN GLOBALNY* — `zanotujNvsFail`&nbsp;352, `nvsPutStr`&nbsp;371, `nvsPutU16`&nbsp;394, `nvsPutU32`&nbsp;417, `nvsPutI16`&nbsp;443, `nvsPutU8`&nbsp;450, `nvsWolneWpisy`&nbsp;465, `syncTimeNTP`&nbsp;508, `logbookJson`&nbsp;509, `setTakenDay`&nbsp;510, `note`&nbsp;512, `awakeTooLong`&nbsp;526, `extendAwake`&nbsp;528

*1.  POMIAR BATERII* — `readBatteryRaw`&nbsp;541, `battPercentFromCurve`&nbsp;581, `resetBatteryFilter`&nbsp;613, `zapiszKoniecLadowania`&nbsp;636, `trackCharging`&nbsp;646, `battSmooth`&nbsp;700, `readBattery`&nbsp;731

*2.  BUZZER  (pasywny piezo -> PWM przez LEDC)* — `buzzerInit`&nbsp;757, `buzzerTone`&nbsp;766, `buzzerTonCicho`&nbsp;777, `buzzerOff`&nbsp;786, `beepAck`&nbsp;798, `beepErr`&nbsp;822, `beepQueued`&nbsp;832, `beepAlreadyTaken`&nbsp;842, `beepNowaWersja`&nbsp;867, `beepLowStock`&nbsp;877, `beepLowBattery`&nbsp;886, `beepBoxOpen`&nbsp;902, `beepCharging`&nbsp;910

*3.  GPIO / WYBUDZANIE* — `configureInputs`&nbsp;920, `powodResetuOpis`&nbsp;942, `zanotujReset`&nbsp;960, `reedPoziomStabilny`&nbsp;1005, `boxIsOpen`&nbsp;1026, `buttonPressed`&nbsp;1029, `wakeName`&nbsp;1031

*4.  HARMONOGRAM* — `godzinaPoprawna`&nbsp;1095, `parseSchedule`&nbsp;1104, `loadSchedule`&nbsp;1117, `saveSchedule`&nbsp;1140, `localMinutesOfDay`&nbsp;1151, `slotMinutes`&nbsp;1158, `localDayNumber`&nbsp;1167, `matchSlot`&nbsp;1175, `secondsToDayBoundary`&nbsp;1190

*4a.  DNI BEZ LEKU* — `localWeekday`&nbsp;1217, `dateKeyToNum`&nbsp;1225, `dawkaNaDobe`&nbsp;1238, `dzisBezLeku`&nbsp;1248, `parseDoseWeek`&nbsp;1257, `parseDoseEx`&nbsp;1275, `saveDosing`&nbsp;1297, `loadDosing`&nbsp;1310

*4b.  PUDELKO ZOSTAWIONE OTWARTE* — `openWarnSecondsLeft`&nbsp;1334

*4c.  DZIENNIK WIECZKA - USUNIETY (D109)* — `jsonEscape`&nbsp;1368, `nvsFailLogDoWyslania`&nbsp;1384, `nvsFailLogJson`&nbsp;1394, `nvsFailLogOznaczWyslany`&nbsp;1412, `trackBoxOpen`&nbsp;1416, `secondsToNextSlot`&nbsp;1463

*5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien)* — `rekordTs`&nbsp;1494, `rekordBezDaty`&nbsp;1501, `tsDoBazy`&nbsp;1510, `queuePush`&nbsp;1514, `queueCount`&nbsp;1537, `queuePeek`&nbsp;1544, `queuePop`&nbsp;1559, `queueDrop`&nbsp;1578, `przesunZnaczniki`&nbsp;1602, `queueShiftTimestamps`&nbsp;1617, `queueNadajCzas`&nbsp;1665, `queueEpokaSkasuj`&nbsp;1708

*6.  WiFi* — `netKlucz`&nbsp;1747, `wifiSieciCount`&nbsp;1751, `wifiSiecSsid`&nbsp;1758, `wifiSiecPass`&nbsp;1767, `wifiListeZapisz`&nbsp;1794, `wifiListeCzytaj`&nbsp;1818, `wifiSiecDodaj`&nbsp;1831, `wifiSiecUsun`&nbsp;1862, `wifiSiecPriorytet`&nbsp;1895, `zapamietajAp`&nbsp;1925, `apPodpowiedzPasuje`&nbsp;1936, `wifiBeginZPodpowiedzia`&nbsp;1944, `wifiCzekajNaLacze`&nbsp;1977, `wifiSprobuj`&nbsp;1990, `wifiConnect`&nbsp;2026, `wifiOff`&nbsp;2124, `wifiUspij`&nbsp;2138, `syncTimeNTP`&nbsp;2143

*7.  FIREBASE  (REST: Auth email/haslo + Realtime Database)* — `tokenZPamieci`&nbsp;2210, `zapomnijToken`&nbsp;2219, `hasloJestPrawdziwe`&nbsp;2264, `hasloZPamieci`&nbsp;2269, `hasloWPamieci`&nbsp;2278, `hasloUtrwal`&nbsp;2282, `hasloDoLogowania`&nbsp;2295, `tgTokenZPamieci`&nbsp;2315, `tgChatZPamieci`&nbsp;2322, `tgSkonfigurowany`&nbsp;2331, `tgUtrwal`&nbsp;2338, `tgZapomnij`&nbsp;2350, `firebaseSignIn`&nbsp;2384, `rtdbUrl`&nbsp;2479, `rtdbSend`&nbsp;2501, `rekordKompletny`&nbsp;2528, `pushEventRecord`&nbsp;2537, `pushLidState`&nbsp;2594, `otaSumaZPamieci`&nbsp;2645, `otaSumaWgranej`&nbsp;2667, `pushStatus`&nbsp;2673, `fetchConfig`&nbsp;2880, `trwaleOdrzucony`&nbsp;3204, `flushQueue`&nbsp;3208

*8.  ZDARZENIA* — `makeRecordAt`&nbsp;3250, `makeRecord`&nbsp;3272, `loadDayMarkers`&nbsp;3282, `clearDayMarkers`&nbsp;3301, `setTakenDay`&nbsp;3315, `setRolloverDay`&nbsp;3323, `zapiszDawke`&nbsp;3353, `oznaczAlarmObsluzony`&nbsp;3396, `alarmJuzObsluzony`&nbsp;3429, `ostatniSlotDoby`&nbsp;3455, `juzDzisBrane`&nbsp;3465, `checkDayRollover`&nbsp;3472, `reportEvent`&nbsp;3554

*9.  ALARM* — `alarmPotwierdzony`&nbsp;3651, `runAlarmWindow`&nbsp;3656

*10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth)* — `htmlEscape`&nbsp;3697, `portalPage`&nbsp;3711, `startWifiPortal`&nbsp;3755

*10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59* — `otaOpisDecyzji`&nbsp;4027, `otaZanotujProbe`&nbsp;4053, `otaWyzerujLicznik`&nbsp;4061, `otaZlecenieWBazie`&nbsp;4091, `otaPobierzOpis`&nbsp;4106, `otaWgraj`&nbsp;4150, `otaSprawdzPoStarcie`&nbsp;4308, `otaPotwierdzDzialanie`&nbsp;4341

*10b. CZARNA SKRZYNKA* — `note`&nbsp;4415, `wartoZapisac`&nbsp;4422, `logbookAdd`&nbsp;4434, `logbookPrint`&nbsp;4473, `logbookJson`&nbsp;4497

*10c. GESTY SERWISOWE I AUTOTEST* — `lidMeldunek`&nbsp;4538, `dozorKrok`&nbsp;4565, `pikNumer`&nbsp;4731, `pikKoniecTestu`&nbsp;4743, `pikBrakSieci`&nbsp;4754, `wynikEtapu`&nbsp;4766, `etapTestu`&nbsp;4785, `autoTest`&nbsp;4790

*10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67)* — `tgWyslijTekst`&nbsp;4937, `tgZglosNieodebrane`&nbsp;4983, `tgSprawdzBaterie`&nbsp;5003, `tgSprawdzZapas`&nbsp;5022, `dniOdEry`&nbsp;5045, `dniDoDaty`&nbsp;5056, `inrPrzypomnienieTeraz`&nbsp;5090, `tgOznaczInrMiniete`&nbsp;5110, `sekundyDoInrPrzypomnienia`&nbsp;5119, `tgSprawdzInr`&nbsp;5141, `tgTekstZapas`&nbsp;5155, `tgTekstInr`&nbsp;5164, `tgTekstNieodebrane`&nbsp;5184, `tgTekstBateria`&nbsp;5193, `tgWyslijZalegle`&nbsp;5212

*11.  DEEP SLEEP* — `otaZglos`&nbsp;5343, `skanujSieci`&nbsp;5371, `otaSprobuj`&nbsp;5420, `kolejnePrzesuniecie`&nbsp;5619, `goToSleep`&nbsp;5624, `planNextSleep`&nbsp;5842

*12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany)* — `petlaLadowania`&nbsp;5935, `setup`&nbsp;6033, `loop`&nbsp;6664


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
