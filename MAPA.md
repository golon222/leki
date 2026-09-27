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

## `index.html` — 8788 linii, ~130 tys. tokenow

Ekrany (`<section>`) i dwa duze bloki. Zakladki `tab-*` odpowiadaja
pozycjom w pasku nawigacji i podekranom Ustawien.

| od | do | co |
|---|---|---|
| 21 | 21 | CSS — poczatek |
| 22 | 263 | SYSTEM WIZUALNY PillBox |
| 264 | 360 | EKRAN GŁÓWNY — KARTA DNIA |
| 361 | 433 | INFORMACJA ZWROTNA |
| 434 | 898 | TABLETKA W 3D |
| 899 | 1058 | tab-cal |
| 1059 | 1107 | tab-inr |
| 1108 | 1180 | tab-ana |
| 1181 | 1265 | tab-set |
| 1266 | 1337 | tab-lek |
| 1338 | 1347 | tab-pud |
| 1348 | 1367 | tab-sinr |
| 1368 | 1407 | tab-wifi |
| 1408 | 1508 | tab-tg |
| 1509 | 1542 | tab-dev |
| 1543 | 1643 | tab-diag |
| 1644 | 1945 | tab-help |
| 1946 | 1967 | tab-ev |
| 1968 | 2058 | tab-hist |
| 2059 | 2067 | JS — poczatek |
| 2068 | 2288 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2289 | 2363 | INFORMACJA ZWROTNA |
| 2364 | 2458 | STREFY CZASOWE |
| 2459 | 2486 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2487 | 2553 | TABLETKA JAKO BRYŁA |
| 2554 | 2568 | LOGOWANIE |
| 2569 | 2708 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2709 | 3089 | START |
| 3090 | 3150 | OSŁONA RYSOWANIA |
| 3151 | 3209 | REKONCYLIACJA |
| 3210 | 3414 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3415 | 3473 | KALENDARZ |
| 3474 | 3744 | HISTORIA ROZPISANIA DAWKI |
| 3745 | 3934 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3935 | 4086 | ARKUSZ DNIA |
| 4087 | 4186 | WZIĄŁEM TERAZ |
| 4187 | 4301 | INR |
| 4302 | 4441 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4442 | 4454 | STATUS PUDEŁKA |
| 4455 | 4604 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4605 | 4766 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4767 | 5049 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 5050 | 5090 | KOLEJKA, KTÓRA NIE SCHODZI |
| 5091 | 5187 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 5188 | 5706 | EKRAN ZDARZEN |
| 5707 | 5825 | ZAPAS TABLETEK |
| 5826 | 6163 | USTAWIENIA |
| 6164 | 6471 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6472 | 6930 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6931 | 7301 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7302 | 7512 | ANALIZA |
| 7513 | 7898 | WYKRESY ANALIZY |
| 7899 | 8055 | RAPORT |
| 8056 | 8117 | KONTEKST DNIA (TAGI) |
| 8118 | 8163 | KOPIA ZAPASOWA |
| 8164 | 8377 | KOPIA NA TELEGRAM |
| 8378 | 8443 | WIEK KOPII |
| 8444 | 8584 | ODTWARZANIE Z KOPII |
| 8585 | 8659 | KOPIE Z BAZY |
| 8660 | 8746 | NAWIGACJA |
| 8747 | 8788 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (225) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `opisPlikFirmware`&nbsp;2109, `pudelkoZnane`&nbsp;2115, `wybranePudelko`&nbsp;2117, `korzenDanych`&nbsp;2141, `odmowaRegul`&nbsp;2160, `sprawdzDostepPudelek`&nbsp;2165, `wybierzPudelko`&nbsp;2188, `profilTydzien`&nbsp;2237, `komoraDnia`&nbsp;2248, `ustawProfil`&nbsp;2263

*INFORMACJA ZWROTNA* — `toast`&nbsp;2310, `busy`&nbsp;2326, `todayKey`&nbsp;2353, `dzisiajKey`&nbsp;2357, `inNightWindow`&nbsp;2360

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2415, `tzName`&nbsp;2431, `tzLabel`&nbsp;2432, `tzOffsetTxt`&nbsp;2433, `devDate`&nbsp;2439, `devKey`&nbsp;2444, `devHM`&nbsp;2449, `slotMin`&nbsp;2450, `pillColors`&nbsp;2452

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2463

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2500, `cieniuj`&nbsp;2527, `doseGraphic`&nbsp;2544

