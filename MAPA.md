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

## `index.html` — 8739 linii, ~130 tys. tokenow

Ekrany (`<section>`) i dwa duze bloki. Zakladki `tab-*` odpowiadaja
pozycjom w pasku nawigacji i podekranom Ustawien.

| od | do | co |
|---|---|---|
| 21 | 21 | CSS — poczatek |
| 22 | 263 | SYSTEM WIZUALNY PillBox |
| 264 | 360 | EKRAN GŁÓWNY — KARTA DNIA |
| 361 | 433 | INFORMACJA ZWROTNA |
| 434 | 893 | TABLETKA W 3D |
| 894 | 1053 | tab-cal |
| 1054 | 1102 | tab-inr |
| 1103 | 1175 | tab-ana |
| 1176 | 1260 | tab-set |
| 1261 | 1332 | tab-lek |
| 1333 | 1342 | tab-pud |
| 1343 | 1362 | tab-sinr |
| 1363 | 1402 | tab-wifi |
| 1403 | 1503 | tab-tg |
| 1504 | 1537 | tab-dev |
| 1538 | 1638 | tab-diag |
| 1639 | 1914 | tab-help |
| 1915 | 1936 | tab-ev |
| 1937 | 2027 | tab-hist |
| 2028 | 2036 | JS — poczatek |
| 2037 | 2241 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2242 | 2316 | INFORMACJA ZWROTNA |
| 2317 | 2411 | STREFY CZASOWE |
| 2412 | 2439 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2440 | 2506 | TABLETKA JAKO BRYŁA |
| 2507 | 2521 | LOGOWANIE |
| 2522 | 2661 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2662 | 3042 | START |
| 3043 | 3103 | OSŁONA RYSOWANIA |
| 3104 | 3162 | REKONCYLIACJA |
| 3163 | 3367 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3368 | 3426 | KALENDARZ |
| 3427 | 3697 | HISTORIA ROZPISANIA DAWKI |
| 3698 | 3887 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3888 | 4039 | ARKUSZ DNIA |
| 4040 | 4139 | WZIĄŁEM TERAZ |
| 4140 | 4254 | INR |
| 4255 | 4394 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4395 | 4407 | STATUS PUDEŁKA |
| 4408 | 4557 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4558 | 4719 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4720 | 5002 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 5003 | 5043 | KOLEJKA, KTÓRA NIE SCHODZI |
| 5044 | 5140 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 5141 | 5659 | EKRAN ZDARZEN |
| 5660 | 5778 | ZAPAS TABLETEK |
| 5779 | 6116 | USTAWIENIA |
| 6117 | 6424 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6425 | 6881 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6882 | 7252 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7253 | 7463 | ANALIZA |
| 7464 | 7849 | WYKRESY ANALIZY |
| 7850 | 8006 | RAPORT |
| 8007 | 8068 | KONTEKST DNIA (TAGI) |
| 8069 | 8114 | KOPIA ZAPASOWA |
| 8115 | 8328 | KOPIA NA TELEGRAM |
| 8329 | 8394 | WIEK KOPII |
| 8395 | 8535 | ODTWARZANIE Z KOPII |
| 8536 | 8610 | KOPIE Z BAZY |
| 8611 | 8697 | NAWIGACJA |
| 8698 | 8739 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (224) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `pudelkoZnane`&nbsp;2068, `wybranePudelko`&nbsp;2070, `korzenDanych`&nbsp;2094, `odmowaRegul`&nbsp;2113, `sprawdzDostepPudelek`&nbsp;2118, `wybierzPudelko`&nbsp;2141, `profilTydzien`&nbsp;2190, `komoraDnia`&nbsp;2201, `ustawProfil`&nbsp;2216

*INFORMACJA ZWROTNA* — `toast`&nbsp;2263, `busy`&nbsp;2279, `todayKey`&nbsp;2306, `dzisiajKey`&nbsp;2310, `inNightWindow`&nbsp;2313

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2368, `tzName`&nbsp;2384, `tzLabel`&nbsp;2385, `tzOffsetTxt`&nbsp;2386, `devDate`&nbsp;2392, `devKey`&nbsp;2397, `devHM`&nbsp;2402, `slotMin`&nbsp;2403, `pillColors`&nbsp;2405

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2416

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2453, `cieniuj`&nbsp;2480, `doseGraphic`&nbsp;2497

