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

## `index.html` — 8491 linii, ~130 tys. tokenow

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
| 1907 | 2075 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2076 | 2150 | INFORMACJA ZWROTNA |
| 2151 | 2245 | STREFY CZASOWE |
| 2246 | 2273 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2274 | 2340 | TABLETKA JAKO BRYŁA |
| 2341 | 2355 | LOGOWANIE |
| 2356 | 2495 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2496 | 2875 | START |
| 2876 | 2936 | OSŁONA RYSOWANIA |
| 2937 | 2995 | REKONCYLIACJA |
| 2996 | 3200 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3201 | 3259 | KALENDARZ |
| 3260 | 3526 | HISTORIA ROZPISANIA DAWKI |
| 3527 | 3708 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3709 | 3860 | ARKUSZ DNIA |
| 3861 | 3960 | WZIĄŁEM TERAZ |
| 3961 | 4075 | INR |
| 4076 | 4215 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4216 | 4228 | STATUS PUDEŁKA |
| 4229 | 4378 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4379 | 4540 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4541 | 4823 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4824 | 4864 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4865 | 4961 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 4962 | 5480 | EKRAN ZDARZEN |
| 5481 | 5599 | ZAPAS TABLETEK |
| 5600 | 5915 | USTAWIENIA |
| 5916 | 6223 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6224 | 6680 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6681 | 7051 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7052 | 7262 | ANALIZA |
| 7263 | 7648 | WYKRESY ANALIZY |
| 7649 | 7805 | RAPORT |
| 7806 | 7867 | KONTEKST DNIA (TAGI) |
| 7868 | 7913 | KOPIA ZAPASOWA |
| 7914 | 8127 | KOPIA NA TELEGRAM |
| 8128 | 8193 | WIEK KOPII |
| 8194 | 8291 | ODTWARZANIE Z KOPII |
| 8292 | 8366 | KOPIE Z BAZY |
| 8367 | 8449 | NAWIGACJA |
| 8450 | 8491 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (221) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `pudelkoZnane`&nbsp;1938, `wybranePudelko`&nbsp;1940, `korzenDanych`&nbsp;1964, `odmowaRegul`&nbsp;1983, `sprawdzDostepPudelek`&nbsp;1988, `wybierzPudelko`&nbsp;2011, `profilTydzien`&nbsp;2060

*INFORMACJA ZWROTNA* — `toast`&nbsp;2097, `busy`&nbsp;2113, `todayKey`&nbsp;2140, `dzisiajKey`&nbsp;2144, `inNightWindow`&nbsp;2147

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2202, `tzName`&nbsp;2218, `tzLabel`&nbsp;2219, `tzOffsetTxt`&nbsp;2220, `devDate`&nbsp;2226, `devKey`&nbsp;2231, `devHM`&nbsp;2236, `slotMin`&nbsp;2237, `pillColors`&nbsp;2239

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2250

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2287, `cieniuj`&nbsp;2314, `doseGraphic`&nbsp;2331

*LOGOWANIE* — `doLogin`&nbsp;2345

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2365, `wyczyscCache`&nbsp;2460, `fbSignOut`&nbsp;2479

*START* — `boot`&nbsp;2497

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;2903, `rysujWszystkie`&nbsp;2916, `renderAll`&nbsp;2920

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;2943, `reconcileDecyzja`&nbsp;2982

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3017, `zapiszReconcile`&nbsp;3029, `doReconcile`&nbsp;3068, `doReconcileWewn`&nbsp;3078, `reconcile`&nbsp;3199

*KALENDARZ* — `tydzienDawek`&nbsp;3239

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3285, `dawkaNaDzien`&nbsp;3296, `dzienBezLeku`&nbsp;3317, `wyjatekNaDzien`&nbsp;3322, `opisDawkowania`&nbsp;3328, `dayDose`&nbsp;3338, `dzienZamkniety`&nbsp;3372, `trackingSince`&nbsp;3378, `beforeTracking`&nbsp;3379, `dayStatus`&nbsp;3381, `renderCalendar`&nbsp;3422, `seriaDni`&nbsp;3487, `doNastepnej`&nbsp;3505, `opisCzasu`&nbsp;3520

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3535, `trwanieTxt`&nbsp;3550, `kiedyDawkaTxt`&nbsp;3561, `odswiezOdDawki`&nbsp;3569, `startTikOdDawki`&nbsp;3583, `renderToday`&nbsp;3593

*ARKUSZ DNIA* — `closeSheet`&nbsp;3720, `renderSheet`&nbsp;3722, `resetDose`&nbsp;3800, `resetPlan`&nbsp;3807, `commitPlan`&nbsp;3812, `clearPlan`&nbsp;3828, `commitDose`&nbsp;3840

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3881, `askConfirm`&nbsp;3950

*INR* — `inrState`&nbsp;3962, `odswiezTerminInr`&nbsp;3975, `addInr`&nbsp;3984, `inrKeysOk`&nbsp;4070

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4085, `inrTerminKey`&nbsp;4093, `inrDoTerminu`&nbsp;4105, `dniTxt`&nbsp;4114, `renderInr`&nbsp;4116, `inrChart`&nbsp;4188

