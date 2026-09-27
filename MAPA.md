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

## `index.html` — 8800 linii, ~130 tys. tokenow

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
| 1644 | 1957 | tab-help |
| 1958 | 1979 | tab-ev |
| 1980 | 2070 | tab-hist |
| 2071 | 2079 | JS — poczatek |
| 2080 | 2300 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2301 | 2375 | INFORMACJA ZWROTNA |
| 2376 | 2470 | STREFY CZASOWE |
| 2471 | 2498 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2499 | 2565 | TABLETKA JAKO BRYŁA |
| 2566 | 2580 | LOGOWANIE |
| 2581 | 2720 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2721 | 3101 | START |
| 3102 | 3162 | OSŁONA RYSOWANIA |
| 3163 | 3221 | REKONCYLIACJA |
| 3222 | 3426 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3427 | 3485 | KALENDARZ |
| 3486 | 3756 | HISTORIA ROZPISANIA DAWKI |
| 3757 | 3946 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3947 | 4098 | ARKUSZ DNIA |
| 4099 | 4198 | WZIĄŁEM TERAZ |
| 4199 | 4313 | INR |
| 4314 | 4453 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4454 | 4466 | STATUS PUDEŁKA |
| 4467 | 4616 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4617 | 4778 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4779 | 5061 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 5062 | 5102 | KOLEJKA, KTÓRA NIE SCHODZI |
| 5103 | 5199 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 5200 | 5718 | EKRAN ZDARZEN |
| 5719 | 5837 | ZAPAS TABLETEK |
| 5838 | 6175 | USTAWIENIA |
| 6176 | 6483 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6484 | 6942 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6943 | 7313 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7314 | 7524 | ANALIZA |
| 7525 | 7910 | WYKRESY ANALIZY |
| 7911 | 8067 | RAPORT |
| 8068 | 8129 | KONTEKST DNIA (TAGI) |
| 8130 | 8175 | KOPIA ZAPASOWA |
| 8176 | 8389 | KOPIA NA TELEGRAM |
| 8390 | 8455 | WIEK KOPII |
| 8456 | 8596 | ODTWARZANIE Z KOPII |
| 8597 | 8671 | KOPIE Z BAZY |
| 8672 | 8758 | NAWIGACJA |
| 8759 | 8800 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (225) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `opisPlikFirmware`&nbsp;2121, `pudelkoZnane`&nbsp;2127, `wybranePudelko`&nbsp;2129, `korzenDanych`&nbsp;2153, `odmowaRegul`&nbsp;2172, `sprawdzDostepPudelek`&nbsp;2177, `wybierzPudelko`&nbsp;2200, `profilTydzien`&nbsp;2249, `komoraDnia`&nbsp;2260, `ustawProfil`&nbsp;2275

*INFORMACJA ZWROTNA* — `toast`&nbsp;2322, `busy`&nbsp;2338, `todayKey`&nbsp;2365, `dzisiajKey`&nbsp;2369, `inNightWindow`&nbsp;2372

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2427, `tzName`&nbsp;2443, `tzLabel`&nbsp;2444, `tzOffsetTxt`&nbsp;2445, `devDate`&nbsp;2451, `devKey`&nbsp;2456, `devHM`&nbsp;2461, `slotMin`&nbsp;2462, `pillColors`&nbsp;2464

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2475

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2512, `cieniuj`&nbsp;2539, `doseGraphic`&nbsp;2556

*LOGOWANIE* — `doLogin`&nbsp;2570

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2590, `wyczyscCache`&nbsp;2685, `fbSignOut`&nbsp;2704

*START* — `boot`&nbsp;2722

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;3129, `rysujWszystkie`&nbsp;3142, `renderAll`&nbsp;3146

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;3169, `reconcileDecyzja`&nbsp;3208

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3243, `zapiszReconcile`&nbsp;3255, `doReconcile`&nbsp;3294, `doReconcileWewn`&nbsp;3304, `reconcile`&nbsp;3425

