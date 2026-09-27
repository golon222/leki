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

## `index.html` — 8621 linii, ~130 tys. tokenow

Ekrany (`<section>`) i dwa duze bloki. Zakladki `tab-*` odpowiadaja
pozycjom w pasku nawigacji i podekranom Ustawien.

| od | do | co |
|---|---|---|
| 21 | 21 | CSS — poczatek |
| 22 | 222 | SYSTEM WIZUALNY PillBox |
| 223 | 319 | EKRAN GŁÓWNY — KARTA DNIA |
| 320 | 392 | INFORMACJA ZWROTNA |
| 393 | 782 | TABLETKA W 3D |
| 783 | 942 | tab-cal |
| 943 | 991 | tab-inr |
| 992 | 1064 | tab-ana |
| 1065 | 1149 | tab-set |
| 1150 | 1221 | tab-lek |
| 1222 | 1231 | tab-pud |
| 1232 | 1251 | tab-sinr |
| 1252 | 1291 | tab-wifi |
| 1292 | 1392 | tab-tg |
| 1393 | 1426 | tab-dev |
| 1427 | 1527 | tab-diag |
| 1528 | 1803 | tab-help |
| 1804 | 1825 | tab-ev |
| 1826 | 1916 | tab-hist |
| 1917 | 1925 | JS — poczatek |
| 1926 | 2123 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2124 | 2198 | INFORMACJA ZWROTNA |
| 2199 | 2293 | STREFY CZASOWE |
| 2294 | 2321 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2322 | 2388 | TABLETKA JAKO BRYŁA |
| 2389 | 2403 | LOGOWANIE |
| 2404 | 2543 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2544 | 2924 | START |
| 2925 | 2985 | OSŁONA RYSOWANIA |
| 2986 | 3044 | REKONCYLIACJA |
| 3045 | 3249 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3250 | 3308 | KALENDARZ |
| 3309 | 3579 | HISTORIA ROZPISANIA DAWKI |
| 3580 | 3769 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3770 | 3921 | ARKUSZ DNIA |
| 3922 | 4021 | WZIĄŁEM TERAZ |
| 4022 | 4136 | INR |
| 4137 | 4276 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4277 | 4289 | STATUS PUDEŁKA |
| 4290 | 4439 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4440 | 4601 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4602 | 4884 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4885 | 4925 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4926 | 5022 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 5023 | 5541 | EKRAN ZDARZEN |
| 5542 | 5660 | ZAPAS TABLETEK |
| 5661 | 5998 | USTAWIENIA |
| 5999 | 6306 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6307 | 6763 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6764 | 7134 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7135 | 7345 | ANALIZA |
| 7346 | 7731 | WYKRESY ANALIZY |
| 7732 | 7888 | RAPORT |
| 7889 | 7950 | KONTEKST DNIA (TAGI) |
| 7951 | 7996 | KOPIA ZAPASOWA |
| 7997 | 8210 | KOPIA NA TELEGRAM |
| 8211 | 8276 | WIEK KOPII |
| 8277 | 8417 | ODTWARZANIE Z KOPII |
| 8418 | 8492 | KOPIE Z BAZY |
| 8493 | 8579 | NAWIGACJA |
| 8580 | 8621 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (224) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `pudelkoZnane`&nbsp;1957, `wybranePudelko`&nbsp;1959, `korzenDanych`&nbsp;1983, `odmowaRegul`&nbsp;2002, `sprawdzDostepPudelek`&nbsp;2007, `wybierzPudelko`&nbsp;2030, `profilTydzien`&nbsp;2079, `komoraDnia`&nbsp;2090, `ustawProfil`&nbsp;2105

*INFORMACJA ZWROTNA* — `toast`&nbsp;2145, `busy`&nbsp;2161, `todayKey`&nbsp;2188, `dzisiajKey`&nbsp;2192, `inNightWindow`&nbsp;2195

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2250, `tzName`&nbsp;2266, `tzLabel`&nbsp;2267, `tzOffsetTxt`&nbsp;2268, `devDate`&nbsp;2274, `devKey`&nbsp;2279, `devHM`&nbsp;2284, `slotMin`&nbsp;2285, `pillColors`&nbsp;2287

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2298

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2335, `cieniuj`&nbsp;2362, `doseGraphic`&nbsp;2379

