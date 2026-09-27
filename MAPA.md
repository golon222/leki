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

## `index.html` — 8657 linii, ~130 tys. tokenow

Ekrany (`<section>`) i dwa duze bloki. Zakladki `tab-*` odpowiadaja
pozycjom w pasku nawigacji i podekranom Ustawien.

| od | do | co |
|---|---|---|
| 21 | 21 | CSS — poczatek |
| 22 | 234 | SYSTEM WIZUALNY PillBox |
| 235 | 331 | EKRAN GŁÓWNY — KARTA DNIA |
| 332 | 404 | INFORMACJA ZWROTNA |
| 405 | 818 | TABLETKA W 3D |
| 819 | 978 | tab-cal |
| 979 | 1027 | tab-inr |
| 1028 | 1100 | tab-ana |
| 1101 | 1185 | tab-set |
| 1186 | 1257 | tab-lek |
| 1258 | 1267 | tab-pud |
| 1268 | 1287 | tab-sinr |
| 1288 | 1327 | tab-wifi |
| 1328 | 1428 | tab-tg |
| 1429 | 1462 | tab-dev |
| 1463 | 1563 | tab-diag |
| 1564 | 1839 | tab-help |
| 1840 | 1861 | tab-ev |
| 1862 | 1952 | tab-hist |
| 1953 | 1961 | JS — poczatek |
| 1962 | 2159 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2160 | 2234 | INFORMACJA ZWROTNA |
| 2235 | 2329 | STREFY CZASOWE |
| 2330 | 2357 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2358 | 2424 | TABLETKA JAKO BRYŁA |
| 2425 | 2439 | LOGOWANIE |
| 2440 | 2579 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2580 | 2960 | START |
| 2961 | 3021 | OSŁONA RYSOWANIA |
| 3022 | 3080 | REKONCYLIACJA |
| 3081 | 3285 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3286 | 3344 | KALENDARZ |
| 3345 | 3615 | HISTORIA ROZPISANIA DAWKI |
| 3616 | 3805 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3806 | 3957 | ARKUSZ DNIA |
| 3958 | 4057 | WZIĄŁEM TERAZ |
| 4058 | 4172 | INR |
| 4173 | 4312 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4313 | 4325 | STATUS PUDEŁKA |
| 4326 | 4475 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4476 | 4637 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4638 | 4920 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4921 | 4961 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4962 | 5058 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 5059 | 5577 | EKRAN ZDARZEN |
| 5578 | 5696 | ZAPAS TABLETEK |
| 5697 | 6034 | USTAWIENIA |
| 6035 | 6342 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6343 | 6799 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6800 | 7170 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7171 | 7381 | ANALIZA |
| 7382 | 7767 | WYKRESY ANALIZY |
| 7768 | 7924 | RAPORT |
| 7925 | 7986 | KONTEKST DNIA (TAGI) |
| 7987 | 8032 | KOPIA ZAPASOWA |
| 8033 | 8246 | KOPIA NA TELEGRAM |
| 8247 | 8312 | WIEK KOPII |
| 8313 | 8453 | ODTWARZANIE Z KOPII |
| 8454 | 8528 | KOPIE Z BAZY |
| 8529 | 8615 | NAWIGACJA |
| 8616 | 8657 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (224) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `pudelkoZnane`&nbsp;1993, `wybranePudelko`&nbsp;1995, `korzenDanych`&nbsp;2019, `odmowaRegul`&nbsp;2038, `sprawdzDostepPudelek`&nbsp;2043, `wybierzPudelko`&nbsp;2066, `profilTydzien`&nbsp;2115, `komoraDnia`&nbsp;2126, `ustawProfil`&nbsp;2141

*INFORMACJA ZWROTNA* — `toast`&nbsp;2181, `busy`&nbsp;2197, `todayKey`&nbsp;2224, `dzisiajKey`&nbsp;2228, `inNightWindow`&nbsp;2231

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2286, `tzName`&nbsp;2302, `tzLabel`&nbsp;2303, `tzOffsetTxt`&nbsp;2304, `devDate`&nbsp;2310, `devKey`&nbsp;2315, `devHM`&nbsp;2320, `slotMin`&nbsp;2321, `pillColors`&nbsp;2323

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2334

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2371, `cieniuj`&nbsp;2398, `doseGraphic`&nbsp;2415