*KALENDARZ* — `tydzienDawek`&nbsp;3465

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3511, `dawkaNaDzien`&nbsp;3522, `dzienBezLeku`&nbsp;3543, `wyjatekNaDzien`&nbsp;3548, `opisDawkowania`&nbsp;3554, `dayDose`&nbsp;3568, `dzienZamkniety`&nbsp;3602, `trackingSince`&nbsp;3608, `beforeTracking`&nbsp;3609, `dayStatus`&nbsp;3611, `renderCalendar`&nbsp;3652, `seriaDni`&nbsp;3717, `doNastepnej`&nbsp;3735, `opisCzasu`&nbsp;3750

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3765, `trwanieTxt`&nbsp;3780, `kiedyDawkaTxt`&nbsp;3791, `odswiezOdDawki`&nbsp;3799, `startTikOdDawki`&nbsp;3813, `renderToday`&nbsp;3823

*ARKUSZ DNIA* — `closeSheet`&nbsp;3958, `renderSheet`&nbsp;3960, `resetDose`&nbsp;4038, `resetPlan`&nbsp;4045, `commitPlan`&nbsp;4050, `clearPlan`&nbsp;4066, `commitDose`&nbsp;4078

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;4119, `askConfirm`&nbsp;4188

*INR* — `inrState`&nbsp;4200, `odswiezTerminInr`&nbsp;4213, `addInr`&nbsp;4222, `inrKeysOk`&nbsp;4308

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4323, `inrTerminKey`&nbsp;4331, `inrDoTerminu`&nbsp;4343, `dniTxt`&nbsp;4352, `renderInr`&nbsp;4354, `inrChart`&nbsp;4426

*STATUS PUDEŁKA* — `relTime`&nbsp;4455, `devDayMon`&nbsp;4464

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4499, `renderBoxLog`&nbsp;4545, `logPrzelacz`&nbsp;4581, `renderNvsFailLog`&nbsp;4589

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4634, `oczekWczytaj`&nbsp;4642, `oczekZapisz`&nbsp;4647, `oczekIle`&nbsp;4650, `zapiszPewnie`&nbsp;4660, `zapiszCfg`&nbsp;4698, `bazaOdmowila`&nbsp;4716, `oczekWyslij`&nbsp;4740, `oczekOdmowy`&nbsp;4777

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4793, `ostrzReguly`&nbsp;4815, `lm`&nbsp;4854, `ostrzMilczy`&nbsp;4860, `nvsMalo`&nbsp;4940, `opisNvsFailKey`&nbsp;4952, `stratyDotyczaLeku`&nbsp;5005, `ostrzStraty`&nbsp;5017, `stratyCicho`&nbsp;5051

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;5083

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;5113, `renderOstrzezenia`&nbsp;5128, `bezPokrycia`&nbsp;5138, `wierszZdarzenia`&nbsp;5144, `renderDiag`&nbsp;5161

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5216, `evPasuje`&nbsp;5221, `renderEvents`&nbsp;5233, `renderOpenWarn`&nbsp;5273, `minutyDoPelna`&nbsp;5326, `opisLadowania`&nbsp;5338, `dni`&nbsp;5358, `opisLadowan`&nbsp;5361, `tempoZHistorii`&nbsp;5414, `prognozaDni`&nbsp;5423, `opisPrognozy`&nbsp;5435, `czasKrotko`&nbsp;5457, `opisCzuwania`&nbsp;5465, `renderStatus`&nbsp;5483

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5722, `dayAfter`&nbsp;5725, `pillsBaseInfo`&nbsp;5736, `settlePills`&nbsp;5746, `dniZapasu`&nbsp;5786, `renderPills`&nbsp;5799, `savePills`&nbsp;5820, `setPills`&nbsp;5831

