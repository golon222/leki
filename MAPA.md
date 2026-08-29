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

## `index.html` — 8114 linii, ~130 tys. tokenow

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
| 4864 | 5198 | EKRAN ZDARZEN |
| 5199 | 5317 | ZAPAS TABLETEK |
| 5318 | 5547 | USTAWIENIA |
| 5548 | 5855 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 5856 | 6312 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6313 | 6674 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 6675 | 6885 | ANALIZA |
| 6886 | 7271 | WYKRESY ANALIZY |
| 7272 | 7428 | RAPORT |
| 7429 | 7490 | KONTEKST DNIA (TAGI) |
| 7491 | 7536 | KOPIA ZAPASOWA |
| 7537 | 7750 | KOPIA NA TELEGRAM |
| 7751 | 7816 | WIEK KOPII |
| 7817 | 7914 | ODTWARZANIE Z KOPII |
| 7915 | 7989 | KOPIE Z BAZY |
| 7990 | 8072 | NAWIGACJA |
| 8073 | 8114 | AUTOMATYCZNA AKTUALIZACJA |

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

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5202, `dayAfter`&nbsp;5205, `pillsBaseInfo`&nbsp;5216, `settlePills`&nbsp;5226, `dniZapasu`&nbsp;5266, `renderPills`&nbsp;5279, `savePills`&nbsp;5300, `setPills`&nbsp;5311

*USTAWIENIA* — `renderKafelki`&nbsp;5322, `renderSettings`&nbsp;5349, `tydzienZPol`&nbsp;5393, `renderWeekEditor`&nbsp;5405, `odswiezPodpowiedzTygodnia`&nbsp;5422, `tydzienZmieniony`&nbsp;5436, `rownajTydzien`&nbsp;5437, `renderPlanList`&nbsp;5450, `renderExceptions`&nbsp;5477, `wyslijSiec`&nbsp;5523

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5565, `tgZapytaj`&nbsp;5575, `tgKodParowania`&nbsp;5615, `tgZnajdzCzat`&nbsp;5631, `tgPolacz`&nbsp;5699, `tgProbna`&nbsp;5732, `tgOdlacz`&nbsp;5739, `renderTgStan`&nbsp;5761

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;5876, `pobierzOpisFirmware`&nbsp;5882, `wyslijAktualizacje`&nbsp;5903, `anulujAktualizacje`&nbsp;5948, `renderOta`&nbsp;5954, `renderNetStan`&nbsp;6239

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6324, `renderSkan`&nbsp;6330, `szukajSieci`&nbsp;6382, `wybierzSiec`&nbsp;6390, `wyslijPolecenieSieci`&nbsp;6408, `siecZIndeksu`&nbsp;6418, `tzChanged`&nbsp;6452, `cfgTime`&nbsp;6457, `addSlot`&nbsp;6465, `zapiszPlanDnia`&nbsp;6478, `saveConfig`&nbsp;6496, `inrKrokiZakresu`&nbsp;6557, `opcjeInr`&nbsp;6564, `inrZakresZmieniony`&nbsp;6575, `wypelnijListyZakresu`&nbsp;6588, `saveInrRange`&nbsp;6599, `wypelnijListeOdstepu`&nbsp;6633, `saveInrEvery`&nbsp;6646, `odswiezPodpowiedzInr`&nbsp;6656

*ANALIZA* — `openTimeOf`&nbsp;6681, `openMinutes`&nbsp;6687, `sredniaPora`&nbsp;6711, `kwantyl`&nbsp;6719, `dniMiedzy`&nbsp;6727, `odstepyZPunktow`&nbsp;6741, `analyze`&nbsp;6750, `inrContext`&nbsp;6852

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;6911, `rytmSVG`&nbsp;6923, `poryWCzasieSVG`&nbsp;6985, `iskraSVG`&nbsp;7053, `dowSVG`&nbsp;7081, `dniRytmu`&nbsp;7117, `skutecznoscTygodniami`&nbsp;7138, `renderAnalysis`&nbsp;7166

