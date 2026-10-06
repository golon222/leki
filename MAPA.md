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

## `index.html` — 8991 linii, ~130 tys. tokenow

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
| 1711 | 2033 | tab-help |
| 2034 | 2055 | tab-ev |
| 2056 | 2146 | tab-hist |
| 2147 | 2155 | JS — poczatek |
| 2156 | 2398 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2399 | 2473 | INFORMACJA ZWROTNA |
| 2474 | 2568 | STREFY CZASOWE |
| 2569 | 2596 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2597 | 2663 | TABLETKA JAKO BRYŁA |
| 2664 | 2678 | LOGOWANIE |
| 2679 | 2818 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2819 | 3199 | START |
| 3200 | 3260 | OSŁONA RYSOWANIA |
| 3261 | 3319 | REKONCYLIACJA |
| 3320 | 3524 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3525 | 3583 | KALENDARZ |
| 3584 | 3854 | HISTORIA ROZPISANIA DAWKI |
| 3855 | 4043 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 4044 | 4195 | ARKUSZ DNIA |
| 4196 | 4295 | WZIĄŁEM TERAZ |
| 4296 | 4410 | INR |
| 4411 | 4550 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4551 | 4563 | STATUS PUDEŁKA |
| 4564 | 4713 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4714 | 4875 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4876 | 5158 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 5159 | 5199 | KOLEJKA, KTÓRA NIE SCHODZI |
| 5200 | 5296 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 5297 | 5868 | EKRAN ZDARZEN |
| 5869 | 5987 | ZAPAS TABLETEK |
| 5988 | 6342 | USTAWIENIA |
| 6343 | 6650 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6651 | 7126 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 7127 | 7498 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7499 | 7709 | ANALIZA |
| 7710 | 8095 | WYKRESY ANALIZY |
| 8096 | 8252 | RAPORT |
| 8253 | 8314 | KONTEKST DNIA (TAGI) |
| 8315 | 8365 | KOPIA ZAPASOWA |
| 8366 | 8579 | KOPIA NA TELEGRAM |
| 8580 | 8645 | WIEK KOPII |
| 8646 | 8786 | ODTWARZANIE Z KOPII |
| 8787 | 8861 | KOPIE Z BAZY |
| 8862 | 8949 | NAWIGACJA |
| 8950 | 8991 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (228) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `opisPlikFirmware`&nbsp;2197, `wersjaZTelegramem`&nbsp;2206, `pudelkoZnane`&nbsp;2212, `wybranePudelko`&nbsp;2214, `korzenDanych`&nbsp;2238, `odmowaRegul`&nbsp;2257, `sprawdzDostepPudelek`&nbsp;2262, `wybierzPudelko`&nbsp;2285, `profilTydzien`&nbsp;2347, `komoraDnia`&nbsp;2358, `ustawProfil`&nbsp;2373

*INFORMACJA ZWROTNA* — `toast`&nbsp;2420, `busy`&nbsp;2436, `todayKey`&nbsp;2463, `dzisiajKey`&nbsp;2467, `inNightWindow`&nbsp;2470

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2525, `tzName`&nbsp;2541, `tzLabel`&nbsp;2542, `tzOffsetTxt`&nbsp;2543, `devDate`&nbsp;2549, `devKey`&nbsp;2554, `devHM`&nbsp;2559, `slotMin`&nbsp;2560, `pillColors`&nbsp;2562

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2573

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2610, `cieniuj`&nbsp;2637, `doseGraphic`&nbsp;2654

*LOGOWANIE* — `doLogin`&nbsp;2668

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2688, `wyczyscCache`&nbsp;2783, `fbSignOut`&nbsp;2802

*START* — `boot`&nbsp;2820

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;3227, `rysujWszystkie`&nbsp;3240, `renderAll`&nbsp;3244

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;3267, `reconcileDecyzja`&nbsp;3306

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3341, `zapiszReconcile`&nbsp;3353, `doReconcile`&nbsp;3392, `doReconcileWewn`&nbsp;3402, `reconcile`&nbsp;3523

