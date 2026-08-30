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

## `firmware/PillBox/PillBox.ino` — 6865 linii

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
| 1730 | 2409 | 6.  WiFi |
| 2410 | 3463 | 7.  FIREBASE  (REST: Auth email/haslo + Realtime Database) |
| 3464 | 3844 | 8.  ZDARZENIA |
| 3845 | 3900 | 9.  ALARM |
| 3901 | 4119 | 10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth) |
| 4120 | 4611 | 10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59 |
| 4612 | 4731 | 10b. CZARNA SKRZYNKA |
| 4732 | 5099 | 10c. GESTY SERWISOWE I AUTOTEST |
| 5100 | 5522 | 10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67) |
| 5523 | 6120 | 11.  DEEP SLEEP |
| 6121 | 6865 | 12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany) |

**Funkcje** (186):

*STAN GLOBALNY* — `zanotujNvsFail`&nbsp;352, `nvsPutStr`&nbsp;371, `nvsPutU16`&nbsp;394, `nvsPutU32`&nbsp;417, `nvsPutI16`&nbsp;443, `nvsPutU8`&nbsp;450, `nvsWolneWpisy`&nbsp;465, `syncTimeNTP`&nbsp;508, `logbookJson`&nbsp;509, `setTakenDay`&nbsp;510, `note`&nbsp;512, `awakeTooLong`&nbsp;526, `extendAwake`&nbsp;528

*1.  POMIAR BATERII* — `readBatteryRaw`&nbsp;541, `battPercentFromCurve`&nbsp;581, `resetBatteryFilter`&nbsp;613, `zapiszKoniecLadowania`&nbsp;636, `trackCharging`&nbsp;646, `battSmooth`&nbsp;700, `readBattery`&nbsp;731

*2.  BUZZER  (pasywny piezo -> PWM przez LEDC)* — `buzzerInit`&nbsp;757, `buzzerTone`&nbsp;766, `buzzerTonCicho`&nbsp;777, `buzzerOff`&nbsp;786, `beepAck`&nbsp;798, `beepErr`&nbsp;822, `beepQueued`&nbsp;832, `beepAlreadyTaken`&nbsp;842, `beepNowaWersja`&nbsp;867, `beepLowStock`&nbsp;877, `beepLowBattery`&nbsp;886, `beepBoxOpen`&nbsp;902, `beepCharging`&nbsp;910

*3.  GPIO / WYBUDZANIE* — `configureInputs`&nbsp;920, `powodResetuOpis`&nbsp;942, `zanotujReset`&nbsp;960, `reedPoziomStabilny`&nbsp;1005, `boxIsOpen`&nbsp;1026, `buttonPressed`&nbsp;1029, `wakeName`&nbsp;1031

*4.  HARMONOGRAM* — `godzinaPoprawna`&nbsp;1095, `parseSchedule`&nbsp;1104, `loadSchedule`&nbsp;1117, `saveSchedule`&nbsp;1140, `localMinutesOfDay`&nbsp;1151, `slotMinutes`&nbsp;1158, `localDayNumber`&nbsp;1167, `matchSlot`&nbsp;1175, `secondsToDayBoundary`&nbsp;1190

*4a.  DNI BEZ LEKU* — `localWeekday`&nbsp;1217, `dateKeyToNum`&nbsp;1225, `dawkaNaDobe`&nbsp;1238, `dzisBezLeku`&nbsp;1248, `parseDoseWeek`&nbsp;1257, `parseDoseEx`&nbsp;1275, `saveDosing`&nbsp;1297, `loadDosing`&nbsp;1310

*4b.  PUDELKO ZOSTAWIONE OTWARTE* — `openWarnSecondsLeft`&nbsp;1334

*4c.  DZIENNIK WIECZKA - USUNIETY (D109)* — `jsonEscape`&nbsp;1368, `nvsFailLogDoWyslania`&nbsp;1384, `nvsFailLogJson`&nbsp;1394, `nvsFailLogOznaczWyslany`&nbsp;1412, `trackBoxOpen`&nbsp;1416, `secondsToNextSlot`&nbsp;1463

*5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien)* — `rekordTs`&nbsp;1494, `rekordBezDaty`&nbsp;1501, `tsDoBazy`&nbsp;1510, `queuePush`&nbsp;1514, `queueCount`&nbsp;1537, `queuePeek`&nbsp;1544, `queuePop`&nbsp;1559, `queueDrop`&nbsp;1578, `przesunZnaczniki`&nbsp;1602, `queueShiftTimestamps`&nbsp;1617, `queueNadajCzas`&nbsp;1665, `queueEpokaSkasuj`&nbsp;1708

*6.  WiFi* — `netKlucz`&nbsp;1747, `wifiSieciCount`&nbsp;1751, `wifiSiecSsid`&nbsp;1758, `wifiSiecPass`&nbsp;1767, `wifiListeZapisz`&nbsp;1794, `wifiListeCzytaj`&nbsp;1818, `wifiSiecDodaj`&nbsp;1831, `wifiSiecUsun`&nbsp;1862, `wifiSiecPriorytet`&nbsp;1895, `zapamietajAp`&nbsp;1925, `apPodpowiedzPasuje`&nbsp;1936, `wifiBeginZPodpowiedzia`&nbsp;1944, `wifiSprobuj`&nbsp;1955, `wifiZdarzenie`&nbsp;2028, `wifiOdNowa`&nbsp;2037, `wifiOstatniKandydat`&nbsp;2058, `wifiZacznijProbe`&nbsp;2063, `wifiOknoCiszy`&nbsp;2107, `wifiStart`&nbsp;2112, `wifiKrokLaczenia`&nbsp;2169, `wifiConnect`&nbsp;2246, `wifiOff`&nbsp;2341, `wifiUspij`&nbsp;2355, `syncTimeNTP`&nbsp;2360