*LOGOWANIE* — `doLogin`&nbsp;2511

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2531, `wyczyscCache`&nbsp;2626, `fbSignOut`&nbsp;2645

*START* — `boot`&nbsp;2663

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;3070, `rysujWszystkie`&nbsp;3083, `renderAll`&nbsp;3087

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;3110, `reconcileDecyzja`&nbsp;3149

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3184, `zapiszReconcile`&nbsp;3196, `doReconcile`&nbsp;3235, `doReconcileWewn`&nbsp;3245, `reconcile`&nbsp;3366

*KALENDARZ* — `tydzienDawek`&nbsp;3406

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3452, `dawkaNaDzien`&nbsp;3463, `dzienBezLeku`&nbsp;3484, `wyjatekNaDzien`&nbsp;3489, `opisDawkowania`&nbsp;3495, `dayDose`&nbsp;3509, `dzienZamkniety`&nbsp;3543, `trackingSince`&nbsp;3549, `beforeTracking`&nbsp;3550, `dayStatus`&nbsp;3552, `renderCalendar`&nbsp;3593, `seriaDni`&nbsp;3658, `doNastepnej`&nbsp;3676, `opisCzasu`&nbsp;3691

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3706, `trwanieTxt`&nbsp;3721, `kiedyDawkaTxt`&nbsp;3732, `odswiezOdDawki`&nbsp;3740, `startTikOdDawki`&nbsp;3754, `renderToday`&nbsp;3764

*ARKUSZ DNIA* — `closeSheet`&nbsp;3899, `renderSheet`&nbsp;3901, `resetDose`&nbsp;3979, `resetPlan`&nbsp;3986, `commitPlan`&nbsp;3991, `clearPlan`&nbsp;4007, `commitDose`&nbsp;4019

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;4060, `askConfirm`&nbsp;4129

*INR* — `inrState`&nbsp;4141, `odswiezTerminInr`&nbsp;4154, `addInr`&nbsp;4163, `inrKeysOk`&nbsp;4249

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4264, `inrTerminKey`&nbsp;4272, `inrDoTerminu`&nbsp;4284, `dniTxt`&nbsp;4293, `renderInr`&nbsp;4295, `inrChart`&nbsp;4367

*STATUS PUDEŁKA* — `relTime`&nbsp;4396, `devDayMon`&nbsp;4405

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4440, `renderBoxLog`&nbsp;4486, `logPrzelacz`&nbsp;4522, `renderNvsFailLog`&nbsp;4530

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4575, `oczekWczytaj`&nbsp;4583, `oczekZapisz`&nbsp;4588, `oczekIle`&nbsp;4591, `zapiszPewnie`&nbsp;4601, `zapiszCfg`&nbsp;4639, `bazaOdmowila`&nbsp;4657, `oczekWyslij`&nbsp;4681, `oczekOdmowy`&nbsp;4718

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4734, `ostrzReguly`&nbsp;4756, `lm`&nbsp;4795, `ostrzMilczy`&nbsp;4801, `nvsMalo`&nbsp;4881, `opisNvsFailKey`&nbsp;4893, `stratyDotyczaLeku`&nbsp;4946, `ostrzStraty`&nbsp;4958, `stratyCicho`&nbsp;4992

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;5024

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;5054, `renderOstrzezenia`&nbsp;5069, `bezPokrycia`&nbsp;5079, `wierszZdarzenia`&nbsp;5085, `renderDiag`&nbsp;5102

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5157, `evPasuje`&nbsp;5162, `renderEvents`&nbsp;5174, `renderOpenWarn`&nbsp;5214, `minutyDoPelna`&nbsp;5267, `opisLadowania`&nbsp;5279, `dni`&nbsp;5299, `opisLadowan`&nbsp;5302, `tempoZHistorii`&nbsp;5355, `prognozaDni`&nbsp;5364, `opisPrognozy`&nbsp;5376, `czasKrotko`&nbsp;5398, `opisCzuwania`&nbsp;5406, `renderStatus`&nbsp;5424

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5663, `dayAfter`&nbsp;5666, `pillsBaseInfo`&nbsp;5677, `settlePills`&nbsp;5687, `dniZapasu`&nbsp;5727, `renderPills`&nbsp;5740, `savePills`&nbsp;5761, `setPills`&nbsp;5772

