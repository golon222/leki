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

## `index.html` — 8186 linii, ~130 tys. tokenow

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
| 4814 | 5205 | EKRAN ZDARZEN |
| 5206 | 5324 | ZAPAS TABLETEK |
| 5325 | 5612 | USTAWIENIA |
| 5613 | 5920 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 5921 | 6377 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6378 | 6748 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 6749 | 6959 | ANALIZA |
| 6960 | 7345 | WYKRESY ANALIZY |
| 7346 | 7502 | RAPORT |
| 7503 | 7564 | KONTEKST DNIA (TAGI) |
| 7565 | 7610 | KOPIA ZAPASOWA |
| 7611 | 7824 | KOPIA NA TELEGRAM |
| 7825 | 7890 | WIEK KOPII |
| 7891 | 7988 | ODTWARZANIE Z KOPII |
| 7989 | 8063 | KOPIE Z BAZY |
| 8064 | 8144 | NAWIGACJA |
| 8145 | 8186 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (208) — nazwa i linia deklaracji:

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

*EKRAN ZDARZEN* — `evFiltr`&nbsp;4830, `evPasuje`&nbsp;4835, `renderEvents`&nbsp;4847, `renderOpenWarn`&nbsp;4887, `minutyDoPelna`&nbsp;4940, `opisLadowania`&nbsp;4952, `dni`&nbsp;4972, `opisLadowan`&nbsp;4975, `renderStatus`&nbsp;4992

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5209, `dayAfter`&nbsp;5212, `pillsBaseInfo`&nbsp;5223, `settlePills`&nbsp;5233, `dniZapasu`&nbsp;5273, `renderPills`&nbsp;5286, `savePills`&nbsp;5307, `setPills`&nbsp;5318

*USTAWIENIA* — `renderKafelki`&nbsp;5329, `renderSettings`&nbsp;5356, `tydzienZPol`&nbsp;5411, `renderWeekEditor`&nbsp;5423, `odswiezPodpowiedzTygodnia`&nbsp;5440, `tydzienZmieniony`&nbsp;5454, `rownajTydzien`&nbsp;5455, `renderPlanList`&nbsp;5503, `renderExceptions`&nbsp;5530, `wyslijSiec`&nbsp;5588

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5630, `tgZapytaj`&nbsp;5640, `tgKodParowania`&nbsp;5680, `tgZnajdzCzat`&nbsp;5696, `tgPolacz`&nbsp;5764, `tgProbna`&nbsp;5797, `tgOdlacz`&nbsp;5804, `renderTgStan`&nbsp;5826

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;5941, `pobierzOpisFirmware`&nbsp;5947, `wyslijAktualizacje`&nbsp;5968, `anulujAktualizacje`&nbsp;6013, `renderOta`&nbsp;6019, `renderNetStan`&nbsp;6304

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6389, `renderSkan`&nbsp;6395, `szukajSieci`&nbsp;6447, `wybierzSiec`&nbsp;6455, `wyslijPolecenieSieci`&nbsp;6473, `siecZIndeksu`&nbsp;6483, `tzChanged`&nbsp;6517, `cfgTime`&nbsp;6522, `addSlot`&nbsp;6530, `zapiszPlanDnia`&nbsp;6543, `saveConfig`&nbsp;6561, `inrKrokiZakresu`&nbsp;6631, `opcjeInr`&nbsp;6638, `inrZakresZmieniony`&nbsp;6649, `wypelnijListyZakresu`&nbsp;6662, `saveInrRange`&nbsp;6673, `wypelnijListeOdstepu`&nbsp;6707, `saveInrEvery`&nbsp;6720, `odswiezPodpowiedzInr`&nbsp;6730

*ANALIZA* — `openTimeOf`&nbsp;6755, `openMinutes`&nbsp;6761, `sredniaPora`&nbsp;6785, `kwantyl`&nbsp;6793, `dniMiedzy`&nbsp;6801, `odstepyZPunktow`&nbsp;6815, `analyze`&nbsp;6824, `inrContext`&nbsp;6926

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;6985, `rytmSVG`&nbsp;6997, `poryWCzasieSVG`&nbsp;7059, `iskraSVG`&nbsp;7127, `dowSVG`&nbsp;7155, `dniRytmu`&nbsp;7191, `skutecznoscTygodniami`&nbsp;7212, `renderAnalysis`&nbsp;7240