*7.  FIREBASE  (REST: Auth email/haslo + Realtime Database)* — `tokenZPamieci`&nbsp;2427, `zapomnijToken`&nbsp;2436, `hasloJestPrawdziwe`&nbsp;2481, `hasloZPamieci`&nbsp;2486, `hasloWPamieci`&nbsp;2495, `hasloUtrwal`&nbsp;2499, `hasloDoLogowania`&nbsp;2512, `tgTokenZPamieci`&nbsp;2532, `tgChatZPamieci`&nbsp;2539, `tgSkonfigurowany`&nbsp;2548, `tgUtrwal`&nbsp;2555, `tgZapomnij`&nbsp;2567, `firebaseSignIn`&nbsp;2601, `rtdbUrl`&nbsp;2696, `rtdbSend`&nbsp;2718, `rekordKompletny`&nbsp;2745, `pushEventRecord`&nbsp;2754, `pushLidState`&nbsp;2811, `otaSumaZPamieci`&nbsp;2862, `otaSumaWgranej`&nbsp;2884, `pushStatus`&nbsp;2890, `fetchConfig`&nbsp;3097, `trwaleOdrzucony`&nbsp;3421, `flushQueue`&nbsp;3425

*8.  ZDARZENIA* — `makeRecordAt`&nbsp;3467, `makeRecord`&nbsp;3489, `loadDayMarkers`&nbsp;3499, `clearDayMarkers`&nbsp;3518, `setTakenDay`&nbsp;3532, `setRolloverDay`&nbsp;3540, `zapiszDawke`&nbsp;3570, `oznaczAlarmObsluzony`&nbsp;3613, `alarmJuzObsluzony`&nbsp;3646, `ostatniSlotDoby`&nbsp;3672, `juzDzisBrane`&nbsp;3682, `checkDayRollover`&nbsp;3689, `reportEvent`&nbsp;3771

*9.  ALARM* — `alarmPotwierdzony`&nbsp;3868, `runAlarmWindow`&nbsp;3873

*10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth)* — `htmlEscape`&nbsp;3914, `portalPage`&nbsp;3928, `startWifiPortal`&nbsp;3972

*10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59* — `otaOpisDecyzji`&nbsp;4244, `otaZanotujProbe`&nbsp;4270, `otaWyzerujLicznik`&nbsp;4278, `otaZlecenieWBazie`&nbsp;4308, `otaPobierzOpis`&nbsp;4323, `otaWgraj`&nbsp;4367, `otaSprawdzPoStarcie`&nbsp;4525, `otaPotwierdzDzialanie`&nbsp;4558

*10b. CZARNA SKRZYNKA* — `note`&nbsp;4632, `wartoZapisac`&nbsp;4639, `logbookAdd`&nbsp;4651, `logbookPrint`&nbsp;4690, `logbookJson`&nbsp;4714

*10c. GESTY SERWISOWE I AUTOTEST* — `lidMeldunek`&nbsp;4755, `pikNumer`&nbsp;4929, `pikKoniecTestu`&nbsp;4941, `pikBrakSieci`&nbsp;4952, `wynikEtapu`&nbsp;4964, `etapTestu`&nbsp;4983, `autoTest`&nbsp;4988

*10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67)* — `tgWyslijTekst`&nbsp;5135, `tgZglosNieodebrane`&nbsp;5181, `tgSprawdzBaterie`&nbsp;5201, `tgSprawdzZapas`&nbsp;5220, `dniOdEry`&nbsp;5243, `dniDoDaty`&nbsp;5254, `inrPrzypomnienieTeraz`&nbsp;5288, `tgOznaczInrMiniete`&nbsp;5308, `sekundyDoInrPrzypomnienia`&nbsp;5317, `tgSprawdzInr`&nbsp;5339, `tgTekstZapas`&nbsp;5353, `tgTekstInr`&nbsp;5362, `tgTekstNieodebrane`&nbsp;5382, `tgTekstBateria`&nbsp;5391, `tgWyslijZalegle`&nbsp;5410

*11.  DEEP SLEEP* — `otaZglos`&nbsp;5541, `skanujSieci`&nbsp;5569, `otaSprobuj`&nbsp;5618, `kolejnePrzesuniecie`&nbsp;5817, `goToSleep`&nbsp;5822, `planNextSleep`&nbsp;6040

*12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany)* — `petlaLadowania`&nbsp;6133, `setup`&nbsp;6231, `loop`&nbsp;6862


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

## `firmware/PillBox/config.h` — 641 linii

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
| 299 | /* --------------------------------------------------------------------- |
| 307 | /* --------------------------------------------------------------------- |
| 325 | /* --------------------------------------------------------------------- |
| 333 | /* --------------------------------------------------------------------- |
| 345 | /* --------------------------------------------------------------------- |
| 376 | /* --------------------------------------------------------------------- |
| 386 | /* --------------------------------------------------------------------- |
| 405 | /* --------------------------------------------------------------------- |
| 434 | /* --------------------------------------------------------------------- |
| 481 | /* --------------------------------------------------------------------- |
| 497 | /* --------------------------------------------------------------------- |
| 568 | /* --------------------------------------------------------------------- |
| 610 | /* --------------------------------------------------------------------- |
| 617 | /* --------------------------------------------------------------------- |
| 623 | /* --------------------------------------------------------------------- |