*LOGOWANIE* — `doLogin`&nbsp;2429

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2449, `wyczyscCache`&nbsp;2544, `fbSignOut`&nbsp;2563

*START* — `boot`&nbsp;2581

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;2988, `rysujWszystkie`&nbsp;3001, `renderAll`&nbsp;3005

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;3028, `reconcileDecyzja`&nbsp;3067

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3102, `zapiszReconcile`&nbsp;3114, `doReconcile`&nbsp;3153, `doReconcileWewn`&nbsp;3163, `reconcile`&nbsp;3284

*KALENDARZ* — `tydzienDawek`&nbsp;3324

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3370, `dawkaNaDzien`&nbsp;3381, `dzienBezLeku`&nbsp;3402, `wyjatekNaDzien`&nbsp;3407, `opisDawkowania`&nbsp;3413, `dayDose`&nbsp;3427, `dzienZamkniety`&nbsp;3461, `trackingSince`&nbsp;3467, `beforeTracking`&nbsp;3468, `dayStatus`&nbsp;3470, `renderCalendar`&nbsp;3511, `seriaDni`&nbsp;3576, `doNastepnej`&nbsp;3594, `opisCzasu`&nbsp;3609

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3624, `trwanieTxt`&nbsp;3639, `kiedyDawkaTxt`&nbsp;3650, `odswiezOdDawki`&nbsp;3658, `startTikOdDawki`&nbsp;3672, `renderToday`&nbsp;3682

*ARKUSZ DNIA* — `closeSheet`&nbsp;3817, `renderSheet`&nbsp;3819, `resetDose`&nbsp;3897, `resetPlan`&nbsp;3904, `commitPlan`&nbsp;3909, `clearPlan`&nbsp;3925, `commitDose`&nbsp;3937

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3978, `askConfirm`&nbsp;4047

*INR* — `inrState`&nbsp;4059, `odswiezTerminInr`&nbsp;4072, `addInr`&nbsp;4081, `inrKeysOk`&nbsp;4167

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4182, `inrTerminKey`&nbsp;4190, `inrDoTerminu`&nbsp;4202, `dniTxt`&nbsp;4211, `renderInr`&nbsp;4213, `inrChart`&nbsp;4285

*STATUS PUDEŁKA* — `relTime`&nbsp;4314, `devDayMon`&nbsp;4323

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4358, `renderBoxLog`&nbsp;4404, `logPrzelacz`&nbsp;4440, `renderNvsFailLog`&nbsp;4448

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4493, `oczekWczytaj`&nbsp;4501, `oczekZapisz`&nbsp;4506, `oczekIle`&nbsp;4509, `zapiszPewnie`&nbsp;4519, `zapiszCfg`&nbsp;4557, `bazaOdmowila`&nbsp;4575, `oczekWyslij`&nbsp;4599, `oczekOdmowy`&nbsp;4636

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4652, `ostrzReguly`&nbsp;4674, `lm`&nbsp;4713, `ostrzMilczy`&nbsp;4719, `nvsMalo`&nbsp;4799, `opisNvsFailKey`&nbsp;4811, `stratyDotyczaLeku`&nbsp;4864, `ostrzStraty`&nbsp;4876, `stratyCicho`&nbsp;4910

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4942

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4972, `renderOstrzezenia`&nbsp;4987, `bezPokrycia`&nbsp;4997, `wierszZdarzenia`&nbsp;5003, `renderDiag`&nbsp;5020

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5075, `evPasuje`&nbsp;5080, `renderEvents`&nbsp;5092, `renderOpenWarn`&nbsp;5132, `minutyDoPelna`&nbsp;5185, `opisLadowania`&nbsp;5197, `dni`&nbsp;5217, `opisLadowan`&nbsp;5220, `tempoZHistorii`&nbsp;5273, `prognozaDni`&nbsp;5282, `opisPrognozy`&nbsp;5294, `czasKrotko`&nbsp;5316, `opisCzuwania`&nbsp;5324, `renderStatus`&nbsp;5342

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5581, `dayAfter`&nbsp;5584, `pillsBaseInfo`&nbsp;5595, `settlePills`&nbsp;5605, `dniZapasu`&nbsp;5645, `renderPills`&nbsp;5658, `savePills`&nbsp;5679, `setPills`&nbsp;5690