*LOGOWANIE* — `doLogin`&nbsp;2558

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2578, `wyczyscCache`&nbsp;2673, `fbSignOut`&nbsp;2692

*START* — `boot`&nbsp;2710

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;3117, `rysujWszystkie`&nbsp;3130, `renderAll`&nbsp;3134

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;3157, `reconcileDecyzja`&nbsp;3196

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3231, `zapiszReconcile`&nbsp;3243, `doReconcile`&nbsp;3282, `doReconcileWewn`&nbsp;3292, `reconcile`&nbsp;3413

*KALENDARZ* — `tydzienDawek`&nbsp;3453

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3499, `dawkaNaDzien`&nbsp;3510, `dzienBezLeku`&nbsp;3531, `wyjatekNaDzien`&nbsp;3536, `opisDawkowania`&nbsp;3542, `dayDose`&nbsp;3556, `dzienZamkniety`&nbsp;3590, `trackingSince`&nbsp;3596, `beforeTracking`&nbsp;3597, `dayStatus`&nbsp;3599, `renderCalendar`&nbsp;3640, `seriaDni`&nbsp;3705, `doNastepnej`&nbsp;3723, `opisCzasu`&nbsp;3738

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3753, `trwanieTxt`&nbsp;3768, `kiedyDawkaTxt`&nbsp;3779, `odswiezOdDawki`&nbsp;3787, `startTikOdDawki`&nbsp;3801, `renderToday`&nbsp;3811

*ARKUSZ DNIA* — `closeSheet`&nbsp;3946, `renderSheet`&nbsp;3948, `resetDose`&nbsp;4026, `resetPlan`&nbsp;4033, `commitPlan`&nbsp;4038, `clearPlan`&nbsp;4054, `commitDose`&nbsp;4066

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;4107, `askConfirm`&nbsp;4176

*INR* — `inrState`&nbsp;4188, `odswiezTerminInr`&nbsp;4201, `addInr`&nbsp;4210, `inrKeysOk`&nbsp;4296

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4311, `inrTerminKey`&nbsp;4319, `inrDoTerminu`&nbsp;4331, `dniTxt`&nbsp;4340, `renderInr`&nbsp;4342, `inrChart`&nbsp;4414

*STATUS PUDEŁKA* — `relTime`&nbsp;4443, `devDayMon`&nbsp;4452

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4487, `renderBoxLog`&nbsp;4533, `logPrzelacz`&nbsp;4569, `renderNvsFailLog`&nbsp;4577

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4622, `oczekWczytaj`&nbsp;4630, `oczekZapisz`&nbsp;4635, `oczekIle`&nbsp;4638, `zapiszPewnie`&nbsp;4648, `zapiszCfg`&nbsp;4686, `bazaOdmowila`&nbsp;4704, `oczekWyslij`&nbsp;4728, `oczekOdmowy`&nbsp;4765

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4781, `ostrzReguly`&nbsp;4803, `lm`&nbsp;4842, `ostrzMilczy`&nbsp;4848, `nvsMalo`&nbsp;4928, `opisNvsFailKey`&nbsp;4940, `stratyDotyczaLeku`&nbsp;4993, `ostrzStraty`&nbsp;5005, `stratyCicho`&nbsp;5039

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;5071

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;5101, `renderOstrzezenia`&nbsp;5116, `bezPokrycia`&nbsp;5126, `wierszZdarzenia`&nbsp;5132, `renderDiag`&nbsp;5149

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5204, `evPasuje`&nbsp;5209, `renderEvents`&nbsp;5221, `renderOpenWarn`&nbsp;5261, `minutyDoPelna`&nbsp;5314, `opisLadowania`&nbsp;5326, `dni`&nbsp;5346, `opisLadowan`&nbsp;5349, `tempoZHistorii`&nbsp;5402, `prognozaDni`&nbsp;5411, `opisPrognozy`&nbsp;5423, `czasKrotko`&nbsp;5445, `opisCzuwania`&nbsp;5453, `renderStatus`&nbsp;5471

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5710, `dayAfter`&nbsp;5713, `pillsBaseInfo`&nbsp;5724, `settlePills`&nbsp;5734, `dniZapasu`&nbsp;5774, `renderPills`&nbsp;5787, `savePills`&nbsp;5808, `setPills`&nbsp;5819