*KALENDARZ* — `tydzienDawek`&nbsp;3563

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3609, `dawkaNaDzien`&nbsp;3620, `dzienBezLeku`&nbsp;3641, `wyjatekNaDzien`&nbsp;3646, `opisDawkowania`&nbsp;3652, `dayDose`&nbsp;3666, `dzienZamkniety`&nbsp;3700, `trackingSince`&nbsp;3706, `beforeTracking`&nbsp;3707, `dayStatus`&nbsp;3709, `renderCalendar`&nbsp;3750, `seriaDni`&nbsp;3815, `doNastepnej`&nbsp;3833, `opisCzasu`&nbsp;3848

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3863, `trwanieTxt`&nbsp;3878, `kiedyDawkaTxt`&nbsp;3889, `odswiezOdDawki`&nbsp;3897, `startTikOdDawki`&nbsp;3911, `renderToday`&nbsp;3921

*ARKUSZ DNIA* — `closeSheet`&nbsp;4055, `renderSheet`&nbsp;4057, `resetDose`&nbsp;4135, `resetPlan`&nbsp;4142, `commitPlan`&nbsp;4147, `clearPlan`&nbsp;4163, `commitDose`&nbsp;4175

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;4216, `askConfirm`&nbsp;4285

*INR* — `inrState`&nbsp;4297, `odswiezTerminInr`&nbsp;4310, `addInr`&nbsp;4319, `inrKeysOk`&nbsp;4405

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4420, `inrTerminKey`&nbsp;4428, `inrDoTerminu`&nbsp;4440, `dniTxt`&nbsp;4449, `renderInr`&nbsp;4451, `inrChart`&nbsp;4523

*STATUS PUDEŁKA* — `relTime`&nbsp;4552, `devDayMon`&nbsp;4561

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4596, `renderBoxLog`&nbsp;4642, `logPrzelacz`&nbsp;4678, `renderNvsFailLog`&nbsp;4686

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4731, `oczekWczytaj`&nbsp;4739, `oczekZapisz`&nbsp;4744, `oczekIle`&nbsp;4747, `zapiszPewnie`&nbsp;4757, `zapiszCfg`&nbsp;4795, `bazaOdmowila`&nbsp;4813, `oczekWyslij`&nbsp;4837, `oczekOdmowy`&nbsp;4874

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4890, `ostrzReguly`&nbsp;4912, `lm`&nbsp;4951, `ostrzMilczy`&nbsp;4957, `nvsMalo`&nbsp;5037, `opisNvsFailKey`&nbsp;5049, `stratyDotyczaLeku`&nbsp;5102, `ostrzStraty`&nbsp;5114, `stratyCicho`&nbsp;5148

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;5180

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;5210, `renderOstrzezenia`&nbsp;5225, `bezPokrycia`&nbsp;5235, `wierszZdarzenia`&nbsp;5241, `renderDiag`&nbsp;5258

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5313, `evPasuje`&nbsp;5318, `renderEvents`&nbsp;5330, `nazwaOtwarcia`&nbsp;5381, `renderOpenWarn`&nbsp;5389, `minutyDoPelna`&nbsp;5442, `opisLadowania`&nbsp;5454, `dni`&nbsp;5474, `opisLadowan`&nbsp;5477, `tempoZHistorii`&nbsp;5530, `prognozaDni`&nbsp;5539, `opisPrognozy`&nbsp;5551, `czasKrotko`&nbsp;5573, `opisCzuwania`&nbsp;5581, `renderStatus`&nbsp;5599

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5872, `dayAfter`&nbsp;5875, `pillsBaseInfo`&nbsp;5886, `settlePills`&nbsp;5896, `dniZapasu`&nbsp;5936, `renderPills`&nbsp;5949, `savePills`&nbsp;5970, `setPills`&nbsp;5981

