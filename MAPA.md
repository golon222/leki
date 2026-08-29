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

## `index.html` — 8134 linii, ~130 tys. tokenow

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
| 4864 | 5218 | EKRAN ZDARZEN |
| 5219 | 5337 | ZAPAS TABLETEK |
| 5338 | 5567 | USTAWIENIA |
| 5568 | 5875 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 5876 | 6332 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6333 | 6694 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 6695 | 6905 | ANALIZA |
| 6906 | 7291 | WYKRESY ANALIZY |
| 7292 | 7448 | RAPORT |
| 7449 | 7510 | KONTEKST DNIA (TAGI) |
| 7511 | 7556 | KOPIA ZAPASOWA |
| 7557 | 7770 | KOPIA NA TELEGRAM |
| 7771 | 7836 | WIEK KOPII |
| 7837 | 7934 | ODTWARZANIE Z KOPII |
| 7935 | 8009 | KOPIE Z BAZY |
| 8010 | 8092 | NAWIGACJA |
| 8093 | 8134 | AUTOMATYCZNA AKTUALIZACJA |

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

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5222, `dayAfter`&nbsp;5225, `pillsBaseInfo`&nbsp;5236, `settlePills`&nbsp;5246, `dniZapasu`&nbsp;5286, `renderPills`&nbsp;5299, `savePills`&nbsp;5320, `setPills`&nbsp;5331

*USTAWIENIA* — `renderKafelki`&nbsp;5342, `renderSettings`&nbsp;5369, `tydzienZPol`&nbsp;5413, `renderWeekEditor`&nbsp;5425, `odswiezPodpowiedzTygodnia`&nbsp;5442, `tydzienZmieniony`&nbsp;5456, `rownajTydzien`&nbsp;5457, `renderPlanList`&nbsp;5470, `renderExceptions`&nbsp;5497, `wyslijSiec`&nbsp;5543

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5585, `tgZapytaj`&nbsp;5595, `tgKodParowania`&nbsp;5635, `tgZnajdzCzat`&nbsp;5651, `tgPolacz`&nbsp;5719, `tgProbna`&nbsp;5752, `tgOdlacz`&nbsp;5759, `renderTgStan`&nbsp;5781

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;5896, `pobierzOpisFirmware`&nbsp;5902, `wyslijAktualizacje`&nbsp;5923, `anulujAktualizacje`&nbsp;5968, `renderOta`&nbsp;5974, `renderNetStan`&nbsp;6259

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6344, `renderSkan`&nbsp;6350, `szukajSieci`&nbsp;6402, `wybierzSiec`&nbsp;6410, `wyslijPolecenieSieci`&nbsp;6428, `siecZIndeksu`&nbsp;6438, `tzChanged`&nbsp;6472, `cfgTime`&nbsp;6477, `addSlot`&nbsp;6485, `zapiszPlanDnia`&nbsp;6498, `saveConfig`&nbsp;6516, `inrKrokiZakresu`&nbsp;6577, `opcjeInr`&nbsp;6584, `inrZakresZmieniony`&nbsp;6595, `wypelnijListyZakresu`&nbsp;6608, `saveInrRange`&nbsp;6619, `wypelnijListeOdstepu`&nbsp;6653, `saveInrEvery`&nbsp;6666, `odswiezPodpowiedzInr`&nbsp;6676

*ANALIZA* — `openTimeOf`&nbsp;6701, `openMinutes`&nbsp;6707, `sredniaPora`&nbsp;6731, `kwantyl`&nbsp;6739, `dniMiedzy`&nbsp;6747, `odstepyZPunktow`&nbsp;6761, `analyze`&nbsp;6770, `inrContext`&nbsp;6872

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;6931, `rytmSVG`&nbsp;6943, `poryWCzasieSVG`&nbsp;7005, `iskraSVG`&nbsp;7073, `dowSVG`&nbsp;7101, `dniRytmu`&nbsp;7137, `skutecznoscTygodniami`&nbsp;7158, `renderAnalysis`&nbsp;7186