*USTAWIENIA* — `renderKafelki`&nbsp;5830, `renderPudelka`&nbsp;5864, `renderSettings`&nbsp;5906, `tydzienZPol`&nbsp;5962, `renderWeekEditor`&nbsp;5974, `odswiezPodpowiedzTygodnia`&nbsp;5991, `tydzienZmieniony`&nbsp;6005, `rownajTydzien`&nbsp;6006, `renderPlanList`&nbsp;6054, `renderExceptions`&nbsp;6081, `wyslijSiec`&nbsp;6139

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6181, `tgZapytaj`&nbsp;6191, `tgKodParowania`&nbsp;6231, `tgZnajdzCzat`&nbsp;6247, `tgPolacz`&nbsp;6315, `tgProbna`&nbsp;6348, `tgOdlacz`&nbsp;6355, `renderTgStan`&nbsp;6377

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6492, `pobierzOpisFirmware`&nbsp;6498, `wyslijAktualizacje`&nbsp;6521, `anulujAktualizacje`&nbsp;6566, `renderOta`&nbsp;6572, `renderNetStan`&nbsp;6857

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6942, `renderSkan`&nbsp;6948, `szukajSieci`&nbsp;7000, `wybierzSiec`&nbsp;7008, `wyslijPolecenieSieci`&nbsp;7026, `siecZIndeksu`&nbsp;7036, `tzChanged`&nbsp;7070, `cfgTime`&nbsp;7075, `addSlot`&nbsp;7083, `zapiszPlanDnia`&nbsp;7096, `saveConfig`&nbsp;7114, `inrKrokiZakresu`&nbsp;7184, `opcjeInr`&nbsp;7191, `inrZakresZmieniony`&nbsp;7202, `wypelnijListyZakresu`&nbsp;7215, `saveInrRange`&nbsp;7226, `wypelnijListeOdstepu`&nbsp;7260, `saveInrEvery`&nbsp;7273, `odswiezPodpowiedzInr`&nbsp;7283

*ANALIZA* — `openTimeOf`&nbsp;7308, `openMinutes`&nbsp;7314, `sredniaPora`&nbsp;7338, `kwantyl`&nbsp;7346, `dniMiedzy`&nbsp;7354, `odstepyZPunktow`&nbsp;7368, `analyze`&nbsp;7377, `inrContext`&nbsp;7479

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7538, `rytmSVG`&nbsp;7550, `poryWCzasieSVG`&nbsp;7612, `iskraSVG`&nbsp;7680, `dowSVG`&nbsp;7708, `dniRytmu`&nbsp;7744, `skutecznoscTygodniami`&nbsp;7765, `renderAnalysis`&nbsp;7793

*RAPORT* — `collectRows`&nbsp;7900, `makeReport`&nbsp;7937

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;8088, `tagiPrzed`&nbsp;8096, `tagPrzelacz`&nbsp;8105

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;8148, `opisKopii`&nbsp;8158

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8198, `tgCzatKopii`&nbsp;8205, `odswiezKopie`&nbsp;8212, `tgKopiaCzatZapisz`&nbsp;8220, `tgKopiaCzatZnajdz`&nbsp;8240, `tgKopiaWlacz`&nbsp;8270, `tgKopiaWylacz`&nbsp;8289, `kopiaNaTelegram`&nbsp;8298, `kopiaAutomat`&nbsp;8349

*WIEK KOPII* — `dniOdDaty`&nbsp;8396, `wiekKopiiTxt`&nbsp;8402, `renderKopiaStan`&nbsp;8410, `zapiszKopie`&nbsp;8425

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8459, `ustawieniaDoOdtworzenia`&nbsp;8507, `wczytajKopie`&nbsp;8522, `kopiaCzytelna`&nbsp;8527, `odtworzKopie`&nbsp;8537, `kopiaWybrana`&nbsp;8570

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8594, `odtworzZBazy`&nbsp;8626, `exportCsv`&nbsp;8638

*NAWIGACJA* — `wrocZEkranu`&nbsp;8745


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