*STATUS PUDEŁKA* — `relTime`&nbsp;4217, `devDayMon`&nbsp;4226

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4261, `renderBoxLog`&nbsp;4307, `logPrzelacz`&nbsp;4343, `renderNvsFailLog`&nbsp;4351

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4396, `oczekWczytaj`&nbsp;4404, `oczekZapisz`&nbsp;4409, `oczekIle`&nbsp;4412, `zapiszPewnie`&nbsp;4422, `zapiszCfg`&nbsp;4460, `bazaOdmowila`&nbsp;4478, `oczekWyslij`&nbsp;4502, `oczekOdmowy`&nbsp;4539

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4555, `ostrzReguly`&nbsp;4577, `lm`&nbsp;4616, `ostrzMilczy`&nbsp;4622, `nvsMalo`&nbsp;4702, `opisNvsFailKey`&nbsp;4714, `stratyDotyczaLeku`&nbsp;4767, `ostrzStraty`&nbsp;4779, `stratyCicho`&nbsp;4813

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4845

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4875, `renderOstrzezenia`&nbsp;4890, `bezPokrycia`&nbsp;4900, `wierszZdarzenia`&nbsp;4906, `renderDiag`&nbsp;4923

*EKRAN ZDARZEN* — `evFiltr`&nbsp;4978, `evPasuje`&nbsp;4983, `renderEvents`&nbsp;4995, `renderOpenWarn`&nbsp;5035, `minutyDoPelna`&nbsp;5088, `opisLadowania`&nbsp;5100, `dni`&nbsp;5120, `opisLadowan`&nbsp;5123, `tempoZHistorii`&nbsp;5176, `prognozaDni`&nbsp;5185, `opisPrognozy`&nbsp;5197, `czasKrotko`&nbsp;5219, `opisCzuwania`&nbsp;5227, `renderStatus`&nbsp;5245

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5484, `dayAfter`&nbsp;5487, `pillsBaseInfo`&nbsp;5498, `settlePills`&nbsp;5508, `dniZapasu`&nbsp;5548, `renderPills`&nbsp;5561, `savePills`&nbsp;5582, `setPills`&nbsp;5593

*USTAWIENIA* — `renderKafelki`&nbsp;5604, `renderPudelka`&nbsp;5638, `renderSettings`&nbsp;5658, `tydzienZPol`&nbsp;5714, `renderWeekEditor`&nbsp;5726, `odswiezPodpowiedzTygodnia`&nbsp;5743, `tydzienZmieniony`&nbsp;5757, `rownajTydzien`&nbsp;5758, `renderPlanList`&nbsp;5806, `renderExceptions`&nbsp;5833, `wyslijSiec`&nbsp;5891

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5933, `tgZapytaj`&nbsp;5943, `tgKodParowania`&nbsp;5983, `tgZnajdzCzat`&nbsp;5999, `tgPolacz`&nbsp;6067, `tgProbna`&nbsp;6100, `tgOdlacz`&nbsp;6107, `renderTgStan`&nbsp;6129

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6244, `pobierzOpisFirmware`&nbsp;6250, `wyslijAktualizacje`&nbsp;6271, `anulujAktualizacje`&nbsp;6316, `renderOta`&nbsp;6322, `renderNetStan`&nbsp;6607

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6692, `renderSkan`&nbsp;6698, `szukajSieci`&nbsp;6750, `wybierzSiec`&nbsp;6758, `wyslijPolecenieSieci`&nbsp;6776, `siecZIndeksu`&nbsp;6786, `tzChanged`&nbsp;6820, `cfgTime`&nbsp;6825, `addSlot`&nbsp;6833, `zapiszPlanDnia`&nbsp;6846, `saveConfig`&nbsp;6864, `inrKrokiZakresu`&nbsp;6934, `opcjeInr`&nbsp;6941, `inrZakresZmieniony`&nbsp;6952, `wypelnijListyZakresu`&nbsp;6965, `saveInrRange`&nbsp;6976, `wypelnijListeOdstepu`&nbsp;7010, `saveInrEvery`&nbsp;7023, `odswiezPodpowiedzInr`&nbsp;7033

*ANALIZA* — `openTimeOf`&nbsp;7058, `openMinutes`&nbsp;7064, `sredniaPora`&nbsp;7088, `kwantyl`&nbsp;7096, `dniMiedzy`&nbsp;7104, `odstepyZPunktow`&nbsp;7118, `analyze`&nbsp;7127, `inrContext`&nbsp;7229

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7288, `rytmSVG`&nbsp;7300, `poryWCzasieSVG`&nbsp;7362, `iskraSVG`&nbsp;7430, `dowSVG`&nbsp;7458, `dniRytmu`&nbsp;7494, `skutecznoscTygodniami`&nbsp;7515, `renderAnalysis`&nbsp;7543

*RAPORT* — `collectRows`&nbsp;7650, `makeReport`&nbsp;7687

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7838, `tagiPrzed`&nbsp;7846, `tagPrzelacz`&nbsp;7855

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7898, `opisKopii`&nbsp;7908

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7948, `tgCzatKopii`&nbsp;7955, `odswiezKopie`&nbsp;7962, `tgKopiaCzatZapisz`&nbsp;7970, `tgKopiaCzatZnajdz`&nbsp;7990, `tgKopiaWlacz`&nbsp;8020, `tgKopiaWylacz`&nbsp;8039, `kopiaNaTelegram`&nbsp;8048, `kopiaAutomat`&nbsp;8099

*WIEK KOPII* — `dniOdDaty`&nbsp;8146, `wiekKopiiTxt`&nbsp;8152, `renderKopiaStan`&nbsp;8160, `zapiszKopie`&nbsp;8175

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8209, `wczytajKopie`&nbsp;8239, `kopiaCzytelna`&nbsp;8244, `odtworzKopie`&nbsp;8254, `kopiaWybrana`&nbsp;8277

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8301, `odtworzZBazy`&nbsp;8333, `exportCsv`&nbsp;8345

*NAWIGACJA* — `wrocZEkranu`&nbsp;8448


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
