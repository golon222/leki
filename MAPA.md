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

## `index.html` — 8946 linii, ~130 tys. tokenow

Ekrany (`<section>`) i dwa duze bloki. Zakladki `tab-*` odpowiadaja
pozycjom w pasku nawigacji i podekranom Ustawien.

| od | do | co |
|---|---|---|
| 21 | 21 | CSS — poczatek |
| 22 | 263 | SYSTEM WIZUALNY PillBox |
| 264 | 360 | EKRAN GŁÓWNY — KARTA DNIA |
| 361 | 433 | INFORMACJA ZWROTNA |
| 434 | 904 | TABLETKA W 3D |
| 905 | 1064 | tab-cal |
| 1065 | 1113 | tab-inr |
| 1114 | 1204 | tab-ana |
| 1205 | 1313 | tab-set |
| 1314 | 1394 | tab-lek |
| 1395 | 1404 | tab-pud |
| 1405 | 1424 | tab-sinr |
| 1425 | 1464 | tab-wifi |
| 1465 | 1565 | tab-tg |
| 1566 | 1599 | tab-dev |
| 1600 | 1709 | tab-diag |
| 1710 | 2032 | tab-help |
| 2033 | 2054 | tab-ev |
| 2055 | 2145 | tab-hist |
| 2146 | 2154 | JS — poczatek |
| 2155 | 2397 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2398 | 2472 | INFORMACJA ZWROTNA |
| 2473 | 2567 | STREFY CZASOWE |
| 2568 | 2595 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2596 | 2662 | TABLETKA JAKO BRYŁA |
| 2663 | 2677 | LOGOWANIE |
| 2678 | 2817 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2818 | 3198 | START |
| 3199 | 3259 | OSŁONA RYSOWANIA |
| 3260 | 3318 | REKONCYLIACJA |
| 3319 | 3523 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3524 | 3582 | KALENDARZ |
| 3583 | 3853 | HISTORIA ROZPISANIA DAWKI |
| 3854 | 4042 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 4043 | 4194 | ARKUSZ DNIA |
| 4195 | 4294 | WZIĄŁEM TERAZ |
| 4295 | 4409 | INR |
| 4410 | 4549 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4550 | 4562 | STATUS PUDEŁKA |
| 4563 | 4712 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4713 | 4874 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4875 | 5157 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 5158 | 5198 | KOLEJKA, KTÓRA NIE SCHODZI |
| 5199 | 5295 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 5296 | 5840 | EKRAN ZDARZEN |
| 5841 | 5959 | ZAPAS TABLETEK |
| 5960 | 6314 | USTAWIENIA |
| 6315 | 6622 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6623 | 7081 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 7082 | 7453 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7454 | 7664 | ANALIZA |
| 7665 | 8050 | WYKRESY ANALIZY |
| 8051 | 8207 | RAPORT |
| 8208 | 8269 | KONTEKST DNIA (TAGI) |
| 8270 | 8320 | KOPIA ZAPASOWA |
| 8321 | 8534 | KOPIA NA TELEGRAM |
| 8535 | 8600 | WIEK KOPII |
| 8601 | 8741 | ODTWARZANIE Z KOPII |
| 8742 | 8816 | KOPIE Z BAZY |
| 8817 | 8904 | NAWIGACJA |
| 8905 | 8946 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (227) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `opisPlikFirmware`&nbsp;2196, `wersjaZTelegramem`&nbsp;2205, `pudelkoZnane`&nbsp;2211, `wybranePudelko`&nbsp;2213, `korzenDanych`&nbsp;2237, `odmowaRegul`&nbsp;2256, `sprawdzDostepPudelek`&nbsp;2261, `wybierzPudelko`&nbsp;2284, `profilTydzien`&nbsp;2346, `komoraDnia`&nbsp;2357, `ustawProfil`&nbsp;2372

*INFORMACJA ZWROTNA* — `toast`&nbsp;2419, `busy`&nbsp;2435, `todayKey`&nbsp;2462, `dzisiajKey`&nbsp;2466, `inNightWindow`&nbsp;2469

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2524, `tzName`&nbsp;2540, `tzLabel`&nbsp;2541, `tzOffsetTxt`&nbsp;2542, `devDate`&nbsp;2548, `devKey`&nbsp;2553, `devHM`&nbsp;2558, `slotMin`&nbsp;2559, `pillColors`&nbsp;2561

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2572

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2609, `cieniuj`&nbsp;2636, `doseGraphic`&nbsp;2653