*USTAWIENIA* — `renderKafelki`&nbsp;5701, `renderPudelka`&nbsp;5735, `renderSettings`&nbsp;5777, `tydzienZPol`&nbsp;5833, `renderWeekEditor`&nbsp;5845, `odswiezPodpowiedzTygodnia`&nbsp;5862, `tydzienZmieniony`&nbsp;5876, `rownajTydzien`&nbsp;5877, `renderPlanList`&nbsp;5925, `renderExceptions`&nbsp;5952, `wyslijSiec`&nbsp;6010

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6052, `tgZapytaj`&nbsp;6062, `tgKodParowania`&nbsp;6102, `tgZnajdzCzat`&nbsp;6118, `tgPolacz`&nbsp;6186, `tgProbna`&nbsp;6219, `tgOdlacz`&nbsp;6226, `renderTgStan`&nbsp;6248

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6363, `pobierzOpisFirmware`&nbsp;6369, `wyslijAktualizacje`&nbsp;6390, `anulujAktualizacje`&nbsp;6435, `renderOta`&nbsp;6441, `renderNetStan`&nbsp;6726

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6811, `renderSkan`&nbsp;6817, `szukajSieci`&nbsp;6869, `wybierzSiec`&nbsp;6877, `wyslijPolecenieSieci`&nbsp;6895, `siecZIndeksu`&nbsp;6905, `tzChanged`&nbsp;6939, `cfgTime`&nbsp;6944, `addSlot`&nbsp;6952, `zapiszPlanDnia`&nbsp;6965, `saveConfig`&nbsp;6983, `inrKrokiZakresu`&nbsp;7053, `opcjeInr`&nbsp;7060, `inrZakresZmieniony`&nbsp;7071, `wypelnijListyZakresu`&nbsp;7084, `saveInrRange`&nbsp;7095, `wypelnijListeOdstepu`&nbsp;7129, `saveInrEvery`&nbsp;7142, `odswiezPodpowiedzInr`&nbsp;7152

*ANALIZA* — `openTimeOf`&nbsp;7177, `openMinutes`&nbsp;7183, `sredniaPora`&nbsp;7207, `kwantyl`&nbsp;7215, `dniMiedzy`&nbsp;7223, `odstepyZPunktow`&nbsp;7237, `analyze`&nbsp;7246, `inrContext`&nbsp;7348

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7407, `rytmSVG`&nbsp;7419, `poryWCzasieSVG`&nbsp;7481, `iskraSVG`&nbsp;7549, `dowSVG`&nbsp;7577, `dniRytmu`&nbsp;7613, `skutecznoscTygodniami`&nbsp;7634, `renderAnalysis`&nbsp;7662

*RAPORT* — `collectRows`&nbsp;7769, `makeReport`&nbsp;7806

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7957, `tagiPrzed`&nbsp;7965, `tagPrzelacz`&nbsp;7974

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;8017, `opisKopii`&nbsp;8027

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8067, `tgCzatKopii`&nbsp;8074, `odswiezKopie`&nbsp;8081, `tgKopiaCzatZapisz`&nbsp;8089, `tgKopiaCzatZnajdz`&nbsp;8109, `tgKopiaWlacz`&nbsp;8139, `tgKopiaWylacz`&nbsp;8158, `kopiaNaTelegram`&nbsp;8167, `kopiaAutomat`&nbsp;8218

*WIEK KOPII* — `dniOdDaty`&nbsp;8265, `wiekKopiiTxt`&nbsp;8271, `renderKopiaStan`&nbsp;8279, `zapiszKopie`&nbsp;8294

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8328, `ustawieniaDoOdtworzenia`&nbsp;8376, `wczytajKopie`&nbsp;8391, `kopiaCzytelna`&nbsp;8396, `odtworzKopie`&nbsp;8406, `kopiaWybrana`&nbsp;8439

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8463, `odtworzZBazy`&nbsp;8495, `exportCsv`&nbsp;8507

*NAWIGACJA* — `wrocZEkranu`&nbsp;8614


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
