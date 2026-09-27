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

## `index.html` — 8843 linii, ~130 tys. tokenow

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
| 1181 | 1273 | tab-set |
| 1274 | 1345 | tab-lek |
| 1346 | 1355 | tab-pud |
| 1356 | 1375 | tab-sinr |
| 1376 | 1415 | tab-wifi |
| 1416 | 1516 | tab-tg |
| 1517 | 1550 | tab-dev |
| 1551 | 1660 | tab-diag |
| 1661 | 1983 | tab-help |
| 1984 | 2005 | tab-ev |
| 2006 | 2096 | tab-hist |
| 2097 | 2105 | JS — poczatek |
| 2106 | 2335 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2336 | 2410 | INFORMACJA ZWROTNA |
| 2411 | 2505 | STREFY CZASOWE |
| 2506 | 2533 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2534 | 2600 | TABLETKA JAKO BRYŁA |
| 2601 | 2615 | LOGOWANIE |
| 2616 | 2755 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2756 | 3136 | START |
| 3137 | 3197 | OSŁONA RYSOWANIA |
| 3198 | 3256 | REKONCYLIACJA |
| 3257 | 3461 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3462 | 3520 | KALENDARZ |
| 3521 | 3791 | HISTORIA ROZPISANIA DAWKI |
| 3792 | 3981 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3982 | 4133 | ARKUSZ DNIA |
| 4134 | 4233 | WZIĄŁEM TERAZ |
| 4234 | 4348 | INR |
| 4349 | 4488 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4489 | 4501 | STATUS PUDEŁKA |
| 4502 | 4651 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4652 | 4813 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4814 | 5096 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 5097 | 5137 | KOLEJKA, KTÓRA NIE SCHODZI |
| 5138 | 5234 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 5235 | 5753 | EKRAN ZDARZEN |
| 5754 | 5872 | ZAPAS TABLETEK |
| 5873 | 6212 | USTAWIENIA |
| 6213 | 6520 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6521 | 6979 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6980 | 7350 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7351 | 7561 | ANALIZA |
| 7562 | 7947 | WYKRESY ANALIZY |
| 7948 | 8104 | RAPORT |
| 8105 | 8166 | KONTEKST DNIA (TAGI) |
| 8167 | 8217 | KOPIA ZAPASOWA |
| 8218 | 8431 | KOPIA NA TELEGRAM |
| 8432 | 8497 | WIEK KOPII |
| 8498 | 8638 | ODTWARZANIE Z KOPII |
| 8639 | 8713 | KOPIE Z BAZY |
| 8714 | 8801 | NAWIGACJA |
| 8802 | 8843 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (226) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `opisPlikFirmware`&nbsp;2147, `wersjaZTelegramem`&nbsp;2156, `pudelkoZnane`&nbsp;2162, `wybranePudelko`&nbsp;2164, `korzenDanych`&nbsp;2188, `odmowaRegul`&nbsp;2207, `sprawdzDostepPudelek`&nbsp;2212, `wybierzPudelko`&nbsp;2235, `profilTydzien`&nbsp;2284, `komoraDnia`&nbsp;2295, `ustawProfil`&nbsp;2310

*INFORMACJA ZWROTNA* — `toast`&nbsp;2357, `busy`&nbsp;2373, `todayKey`&nbsp;2400, `dzisiajKey`&nbsp;2404, `inNightWindow`&nbsp;2407

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2462, `tzName`&nbsp;2478, `tzLabel`&nbsp;2479, `tzOffsetTxt`&nbsp;2480, `devDate`&nbsp;2486, `devKey`&nbsp;2491, `devHM`&nbsp;2496, `slotMin`&nbsp;2497, `pillColors`&nbsp;2499

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2510

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2547, `cieniuj`&nbsp;2574, `doseGraphic`&nbsp;2591

*LOGOWANIE* — `doLogin`&nbsp;2605

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2625, `wyczyscCache`&nbsp;2720, `fbSignOut`&nbsp;2739