*RAPORT* — `collectRows`&nbsp;7273, `makeReport`&nbsp;7310

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7461, `tagiPrzed`&nbsp;7469, `tagPrzelacz`&nbsp;7478

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7521, `opisKopii`&nbsp;7531

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7571, `tgCzatKopii`&nbsp;7578, `odswiezKopie`&nbsp;7585, `tgKopiaCzatZapisz`&nbsp;7593, `tgKopiaCzatZnajdz`&nbsp;7613, `tgKopiaWlacz`&nbsp;7643, `tgKopiaWylacz`&nbsp;7662, `kopiaNaTelegram`&nbsp;7671, `kopiaAutomat`&nbsp;7722

*WIEK KOPII* — `dniOdDaty`&nbsp;7769, `wiekKopiiTxt`&nbsp;7775, `renderKopiaStan`&nbsp;7783, `zapiszKopie`&nbsp;7798

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;7832, `wczytajKopie`&nbsp;7862, `kopiaCzytelna`&nbsp;7867, `odtworzKopie`&nbsp;7877, `kopiaWybrana`&nbsp;7900

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;7924, `odtworzZBazy`&nbsp;7956, `exportCsv`&nbsp;7968

*NAWIGACJA* — `wrocZEkranu`&nbsp;8071


---

## `firmware/PillBox/PillBox.ino` — 6567 linii

| od | do | blok |
|---|---|---|
| 1 | 64 | PillBox.ino  -  Inteligentne pudelko na leki / IoT Pill Reminder |
| 65 | 245 | PAMIEC RTC  (przezywa deep sleep, ginie po odlaczeniu zasilania) |
| 246 | 471 | STAN GLOBALNY |
| 472 | 692 | 1.  POMIAR BATERII |
| 693 | 855 | 2.  BUZZER  (pasywny piezo -> PWM przez LEDC) |
| 856 | 1020 | 3.  GPIO / WYBUDZANIE |
| 1021 | 1135 | 4.  HARMONOGRAM |
| 1136 | 1261 | 4a.  DNI BEZ LEKU |
| 1262 | 1282 | 4b.  PUDELKO ZOSTAWIONE OTWARTE |
| 1283 | 1506 | 4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego |
| 1507 | 1758 | 5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien) |
| 1759 | 2144 | 6.  WiFi |
| 2145 | 3200 | 7.  FIREBASE  (REST: Auth email/haslo + Realtime Database) |
| 3201 | 3565 | 8.  ZDARZENIA |
| 3566 | 3621 | 9.  ALARM |
| 3622 | 3840 | 10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth) |
| 3841 | 4332 | 10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59 |
| 4333 | 4452 | 10b. CZARNA SKRZYNKA |
| 4453 | 4801 | 10c. GESTY SERWISOWE I AUTOTEST |
| 4802 | 5224 | 10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67) |
| 5225 | 5822 | 11.  DEEP SLEEP |
| 5823 | 6567 | 12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany) |

**Funkcje** (178):

*STAN GLOBALNY* — `zanotujNvsFail`&nbsp;324, `nvsPutStr`&nbsp;343, `nvsPutU16`&nbsp;366, `nvsPutU32`&nbsp;389, `nvsWolneWpisy`&nbsp;404, `syncTimeNTP`&nbsp;447, `logbookJson`&nbsp;448, `setTakenDay`&nbsp;449, `note`&nbsp;451, `awakeTooLong`&nbsp;465, `extendAwake`&nbsp;467

*1.  POMIAR BATERII* — `readBatteryRaw`&nbsp;480, `battPercentFromCurve`&nbsp;520, `resetBatteryFilter`&nbsp;552, `zapiszKoniecLadowania`&nbsp;575, `trackCharging`&nbsp;585, `battSmooth`&nbsp;639, `readBattery`&nbsp;670

*2.  BUZZER  (pasywny piezo -> PWM przez LEDC)* — `buzzerInit`&nbsp;696, `buzzerTone`&nbsp;705, `buzzerTonCicho`&nbsp;716, `buzzerOff`&nbsp;725, `beepAck`&nbsp;737, `beepErr`&nbsp;761, `beepQueued`&nbsp;771, `beepAlreadyTaken`&nbsp;781, `beepNowaWersja`&nbsp;806, `beepLowStock`&nbsp;816, `beepLowBattery`&nbsp;825, `beepBoxOpen`&nbsp;841, `beepCharging`&nbsp;849