*USTAWIENIA* — `opisLeku`&nbsp;5999, `renderKafelki`&nbsp;6005, `renderPudelka`&nbsp;6040, `renderSettings`&nbsp;6082, `tydzienZPol`&nbsp;6141, `renderWeekEditor`&nbsp;6153, `odswiezPodpowiedzTygodnia`&nbsp;6170, `tydzienZmieniony`&nbsp;6184, `rownajTydzien`&nbsp;6185, `renderPlanList`&nbsp;6233, `renderExceptions`&nbsp;6260, `wyslijSiec`&nbsp;6318

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6360, `tgZapytaj`&nbsp;6370, `tgKodParowania`&nbsp;6410, `tgZnajdzCzat`&nbsp;6426, `tgPolacz`&nbsp;6494, `tgProbna`&nbsp;6527, `tgOdlacz`&nbsp;6534, `renderTgStan`&nbsp;6556

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6671, `pobierzOpisFirmware`&nbsp;6677, `wyslijAktualizacje`&nbsp;6700, `anulujAktualizacje`&nbsp;6745, `renderOta`&nbsp;6751, `renderNetStan`&nbsp;7053

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;7138, `renderSkan`&nbsp;7144, `szukajSieci`&nbsp;7196, `wybierzSiec`&nbsp;7204, `wyslijPolecenieSieci`&nbsp;7222, `siecZIndeksu`&nbsp;7232, `tzChanged`&nbsp;7266, `cfgTime`&nbsp;7271, `addSlot`&nbsp;7279, `zapiszPlanDnia`&nbsp;7292, `saveConfig`&nbsp;7310, `inrKrokiZakresu`&nbsp;7381, `opcjeInr`&nbsp;7388, `inrZakresZmieniony`&nbsp;7399, `wypelnijListyZakresu`&nbsp;7412, `saveInrRange`&nbsp;7423, `wypelnijListeOdstepu`&nbsp;7457, `saveInrEvery`&nbsp;7470, `odswiezPodpowiedzInr`&nbsp;7480

*ANALIZA* — `openTimeOf`&nbsp;7505, `openMinutes`&nbsp;7511, `sredniaPora`&nbsp;7535, `kwantyl`&nbsp;7543, `dniMiedzy`&nbsp;7551, `odstepyZPunktow`&nbsp;7565, `analyze`&nbsp;7574, `inrContext`&nbsp;7676

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7735, `rytmSVG`&nbsp;7747, `poryWCzasieSVG`&nbsp;7809, `iskraSVG`&nbsp;7877, `dowSVG`&nbsp;7905, `dniRytmu`&nbsp;7941, `skutecznoscTygodniami`&nbsp;7962, `renderAnalysis`&nbsp;7990

*RAPORT* — `collectRows`&nbsp;8097, `makeReport`&nbsp;8134

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;8285, `tagiPrzed`&nbsp;8293, `tagPrzelacz`&nbsp;8302

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;8345, `opisKopii`&nbsp;8355

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8400, `tgCzatKopii`&nbsp;8407, `odswiezKopie`&nbsp;8414, `tgKopiaCzatZapisz`&nbsp;8422, `tgKopiaCzatZnajdz`&nbsp;8442, `tgKopiaWlacz`&nbsp;8472, `tgKopiaWylacz`&nbsp;8491, `kopiaNaTelegram`&nbsp;8500, `kopiaAutomat`&nbsp;8551

*WIEK KOPII* — `dniOdDaty`&nbsp;8598, `wiekKopiiTxt`&nbsp;8604, `renderKopiaStan`&nbsp;8612, `zapiszKopie`&nbsp;8627

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8661, `ustawieniaDoOdtworzenia`&nbsp;8709, `wczytajKopie`&nbsp;8724, `kopiaCzytelna`&nbsp;8729, `odtworzKopie`&nbsp;8739, `kopiaWybrana`&nbsp;8772

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8796, `odtworzZBazy`&nbsp;8828, `exportCsv`&nbsp;8840

*NAWIGACJA* — `wrocZEkranu`&nbsp;8948


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