*USTAWIENIA* — `renderKafelki`&nbsp;5783, `renderPudelka`&nbsp;5817, `renderSettings`&nbsp;5859, `tydzienZPol`&nbsp;5915, `renderWeekEditor`&nbsp;5927, `odswiezPodpowiedzTygodnia`&nbsp;5944, `tydzienZmieniony`&nbsp;5958, `rownajTydzien`&nbsp;5959, `renderPlanList`&nbsp;6007, `renderExceptions`&nbsp;6034, `wyslijSiec`&nbsp;6092

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6134, `tgZapytaj`&nbsp;6144, `tgKodParowania`&nbsp;6184, `tgZnajdzCzat`&nbsp;6200, `tgPolacz`&nbsp;6268, `tgProbna`&nbsp;6301, `tgOdlacz`&nbsp;6308, `renderTgStan`&nbsp;6330

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6445, `pobierzOpisFirmware`&nbsp;6451, `wyslijAktualizacje`&nbsp;6472, `anulujAktualizacje`&nbsp;6517, `renderOta`&nbsp;6523, `renderNetStan`&nbsp;6808

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6893, `renderSkan`&nbsp;6899, `szukajSieci`&nbsp;6951, `wybierzSiec`&nbsp;6959, `wyslijPolecenieSieci`&nbsp;6977, `siecZIndeksu`&nbsp;6987, `tzChanged`&nbsp;7021, `cfgTime`&nbsp;7026, `addSlot`&nbsp;7034, `zapiszPlanDnia`&nbsp;7047, `saveConfig`&nbsp;7065, `inrKrokiZakresu`&nbsp;7135, `opcjeInr`&nbsp;7142, `inrZakresZmieniony`&nbsp;7153, `wypelnijListyZakresu`&nbsp;7166, `saveInrRange`&nbsp;7177, `wypelnijListeOdstepu`&nbsp;7211, `saveInrEvery`&nbsp;7224, `odswiezPodpowiedzInr`&nbsp;7234

*ANALIZA* — `openTimeOf`&nbsp;7259, `openMinutes`&nbsp;7265, `sredniaPora`&nbsp;7289, `kwantyl`&nbsp;7297, `dniMiedzy`&nbsp;7305, `odstepyZPunktow`&nbsp;7319, `analyze`&nbsp;7328, `inrContext`&nbsp;7430

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7489, `rytmSVG`&nbsp;7501, `poryWCzasieSVG`&nbsp;7563, `iskraSVG`&nbsp;7631, `dowSVG`&nbsp;7659, `dniRytmu`&nbsp;7695, `skutecznoscTygodniami`&nbsp;7716, `renderAnalysis`&nbsp;7744

*RAPORT* — `collectRows`&nbsp;7851, `makeReport`&nbsp;7888

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;8039, `tagiPrzed`&nbsp;8047, `tagPrzelacz`&nbsp;8056

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;8099, `opisKopii`&nbsp;8109

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8149, `tgCzatKopii`&nbsp;8156, `odswiezKopie`&nbsp;8163, `tgKopiaCzatZapisz`&nbsp;8171, `tgKopiaCzatZnajdz`&nbsp;8191, `tgKopiaWlacz`&nbsp;8221, `tgKopiaWylacz`&nbsp;8240, `kopiaNaTelegram`&nbsp;8249, `kopiaAutomat`&nbsp;8300

