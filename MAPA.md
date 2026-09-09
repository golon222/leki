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

## `index.html` — 8313 linii, ~130 tys. tokenow

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
| 1052 | 1127 | tab-set |
| 1128 | 1194 | tab-lek |
| 1195 | 1214 | tab-sinr |
| 1215 | 1254 | tab-wifi |
| 1255 | 1355 | tab-tg |
| 1356 | 1389 | tab-dev |
| 1390 | 1490 | tab-diag |
| 1491 | 1766 | tab-help |
| 1767 | 1788 | tab-ev |
| 1789 | 1879 | tab-hist |
| 1880 | 1888 | JS — poczatek |
| 1889 | 1942 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 1943 | 2017 | INFORMACJA ZWROTNA |
| 2018 | 2112 | STREFY CZASOWE |
| 2113 | 2140 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2141 | 2207 | TABLETKA JAKO BRYŁA |
| 2208 | 2222 | LOGOWANIE |
| 2223 | 2362 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2363 | 2727 | START |
| 2728 | 2788 | OSŁONA RYSOWANIA |
| 2789 | 2847 | REKONCYLIACJA |
| 2848 | 3052 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3053 | 3111 | KALENDARZ |
| 3112 | 3378 | HISTORIA ROZPISANIA DAWKI |
| 3379 | 3560 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3561 | 3712 | ARKUSZ DNIA |
| 3713 | 3812 | WZIĄŁEM TERAZ |
| 3813 | 3927 | INR |
| 3928 | 4067 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4068 | 4080 | STATUS PUDEŁKA |
| 4081 | 4230 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4231 | 4392 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4393 | 4675 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4676 | 4716 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4717 | 4813 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 4814 | 5332 | EKRAN ZDARZEN |
| 5333 | 5451 | ZAPAS TABLETEK |
| 5452 | 5739 | USTAWIENIA |
| 5740 | 6047 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6048 | 6504 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6505 | 6875 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 6876 | 7086 | ANALIZA |
| 7087 | 7472 | WYKRESY ANALIZY |
| 7473 | 7629 | RAPORT |
| 7630 | 7691 | KONTEKST DNIA (TAGI) |
| 7692 | 7737 | KOPIA ZAPASOWA |
| 7738 | 7951 | KOPIA NA TELEGRAM |
| 7952 | 8017 | WIEK KOPII |
| 8018 | 8115 | ODTWARZANIE Z KOPII |
| 8116 | 8190 | KOPIE Z BAZY |
| 8191 | 8271 | NAWIGACJA |
| 8272 | 8313 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (213) — nazwa i linia deklaracji:

*INFORMACJA ZWROTNA* — `toast`&nbsp;1964, `busy`&nbsp;1980, `todayKey`&nbsp;2007, `dzisiajKey`&nbsp;2011, `inNightWindow`&nbsp;2014

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2069, `tzName`&nbsp;2085, `tzLabel`&nbsp;2086, `tzOffsetTxt`&nbsp;2087, `devDate`&nbsp;2093, `devKey`&nbsp;2098, `devHM`&nbsp;2103, `slotMin`&nbsp;2104, `pillColors`&nbsp;2106

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2117

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2154, `cieniuj`&nbsp;2181, `doseGraphic`&nbsp;2198

*LOGOWANIE* — `doLogin`&nbsp;2212

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2232, `wyczyscCache`&nbsp;2327, `fbSignOut`&nbsp;2346

*START* — `boot`&nbsp;2364

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;2755, `rysujWszystkie`&nbsp;2768, `renderAll`&nbsp;2772

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;2795, `reconcileDecyzja`&nbsp;2834

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;2869, `zapiszReconcile`&nbsp;2881, `doReconcile`&nbsp;2920, `doReconcileWewn`&nbsp;2930, `reconcile`&nbsp;3051

