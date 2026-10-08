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

## `index.html` — 9119 linii, ~130 tys. tokenow

Ekrany (`<section>`) i dwa duze bloki. Zakladki `tab-*` odpowiadaja
pozycjom w pasku nawigacji i podekranom Ustawien.

| od | do | co |
|---|---|---|
| 21 | 21 | CSS — poczatek |
| 22 | 263 | SYSTEM WIZUALNY PillBox |
| 264 | 360 | EKRAN GŁÓWNY — KARTA DNIA |
| 361 | 433 | INFORMACJA ZWROTNA |
| 434 | 904 | TABLETKA W 3D |
| 905 | 1065 | tab-cal |
| 1066 | 1114 | tab-inr |
| 1115 | 1205 | tab-ana |
| 1206 | 1314 | tab-set |
| 1315 | 1395 | tab-lek |
| 1396 | 1405 | tab-pud |
| 1406 | 1425 | tab-sinr |
| 1426 | 1465 | tab-wifi |
| 1466 | 1566 | tab-tg |
| 1567 | 1600 | tab-dev |
| 1601 | 1710 | tab-diag |
| 1711 | 2037 | tab-help |
| 2038 | 2059 | tab-ev |
| 2060 | 2150 | tab-hist |
| 2151 | 2159 | JS — poczatek |
| 2160 | 2393 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2394 | 2468 | INFORMACJA ZWROTNA |
| 2469 | 2563 | STREFY CZASOWE |
| 2564 | 2591 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2592 | 2658 | TABLETKA JAKO BRYŁA |
| 2659 | 2673 | LOGOWANIE |
| 2674 | 2813 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2814 | 3194 | START |
| 3195 | 3255 | OSŁONA RYSOWANIA |
| 3256 | 3314 | REKONCYLIACJA |
| 3315 | 3519 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3520 | 3578 | KALENDARZ |
| 3579 | 3849 | HISTORIA ROZPISANIA DAWKI |
| 3850 | 4057 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 4058 | 4209 | ARKUSZ DNIA |
| 4210 | 4309 | WZIĄŁEM TERAZ |
| 4310 | 4424 | INR |
| 4425 | 4564 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4565 | 4577 | STATUS PUDEŁKA |
| 4578 | 4727 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4728 | 4889 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4890 | 5172 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 5173 | 5213 | KOLEJKA, KTÓRA NIE SCHODZI |
| 5214 | 5310 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 5311 | 5996 | EKRAN ZDARZEN |
| 5997 | 6115 | ZAPAS TABLETEK |
| 6116 | 6470 | USTAWIENIA |
| 6471 | 6778 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6779 | 7254 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 7255 | 7626 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7627 | 7837 | ANALIZA |
| 7838 | 8223 | WYKRESY ANALIZY |
| 8224 | 8380 | RAPORT |
| 8381 | 8442 | KONTEKST DNIA (TAGI) |
| 8443 | 8493 | KOPIA ZAPASOWA |
| 8494 | 8707 | KOPIA NA TELEGRAM |
| 8708 | 8773 | WIEK KOPII |
| 8774 | 8914 | ODTWARZANIE Z KOPII |
| 8915 | 8989 | KOPIE Z BAZY |
| 8990 | 9077 | NAWIGACJA |
| 9078 | 9119 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (231) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `opisPlikFirmware`&nbsp;2201, `wersjaZTelegramem`&nbsp;2210, `wersjaZOtwarciem`&nbsp;2222, `pudelkoZnane`&nbsp;2228, `wybranePudelko`&nbsp;2230, `korzenDanych`&nbsp;2254, `odmowaRegul`&nbsp;2273, `sprawdzDostepPudelek`&nbsp;2278, `wybierzPudelko`&nbsp;2301, `profilTydzien`&nbsp;2363, `ustawProfil`&nbsp;2368

*INFORMACJA ZWROTNA* — `toast`&nbsp;2415, `busy`&nbsp;2431, `todayKey`&nbsp;2458, `dzisiajKey`&nbsp;2462, `inNightWindow`&nbsp;2465

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2520, `tzName`&nbsp;2536, `tzLabel`&nbsp;2537, `tzOffsetTxt`&nbsp;2538, `devDate`&nbsp;2544, `devKey`&nbsp;2549, `devHM`&nbsp;2554, `slotMin`&nbsp;2555, `pillColors`&nbsp;2557

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2568

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2605, `cieniuj`&nbsp;2632, `doseGraphic`&nbsp;2649

