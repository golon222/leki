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

## `index.html` — 8410 linii, ~130 tys. tokenow

Ekrany (`<section>`) i dwa duze bloki. Zakladki `tab-*` odpowiadaja
pozycjom w pasku nawigacji i podekranom Ustawien.

| od | do | co |
|---|---|---|
| 21 | 21 | CSS — poczatek |
| 22 | 222 | SYSTEM WIZUALNY PillBox |
| 223 | 319 | EKRAN GŁÓWNY — KARTA DNIA |
| 320 | 392 | INFORMACJA ZWROTNA |
| 393 | 769 | TABLETKA W 3D |
| 770 | 929 | tab-cal |
| 930 | 978 | tab-inr |
| 979 | 1051 | tab-ana |
| 1052 | 1136 | tab-set |
| 1137 | 1203 | tab-lek |
| 1204 | 1212 | tab-pud |
| 1213 | 1232 | tab-sinr |
| 1233 | 1272 | tab-wifi |
| 1273 | 1373 | tab-tg |
| 1374 | 1407 | tab-dev |
| 1408 | 1508 | tab-diag |
| 1509 | 1784 | tab-help |
| 1785 | 1806 | tab-ev |
| 1807 | 1897 | tab-hist |
| 1898 | 1906 | JS — poczatek |
| 1907 | 2017 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2018 | 2092 | INFORMACJA ZWROTNA |
| 2093 | 2187 | STREFY CZASOWE |
| 2188 | 2215 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2216 | 2282 | TABLETKA JAKO BRYŁA |
| 2283 | 2297 | LOGOWANIE |
| 2298 | 2437 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2438 | 2802 | START |
| 2803 | 2863 | OSŁONA RYSOWANIA |
| 2864 | 2922 | REKONCYLIACJA |
| 2923 | 3127 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3128 | 3186 | KALENDARZ |
| 3187 | 3453 | HISTORIA ROZPISANIA DAWKI |
| 3454 | 3635 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3636 | 3787 | ARKUSZ DNIA |
| 3788 | 3887 | WZIĄŁEM TERAZ |
| 3888 | 4002 | INR |
| 4003 | 4142 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4143 | 4155 | STATUS PUDEŁKA |
| 4156 | 4305 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4306 | 4467 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4468 | 4750 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4751 | 4791 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4792 | 4888 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 4889 | 5407 | EKRAN ZDARZEN |
| 5408 | 5526 | ZAPAS TABLETEK |
| 5527 | 5834 | USTAWIENIA |
| 5835 | 6142 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6143 | 6599 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6600 | 6970 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 6971 | 7181 | ANALIZA |
| 7182 | 7567 | WYKRESY ANALIZY |
| 7568 | 7724 | RAPORT |
| 7725 | 7786 | KONTEKST DNIA (TAGI) |
| 7787 | 7832 | KOPIA ZAPASOWA |
| 7833 | 8046 | KOPIA NA TELEGRAM |
| 8047 | 8112 | WIEK KOPII |
| 8113 | 8210 | ODTWARZANIE Z KOPII |
| 8211 | 8285 | KOPIE Z BAZY |
| 8286 | 8368 | NAWIGACJA |
| 8369 | 8410 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (218) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `pudelkoZnane`&nbsp;1938, `wybranePudelko`&nbsp;1940, `wybierzPudelko`&nbsp;1956, `profilTydzien`&nbsp;2002

*INFORMACJA ZWROTNA* — `toast`&nbsp;2039, `busy`&nbsp;2055, `todayKey`&nbsp;2082, `dzisiajKey`&nbsp;2086, `inNightWindow`&nbsp;2089

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2144, `tzName`&nbsp;2160, `tzLabel`&nbsp;2161, `tzOffsetTxt`&nbsp;2162, `devDate`&nbsp;2168, `devKey`&nbsp;2173, `devHM`&nbsp;2178, `slotMin`&nbsp;2179, `pillColors`&nbsp;2181

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2192

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2229, `cieniuj`&nbsp;2256, `doseGraphic`&nbsp;2273