*KALENDARZ* — `tydzienDawek`&nbsp;3091

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3137, `dawkaNaDzien`&nbsp;3148, `dzienBezLeku`&nbsp;3169, `wyjatekNaDzien`&nbsp;3174, `opisDawkowania`&nbsp;3180, `dayDose`&nbsp;3190, `dzienZamkniety`&nbsp;3224, `trackingSince`&nbsp;3230, `beforeTracking`&nbsp;3231, `dayStatus`&nbsp;3233, `renderCalendar`&nbsp;3274, `seriaDni`&nbsp;3339, `doNastepnej`&nbsp;3357, `opisCzasu`&nbsp;3372

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3387, `trwanieTxt`&nbsp;3402, `kiedyDawkaTxt`&nbsp;3413, `odswiezOdDawki`&nbsp;3421, `startTikOdDawki`&nbsp;3435, `renderToday`&nbsp;3445

*ARKUSZ DNIA* — `closeSheet`&nbsp;3572, `renderSheet`&nbsp;3574, `resetDose`&nbsp;3652, `resetPlan`&nbsp;3659, `commitPlan`&nbsp;3664, `clearPlan`&nbsp;3680, `commitDose`&nbsp;3692

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3733, `askConfirm`&nbsp;3802

*INR* — `inrState`&nbsp;3814, `odswiezTerminInr`&nbsp;3827, `addInr`&nbsp;3836, `inrKeysOk`&nbsp;3922

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;3937, `inrTerminKey`&nbsp;3945, `inrDoTerminu`&nbsp;3957, `dniTxt`&nbsp;3966, `renderInr`&nbsp;3968, `inrChart`&nbsp;4040

*STATUS PUDEŁKA* — `relTime`&nbsp;4069, `devDayMon`&nbsp;4078

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4113, `renderBoxLog`&nbsp;4159, `logPrzelacz`&nbsp;4195, `renderNvsFailLog`&nbsp;4203

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4248, `oczekWczytaj`&nbsp;4256, `oczekZapisz`&nbsp;4261, `oczekIle`&nbsp;4264, `zapiszPewnie`&nbsp;4274, `zapiszCfg`&nbsp;4312, `bazaOdmowila`&nbsp;4330, `oczekWyslij`&nbsp;4354, `oczekOdmowy`&nbsp;4391

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4407, `ostrzReguly`&nbsp;4429, `lm`&nbsp;4468, `ostrzMilczy`&nbsp;4474, `nvsMalo`&nbsp;4554, `opisNvsFailKey`&nbsp;4566, `stratyDotyczaLeku`&nbsp;4619, `ostrzStraty`&nbsp;4631, `stratyCicho`&nbsp;4665

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4697

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4727, `renderOstrzezenia`&nbsp;4742, `bezPokrycia`&nbsp;4752, `wierszZdarzenia`&nbsp;4758, `renderDiag`&nbsp;4775

*EKRAN ZDARZEN* — `evFiltr`&nbsp;4830, `evPasuje`&nbsp;4835, `renderEvents`&nbsp;4847, `renderOpenWarn`&nbsp;4887, `minutyDoPelna`&nbsp;4940, `opisLadowania`&nbsp;4952, `dni`&nbsp;4972, `opisLadowan`&nbsp;4975, `tempoZHistorii`&nbsp;5028, `prognozaDni`&nbsp;5037, `opisPrognozy`&nbsp;5049, `czasKrotko`&nbsp;5071, `opisCzuwania`&nbsp;5079, `renderStatus`&nbsp;5097

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5336, `dayAfter`&nbsp;5339, `pillsBaseInfo`&nbsp;5350, `settlePills`&nbsp;5360, `dniZapasu`&nbsp;5400, `renderPills`&nbsp;5413, `savePills`&nbsp;5434, `setPills`&nbsp;5445