*LOGOWANIE* — `doLogin`&nbsp;2667

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2687, `wyczyscCache`&nbsp;2782, `fbSignOut`&nbsp;2801

*START* — `boot`&nbsp;2819

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;3226, `rysujWszystkie`&nbsp;3239, `renderAll`&nbsp;3243

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;3266, `reconcileDecyzja`&nbsp;3305

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3340, `zapiszReconcile`&nbsp;3352, `doReconcile`&nbsp;3391, `doReconcileWewn`&nbsp;3401, `reconcile`&nbsp;3522

*KALENDARZ* — `tydzienDawek`&nbsp;3562

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3608, `dawkaNaDzien`&nbsp;3619, `dzienBezLeku`&nbsp;3640, `wyjatekNaDzien`&nbsp;3645, `opisDawkowania`&nbsp;3651, `dayDose`&nbsp;3665, `dzienZamkniety`&nbsp;3699, `trackingSince`&nbsp;3705, `beforeTracking`&nbsp;3706, `dayStatus`&nbsp;3708, `renderCalendar`&nbsp;3749, `seriaDni`&nbsp;3814, `doNastepnej`&nbsp;3832, `opisCzasu`&nbsp;3847

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3862, `trwanieTxt`&nbsp;3877, `kiedyDawkaTxt`&nbsp;3888, `odswiezOdDawki`&nbsp;3896, `startTikOdDawki`&nbsp;3910, `renderToday`&nbsp;3920

*ARKUSZ DNIA* — `closeSheet`&nbsp;4054, `renderSheet`&nbsp;4056, `resetDose`&nbsp;4134, `resetPlan`&nbsp;4141, `commitPlan`&nbsp;4146, `clearPlan`&nbsp;4162, `commitDose`&nbsp;4174

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;4215, `askConfirm`&nbsp;4284

*INR* — `inrState`&nbsp;4296, `odswiezTerminInr`&nbsp;4309, `addInr`&nbsp;4318, `inrKeysOk`&nbsp;4404

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4419, `inrTerminKey`&nbsp;4427, `inrDoTerminu`&nbsp;4439, `dniTxt`&nbsp;4448, `renderInr`&nbsp;4450, `inrChart`&nbsp;4522

*STATUS PUDEŁKA* — `relTime`&nbsp;4551, `devDayMon`&nbsp;4560

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4595, `renderBoxLog`&nbsp;4641, `logPrzelacz`&nbsp;4677, `renderNvsFailLog`&nbsp;4685

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4730, `oczekWczytaj`&nbsp;4738, `oczekZapisz`&nbsp;4743, `oczekIle`&nbsp;4746, `zapiszPewnie`&nbsp;4756, `zapiszCfg`&nbsp;4794, `bazaOdmowila`&nbsp;4812, `oczekWyslij`&nbsp;4836, `oczekOdmowy`&nbsp;4873

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4889, `ostrzReguly`&nbsp;4911, `lm`&nbsp;4950, `ostrzMilczy`&nbsp;4956, `nvsMalo`&nbsp;5036, `opisNvsFailKey`&nbsp;5048, `stratyDotyczaLeku`&nbsp;5101, `ostrzStraty`&nbsp;5113, `stratyCicho`&nbsp;5147

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;5179

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;5209, `renderOstrzezenia`&nbsp;5224, `bezPokrycia`&nbsp;5234, `wierszZdarzenia`&nbsp;5240, `renderDiag`&nbsp;5257

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5312, `evPasuje`&nbsp;5317, `renderEvents`&nbsp;5329, `renderOpenWarn`&nbsp;5369, `minutyDoPelna`&nbsp;5422, `opisLadowania`&nbsp;5434, `dni`&nbsp;5454, `opisLadowan`&nbsp;5457, `tempoZHistorii`&nbsp;5510, `prognozaDni`&nbsp;5519, `opisPrognozy`&nbsp;5531, `czasKrotko`&nbsp;5553, `opisCzuwania`&nbsp;5561, `renderStatus`&nbsp;5579

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5844, `dayAfter`&nbsp;5847, `pillsBaseInfo`&nbsp;5858, `settlePills`&nbsp;5868, `dniZapasu`&nbsp;5908, `renderPills`&nbsp;5921, `savePills`&nbsp;5942, `setPills`&nbsp;5953