*RAPORT* — `collectRows`&nbsp;7347, `makeReport`&nbsp;7384

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7535, `tagiPrzed`&nbsp;7543, `tagPrzelacz`&nbsp;7552

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7595, `opisKopii`&nbsp;7605

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7645, `tgCzatKopii`&nbsp;7652, `odswiezKopie`&nbsp;7659, `tgKopiaCzatZapisz`&nbsp;7667, `tgKopiaCzatZnajdz`&nbsp;7687, `tgKopiaWlacz`&nbsp;7717, `tgKopiaWylacz`&nbsp;7736, `kopiaNaTelegram`&nbsp;7745, `kopiaAutomat`&nbsp;7796

*WIEK KOPII* — `dniOdDaty`&nbsp;7843, `wiekKopiiTxt`&nbsp;7849, `renderKopiaStan`&nbsp;7857, `zapiszKopie`&nbsp;7872

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;7906, `wczytajKopie`&nbsp;7936, `kopiaCzytelna`&nbsp;7941, `odtworzKopie`&nbsp;7951, `kopiaWybrana`&nbsp;7974

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;7998, `odtworzZBazy`&nbsp;8030, `exportCsv`&nbsp;8042

*NAWIGACJA* — `wrocZEkranu`&nbsp;8143


---

## `firmware/PillBox/PillBox.ino` — 6818 linii

| od | do | blok |
|---|---|---|
| 1 | 64 | PillBox.ino  -  Inteligentne pudelko na leki / IoT Pill Reminder |
| 65 | 309 | PAMIEC RTC  (przezywa deep sleep, ginie po odlaczeniu zasilania) |
| 310 | 572 | STAN GLOBALNY |
| 573 | 793 | 1.  POMIAR BATERII |
| 794 | 956 | 2.  BUZZER  (pasywny piezo -> PWM przez LEDC) |
| 957 | 1144 | 3.  GPIO / WYBUDZANIE |
| 1145 | 1259 | 4.  HARMONOGRAM |
| 1260 | 1385 | 4a.  DNI BEZ LEKU |
| 1386 | 1407 | 4b.  PUDELKO ZOSTAWIONE OTWARTE |
| 1408 | 1540 | 4c.  DZIENNIK WIECZKA - USUNIETY (D109) |
| 1541 | 1792 | 5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien) |
| 1793 | 2278 | 6.  WiFi |
| 2279 | 3359 | 7.  FIREBASE  (REST: Auth email/haslo + Realtime Database) |
| 3360 | 3744 | 8.  ZDARZENIA |
| 3745 | 3800 | 9.  ALARM |
| 3801 | 4019 | 10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth) |
| 4020 | 4511 | 10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59 |
| 4512 | 4631 | 10b. CZARNA SKRZYNKA |
| 4632 | 5028 | 10c. GESTY SERWISOWE I AUTOTEST |
| 5029 | 5451 | 10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67) |
| 5452 | 6049 | 11.  DEEP SLEEP |
| 6050 | 6818 | 12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany) |

**Funkcje** (183):

*STAN GLOBALNY* — `zanotujNvsFail`&nbsp;388, `nvsPutStr`&nbsp;407, `nvsPutU16`&nbsp;430, `nvsPutU32`&nbsp;453, `nvsPutI16`&nbsp;479, `nvsPutU8`&nbsp;486, `nvsWolneWpisy`&nbsp;501, `syncTimeNTP`&nbsp;544, `logbookJson`&nbsp;545, `setTakenDay`&nbsp;550, `note`&nbsp;552, `awakeTooLong`&nbsp;566, `extendAwake`&nbsp;568

*1.  POMIAR BATERII* — `readBatteryRaw`&nbsp;581, `battPercentFromCurve`&nbsp;621, `resetBatteryFilter`&nbsp;653, `zapiszKoniecLadowania`&nbsp;676, `trackCharging`&nbsp;686, `battSmooth`&nbsp;740, `readBattery`&nbsp;771

*2.  BUZZER  (pasywny piezo -> PWM przez LEDC)* — `buzzerInit`&nbsp;797, `buzzerTone`&nbsp;806, `buzzerTonCicho`&nbsp;817, `buzzerOff`&nbsp;826, `beepAck`&nbsp;838, `beepErr`&nbsp;862, `beepQueued`&nbsp;872, `beepAlreadyTaken`&nbsp;882, `beepNowaWersja`&nbsp;907, `beepLowStock`&nbsp;917, `beepLowBattery`&nbsp;926, `beepBoxOpen`&nbsp;942, `beepCharging`&nbsp;950