*USTAWIENIA* — `renderKafelki`&nbsp;5456, `renderSettings`&nbsp;5483, `tydzienZPol`&nbsp;5538, `renderWeekEditor`&nbsp;5550, `odswiezPodpowiedzTygodnia`&nbsp;5567, `tydzienZmieniony`&nbsp;5581, `rownajTydzien`&nbsp;5582, `renderPlanList`&nbsp;5630, `renderExceptions`&nbsp;5657, `wyslijSiec`&nbsp;5715

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5757, `tgZapytaj`&nbsp;5767, `tgKodParowania`&nbsp;5807, `tgZnajdzCzat`&nbsp;5823, `tgPolacz`&nbsp;5891, `tgProbna`&nbsp;5924, `tgOdlacz`&nbsp;5931, `renderTgStan`&nbsp;5953

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6068, `pobierzOpisFirmware`&nbsp;6074, `wyslijAktualizacje`&nbsp;6095, `anulujAktualizacje`&nbsp;6140, `renderOta`&nbsp;6146, `renderNetStan`&nbsp;6431

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6516, `renderSkan`&nbsp;6522, `szukajSieci`&nbsp;6574, `wybierzSiec`&nbsp;6582, `wyslijPolecenieSieci`&nbsp;6600, `siecZIndeksu`&nbsp;6610, `tzChanged`&nbsp;6644, `cfgTime`&nbsp;6649, `addSlot`&nbsp;6657, `zapiszPlanDnia`&nbsp;6670, `saveConfig`&nbsp;6688, `inrKrokiZakresu`&nbsp;6758, `opcjeInr`&nbsp;6765, `inrZakresZmieniony`&nbsp;6776, `wypelnijListyZakresu`&nbsp;6789, `saveInrRange`&nbsp;6800, `wypelnijListeOdstepu`&nbsp;6834, `saveInrEvery`&nbsp;6847, `odswiezPodpowiedzInr`&nbsp;6857

*ANALIZA* — `openTimeOf`&nbsp;6882, `openMinutes`&nbsp;6888, `sredniaPora`&nbsp;6912, `kwantyl`&nbsp;6920, `dniMiedzy`&nbsp;6928, `odstepyZPunktow`&nbsp;6942, `analyze`&nbsp;6951, `inrContext`&nbsp;7053

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7112, `rytmSVG`&nbsp;7124, `poryWCzasieSVG`&nbsp;7186, `iskraSVG`&nbsp;7254, `dowSVG`&nbsp;7282, `dniRytmu`&nbsp;7318, `skutecznoscTygodniami`&nbsp;7339, `renderAnalysis`&nbsp;7367

*RAPORT* — `collectRows`&nbsp;7474, `makeReport`&nbsp;7511

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7662, `tagiPrzed`&nbsp;7670, `tagPrzelacz`&nbsp;7679

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7722, `opisKopii`&nbsp;7732

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7772, `tgCzatKopii`&nbsp;7779, `odswiezKopie`&nbsp;7786, `tgKopiaCzatZapisz`&nbsp;7794, `tgKopiaCzatZnajdz`&nbsp;7814, `tgKopiaWlacz`&nbsp;7844, `tgKopiaWylacz`&nbsp;7863, `kopiaNaTelegram`&nbsp;7872, `kopiaAutomat`&nbsp;7923

*WIEK KOPII* — `dniOdDaty`&nbsp;7970, `wiekKopiiTxt`&nbsp;7976, `renderKopiaStan`&nbsp;7984, `zapiszKopie`&nbsp;7999

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8033, `wczytajKopie`&nbsp;8063, `kopiaCzytelna`&nbsp;8068, `odtworzKopie`&nbsp;8078, `kopiaWybrana`&nbsp;8101

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8125, `odtworzZBazy`&nbsp;8157, `exportCsv`&nbsp;8169

*NAWIGACJA* — `wrocZEkranu`&nbsp;8270


---

## `firmware/PillBox/PillBox.ino` — 6958 linii

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
| 5582 | 6189 | 11.  DEEP SLEEP |
| 6190 | 6958 | 12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany) |

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

*12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany)* — `petlaLadowania`&nbsp;6202, `setup`&nbsp;6300, `loop`&nbsp;6955


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