*LOGOWANIE* — `doLogin`&nbsp;2393

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2413, `wyczyscCache`&nbsp;2508, `fbSignOut`&nbsp;2527

*START* — `boot`&nbsp;2545

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;2952, `rysujWszystkie`&nbsp;2965, `renderAll`&nbsp;2969

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;2992, `reconcileDecyzja`&nbsp;3031

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3066, `zapiszReconcile`&nbsp;3078, `doReconcile`&nbsp;3117, `doReconcileWewn`&nbsp;3127, `reconcile`&nbsp;3248

*KALENDARZ* — `tydzienDawek`&nbsp;3288

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3334, `dawkaNaDzien`&nbsp;3345, `dzienBezLeku`&nbsp;3366, `wyjatekNaDzien`&nbsp;3371, `opisDawkowania`&nbsp;3377, `dayDose`&nbsp;3391, `dzienZamkniety`&nbsp;3425, `trackingSince`&nbsp;3431, `beforeTracking`&nbsp;3432, `dayStatus`&nbsp;3434, `renderCalendar`&nbsp;3475, `seriaDni`&nbsp;3540, `doNastepnej`&nbsp;3558, `opisCzasu`&nbsp;3573

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3588, `trwanieTxt`&nbsp;3603, `kiedyDawkaTxt`&nbsp;3614, `odswiezOdDawki`&nbsp;3622, `startTikOdDawki`&nbsp;3636, `renderToday`&nbsp;3646

*ARKUSZ DNIA* — `closeSheet`&nbsp;3781, `renderSheet`&nbsp;3783, `resetDose`&nbsp;3861, `resetPlan`&nbsp;3868, `commitPlan`&nbsp;3873, `clearPlan`&nbsp;3889, `commitDose`&nbsp;3901

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3942, `askConfirm`&nbsp;4011

*INR* — `inrState`&nbsp;4023, `odswiezTerminInr`&nbsp;4036, `addInr`&nbsp;4045, `inrKeysOk`&nbsp;4131

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4146, `inrTerminKey`&nbsp;4154, `inrDoTerminu`&nbsp;4166, `dniTxt`&nbsp;4175, `renderInr`&nbsp;4177, `inrChart`&nbsp;4249

*STATUS PUDEŁKA* — `relTime`&nbsp;4278, `devDayMon`&nbsp;4287

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4322, `renderBoxLog`&nbsp;4368, `logPrzelacz`&nbsp;4404, `renderNvsFailLog`&nbsp;4412

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4457, `oczekWczytaj`&nbsp;4465, `oczekZapisz`&nbsp;4470, `oczekIle`&nbsp;4473, `zapiszPewnie`&nbsp;4483, `zapiszCfg`&nbsp;4521, `bazaOdmowila`&nbsp;4539, `oczekWyslij`&nbsp;4563, `oczekOdmowy`&nbsp;4600

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4616, `ostrzReguly`&nbsp;4638, `lm`&nbsp;4677, `ostrzMilczy`&nbsp;4683, `nvsMalo`&nbsp;4763, `opisNvsFailKey`&nbsp;4775, `stratyDotyczaLeku`&nbsp;4828, `ostrzStraty`&nbsp;4840, `stratyCicho`&nbsp;4874

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4906

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4936, `renderOstrzezenia`&nbsp;4951, `bezPokrycia`&nbsp;4961, `wierszZdarzenia`&nbsp;4967, `renderDiag`&nbsp;4984

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5039, `evPasuje`&nbsp;5044, `renderEvents`&nbsp;5056, `renderOpenWarn`&nbsp;5096, `minutyDoPelna`&nbsp;5149, `opisLadowania`&nbsp;5161, `dni`&nbsp;5181, `opisLadowan`&nbsp;5184, `tempoZHistorii`&nbsp;5237, `prognozaDni`&nbsp;5246, `opisPrognozy`&nbsp;5258, `czasKrotko`&nbsp;5280, `opisCzuwania`&nbsp;5288, `renderStatus`&nbsp;5306

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5545, `dayAfter`&nbsp;5548, `pillsBaseInfo`&nbsp;5559, `settlePills`&nbsp;5569, `dniZapasu`&nbsp;5609, `renderPills`&nbsp;5622, `savePills`&nbsp;5643, `setPills`&nbsp;5654