*3.  GPIO / WYBUDZANIE* — `configureInputs`&nbsp;960, `powodResetuOpis`&nbsp;982, `zanotujReset`&nbsp;1000, `reedPoziomStabilny`&nbsp;1045, `boxIsOpen`&nbsp;1069, `boxIsOpenPewnie`&nbsp;1088, `buttonPressed`&nbsp;1092, `wakeName`&nbsp;1094

*4.  HARMONOGRAM* — `godzinaPoprawna`&nbsp;1158, `parseSchedule`&nbsp;1167, `loadSchedule`&nbsp;1180, `saveSchedule`&nbsp;1203, `localMinutesOfDay`&nbsp;1214, `slotMinutes`&nbsp;1221, `localDayNumber`&nbsp;1230, `matchSlot`&nbsp;1238, `secondsToDayBoundary`&nbsp;1253

*4a.  DNI BEZ LEKU* — `localWeekday`&nbsp;1280, `dateKeyToNum`&nbsp;1288, `dawkaNaDobe`&nbsp;1301, `dzisBezLeku`&nbsp;1311, `parseDoseWeek`&nbsp;1320, `parseDoseEx`&nbsp;1338, `saveDosing`&nbsp;1360, `loadDosing`&nbsp;1373

*4b.  PUDELKO ZOSTAWIONE OTWARTE* — `openWarnSecondsLeft`&nbsp;1397

*4c.  DZIENNIK WIECZKA - USUNIETY (D109)* — `jsonEscape`&nbsp;1431, `nvsFailLogDoWyslania`&nbsp;1447, `nvsFailLogJson`&nbsp;1457, `nvsFailLogOznaczWyslany`&nbsp;1475, `trackBoxOpen`&nbsp;1479, `secondsToNextSlot`&nbsp;1526

*5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien)* — `rekordTs`&nbsp;1557, `rekordBezDaty`&nbsp;1564, `tsDoBazy`&nbsp;1573, `queuePush`&nbsp;1577, `queueCount`&nbsp;1600, `queuePeek`&nbsp;1607, `queuePop`&nbsp;1622, `queueDrop`&nbsp;1641, `przesunZnaczniki`&nbsp;1665, `queueShiftTimestamps`&nbsp;1680, `queueNadajCzas`&nbsp;1728, `queueEpokaSkasuj`&nbsp;1771

*6.  WiFi* — `netKlucz`&nbsp;1810, `wifiSieciCount`&nbsp;1814, `wifiSiecSsid`&nbsp;1821, `wifiSiecPass`&nbsp;1830, `wifiListeZapisz`&nbsp;1857, `wifiListeCzytaj`&nbsp;1881, `wifiSiecDodaj`&nbsp;1894, `wifiSiecUsun`&nbsp;1925, `wifiSiecPriorytet`&nbsp;1958, `zapamietajAp`&nbsp;1988, `apPodpowiedzPasuje`&nbsp;1999, `wifiBeginZPodpowiedzia`&nbsp;2007, `wifiCzekajNaLacze`&nbsp;2040, `wifiSprobuj`&nbsp;2053, `wifiConnect`&nbsp;2089, `wifiOff`&nbsp;2210, `wifiUspij`&nbsp;2224, `syncTimeNTP`&nbsp;2229

*7.  FIREBASE  (REST: Auth email/haslo + Realtime Database)* — `tokenZPamieci`&nbsp;2296, `zapomnijToken`&nbsp;2305, `hasloJestPrawdziwe`&nbsp;2350, `hasloZPamieci`&nbsp;2355, `hasloWPamieci`&nbsp;2364, `hasloUtrwal`&nbsp;2368, `hasloDoLogowania`&nbsp;2381, `tgTokenZPamieci`&nbsp;2401, `tgChatZPamieci`&nbsp;2408, `tgSkonfigurowany`&nbsp;2417, `tgUtrwal`&nbsp;2424, `tgZapomnij`&nbsp;2436, `firebaseSignIn`&nbsp;2470, `rtdbUrl`&nbsp;2565, `rtdbSend`&nbsp;2587, `rekordKompletny`&nbsp;2614, `pushEventRecord`&nbsp;2623, `pushLidState`&nbsp;2680, `otaSumaZPamieci`&nbsp;2731, `otaSumaWgranej`&nbsp;2753, `pushStatus`&nbsp;2776, `fetchConfig`&nbsp;2993, `trwaleOdrzucony`&nbsp;3317, `flushQueue`&nbsp;3321