*3.  GPIO / WYBUDZANIE* — `configureInputs`&nbsp;859, `powodResetuOpis`&nbsp;881, `zanotujReset`&nbsp;899, `reedPoziomStabilny`&nbsp;944, `boxIsOpen`&nbsp;965, `buttonPressed`&nbsp;968, `wakeName`&nbsp;970

*4.  HARMONOGRAM* — `godzinaPoprawna`&nbsp;1034, `parseSchedule`&nbsp;1043, `loadSchedule`&nbsp;1056, `saveSchedule`&nbsp;1079, `localMinutesOfDay`&nbsp;1090, `slotMinutes`&nbsp;1097, `localDayNumber`&nbsp;1106, `matchSlot`&nbsp;1114, `secondsToDayBoundary`&nbsp;1129

*4a.  DNI BEZ LEKU* — `localWeekday`&nbsp;1156, `dateKeyToNum`&nbsp;1164, `dawkaNaDobe`&nbsp;1177, `dzisBezLeku`&nbsp;1187, `parseDoseWeek`&nbsp;1196, `parseDoseEx`&nbsp;1214, `saveDosing`&nbsp;1236, `loadDosing`&nbsp;1249

*4b.  PUDELKO ZOSTAWIONE OTWARTE* — `openWarnSecondsLeft`&nbsp;1273

*4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego* — `lidLogAdd`&nbsp;1316, `lidLogCount`&nbsp;1340, `jsonEscape`&nbsp;1350, `lidLogJson`&nbsp;1366, `lidLogClear`&nbsp;1396, `nvsFailLogDoWyslania`&nbsp;1411, `nvsFailLogJson`&nbsp;1421, `nvsFailLogOznaczWyslany`&nbsp;1439, `trackBoxOpen`&nbsp;1443, `secondsToNextSlot`&nbsp;1492

*5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien)* — `rekordTs`&nbsp;1523, `rekordBezDaty`&nbsp;1530, `tsDoBazy`&nbsp;1539, `queuePush`&nbsp;1543, `queueCount`&nbsp;1566, `queuePeek`&nbsp;1573, `queuePop`&nbsp;1588, `queueDrop`&nbsp;1607, `przesunZnaczniki`&nbsp;1631, `queueShiftTimestamps`&nbsp;1646, `queueNadajCzas`&nbsp;1694, `queueEpokaSkasuj`&nbsp;1737

*6.  WiFi* — `netKlucz`&nbsp;1776, `wifiSieciCount`&nbsp;1780, `wifiSiecSsid`&nbsp;1787, `wifiSiecPass`&nbsp;1796, `wifiListeZapisz`&nbsp;1823, `wifiListeCzytaj`&nbsp;1847, `wifiSiecDodaj`&nbsp;1860, `wifiSiecUsun`&nbsp;1891, `wifiSiecPriorytet`&nbsp;1924, `wifiSprobuj`&nbsp;1952, `wifiStart`&nbsp;1987, `wifiConnect`&nbsp;2007, `wifiOff`&nbsp;2076, `wifiUspij`&nbsp;2090, `syncTimeNTP`&nbsp;2095

*7.  FIREBASE  (REST: Auth email/haslo + Realtime Database)* — `tokenZPamieci`&nbsp;2162, `zapomnijToken`&nbsp;2171, `hasloJestPrawdziwe`&nbsp;2216, `hasloZPamieci`&nbsp;2221, `hasloWPamieci`&nbsp;2230, `hasloUtrwal`&nbsp;2234, `hasloDoLogowania`&nbsp;2247, `tgTokenZPamieci`&nbsp;2267, `tgChatZPamieci`&nbsp;2274, `tgSkonfigurowany`&nbsp;2283, `tgUtrwal`&nbsp;2290, `tgZapomnij`&nbsp;2302, `firebaseSignIn`&nbsp;2336, `rtdbUrl`&nbsp;2431, `rtdbSend`&nbsp;2453, `rekordKompletny`&nbsp;2480, `pushEventRecord`&nbsp;2489, `pushLidState`&nbsp;2546, `otaSumaZPamieci`&nbsp;2588, `otaSumaWgranej`&nbsp;2610, `pushStatus`&nbsp;2616, `fetchConfig`&nbsp;2834, `trwaleOdrzucony`&nbsp;3158, `flushQueue`&nbsp;3162