*USTAWIENIA* — `opisLeku`&nbsp;5971, `renderKafelki`&nbsp;5977, `renderPudelka`&nbsp;6012, `renderSettings`&nbsp;6054, `tydzienZPol`&nbsp;6113, `renderWeekEditor`&nbsp;6125, `odswiezPodpowiedzTygodnia`&nbsp;6142, `tydzienZmieniony`&nbsp;6156, `rownajTydzien`&nbsp;6157, `renderPlanList`&nbsp;6205, `renderExceptions`&nbsp;6232, `wyslijSiec`&nbsp;6290

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6332, `tgZapytaj`&nbsp;6342, `tgKodParowania`&nbsp;6382, `tgZnajdzCzat`&nbsp;6398, `tgPolacz`&nbsp;6466, `tgProbna`&nbsp;6499, `tgOdlacz`&nbsp;6506, `renderTgStan`&nbsp;6528

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6643, `pobierzOpisFirmware`&nbsp;6649, `wyslijAktualizacje`&nbsp;6672, `anulujAktualizacje`&nbsp;6717, `renderOta`&nbsp;6723, `renderNetStan`&nbsp;7008

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;7093, `renderSkan`&nbsp;7099, `szukajSieci`&nbsp;7151, `wybierzSiec`&nbsp;7159, `wyslijPolecenieSieci`&nbsp;7177, `siecZIndeksu`&nbsp;7187, `tzChanged`&nbsp;7221, `cfgTime`&nbsp;7226, `addSlot`&nbsp;7234, `zapiszPlanDnia`&nbsp;7247, `saveConfig`&nbsp;7265, `inrKrokiZakresu`&nbsp;7336, `opcjeInr`&nbsp;7343, `inrZakresZmieniony`&nbsp;7354, `wypelnijListyZakresu`&nbsp;7367, `saveInrRange`&nbsp;7378, `wypelnijListeOdstepu`&nbsp;7412, `saveInrEvery`&nbsp;7425, `odswiezPodpowiedzInr`&nbsp;7435

*ANALIZA* — `openTimeOf`&nbsp;7460, `openMinutes`&nbsp;7466, `sredniaPora`&nbsp;7490, `kwantyl`&nbsp;7498, `dniMiedzy`&nbsp;7506, `odstepyZPunktow`&nbsp;7520, `analyze`&nbsp;7529, `inrContext`&nbsp;7631

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7690, `rytmSVG`&nbsp;7702, `poryWCzasieSVG`&nbsp;7764, `iskraSVG`&nbsp;7832, `dowSVG`&nbsp;7860, `dniRytmu`&nbsp;7896, `skutecznoscTygodniami`&nbsp;7917, `renderAnalysis`&nbsp;7945

*RAPORT* — `collectRows`&nbsp;8052, `makeReport`&nbsp;8089

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;8240, `tagiPrzed`&nbsp;8248, `tagPrzelacz`&nbsp;8257

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;8300, `opisKopii`&nbsp;8310

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8355, `tgCzatKopii`&nbsp;8362, `odswiezKopie`&nbsp;8369, `tgKopiaCzatZapisz`&nbsp;8377, `tgKopiaCzatZnajdz`&nbsp;8397, `tgKopiaWlacz`&nbsp;8427, `tgKopiaWylacz`&nbsp;8446, `kopiaNaTelegram`&nbsp;8455, `kopiaAutomat`&nbsp;8506

*WIEK KOPII* — `dniOdDaty`&nbsp;8553, `wiekKopiiTxt`&nbsp;8559, `renderKopiaStan`&nbsp;8567, `zapiszKopie`&nbsp;8582

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8616, `ustawieniaDoOdtworzenia`&nbsp;8664, `wczytajKopie`&nbsp;8679, `kopiaCzytelna`&nbsp;8684, `odtworzKopie`&nbsp;8694, `kopiaWybrana`&nbsp;8727

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8751, `odtworzZBazy`&nbsp;8783, `exportCsv`&nbsp;8795

*NAWIGACJA* — `wrocZEkranu`&nbsp;8903


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
