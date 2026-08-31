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

## `firmware/PillBox/PillBox.ino` — 6749 linii

| od | do | blok |
|---|---|---|
| 1 | 64 | PillBox.ino  -  Inteligentne pudelko na leki / IoT Pill Reminder |
| 65 | 309 | PAMIEC RTC  (przezywa deep sleep, ginie po odlaczeniu zasilania) |
| 310 | 568 | STAN GLOBALNY |
| 569 | 789 | 1.  POMIAR BATERII |
| 790 | 952 | 2.  BUZZER  (pasywny piezo -> PWM przez LEDC) |
| 953 | 1120 | 3.  GPIO / WYBUDZANIE |
| 1121 | 1235 | 4.  HARMONOGRAM |
| 1236 | 1361 | 4a.  DNI BEZ LEKU |
| 1362 | 1383 | 4b.  PUDELKO ZOSTAWIONE OTWARTE |
| 1384 | 1516 | 4c.  DZIENNIK WIECZKA - USUNIETY (D109) |
| 1517 | 1768 | 5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien) |
| 1769 | 2254 | 6.  WiFi |
| 2255 | 3318 | 7.  FIREBASE  (REST: Auth email/haslo + Realtime Database) |
| 3319 | 3699 | 8.  ZDARZENIA |
| 3700 | 3755 | 9.  ALARM |
| 3756 | 3974 | 10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth) |
| 3975 | 4466 | 10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59 |
| 4467 | 4586 | 10b. CZARNA SKRZYNKA |
| 4587 | 4983 | 10c. GESTY SERWISOWE I AUTOTEST |
| 4984 | 5406 | 10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67) |
| 5407 | 6004 | 11.  DEEP SLEEP |
| 6005 | 6749 | 12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany) |

**Funkcje** (182):

*STAN GLOBALNY* — `zanotujNvsFail`&nbsp;388, `nvsPutStr`&nbsp;407, `nvsPutU16`&nbsp;430, `nvsPutU32`&nbsp;453, `nvsPutI16`&nbsp;479, `nvsPutU8`&nbsp;486, `nvsWolneWpisy`&nbsp;501, `syncTimeNTP`&nbsp;544, `logbookJson`&nbsp;545, `setTakenDay`&nbsp;546, `note`&nbsp;548, `awakeTooLong`&nbsp;562, `extendAwake`&nbsp;564

*1.  POMIAR BATERII* — `readBatteryRaw`&nbsp;577, `battPercentFromCurve`&nbsp;617, `resetBatteryFilter`&nbsp;649, `zapiszKoniecLadowania`&nbsp;672, `trackCharging`&nbsp;682, `battSmooth`&nbsp;736, `readBattery`&nbsp;767

*2.  BUZZER  (pasywny piezo -> PWM przez LEDC)* — `buzzerInit`&nbsp;793, `buzzerTone`&nbsp;802, `buzzerTonCicho`&nbsp;813, `buzzerOff`&nbsp;822, `beepAck`&nbsp;834, `beepErr`&nbsp;858, `beepQueued`&nbsp;868, `beepAlreadyTaken`&nbsp;878, `beepNowaWersja`&nbsp;903, `beepLowStock`&nbsp;913, `beepLowBattery`&nbsp;922, `beepBoxOpen`&nbsp;938, `beepCharging`&nbsp;946

*3.  GPIO / WYBUDZANIE* — `configureInputs`&nbsp;956, `powodResetuOpis`&nbsp;978, `zanotujReset`&nbsp;996, `reedPoziomStabilny`&nbsp;1041, `boxIsOpen`&nbsp;1065, `buttonPressed`&nbsp;1068, `wakeName`&nbsp;1070

*4.  HARMONOGRAM* — `godzinaPoprawna`&nbsp;1134, `parseSchedule`&nbsp;1143, `loadSchedule`&nbsp;1156, `saveSchedule`&nbsp;1179, `localMinutesOfDay`&nbsp;1190, `slotMinutes`&nbsp;1197, `localDayNumber`&nbsp;1206, `matchSlot`&nbsp;1214, `secondsToDayBoundary`&nbsp;1229

*4a.  DNI BEZ LEKU* — `localWeekday`&nbsp;1256, `dateKeyToNum`&nbsp;1264, `dawkaNaDobe`&nbsp;1277, `dzisBezLeku`&nbsp;1287, `parseDoseWeek`&nbsp;1296, `parseDoseEx`&nbsp;1314, `saveDosing`&nbsp;1336, `loadDosing`&nbsp;1349

*4b.  PUDELKO ZOSTAWIONE OTWARTE* — `openWarnSecondsLeft`&nbsp;1373

*4c.  DZIENNIK WIECZKA - USUNIETY (D109)* — `jsonEscape`&nbsp;1407, `nvsFailLogDoWyslania`&nbsp;1423, `nvsFailLogJson`&nbsp;1433, `nvsFailLogOznaczWyslany`&nbsp;1451, `trackBoxOpen`&nbsp;1455, `secondsToNextSlot`&nbsp;1502

*5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien)* — `rekordTs`&nbsp;1533, `rekordBezDaty`&nbsp;1540, `tsDoBazy`&nbsp;1549, `queuePush`&nbsp;1553, `queueCount`&nbsp;1576, `queuePeek`&nbsp;1583, `queuePop`&nbsp;1598, `queueDrop`&nbsp;1617, `przesunZnaczniki`&nbsp;1641, `queueShiftTimestamps`&nbsp;1656, `queueNadajCzas`&nbsp;1704, `queueEpokaSkasuj`&nbsp;1747