*START* — `boot`&nbsp;2757

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;3164, `rysujWszystkie`&nbsp;3177, `renderAll`&nbsp;3181

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;3204, `reconcileDecyzja`&nbsp;3243

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3278, `zapiszReconcile`&nbsp;3290, `doReconcile`&nbsp;3329, `doReconcileWewn`&nbsp;3339, `reconcile`&nbsp;3460

*KALENDARZ* — `tydzienDawek`&nbsp;3500

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3546, `dawkaNaDzien`&nbsp;3557, `dzienBezLeku`&nbsp;3578, `wyjatekNaDzien`&nbsp;3583, `opisDawkowania`&nbsp;3589, `dayDose`&nbsp;3603, `dzienZamkniety`&nbsp;3637, `trackingSince`&nbsp;3643, `beforeTracking`&nbsp;3644, `dayStatus`&nbsp;3646, `renderCalendar`&nbsp;3687, `seriaDni`&nbsp;3752, `doNastepnej`&nbsp;3770, `opisCzasu`&nbsp;3785

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3800, `trwanieTxt`&nbsp;3815, `kiedyDawkaTxt`&nbsp;3826, `odswiezOdDawki`&nbsp;3834, `startTikOdDawki`&nbsp;3848, `renderToday`&nbsp;3858

*ARKUSZ DNIA* — `closeSheet`&nbsp;3993, `renderSheet`&nbsp;3995, `resetDose`&nbsp;4073, `resetPlan`&nbsp;4080, `commitPlan`&nbsp;4085, `clearPlan`&nbsp;4101, `commitDose`&nbsp;4113

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;4154, `askConfirm`&nbsp;4223

*INR* — `inrState`&nbsp;4235, `odswiezTerminInr`&nbsp;4248, `addInr`&nbsp;4257, `inrKeysOk`&nbsp;4343

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4358, `inrTerminKey`&nbsp;4366, `inrDoTerminu`&nbsp;4378, `dniTxt`&nbsp;4387, `renderInr`&nbsp;4389, `inrChart`&nbsp;4461

*STATUS PUDEŁKA* — `relTime`&nbsp;4490, `devDayMon`&nbsp;4499

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4534, `renderBoxLog`&nbsp;4580, `logPrzelacz`&nbsp;4616, `renderNvsFailLog`&nbsp;4624

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4669, `oczekWczytaj`&nbsp;4677, `oczekZapisz`&nbsp;4682, `oczekIle`&nbsp;4685, `zapiszPewnie`&nbsp;4695, `zapiszCfg`&nbsp;4733, `bazaOdmowila`&nbsp;4751, `oczekWyslij`&nbsp;4775, `oczekOdmowy`&nbsp;4812

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4828, `ostrzReguly`&nbsp;4850, `lm`&nbsp;4889, `ostrzMilczy`&nbsp;4895, `nvsMalo`&nbsp;4975, `opisNvsFailKey`&nbsp;4987, `stratyDotyczaLeku`&nbsp;5040, `ostrzStraty`&nbsp;5052, `stratyCicho`&nbsp;5086

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;5118

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;5148, `renderOstrzezenia`&nbsp;5163, `bezPokrycia`&nbsp;5173, `wierszZdarzenia`&nbsp;5179, `renderDiag`&nbsp;5196

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5251, `evPasuje`&nbsp;5256, `renderEvents`&nbsp;5268, `renderOpenWarn`&nbsp;5308, `minutyDoPelna`&nbsp;5361, `opisLadowania`&nbsp;5373, `dni`&nbsp;5393, `opisLadowan`&nbsp;5396, `tempoZHistorii`&nbsp;5449, `prognozaDni`&nbsp;5458, `opisPrognozy`&nbsp;5470, `czasKrotko`&nbsp;5492, `opisCzuwania`&nbsp;5500, `renderStatus`&nbsp;5518

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5757, `dayAfter`&nbsp;5760, `pillsBaseInfo`&nbsp;5771, `settlePills`&nbsp;5781, `dniZapasu`&nbsp;5821, `renderPills`&nbsp;5834, `savePills`&nbsp;5855, `setPills`&nbsp;5866