*WIEK KOPII* — `dniOdDaty`&nbsp;8347, `wiekKopiiTxt`&nbsp;8353, `renderKopiaStan`&nbsp;8361, `zapiszKopie`&nbsp;8376

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8410, `ustawieniaDoOdtworzenia`&nbsp;8458, `wczytajKopie`&nbsp;8473, `kopiaCzytelna`&nbsp;8478, `odtworzKopie`&nbsp;8488, `kopiaWybrana`&nbsp;8521

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8545, `odtworzZBazy`&nbsp;8577, `exportCsv`&nbsp;8589

*NAWIGACJA* — `wrocZEkranu`&nbsp;8696


---

## `firmware/PillBox/PillBox.ino` — 6981 linii

| od | do | blok |
|---|---|---|
| 1 | 64 | PillBox.ino  -  Inteligentne pudelko na leki / IoT Pill Reminder |
| 65 | 341 | PAMIEC RTC  (przezywa deep sleep, ginie po odlaczeniu zasilania) |
| 342 | 620 | STAN GLOBALNY |
| 621 | 883 | 1.  POMIAR BATERII |
| 884 | 1046 | 2.  BUZZER  (pasywny piezo -> PWM przez LEDC) |
| 1047 | 1234 | 3.  GPIO / WYBUDZANIE |
| 1235 | 1349 | 4.  HARMONOGRAM |
| 1350 | 1475 | 4a.  DNI BEZ LEKU |
| 1476 | 1497 | 4b.  PUDELKO ZOSTAWIONE OTWARTE |
| 1498 | 1630 | 4c.  DZIENNIK WIECZKA - USUNIETY (D109) |
| 1631 | 1882 | 5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien) |
| 1883 | 2399 | 6.  WiFi |
| 2400 | 3489 | 7.  FIREBASE  (REST: Auth email/haslo + Realtime Database) |
| 3490 | 3874 | 8.  ZDARZENIA |
| 3875 | 3930 | 9.  ALARM |
| 3931 | 4149 | 10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth) |
| 4150 | 4641 | 10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59 |
| 4642 | 4761 | 10b. CZARNA SKRZYNKA |
| 4762 | 5158 | 10c. GESTY SERWISOWE I AUTOTEST |
| 5159 | 5581 | 10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67) |
| 5582 | 6212 | 11.  DEEP SLEEP |
| 6213 | 6981 | 12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany) |

**Funkcje** (186):

*STAN GLOBALNY* — `zanotujNvsFail`&nbsp;420, `nvsPutStr`&nbsp;439, `nvsPutU16`&nbsp;462, `nvsPutU32`&nbsp;485, `nvsPutI16`&nbsp;511, `nvsPutU8`&nbsp;518, `nvsWolneWpisy`&nbsp;533, `radioDolicz`&nbsp;584, `syncTimeNTP`&nbsp;592, `logbookJson`&nbsp;593, `setTakenDay`&nbsp;598, `note`&nbsp;600, `awakeTooLong`&nbsp;614, `extendAwake`&nbsp;616

*1.  POMIAR BATERII* — `readBatteryRaw`&nbsp;629, `battPercentFromCurve`&nbsp;685, `resetBatteryFilter`&nbsp;717, `zapiszKoniecLadowania`&nbsp;740, `trackCharging`&nbsp;755, `battSmooth`&nbsp;809, `readBattery`&nbsp;861

*2.  BUZZER  (pasywny piezo -> PWM przez LEDC)* — `buzzerInit`&nbsp;887, `buzzerTone`&nbsp;896, `buzzerTonCicho`&nbsp;907, `buzzerOff`&nbsp;916, `beepAck`&nbsp;928, `beepErr`&nbsp;952, `beepQueued`&nbsp;962, `beepAlreadyTaken`&nbsp;972, `beepNowaWersja`&nbsp;997, `beepLowStock`&nbsp;1007, `beepLowBattery`&nbsp;1016, `beepBoxOpen`&nbsp;1032, `beepCharging`&nbsp;1040

*3.  GPIO / WYBUDZANIE* — `configureInputs`&nbsp;1050, `powodResetuOpis`&nbsp;1072, `zanotujReset`&nbsp;1090, `reedPoziomStabilny`&nbsp;1135, `boxIsOpen`&nbsp;1159, `boxIsOpenPewnie`&nbsp;1178, `buttonPressed`&nbsp;1182, `wakeName`&nbsp;1184