*8.  ZDARZENIA* — `makeRecordAt`&nbsp;3363, `makeRecord`&nbsp;3385, `loadDayMarkers`&nbsp;3395, `clearDayMarkers`&nbsp;3414, `setTakenDay`&nbsp;3428, `setRolloverDay`&nbsp;3436, `zapiszDawke`&nbsp;3466, `oznaczAlarmObsluzony`&nbsp;3509, `alarmJuzObsluzony`&nbsp;3542, `ostatniSlotDoby`&nbsp;3568, `juzDzisBrane`&nbsp;3578, `checkDayRollover`&nbsp;3585, `reportEvent`&nbsp;3667

*9.  ALARM* — `alarmPotwierdzony`&nbsp;3768, `runAlarmWindow`&nbsp;3773

*10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth)* — `htmlEscape`&nbsp;3814, `portalPage`&nbsp;3828, `startWifiPortal`&nbsp;3872

*10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59* — `otaOpisDecyzji`&nbsp;4144, `otaZanotujProbe`&nbsp;4170, `otaWyzerujLicznik`&nbsp;4178, `otaZlecenieWBazie`&nbsp;4208, `otaPobierzOpis`&nbsp;4223, `otaWgraj`&nbsp;4267, `otaSprawdzPoStarcie`&nbsp;4425, `otaPotwierdzDzialanie`&nbsp;4458

*10b. CZARNA SKRZYNKA* — `note`&nbsp;4532, `wartoZapisac`&nbsp;4539, `logbookAdd`&nbsp;4551, `logbookPrint`&nbsp;4590, `logbookJson`&nbsp;4614

*10c. GESTY SERWISOWE I AUTOTEST* — `netSkadOpis`&nbsp;4657, `lidMeldunek`&nbsp;4665, `dozorKrok`&nbsp;4692, `pikNumer`&nbsp;4858, `pikKoniecTestu`&nbsp;4870, `pikBrakSieci`&nbsp;4881, `wynikEtapu`&nbsp;4893, `etapTestu`&nbsp;4912, `autoTest`&nbsp;4917

*10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67)* — `tgWyslijTekst`&nbsp;5064, `tgZglosNieodebrane`&nbsp;5110, `tgSprawdzBaterie`&nbsp;5130, `tgSprawdzZapas`&nbsp;5149, `dniOdEry`&nbsp;5172, `dniDoDaty`&nbsp;5183, `inrPrzypomnienieTeraz`&nbsp;5217, `tgOznaczInrMiniete`&nbsp;5237, `sekundyDoInrPrzypomnienia`&nbsp;5246, `tgSprawdzInr`&nbsp;5268, `tgTekstZapas`&nbsp;5282, `tgTekstInr`&nbsp;5291, `tgTekstNieodebrane`&nbsp;5311, `tgTekstBateria`&nbsp;5320, `tgWyslijZalegle`&nbsp;5339

*11.  DEEP SLEEP* — `otaZglos`&nbsp;5470, `skanujSieci`&nbsp;5498, `otaSprobuj`&nbsp;5547, `kolejnePrzesuniecie`&nbsp;5746, `goToSleep`&nbsp;5751, `planNextSleep`&nbsp;5969

*12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany)* — `petlaLadowania`&nbsp;6062, `setup`&nbsp;6160, `loop`&nbsp;6815


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

## `firmware/PillBox/config.h` — 633 linii

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
| 368 | /* --------------------------------------------------------------------- |
| 378 | /* --------------------------------------------------------------------- |
| 397 | /* --------------------------------------------------------------------- |
| 426 | /* --------------------------------------------------------------------- |
| 473 | /* --------------------------------------------------------------------- |
| 489 | /* --------------------------------------------------------------------- |
| 560 | /* --------------------------------------------------------------------- |
| 602 | /* --------------------------------------------------------------------- |
| 609 | /* --------------------------------------------------------------------- |
| 615 | /* --------------------------------------------------------------------- |