*USTAWIENIA* — `renderKafelki`&nbsp;5665, `renderPudelka`&nbsp;5699, `renderSettings`&nbsp;5741, `tydzienZPol`&nbsp;5797, `renderWeekEditor`&nbsp;5809, `odswiezPodpowiedzTygodnia`&nbsp;5826, `tydzienZmieniony`&nbsp;5840, `rownajTydzien`&nbsp;5841, `renderPlanList`&nbsp;5889, `renderExceptions`&nbsp;5916, `wyslijSiec`&nbsp;5974

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6016, `tgZapytaj`&nbsp;6026, `tgKodParowania`&nbsp;6066, `tgZnajdzCzat`&nbsp;6082, `tgPolacz`&nbsp;6150, `tgProbna`&nbsp;6183, `tgOdlacz`&nbsp;6190, `renderTgStan`&nbsp;6212

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6327, `pobierzOpisFirmware`&nbsp;6333, `wyslijAktualizacje`&nbsp;6354, `anulujAktualizacje`&nbsp;6399, `renderOta`&nbsp;6405, `renderNetStan`&nbsp;6690

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6775, `renderSkan`&nbsp;6781, `szukajSieci`&nbsp;6833, `wybierzSiec`&nbsp;6841, `wyslijPolecenieSieci`&nbsp;6859, `siecZIndeksu`&nbsp;6869, `tzChanged`&nbsp;6903, `cfgTime`&nbsp;6908, `addSlot`&nbsp;6916, `zapiszPlanDnia`&nbsp;6929, `saveConfig`&nbsp;6947, `inrKrokiZakresu`&nbsp;7017, `opcjeInr`&nbsp;7024, `inrZakresZmieniony`&nbsp;7035, `wypelnijListyZakresu`&nbsp;7048, `saveInrRange`&nbsp;7059, `wypelnijListeOdstepu`&nbsp;7093, `saveInrEvery`&nbsp;7106, `odswiezPodpowiedzInr`&nbsp;7116

*ANALIZA* — `openTimeOf`&nbsp;7141, `openMinutes`&nbsp;7147, `sredniaPora`&nbsp;7171, `kwantyl`&nbsp;7179, `dniMiedzy`&nbsp;7187, `odstepyZPunktow`&nbsp;7201, `analyze`&nbsp;7210, `inrContext`&nbsp;7312

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7371, `rytmSVG`&nbsp;7383, `poryWCzasieSVG`&nbsp;7445, `iskraSVG`&nbsp;7513, `dowSVG`&nbsp;7541, `dniRytmu`&nbsp;7577, `skutecznoscTygodniami`&nbsp;7598, `renderAnalysis`&nbsp;7626

*RAPORT* — `collectRows`&nbsp;7733, `makeReport`&nbsp;7770

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7921, `tagiPrzed`&nbsp;7929, `tagPrzelacz`&nbsp;7938

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7981, `opisKopii`&nbsp;7991

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8031, `tgCzatKopii`&nbsp;8038, `odswiezKopie`&nbsp;8045, `tgKopiaCzatZapisz`&nbsp;8053, `tgKopiaCzatZnajdz`&nbsp;8073, `tgKopiaWlacz`&nbsp;8103, `tgKopiaWylacz`&nbsp;8122, `kopiaNaTelegram`&nbsp;8131, `kopiaAutomat`&nbsp;8182

*WIEK KOPII* — `dniOdDaty`&nbsp;8229, `wiekKopiiTxt`&nbsp;8235, `renderKopiaStan`&nbsp;8243, `zapiszKopie`&nbsp;8258

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8292, `ustawieniaDoOdtworzenia`&nbsp;8340, `wczytajKopie`&nbsp;8355, `kopiaCzytelna`&nbsp;8360, `odtworzKopie`&nbsp;8370, `kopiaWybrana`&nbsp;8403

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8427, `odtworzZBazy`&nbsp;8459, `exportCsv`&nbsp;8471

*NAWIGACJA* — `wrocZEkranu`&nbsp;8578


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