*4.  HARMONOGRAM* — `godzinaPoprawna`&nbsp;1248, `parseSchedule`&nbsp;1257, `loadSchedule`&nbsp;1270, `saveSchedule`&nbsp;1293, `localMinutesOfDay`&nbsp;1304, `slotMinutes`&nbsp;1311, `localDayNumber`&nbsp;1320, `matchSlot`&nbsp;1328, `secondsToDayBoundary`&nbsp;1343

*4a.  DNI BEZ LEKU* — `localWeekday`&nbsp;1370, `dateKeyToNum`&nbsp;1378, `dawkaNaDobe`&nbsp;1391, `dzisBezLeku`&nbsp;1401, `parseDoseWeek`&nbsp;1410, `parseDoseEx`&nbsp;1428, `saveDosing`&nbsp;1450, `loadDosing`&nbsp;1463

*4b.  PUDELKO ZOSTAWIONE OTWARTE* — `openWarnSecondsLeft`&nbsp;1487

*4c.  DZIENNIK WIECZKA - USUNIETY (D109)* — `jsonEscape`&nbsp;1521, `nvsFailLogDoWyslania`&nbsp;1537, `nvsFailLogJson`&nbsp;1547, `nvsFailLogOznaczWyslany`&nbsp;1565, `trackBoxOpen`&nbsp;1569, `secondsToNextSlot`&nbsp;1616

*5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien)* — `rekordTs`&nbsp;1647, `rekordBezDaty`&nbsp;1654, `tsDoBazy`&nbsp;1663, `queuePush`&nbsp;1667, `queueCount`&nbsp;1690, `queuePeek`&nbsp;1697, `queuePop`&nbsp;1712, `queueDrop`&nbsp;1731, `przesunZnaczniki`&nbsp;1755, `queueShiftTimestamps`&nbsp;1770, `queueNadajCzas`&nbsp;1818, `queueEpokaSkasuj`&nbsp;1861

*6.  WiFi* — `netKlucz`&nbsp;1900, `wifiSieciCount`&nbsp;1904, `wifiSiecSsid`&nbsp;1911, `wifiSiecPass`&nbsp;1920, `wifiListeZapisz`&nbsp;1947, `wifiListeCzytaj`&nbsp;1971, `wifiSiecDodaj`&nbsp;1984, `wifiSiecUsun`&nbsp;2015, `wifiSiecPriorytet`&nbsp;2048, `zapamietajAp`&nbsp;2078, `apPodpowiedzPasuje`&nbsp;2089, `wifiBeginZPodpowiedzia`&nbsp;2097, `wifiCzekajNaLacze`&nbsp;2130, `wifiSprobuj`&nbsp;2143, `netSkadZnany`&nbsp;2180, `netSkadZapamietaj`&nbsp;2195, `wifiConnect`&nbsp;2207, `wifiOff`&nbsp;2330, `wifiUspij`&nbsp;2344, `syncTimeNTP`&nbsp;2350

*7.  FIREBASE  (REST: Auth email/haslo + Realtime Database)* — `tokenZPamieci`&nbsp;2417, `zapomnijToken`&nbsp;2426, `hasloJestPrawdziwe`&nbsp;2471, `hasloZPamieci`&nbsp;2476, `hasloWPamieci`&nbsp;2485, `hasloUtrwal`&nbsp;2489, `hasloDoLogowania`&nbsp;2502, `tgTokenZPamieci`&nbsp;2522, `tgChatZPamieci`&nbsp;2529, `tgSkonfigurowany`&nbsp;2538, `tgUtrwal`&nbsp;2545, `tgZapomnij`&nbsp;2557, `firebaseSignIn`&nbsp;2591, `rtdbUrl`&nbsp;2686, `rtdbSend`&nbsp;2708, `rekordKompletny`&nbsp;2735, `pushEventRecord`&nbsp;2744, `pushLidState`&nbsp;2801, `otaSumaZPamieci`&nbsp;2852, `otaSumaWgranej`&nbsp;2874, `pushStatus`&nbsp;2897, `fetchConfig`&nbsp;3123, `trwaleOdrzucony`&nbsp;3447, `flushQueue`&nbsp;3451