*LOGOWANIE* — `doLogin`&nbsp;2663

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2683, `wyczyscCache`&nbsp;2778, `fbSignOut`&nbsp;2797

*START* — `boot`&nbsp;2815

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;3222, `rysujWszystkie`&nbsp;3235, `renderAll`&nbsp;3239

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;3262, `reconcileDecyzja`&nbsp;3301

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3336, `zapiszReconcile`&nbsp;3348, `doReconcile`&nbsp;3387, `doReconcileWewn`&nbsp;3397, `reconcile`&nbsp;3518

*KALENDARZ* — `tydzienDawek`&nbsp;3558

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3604, `dawkaNaDzien`&nbsp;3615, `dzienBezLeku`&nbsp;3636, `wyjatekNaDzien`&nbsp;3641, `opisDawkowania`&nbsp;3647, `dayDose`&nbsp;3661, `dzienZamkniety`&nbsp;3695, `trackingSince`&nbsp;3701, `beforeTracking`&nbsp;3702, `dayStatus`&nbsp;3704, `renderCalendar`&nbsp;3745, `seriaDni`&nbsp;3810, `doNastepnej`&nbsp;3828, `opisCzasu`&nbsp;3843

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3858, `trwanieTxt`&nbsp;3873, `kiedyDawkaTxt`&nbsp;3884, `odswiezOdDawki`&nbsp;3892, `startTikOdDawki`&nbsp;3906, `renderToday`&nbsp;3916

*ARKUSZ DNIA* — `closeSheet`&nbsp;4069, `renderSheet`&nbsp;4071, `resetDose`&nbsp;4149, `resetPlan`&nbsp;4156, `commitPlan`&nbsp;4161, `clearPlan`&nbsp;4177, `commitDose`&nbsp;4189

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;4230, `askConfirm`&nbsp;4299

*INR* — `inrState`&nbsp;4311, `odswiezTerminInr`&nbsp;4324, `addInr`&nbsp;4333, `inrKeysOk`&nbsp;4419

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4434, `inrTerminKey`&nbsp;4442, `inrDoTerminu`&nbsp;4454, `dniTxt`&nbsp;4463, `renderInr`&nbsp;4465, `inrChart`&nbsp;4537

*STATUS PUDEŁKA* — `relTime`&nbsp;4566, `devDayMon`&nbsp;4575

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4610, `renderBoxLog`&nbsp;4656, `logPrzelacz`&nbsp;4692, `renderNvsFailLog`&nbsp;4700

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4745, `oczekWczytaj`&nbsp;4753, `oczekZapisz`&nbsp;4758, `oczekIle`&nbsp;4761, `zapiszPewnie`&nbsp;4771, `zapiszCfg`&nbsp;4809, `bazaOdmowila`&nbsp;4827, `oczekWyslij`&nbsp;4851, `oczekOdmowy`&nbsp;4888

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4904, `ostrzReguly`&nbsp;4926, `lm`&nbsp;4965, `ostrzMilczy`&nbsp;4971, `nvsMalo`&nbsp;5051, `opisNvsFailKey`&nbsp;5063, `stratyDotyczaLeku`&nbsp;5116, `ostrzStraty`&nbsp;5128, `stratyCicho`&nbsp;5162

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;5194

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;5224, `renderOstrzezenia`&nbsp;5239, `bezPokrycia`&nbsp;5249, `wierszZdarzenia`&nbsp;5255, `renderDiag`&nbsp;5272

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5327, `evPasuje`&nbsp;5332, `renderEvents`&nbsp;5344, `opisPomiaruBaterii`&nbsp;5404, `nazwaOtwarcia`&nbsp;5416, `opisOtwartej`&nbsp;5434, `renderOpenWarn`&nbsp;5441, `minutyDoPelna`&nbsp;5502, `opisLadowania`&nbsp;5514, `opisLadowaniaCzujnik`&nbsp;5539, `dni`&nbsp;5555, `opisLadowan`&nbsp;5558, `tempoZHistorii`&nbsp;5611, `prognozaDni`&nbsp;5620, `opisPrognozy`&nbsp;5632, `czasKrotko`&nbsp;5654, `opisCzuwania`&nbsp;5662, `renderStatus`&nbsp;5680

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;6000, `dayAfter`&nbsp;6003, `pillsBaseInfo`&nbsp;6014, `settlePills`&nbsp;6024, `dniZapasu`&nbsp;6064, `renderPills`&nbsp;6077, `savePills`&nbsp;6098, `setPills`&nbsp;6109