*LOGOWANIE* — `doLogin`&nbsp;2287

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2307, `wyczyscCache`&nbsp;2402, `fbSignOut`&nbsp;2421

*START* — `boot`&nbsp;2439

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;2830, `rysujWszystkie`&nbsp;2843, `renderAll`&nbsp;2847

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;2870, `reconcileDecyzja`&nbsp;2909

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;2944, `zapiszReconcile`&nbsp;2956, `doReconcile`&nbsp;2995, `doReconcileWewn`&nbsp;3005, `reconcile`&nbsp;3126

*KALENDARZ* — `tydzienDawek`&nbsp;3166

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3212, `dawkaNaDzien`&nbsp;3223, `dzienBezLeku`&nbsp;3244, `wyjatekNaDzien`&nbsp;3249, `opisDawkowania`&nbsp;3255, `dayDose`&nbsp;3265, `dzienZamkniety`&nbsp;3299, `trackingSince`&nbsp;3305, `beforeTracking`&nbsp;3306, `dayStatus`&nbsp;3308, `renderCalendar`&nbsp;3349, `seriaDni`&nbsp;3414, `doNastepnej`&nbsp;3432, `opisCzasu`&nbsp;3447

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3462, `trwanieTxt`&nbsp;3477, `kiedyDawkaTxt`&nbsp;3488, `odswiezOdDawki`&nbsp;3496, `startTikOdDawki`&nbsp;3510, `renderToday`&nbsp;3520

*ARKUSZ DNIA* — `closeSheet`&nbsp;3647, `renderSheet`&nbsp;3649, `resetDose`&nbsp;3727, `resetPlan`&nbsp;3734, `commitPlan`&nbsp;3739, `clearPlan`&nbsp;3755, `commitDose`&nbsp;3767

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3808, `askConfirm`&nbsp;3877

*INR* — `inrState`&nbsp;3889, `odswiezTerminInr`&nbsp;3902, `addInr`&nbsp;3911, `inrKeysOk`&nbsp;3997

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4012, `inrTerminKey`&nbsp;4020, `inrDoTerminu`&nbsp;4032, `dniTxt`&nbsp;4041, `renderInr`&nbsp;4043, `inrChart`&nbsp;4115

*STATUS PUDEŁKA* — `relTime`&nbsp;4144, `devDayMon`&nbsp;4153

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4188, `renderBoxLog`&nbsp;4234, `logPrzelacz`&nbsp;4270, `renderNvsFailLog`&nbsp;4278

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4323, `oczekWczytaj`&nbsp;4331, `oczekZapisz`&nbsp;4336, `oczekIle`&nbsp;4339, `zapiszPewnie`&nbsp;4349, `zapiszCfg`&nbsp;4387, `bazaOdmowila`&nbsp;4405, `oczekWyslij`&nbsp;4429, `oczekOdmowy`&nbsp;4466

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4482, `ostrzReguly`&nbsp;4504, `lm`&nbsp;4543, `ostrzMilczy`&nbsp;4549, `nvsMalo`&nbsp;4629, `opisNvsFailKey`&nbsp;4641, `stratyDotyczaLeku`&nbsp;4694, `ostrzStraty`&nbsp;4706, `stratyCicho`&nbsp;4740

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4772

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4802, `renderOstrzezenia`&nbsp;4817, `bezPokrycia`&nbsp;4827, `wierszZdarzenia`&nbsp;4833, `renderDiag`&nbsp;4850

*EKRAN ZDARZEN* — `evFiltr`&nbsp;4905, `evPasuje`&nbsp;4910, `renderEvents`&nbsp;4922, `renderOpenWarn`&nbsp;4962, `minutyDoPelna`&nbsp;5015, `opisLadowania`&nbsp;5027, `dni`&nbsp;5047, `opisLadowan`&nbsp;5050, `tempoZHistorii`&nbsp;5103, `prognozaDni`&nbsp;5112, `opisPrognozy`&nbsp;5124, `czasKrotko`&nbsp;5146, `opisCzuwania`&nbsp;5154, `renderStatus`&nbsp;5172

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5411, `dayAfter`&nbsp;5414, `pillsBaseInfo`&nbsp;5425, `settlePills`&nbsp;5435, `dniZapasu`&nbsp;5475, `renderPills`&nbsp;5488, `savePills`&nbsp;5509, `setPills`&nbsp;5520