*8.  ZDARZENIA* — `makeRecordAt`&nbsp;3493, `makeRecord`&nbsp;3515, `loadDayMarkers`&nbsp;3525, `clearDayMarkers`&nbsp;3544, `setTakenDay`&nbsp;3558, `setRolloverDay`&nbsp;3566, `zapiszDawke`&nbsp;3596, `oznaczAlarmObsluzony`&nbsp;3639, `alarmJuzObsluzony`&nbsp;3672, `ostatniSlotDoby`&nbsp;3698, `juzDzisBrane`&nbsp;3708, `checkDayRollover`&nbsp;3715, `reportEvent`&nbsp;3797

*9.  ALARM* — `alarmPotwierdzony`&nbsp;3898, `runAlarmWindow`&nbsp;3903

*10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth)* — `htmlEscape`&nbsp;3944, `portalPage`&nbsp;3958, `startWifiPortal`&nbsp;4002

*10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59* — `otaOpisDecyzji`&nbsp;4274, `otaZanotujProbe`&nbsp;4300, `otaWyzerujLicznik`&nbsp;4308, `otaZlecenieWBazie`&nbsp;4338, `otaPobierzOpis`&nbsp;4353, `otaWgraj`&nbsp;4397, `otaSprawdzPoStarcie`&nbsp;4555, `otaPotwierdzDzialanie`&nbsp;4588

*10b. CZARNA SKRZYNKA* — `note`&nbsp;4662, `wartoZapisac`&nbsp;4669, `logbookAdd`&nbsp;4681, `logbookPrint`&nbsp;4720, `logbookJson`&nbsp;4744

*10c. GESTY SERWISOWE I AUTOTEST* — `netSkadOpis`&nbsp;4787, `lidMeldunek`&nbsp;4795, `dozorKrok`&nbsp;4822, `pikNumer`&nbsp;4988, `pikKoniecTestu`&nbsp;5000, `pikBrakSieci`&nbsp;5011, `wynikEtapu`&nbsp;5023, `etapTestu`&nbsp;5042, `autoTest`&nbsp;5047

*10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67)* — `tgWyslijTekst`&nbsp;5194, `tgZglosNieodebrane`&nbsp;5240, `tgSprawdzBaterie`&nbsp;5260, `tgSprawdzZapas`&nbsp;5279, `dniOdEry`&nbsp;5302, `dniDoDaty`&nbsp;5313, `inrPrzypomnienieTeraz`&nbsp;5347, `tgOznaczInrMiniete`&nbsp;5367, `sekundyDoInrPrzypomnienia`&nbsp;5376, `tgSprawdzInr`&nbsp;5398, `tgTekstZapas`&nbsp;5412, `tgTekstInr`&nbsp;5421, `tgTekstNieodebrane`&nbsp;5441, `tgTekstBateria`&nbsp;5450, `tgWyslijZalegle`&nbsp;5469

*11.  DEEP SLEEP* — `otaZglos`&nbsp;5600, `skanujSieci`&nbsp;5628, `otaSprobuj`&nbsp;5677, `kolejnePrzesuniecie`&nbsp;5876, `goToSleep`&nbsp;5881, `planNextSleep`&nbsp;6109

*12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany)* — `petlaLadowania`&nbsp;6225, `setup`&nbsp;6323, `loop`&nbsp;6978


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

## `firmware/PillBox/config.h` — 690 linii

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
| 387 | /* --------------------------------------------------------------------- |
| 397 | /* --------------------------------------------------------------------- |
| 416 | /* --------------------------------------------------------------------- |
| 464 | /* --------------------------------------------------------------------- |
| 530 | /* --------------------------------------------------------------------- |
| 546 | /* --------------------------------------------------------------------- |
| 617 | /* --------------------------------------------------------------------- |
| 659 | /* --------------------------------------------------------------------- |
| 666 | /* --------------------------------------------------------------------- |
| 672 | /* --------------------------------------------------------------------- |