*6.  WiFi* — `netKlucz`&nbsp;1786, `wifiSieciCount`&nbsp;1790, `wifiSiecSsid`&nbsp;1797, `wifiSiecPass`&nbsp;1806, `wifiListeZapisz`&nbsp;1833, `wifiListeCzytaj`&nbsp;1857, `wifiSiecDodaj`&nbsp;1870, `wifiSiecUsun`&nbsp;1901, `wifiSiecPriorytet`&nbsp;1934, `zapamietajAp`&nbsp;1964, `apPodpowiedzPasuje`&nbsp;1975, `wifiBeginZPodpowiedzia`&nbsp;1983, `wifiCzekajNaLacze`&nbsp;2016, `wifiSprobuj`&nbsp;2029, `wifiConnect`&nbsp;2065, `wifiOff`&nbsp;2186, `wifiUspij`&nbsp;2200, `syncTimeNTP`&nbsp;2205

*7.  FIREBASE  (REST: Auth email/haslo + Realtime Database)* — `tokenZPamieci`&nbsp;2272, `zapomnijToken`&nbsp;2281, `hasloJestPrawdziwe`&nbsp;2326, `hasloZPamieci`&nbsp;2331, `hasloWPamieci`&nbsp;2340, `hasloUtrwal`&nbsp;2344, `hasloDoLogowania`&nbsp;2357, `tgTokenZPamieci`&nbsp;2377, `tgChatZPamieci`&nbsp;2384, `tgSkonfigurowany`&nbsp;2393, `tgUtrwal`&nbsp;2400, `tgZapomnij`&nbsp;2412, `firebaseSignIn`&nbsp;2446, `rtdbUrl`&nbsp;2541, `rtdbSend`&nbsp;2563, `rekordKompletny`&nbsp;2590, `pushEventRecord`&nbsp;2599, `pushLidState`&nbsp;2656, `otaSumaZPamieci`&nbsp;2707, `otaSumaWgranej`&nbsp;2729, `pushStatus`&nbsp;2735, `fetchConfig`&nbsp;2952, `trwaleOdrzucony`&nbsp;3276, `flushQueue`&nbsp;3280

*8.  ZDARZENIA* — `makeRecordAt`&nbsp;3322, `makeRecord`&nbsp;3344, `loadDayMarkers`&nbsp;3354, `clearDayMarkers`&nbsp;3373, `setTakenDay`&nbsp;3387, `setRolloverDay`&nbsp;3395, `zapiszDawke`&nbsp;3425, `oznaczAlarmObsluzony`&nbsp;3468, `alarmJuzObsluzony`&nbsp;3501, `ostatniSlotDoby`&nbsp;3527, `juzDzisBrane`&nbsp;3537, `checkDayRollover`&nbsp;3544, `reportEvent`&nbsp;3626

*9.  ALARM* — `alarmPotwierdzony`&nbsp;3723, `runAlarmWindow`&nbsp;3728

*10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth)* — `htmlEscape`&nbsp;3769, `portalPage`&nbsp;3783, `startWifiPortal`&nbsp;3827

*10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59* — `otaOpisDecyzji`&nbsp;4099, `otaZanotujProbe`&nbsp;4125, `otaWyzerujLicznik`&nbsp;4133, `otaZlecenieWBazie`&nbsp;4163, `otaPobierzOpis`&nbsp;4178, `otaWgraj`&nbsp;4222, `otaSprawdzPoStarcie`&nbsp;4380, `otaPotwierdzDzialanie`&nbsp;4413

*10b. CZARNA SKRZYNKA* — `note`&nbsp;4487, `wartoZapisac`&nbsp;4494, `logbookAdd`&nbsp;4506, `logbookPrint`&nbsp;4545, `logbookJson`&nbsp;4569

*10c. GESTY SERWISOWE I AUTOTEST* — `netSkadOpis`&nbsp;4612, `lidMeldunek`&nbsp;4620, `dozorKrok`&nbsp;4647, `pikNumer`&nbsp;4813, `pikKoniecTestu`&nbsp;4825, `pikBrakSieci`&nbsp;4836, `wynikEtapu`&nbsp;4848, `etapTestu`&nbsp;4867, `autoTest`&nbsp;4872

*10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67)* — `tgWyslijTekst`&nbsp;5019, `tgZglosNieodebrane`&nbsp;5065, `tgSprawdzBaterie`&nbsp;5085, `tgSprawdzZapas`&nbsp;5104, `dniOdEry`&nbsp;5127, `dniDoDaty`&nbsp;5138, `inrPrzypomnienieTeraz`&nbsp;5172, `tgOznaczInrMiniete`&nbsp;5192, `sekundyDoInrPrzypomnienia`&nbsp;5201, `tgSprawdzInr`&nbsp;5223, `tgTekstZapas`&nbsp;5237, `tgTekstInr`&nbsp;5246, `tgTekstNieodebrane`&nbsp;5266, `tgTekstBateria`&nbsp;5275, `tgWyslijZalegle`&nbsp;5294

*11.  DEEP SLEEP* — `otaZglos`&nbsp;5425, `skanujSieci`&nbsp;5453, `otaSprobuj`&nbsp;5502, `kolejnePrzesuniecie`&nbsp;5701, `goToSleep`&nbsp;5706, `planNextSleep`&nbsp;5924

*12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany)* — `petlaLadowania`&nbsp;6017, `setup`&nbsp;6115, `loop`&nbsp;6746


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