*8.  ZDARZENIA* — `makeRecordAt`&nbsp;3204, `makeRecord`&nbsp;3226, `loadDayMarkers`&nbsp;3236, `clearDayMarkers`&nbsp;3255, `setTakenDay`&nbsp;3269, `setRolloverDay`&nbsp;3277, `zapiszDawke`&nbsp;3307, `oznaczAlarmObsluzony`&nbsp;3350, `alarmJuzObsluzony`&nbsp;3367, `ostatniSlotDoby`&nbsp;3393, `juzDzisBrane`&nbsp;3403, `checkDayRollover`&nbsp;3410, `reportEvent`&nbsp;3492

*9.  ALARM* — `alarmPotwierdzony`&nbsp;3589, `runAlarmWindow`&nbsp;3594

*10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth)* — `htmlEscape`&nbsp;3635, `portalPage`&nbsp;3649, `startWifiPortal`&nbsp;3693

*10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59* — `otaOpisDecyzji`&nbsp;3965, `otaZanotujProbe`&nbsp;3991, `otaWyzerujLicznik`&nbsp;3999, `otaZlecenieWBazie`&nbsp;4029, `otaPobierzOpis`&nbsp;4044, `otaWgraj`&nbsp;4088, `otaSprawdzPoStarcie`&nbsp;4246, `otaPotwierdzDzialanie`&nbsp;4279

*10b. CZARNA SKRZYNKA* — `note`&nbsp;4353, `wartoZapisac`&nbsp;4360, `logbookAdd`&nbsp;4372, `logbookPrint`&nbsp;4411, `logbookJson`&nbsp;4435

*10c. GESTY SERWISOWE I AUTOTEST* — `pikNumer`&nbsp;4631, `pikKoniecTestu`&nbsp;4643, `pikBrakSieci`&nbsp;4654, `wynikEtapu`&nbsp;4666, `etapTestu`&nbsp;4685, `autoTest`&nbsp;4690

*10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67)* — `tgWyslijTekst`&nbsp;4837, `tgZglosNieodebrane`&nbsp;4883, `tgSprawdzBaterie`&nbsp;4903, `tgSprawdzZapas`&nbsp;4922, `dniOdEry`&nbsp;4945, `dniDoDaty`&nbsp;4956, `inrPrzypomnienieTeraz`&nbsp;4990, `tgOznaczInrMiniete`&nbsp;5010, `sekundyDoInrPrzypomnienia`&nbsp;5019, `tgSprawdzInr`&nbsp;5041, `tgTekstZapas`&nbsp;5055, `tgTekstInr`&nbsp;5064, `tgTekstNieodebrane`&nbsp;5084, `tgTekstBateria`&nbsp;5093, `tgWyslijZalegle`&nbsp;5112

*11.  DEEP SLEEP* — `otaZglos`&nbsp;5243, `skanujSieci`&nbsp;5271, `otaSprobuj`&nbsp;5320, `kolejnePrzesuniecie`&nbsp;5519, `goToSleep`&nbsp;5524, `planNextSleep`&nbsp;5742

*12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany)* — `petlaLadowania`&nbsp;5835, `setup`&nbsp;5933, `loop`&nbsp;6564


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

## `firmware/PillBox/config.h` — 612 linii

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
| 269 | /* --------------------------------------------------------------------- |
| 277 | /* --------------------------------------------------------------------- |
| 295 | /* --------------------------------------------------------------------- |
| 303 | /* --------------------------------------------------------------------- |
| 315 | /* --------------------------------------------------------------------- |
| 346 | /* --------------------------------------------------------------------- |
| 356 | /* --------------------------------------------------------------------- |
| 375 | /* --------------------------------------------------------------------- |
| 404 | /* --------------------------------------------------------------------- |
| 451 | /* --------------------------------------------------------------------- |
| 468 | /* --------------------------------------------------------------------- |
| 539 | /* --------------------------------------------------------------------- |
| 581 | /* --------------------------------------------------------------------- |
| 588 | /* --------------------------------------------------------------------- |
| 594 | /* --------------------------------------------------------------------- |