*USTAWIENIA* — `opisLeku`&nbsp;6127, `renderKafelki`&nbsp;6133, `renderPudelka`&nbsp;6168, `renderSettings`&nbsp;6210, `tydzienZPol`&nbsp;6269, `renderWeekEditor`&nbsp;6281, `odswiezPodpowiedzTygodnia`&nbsp;6298, `tydzienZmieniony`&nbsp;6312, `rownajTydzien`&nbsp;6313, `renderPlanList`&nbsp;6361, `renderExceptions`&nbsp;6388, `wyslijSiec`&nbsp;6446

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6488, `tgZapytaj`&nbsp;6498, `tgKodParowania`&nbsp;6538, `tgZnajdzCzat`&nbsp;6554, `tgPolacz`&nbsp;6622, `tgProbna`&nbsp;6655, `tgOdlacz`&nbsp;6662, `renderTgStan`&nbsp;6684

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6799, `pobierzOpisFirmware`&nbsp;6805, `wyslijAktualizacje`&nbsp;6828, `anulujAktualizacje`&nbsp;6873, `renderOta`&nbsp;6879, `renderNetStan`&nbsp;7181

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;7266, `renderSkan`&nbsp;7272, `szukajSieci`&nbsp;7324, `wybierzSiec`&nbsp;7332, `wyslijPolecenieSieci`&nbsp;7350, `siecZIndeksu`&nbsp;7360, `tzChanged`&nbsp;7394, `cfgTime`&nbsp;7399, `addSlot`&nbsp;7407, `zapiszPlanDnia`&nbsp;7420, `saveConfig`&nbsp;7438, `inrKrokiZakresu`&nbsp;7509, `opcjeInr`&nbsp;7516, `inrZakresZmieniony`&nbsp;7527, `wypelnijListyZakresu`&nbsp;7540, `saveInrRange`&nbsp;7551, `wypelnijListeOdstepu`&nbsp;7585, `saveInrEvery`&nbsp;7598, `odswiezPodpowiedzInr`&nbsp;7608

*ANALIZA* — `openTimeOf`&nbsp;7633, `openMinutes`&nbsp;7639, `sredniaPora`&nbsp;7663, `kwantyl`&nbsp;7671, `dniMiedzy`&nbsp;7679, `odstepyZPunktow`&nbsp;7693, `analyze`&nbsp;7702, `inrContext`&nbsp;7804

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7863, `rytmSVG`&nbsp;7875, `poryWCzasieSVG`&nbsp;7937, `iskraSVG`&nbsp;8005, `dowSVG`&nbsp;8033, `dniRytmu`&nbsp;8069, `skutecznoscTygodniami`&nbsp;8090, `renderAnalysis`&nbsp;8118

*RAPORT* — `collectRows`&nbsp;8225, `makeReport`&nbsp;8262

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;8413, `tagiPrzed`&nbsp;8421, `tagPrzelacz`&nbsp;8430

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;8473, `opisKopii`&nbsp;8483

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8528, `tgCzatKopii`&nbsp;8535, `odswiezKopie`&nbsp;8542, `tgKopiaCzatZapisz`&nbsp;8550, `tgKopiaCzatZnajdz`&nbsp;8570, `tgKopiaWlacz`&nbsp;8600, `tgKopiaWylacz`&nbsp;8619, `kopiaNaTelegram`&nbsp;8628, `kopiaAutomat`&nbsp;8679

*WIEK KOPII* — `dniOdDaty`&nbsp;8726, `wiekKopiiTxt`&nbsp;8732, `renderKopiaStan`&nbsp;8740, `zapiszKopie`&nbsp;8755

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8789, `ustawieniaDoOdtworzenia`&nbsp;8837, `wczytajKopie`&nbsp;8852, `kopiaCzytelna`&nbsp;8857, `odtworzKopie`&nbsp;8867, `kopiaWybrana`&nbsp;8900

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8924, `odtworzZBazy`&nbsp;8956, `exportCsv`&nbsp;8968

*NAWIGACJA* — `wrocZEkranu`&nbsp;9076


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