*RAPORT* — `collectRows`&nbsp;7293, `makeReport`&nbsp;7330

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7481, `tagiPrzed`&nbsp;7489, `tagPrzelacz`&nbsp;7498

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7541, `opisKopii`&nbsp;7551

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7591, `tgCzatKopii`&nbsp;7598, `odswiezKopie`&nbsp;7605, `tgKopiaCzatZapisz`&nbsp;7613, `tgKopiaCzatZnajdz`&nbsp;7633, `tgKopiaWlacz`&nbsp;7663, `tgKopiaWylacz`&nbsp;7682, `kopiaNaTelegram`&nbsp;7691, `kopiaAutomat`&nbsp;7742

*WIEK KOPII* — `dniOdDaty`&nbsp;7789, `wiekKopiiTxt`&nbsp;7795, `renderKopiaStan`&nbsp;7803, `zapiszKopie`&nbsp;7818

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;7852, `wczytajKopie`&nbsp;7882, `kopiaCzytelna`&nbsp;7887, `odtworzKopie`&nbsp;7897, `kopiaWybrana`&nbsp;7920

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;7944, `odtworzZBazy`&nbsp;7976, `exportCsv`&nbsp;7988

*NAWIGACJA* — `wrocZEkranu`&nbsp;8091


---

## `firmware/PillBox/PillBox.ino` — 6678 linii

| od | do | blok |
|---|---|---|
| 1 | 64 | PillBox.ino  -  Inteligentne pudelko na leki / IoT Pill Reminder |
| 65 | 252 | PAMIEC RTC  (przezywa deep sleep, ginie po odlaczeniu zasilania) |
| 253 | 478 | STAN GLOBALNY |
| 479 | 699 | 1.  POMIAR BATERII |
| 700 | 862 | 2.  BUZZER  (pasywny piezo -> PWM przez LEDC) |
| 863 | 1027 | 3.  GPIO / WYBUDZANIE |
| 1028 | 1142 | 4.  HARMONOGRAM |
| 1143 | 1268 | 4a.  DNI BEZ LEKU |
| 1269 | 1289 | 4b.  PUDELKO ZOSTAWIONE OTWARTE |
| 1290 | 1513 | 4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego |
| 1514 | 1765 | 5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien) |
| 1766 | 2250 | 6.  WiFi |
| 2251 | 3312 | 7.  FIREBASE  (REST: Auth email/haslo + Realtime Database) |
| 3313 | 3677 | 8.  ZDARZENIA |
| 3678 | 3733 | 9.  ALARM |
| 3734 | 3952 | 10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth) |
| 3953 | 4444 | 10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59 |
| 4445 | 4564 | 10b. CZARNA SKRZYNKA |
| 4565 | 4912 | 10c. GESTY SERWISOWE I AUTOTEST |
| 4913 | 5335 | 10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67) |
| 5336 | 5933 | 11.  DEEP SLEEP |
| 5934 | 6678 | 12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany) |

**Funkcje** (181):

*STAN GLOBALNY* — `zanotujNvsFail`&nbsp;331, `nvsPutStr`&nbsp;350, `nvsPutU16`&nbsp;373, `nvsPutU32`&nbsp;396, `nvsWolneWpisy`&nbsp;411, `syncTimeNTP`&nbsp;454, `logbookJson`&nbsp;455, `setTakenDay`&nbsp;456, `note`&nbsp;458, `awakeTooLong`&nbsp;472, `extendAwake`&nbsp;474

*1.  POMIAR BATERII* — `readBatteryRaw`&nbsp;487, `battPercentFromCurve`&nbsp;527, `resetBatteryFilter`&nbsp;559, `zapiszKoniecLadowania`&nbsp;582, `trackCharging`&nbsp;592, `battSmooth`&nbsp;646, `readBattery`&nbsp;677

*2.  BUZZER  (pasywny piezo -> PWM przez LEDC)* — `buzzerInit`&nbsp;703, `buzzerTone`&nbsp;712, `buzzerTonCicho`&nbsp;723, `buzzerOff`&nbsp;732, `beepAck`&nbsp;744, `beepErr`&nbsp;768, `beepQueued`&nbsp;778, `beepAlreadyTaken`&nbsp;788, `beepNowaWersja`&nbsp;813, `beepLowStock`&nbsp;823, `beepLowBattery`&nbsp;832, `beepBoxOpen`&nbsp;848, `beepCharging`&nbsp;856