*USTAWIENIA* — `renderKafelki`&nbsp;5531, `renderPudelka`&nbsp;5560, `renderSettings`&nbsp;5577, `tydzienZPol`&nbsp;5633, `renderWeekEditor`&nbsp;5645, `odswiezPodpowiedzTygodnia`&nbsp;5662, `tydzienZmieniony`&nbsp;5676, `rownajTydzien`&nbsp;5677, `renderPlanList`&nbsp;5725, `renderExceptions`&nbsp;5752, `wyslijSiec`&nbsp;5810

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5852, `tgZapytaj`&nbsp;5862, `tgKodParowania`&nbsp;5902, `tgZnajdzCzat`&nbsp;5918, `tgPolacz`&nbsp;5986, `tgProbna`&nbsp;6019, `tgOdlacz`&nbsp;6026, `renderTgStan`&nbsp;6048

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6163, `pobierzOpisFirmware`&nbsp;6169, `wyslijAktualizacje`&nbsp;6190, `anulujAktualizacje`&nbsp;6235, `renderOta`&nbsp;6241, `renderNetStan`&nbsp;6526

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6611, `renderSkan`&nbsp;6617, `szukajSieci`&nbsp;6669, `wybierzSiec`&nbsp;6677, `wyslijPolecenieSieci`&nbsp;6695, `siecZIndeksu`&nbsp;6705, `tzChanged`&nbsp;6739, `cfgTime`&nbsp;6744, `addSlot`&nbsp;6752, `zapiszPlanDnia`&nbsp;6765, `saveConfig`&nbsp;6783, `inrKrokiZakresu`&nbsp;6853, `opcjeInr`&nbsp;6860, `inrZakresZmieniony`&nbsp;6871, `wypelnijListyZakresu`&nbsp;6884, `saveInrRange`&nbsp;6895, `wypelnijListeOdstepu`&nbsp;6929, `saveInrEvery`&nbsp;6942, `odswiezPodpowiedzInr`&nbsp;6952

*ANALIZA* — `openTimeOf`&nbsp;6977, `openMinutes`&nbsp;6983, `sredniaPora`&nbsp;7007, `kwantyl`&nbsp;7015, `dniMiedzy`&nbsp;7023, `odstepyZPunktow`&nbsp;7037, `analyze`&nbsp;7046, `inrContext`&nbsp;7148

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7207, `rytmSVG`&nbsp;7219, `poryWCzasieSVG`&nbsp;7281, `iskraSVG`&nbsp;7349, `dowSVG`&nbsp;7377, `dniRytmu`&nbsp;7413, `skutecznoscTygodniami`&nbsp;7434, `renderAnalysis`&nbsp;7462

*RAPORT* — `collectRows`&nbsp;7569, `makeReport`&nbsp;7606

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7757, `tagiPrzed`&nbsp;7765, `tagPrzelacz`&nbsp;7774

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7817, `opisKopii`&nbsp;7827

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7867, `tgCzatKopii`&nbsp;7874, `odswiezKopie`&nbsp;7881, `tgKopiaCzatZapisz`&nbsp;7889, `tgKopiaCzatZnajdz`&nbsp;7909, `tgKopiaWlacz`&nbsp;7939, `tgKopiaWylacz`&nbsp;7958, `kopiaNaTelegram`&nbsp;7967, `kopiaAutomat`&nbsp;8018

*WIEK KOPII* — `dniOdDaty`&nbsp;8065, `wiekKopiiTxt`&nbsp;8071, `renderKopiaStan`&nbsp;8079, `zapiszKopie`&nbsp;8094

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8128, `wczytajKopie`&nbsp;8158, `kopiaCzytelna`&nbsp;8163, `odtworzKopie`&nbsp;8173, `kopiaWybrana`&nbsp;8196

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8220, `odtworzZBazy`&nbsp;8252, `exportCsv`&nbsp;8264

*NAWIGACJA* — `wrocZEkranu`&nbsp;8367


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