*USTAWIENIA* — `renderKafelki`&nbsp;5877, `renderPudelka`&nbsp;5913, `renderSettings`&nbsp;5955, `tydzienZPol`&nbsp;6011, `renderWeekEditor`&nbsp;6023, `odswiezPodpowiedzTygodnia`&nbsp;6040, `tydzienZmieniony`&nbsp;6054, `rownajTydzien`&nbsp;6055, `renderPlanList`&nbsp;6103, `renderExceptions`&nbsp;6130, `wyslijSiec`&nbsp;6188

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6230, `tgZapytaj`&nbsp;6240, `tgKodParowania`&nbsp;6280, `tgZnajdzCzat`&nbsp;6296, `tgPolacz`&nbsp;6364, `tgProbna`&nbsp;6397, `tgOdlacz`&nbsp;6404, `renderTgStan`&nbsp;6426

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6541, `pobierzOpisFirmware`&nbsp;6547, `wyslijAktualizacje`&nbsp;6570, `anulujAktualizacje`&nbsp;6615, `renderOta`&nbsp;6621, `renderNetStan`&nbsp;6906

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6991, `renderSkan`&nbsp;6997, `szukajSieci`&nbsp;7049, `wybierzSiec`&nbsp;7057, `wyslijPolecenieSieci`&nbsp;7075, `siecZIndeksu`&nbsp;7085, `tzChanged`&nbsp;7119, `cfgTime`&nbsp;7124, `addSlot`&nbsp;7132, `zapiszPlanDnia`&nbsp;7145, `saveConfig`&nbsp;7163, `inrKrokiZakresu`&nbsp;7233, `opcjeInr`&nbsp;7240, `inrZakresZmieniony`&nbsp;7251, `wypelnijListyZakresu`&nbsp;7264, `saveInrRange`&nbsp;7275, `wypelnijListeOdstepu`&nbsp;7309, `saveInrEvery`&nbsp;7322, `odswiezPodpowiedzInr`&nbsp;7332

*ANALIZA* — `openTimeOf`&nbsp;7357, `openMinutes`&nbsp;7363, `sredniaPora`&nbsp;7387, `kwantyl`&nbsp;7395, `dniMiedzy`&nbsp;7403, `odstepyZPunktow`&nbsp;7417, `analyze`&nbsp;7426, `inrContext`&nbsp;7528

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7587, `rytmSVG`&nbsp;7599, `poryWCzasieSVG`&nbsp;7661, `iskraSVG`&nbsp;7729, `dowSVG`&nbsp;7757, `dniRytmu`&nbsp;7793, `skutecznoscTygodniami`&nbsp;7814, `renderAnalysis`&nbsp;7842

*RAPORT* — `collectRows`&nbsp;7949, `makeReport`&nbsp;7986

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;8137, `tagiPrzed`&nbsp;8145, `tagPrzelacz`&nbsp;8154

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;8197, `opisKopii`&nbsp;8207

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8252, `tgCzatKopii`&nbsp;8259, `odswiezKopie`&nbsp;8266, `tgKopiaCzatZapisz`&nbsp;8274, `tgKopiaCzatZnajdz`&nbsp;8294, `tgKopiaWlacz`&nbsp;8324, `tgKopiaWylacz`&nbsp;8343, `kopiaNaTelegram`&nbsp;8352, `kopiaAutomat`&nbsp;8403

*WIEK KOPII* — `dniOdDaty`&nbsp;8450, `wiekKopiiTxt`&nbsp;8456, `renderKopiaStan`&nbsp;8464, `zapiszKopie`&nbsp;8479

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8513, `ustawieniaDoOdtworzenia`&nbsp;8561, `wczytajKopie`&nbsp;8576, `kopiaCzytelna`&nbsp;8581, `odtworzKopie`&nbsp;8591, `kopiaWybrana`&nbsp;8624

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8648, `odtworzZBazy`&nbsp;8680, `exportCsv`&nbsp;8692

*NAWIGACJA* — `wrocZEkranu`&nbsp;8800


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