*3.  GPIO / WYBUDZANIE* — `configureInputs`&nbsp;866, `powodResetuOpis`&nbsp;888, `zanotujReset`&nbsp;906, `reedPoziomStabilny`&nbsp;951, `boxIsOpen`&nbsp;972, `buttonPressed`&nbsp;975, `wakeName`&nbsp;977

*4.  HARMONOGRAM* — `godzinaPoprawna`&nbsp;1041, `parseSchedule`&nbsp;1050, `loadSchedule`&nbsp;1063, `saveSchedule`&nbsp;1086, `localMinutesOfDay`&nbsp;1097, `slotMinutes`&nbsp;1104, `localDayNumber`&nbsp;1113, `matchSlot`&nbsp;1121, `secondsToDayBoundary`&nbsp;1136

*4a.  DNI BEZ LEKU* — `localWeekday`&nbsp;1163, `dateKeyToNum`&nbsp;1171, `dawkaNaDobe`&nbsp;1184, `dzisBezLeku`&nbsp;1194, `parseDoseWeek`&nbsp;1203, `parseDoseEx`&nbsp;1221, `saveDosing`&nbsp;1243, `loadDosing`&nbsp;1256

*4b.  PUDELKO ZOSTAWIONE OTWARTE* — `openWarnSecondsLeft`&nbsp;1280

*4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego* — `lidLogAdd`&nbsp;1323, `lidLogCount`&nbsp;1347, `jsonEscape`&nbsp;1357, `lidLogJson`&nbsp;1373, `lidLogClear`&nbsp;1403, `nvsFailLogDoWyslania`&nbsp;1418, `nvsFailLogJson`&nbsp;1428, `nvsFailLogOznaczWyslany`&nbsp;1446, `trackBoxOpen`&nbsp;1450, `secondsToNextSlot`&nbsp;1499

*5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien)* — `rekordTs`&nbsp;1530, `rekordBezDaty`&nbsp;1537, `tsDoBazy`&nbsp;1546, `queuePush`&nbsp;1550, `queueCount`&nbsp;1573, `queuePeek`&nbsp;1580, `queuePop`&nbsp;1595, `queueDrop`&nbsp;1614, `przesunZnaczniki`&nbsp;1638, `queueShiftTimestamps`&nbsp;1653, `queueNadajCzas`&nbsp;1701, `queueEpokaSkasuj`&nbsp;1744

*6.  WiFi* — `netKlucz`&nbsp;1783, `wifiSieciCount`&nbsp;1787, `wifiSiecSsid`&nbsp;1794, `wifiSiecPass`&nbsp;1803, `wifiListeZapisz`&nbsp;1830, `wifiListeCzytaj`&nbsp;1854, `wifiSiecDodaj`&nbsp;1867, `wifiSiecUsun`&nbsp;1898, `wifiSiecPriorytet`&nbsp;1931, `wifiSprobuj`&nbsp;1959, `wifiOdNowa`&nbsp;2003, `wifiZacznijProbe`&nbsp;2014, `wifiStart`&nbsp;2034, `wifiKrokLaczenia`&nbsp;2074, `wifiConnect`&nbsp;2113, `wifiOff`&nbsp;2182, `wifiUspij`&nbsp;2196, `syncTimeNTP`&nbsp;2201

*7.  FIREBASE  (REST: Auth email/haslo + Realtime Database)* — `tokenZPamieci`&nbsp;2268, `zapomnijToken`&nbsp;2277, `hasloJestPrawdziwe`&nbsp;2322, `hasloZPamieci`&nbsp;2327, `hasloWPamieci`&nbsp;2336, `hasloUtrwal`&nbsp;2340, `hasloDoLogowania`&nbsp;2353, `tgTokenZPamieci`&nbsp;2373, `tgChatZPamieci`&nbsp;2380, `tgSkonfigurowany`&nbsp;2389, `tgUtrwal`&nbsp;2396, `tgZapomnij`&nbsp;2408, `firebaseSignIn`&nbsp;2442, `rtdbUrl`&nbsp;2537, `rtdbSend`&nbsp;2559, `rekordKompletny`&nbsp;2586, `pushEventRecord`&nbsp;2595, `pushLidState`&nbsp;2652, `otaSumaZPamieci`&nbsp;2694, `otaSumaWgranej`&nbsp;2716, `pushStatus`&nbsp;2722, `fetchConfig`&nbsp;2946, `trwaleOdrzucony`&nbsp;3270, `flushQueue`&nbsp;3274

