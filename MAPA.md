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

## `index.html` — 8958 linii, ~130 tys. tokenow

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
| 5297 | 5852 | EKRAN ZDARZEN |
| 5853 | 5971 | ZAPAS TABLETEK |
| 5972 | 6326 | USTAWIENIA |
| 6327 | 6634 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6635 | 7093 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 7094 | 7465 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7466 | 7676 | ANALIZA |
| 7677 | 8062 | WYKRESY ANALIZY |
| 8063 | 8219 | RAPORT |
| 8220 | 8281 | KONTEKST DNIA (TAGI) |
| 8282 | 8332 | KOPIA ZAPASOWA |
| 8333 | 8546 | KOPIA NA TELEGRAM |
| 8547 | 8612 | WIEK KOPII |
| 8613 | 8753 | ODTWARZANIE Z KOPII |
| 8754 | 8828 | KOPIE Z BAZY |
| 8829 | 8916 | NAWIGACJA |
| 8917 | 8958 | AUTOMATYCZNA AKTUALIZACJA |

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

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5313, `evPasuje`&nbsp;5318, `renderEvents`&nbsp;5330, `nazwaOtwarcia`&nbsp;5373, `renderOpenWarn`&nbsp;5381, `minutyDoPelna`&nbsp;5434, `opisLadowania`&nbsp;5446, `dni`&nbsp;5466, `opisLadowan`&nbsp;5469, `tempoZHistorii`&nbsp;5522, `prognozaDni`&nbsp;5531, `opisPrognozy`&nbsp;5543, `czasKrotko`&nbsp;5565, `opisCzuwania`&nbsp;5573, `renderStatus`&nbsp;5591

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5856, `dayAfter`&nbsp;5859, `pillsBaseInfo`&nbsp;5870, `settlePills`&nbsp;5880, `dniZapasu`&nbsp;5920, `renderPills`&nbsp;5933, `savePills`&nbsp;5954, `setPills`&nbsp;5965

*USTAWIENIA* — `opisLeku`&nbsp;5983, `renderKafelki`&nbsp;5989, `renderPudelka`&nbsp;6024, `renderSettings`&nbsp;6066, `tydzienZPol`&nbsp;6125, `renderWeekEditor`&nbsp;6137, `odswiezPodpowiedzTygodnia`&nbsp;6154, `tydzienZmieniony`&nbsp;6168, `rownajTydzien`&nbsp;6169, `renderPlanList`&nbsp;6217, `renderExceptions`&nbsp;6244, `wyslijSiec`&nbsp;6302

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6344, `tgZapytaj`&nbsp;6354, `tgKodParowania`&nbsp;6394, `tgZnajdzCzat`&nbsp;6410, `tgPolacz`&nbsp;6478, `tgProbna`&nbsp;6511, `tgOdlacz`&nbsp;6518, `renderTgStan`&nbsp;6540

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6655, `pobierzOpisFirmware`&nbsp;6661, `wyslijAktualizacje`&nbsp;6684, `anulujAktualizacje`&nbsp;6729, `renderOta`&nbsp;6735, `renderNetStan`&nbsp;7020

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;7105, `renderSkan`&nbsp;7111, `szukajSieci`&nbsp;7163, `wybierzSiec`&nbsp;7171, `wyslijPolecenieSieci`&nbsp;7189, `siecZIndeksu`&nbsp;7199, `tzChanged`&nbsp;7233, `cfgTime`&nbsp;7238, `addSlot`&nbsp;7246, `zapiszPlanDnia`&nbsp;7259, `saveConfig`&nbsp;7277, `inrKrokiZakresu`&nbsp;7348, `opcjeInr`&nbsp;7355, `inrZakresZmieniony`&nbsp;7366, `wypelnijListyZakresu`&nbsp;7379, `saveInrRange`&nbsp;7390, `wypelnijListeOdstepu`&nbsp;7424, `saveInrEvery`&nbsp;7437, `odswiezPodpowiedzInr`&nbsp;7447

*ANALIZA* — `openTimeOf`&nbsp;7472, `openMinutes`&nbsp;7478, `sredniaPora`&nbsp;7502, `kwantyl`&nbsp;7510, `dniMiedzy`&nbsp;7518, `odstepyZPunktow`&nbsp;7532, `analyze`&nbsp;7541, `inrContext`&nbsp;7643

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7702, `rytmSVG`&nbsp;7714, `poryWCzasieSVG`&nbsp;7776, `iskraSVG`&nbsp;7844, `dowSVG`&nbsp;7872, `dniRytmu`&nbsp;7908, `skutecznoscTygodniami`&nbsp;7929, `renderAnalysis`&nbsp;7957

*RAPORT* — `collectRows`&nbsp;8064, `makeReport`&nbsp;8101

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;8252, `tagiPrzed`&nbsp;8260, `tagPrzelacz`&nbsp;8269

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;8312, `opisKopii`&nbsp;8322

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8367, `tgCzatKopii`&nbsp;8374, `odswiezKopie`&nbsp;8381, `tgKopiaCzatZapisz`&nbsp;8389, `tgKopiaCzatZnajdz`&nbsp;8409, `tgKopiaWlacz`&nbsp;8439, `tgKopiaWylacz`&nbsp;8458, `kopiaNaTelegram`&nbsp;8467, `kopiaAutomat`&nbsp;8518

*WIEK KOPII* — `dniOdDaty`&nbsp;8565, `wiekKopiiTxt`&nbsp;8571, `renderKopiaStan`&nbsp;8579, `zapiszKopie`&nbsp;8594

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8628, `ustawieniaDoOdtworzenia`&nbsp;8676, `wczytajKopie`&nbsp;8691, `kopiaCzytelna`&nbsp;8696, `odtworzKopie`&nbsp;8706, `kopiaWybrana`&nbsp;8739

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8763, `odtworzZBazy`&nbsp;8795, `exportCsv`&nbsp;8807

*NAWIGACJA* — `wrocZEkranu`&nbsp;8915


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