*USTAWIENIA* — `renderKafelki`&nbsp;5842, `renderPudelka`&nbsp;5876, `renderSettings`&nbsp;5918, `tydzienZPol`&nbsp;5974, `renderWeekEditor`&nbsp;5986, `odswiezPodpowiedzTygodnia`&nbsp;6003, `tydzienZmieniony`&nbsp;6017, `rownajTydzien`&nbsp;6018, `renderPlanList`&nbsp;6066, `renderExceptions`&nbsp;6093, `wyslijSiec`&nbsp;6151

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6193, `tgZapytaj`&nbsp;6203, `tgKodParowania`&nbsp;6243, `tgZnajdzCzat`&nbsp;6259, `tgPolacz`&nbsp;6327, `tgProbna`&nbsp;6360, `tgOdlacz`&nbsp;6367, `renderTgStan`&nbsp;6389

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6504, `pobierzOpisFirmware`&nbsp;6510, `wyslijAktualizacje`&nbsp;6533, `anulujAktualizacje`&nbsp;6578, `renderOta`&nbsp;6584, `renderNetStan`&nbsp;6869

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6954, `renderSkan`&nbsp;6960, `szukajSieci`&nbsp;7012, `wybierzSiec`&nbsp;7020, `wyslijPolecenieSieci`&nbsp;7038, `siecZIndeksu`&nbsp;7048, `tzChanged`&nbsp;7082, `cfgTime`&nbsp;7087, `addSlot`&nbsp;7095, `zapiszPlanDnia`&nbsp;7108, `saveConfig`&nbsp;7126, `inrKrokiZakresu`&nbsp;7196, `opcjeInr`&nbsp;7203, `inrZakresZmieniony`&nbsp;7214, `wypelnijListyZakresu`&nbsp;7227, `saveInrRange`&nbsp;7238, `wypelnijListeOdstepu`&nbsp;7272, `saveInrEvery`&nbsp;7285, `odswiezPodpowiedzInr`&nbsp;7295

*ANALIZA* — `openTimeOf`&nbsp;7320, `openMinutes`&nbsp;7326, `sredniaPora`&nbsp;7350, `kwantyl`&nbsp;7358, `dniMiedzy`&nbsp;7366, `odstepyZPunktow`&nbsp;7380, `analyze`&nbsp;7389, `inrContext`&nbsp;7491

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7550, `rytmSVG`&nbsp;7562, `poryWCzasieSVG`&nbsp;7624, `iskraSVG`&nbsp;7692, `dowSVG`&nbsp;7720, `dniRytmu`&nbsp;7756, `skutecznoscTygodniami`&nbsp;7777, `renderAnalysis`&nbsp;7805

*RAPORT* — `collectRows`&nbsp;7912, `makeReport`&nbsp;7949

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;8100, `tagiPrzed`&nbsp;8108, `tagPrzelacz`&nbsp;8117

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;8160, `opisKopii`&nbsp;8170

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8210, `tgCzatKopii`&nbsp;8217, `odswiezKopie`&nbsp;8224, `tgKopiaCzatZapisz`&nbsp;8232, `tgKopiaCzatZnajdz`&nbsp;8252, `tgKopiaWlacz`&nbsp;8282, `tgKopiaWylacz`&nbsp;8301, `kopiaNaTelegram`&nbsp;8310, `kopiaAutomat`&nbsp;8361

*WIEK KOPII* — `dniOdDaty`&nbsp;8408, `wiekKopiiTxt`&nbsp;8414, `renderKopiaStan`&nbsp;8422, `zapiszKopie`&nbsp;8437

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8471, `ustawieniaDoOdtworzenia`&nbsp;8519, `wczytajKopie`&nbsp;8534, `kopiaCzytelna`&nbsp;8539, `odtworzKopie`&nbsp;8549, `kopiaWybrana`&nbsp;8582

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8606, `odtworzZBazy`&nbsp;8638, `exportCsv`&nbsp;8650

*NAWIGACJA* — `wrocZEkranu`&nbsp;8757


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