*8.  ZDARZENIA* — `makeRecordAt`&nbsp;3316, `makeRecord`&nbsp;3338, `loadDayMarkers`&nbsp;3348, `clearDayMarkers`&nbsp;3367, `setTakenDay`&nbsp;3381, `setRolloverDay`&nbsp;3389, `zapiszDawke`&nbsp;3419, `oznaczAlarmObsluzony`&nbsp;3462, `alarmJuzObsluzony`&nbsp;3479, `ostatniSlotDoby`&nbsp;3505, `juzDzisBrane`&nbsp;3515, `checkDayRollover`&nbsp;3522, `reportEvent`&nbsp;3604

*9.  ALARM* — `alarmPotwierdzony`&nbsp;3701, `runAlarmWindow`&nbsp;3706

*10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth)* — `htmlEscape`&nbsp;3747, `portalPage`&nbsp;3761, `startWifiPortal`&nbsp;3805

*10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59* — `otaOpisDecyzji`&nbsp;4077, `otaZanotujProbe`&nbsp;4103, `otaWyzerujLicznik`&nbsp;4111, `otaZlecenieWBazie`&nbsp;4141, `otaPobierzOpis`&nbsp;4156, `otaWgraj`&nbsp;4200, `otaSprawdzPoStarcie`&nbsp;4358, `otaPotwierdzDzialanie`&nbsp;4391

*10b. CZARNA SKRZYNKA* — `note`&nbsp;4465, `wartoZapisac`&nbsp;4472, `logbookAdd`&nbsp;4484, `logbookPrint`&nbsp;4523, `logbookJson`&nbsp;4547

*10c. GESTY SERWISOWE I AUTOTEST* — `pikNumer`&nbsp;4742, `pikKoniecTestu`&nbsp;4754, `pikBrakSieci`&nbsp;4765, `wynikEtapu`&nbsp;4777, `etapTestu`&nbsp;4796, `autoTest`&nbsp;4801

*10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67)* — `tgWyslijTekst`&nbsp;4948, `tgZglosNieodebrane`&nbsp;4994, `tgSprawdzBaterie`&nbsp;5014, `tgSprawdzZapas`&nbsp;5033, `dniOdEry`&nbsp;5056, `dniDoDaty`&nbsp;5067, `inrPrzypomnienieTeraz`&nbsp;5101, `tgOznaczInrMiniete`&nbsp;5121, `sekundyDoInrPrzypomnienia`&nbsp;5130, `tgSprawdzInr`&nbsp;5152, `tgTekstZapas`&nbsp;5166, `tgTekstInr`&nbsp;5175, `tgTekstNieodebrane`&nbsp;5195, `tgTekstBateria`&nbsp;5204, `tgWyslijZalegle`&nbsp;5223

*11.  DEEP SLEEP* — `otaZglos`&nbsp;5354, `skanujSieci`&nbsp;5382, `otaSprobuj`&nbsp;5431, `kolejnePrzesuniecie`&nbsp;5630, `goToSleep`&nbsp;5635, `planNextSleep`&nbsp;5853

*12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany)* — `petlaLadowania`&nbsp;5946, `setup`&nbsp;6044, `loop`&nbsp;6675


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

## `firmware/PillBox/config.h` — 620 linii

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
| 277 | /* --------------------------------------------------------------------- |
| 285 | /* --------------------------------------------------------------------- |
| 303 | /* --------------------------------------------------------------------- |
| 311 | /* --------------------------------------------------------------------- |
| 323 | /* --------------------------------------------------------------------- |
| 354 | /* --------------------------------------------------------------------- |
| 364 | /* --------------------------------------------------------------------- |
| 383 | /* --------------------------------------------------------------------- |
| 412 | /* --------------------------------------------------------------------- |
| 459 | /* --------------------------------------------------------------------- |
| 476 | /* --------------------------------------------------------------------- |
| 547 | /* --------------------------------------------------------------------- |
| 589 | /* --------------------------------------------------------------------- |
| 596 | /* --------------------------------------------------------------------- |
| 602 | /* --------------------------------------------------------------------- |
