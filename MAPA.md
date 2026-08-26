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

## `index.html` — 8036 linii, ~130 tys. tokenow

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
| 1390 | 1503 | tab-diag |
| 1504 | 1782 | tab-help |
| 1783 | 1804 | tab-ev |
| 1805 | 1821 | tab-hist |
| 1822 | 1905 | tab-lid |
| 1906 | 1914 | JS — poczatek |
| 1915 | 1970 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 1971 | 2045 | INFORMACJA ZWROTNA |
| 2046 | 2140 | STREFY CZASOWE |
| 2141 | 2168 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2169 | 2235 | TABLETKA JAKO BRYŁA |
| 2236 | 2250 | LOGOWANIE |
| 2251 | 2390 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2391 | 2749 | START |
| 2750 | 2810 | OSŁONA RYSOWANIA |
| 2811 | 3007 | REKONCYLIACJA |
| 3008 | 3066 | KALENDARZ |
| 3067 | 3333 | HISTORIA ROZPISANIA DAWKI |
| 3334 | 3515 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3516 | 3667 | ARKUSZ DNIA |
| 3668 | 3763 | WZIĄŁEM TERAZ |
| 3764 | 3863 | INR |
| 3864 | 4003 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4004 | 4016 | STATUS PUDEŁKA |
| 4017 | 4167 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4168 | 4272 | DZIENNIK WIECZKA — narzędzie na czas testu terenowego. |
| 4273 | 4442 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4443 | 4689 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4690 | 4730 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4731 | 4824 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 4825 | 5130 | EKRAN ZDARZEN |
| 5131 | 5249 | ZAPAS TABLETEK |
| 5250 | 5479 | USTAWIENIA |
| 5480 | 5787 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 5788 | 6244 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6245 | 6606 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 6607 | 6817 | ANALIZA |
| 6818 | 7203 | WYKRESY ANALIZY |
| 7204 | 7355 | RAPORT |
| 7356 | 7417 | KONTEKST DNIA (TAGI) |
| 7418 | 7463 | KOPIA ZAPASOWA |
| 7464 | 7677 | KOPIA NA TELEGRAM |
| 7678 | 7743 | WIEK KOPII |
| 7744 | 7841 | ODTWARZANIE Z KOPII |
| 7842 | 7911 | KOPIE Z BAZY |
| 7912 | 7994 | NAWIGACJA |
| 7995 | 8036 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (209) — nazwa i linia deklaracji:

*INFORMACJA ZWROTNA* — `toast`&nbsp;1992, `busy`&nbsp;2008, `todayKey`&nbsp;2035, `dzisiajKey`&nbsp;2039, `inNightWindow`&nbsp;2042

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2097, `tzName`&nbsp;2113, `tzLabel`&nbsp;2114, `tzOffsetTxt`&nbsp;2115, `devDate`&nbsp;2121, `devKey`&nbsp;2126, `devHM`&nbsp;2131, `slotMin`&nbsp;2132, `pillColors`&nbsp;2134

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2145

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2182, `cieniuj`&nbsp;2209, `doseGraphic`&nbsp;2226

*LOGOWANIE* — `doLogin`&nbsp;2240

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2260, `wyczyscCache`&nbsp;2355, `fbSignOut`&nbsp;2374

*START* — `boot`&nbsp;2392

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;2777, `rysujWszystkie`&nbsp;2790, `renderAll`&nbsp;2794

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;2817, `reconcileDecyzja`&nbsp;2856, `zapiszReconcile`&nbsp;2873, `doReconcile`&nbsp;2890, `reconcile`&nbsp;3006

*KALENDARZ* — `tydzienDawek`&nbsp;3046

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3092, `dawkaNaDzien`&nbsp;3103, `dzienBezLeku`&nbsp;3124, `wyjatekNaDzien`&nbsp;3129, `opisDawkowania`&nbsp;3135, `dayDose`&nbsp;3145, `dzienZamkniety`&nbsp;3179, `trackingSince`&nbsp;3185, `beforeTracking`&nbsp;3186, `dayStatus`&nbsp;3188, `renderCalendar`&nbsp;3229, `seriaDni`&nbsp;3294, `doNastepnej`&nbsp;3312, `opisCzasu`&nbsp;3327

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3342, `trwanieTxt`&nbsp;3357, `kiedyDawkaTxt`&nbsp;3368, `odswiezOdDawki`&nbsp;3376, `startTikOdDawki`&nbsp;3390, `renderToday`&nbsp;3400

*ARKUSZ DNIA* — `closeSheet`&nbsp;3527, `renderSheet`&nbsp;3529, `resetDose`&nbsp;3607, `resetPlan`&nbsp;3614, `commitPlan`&nbsp;3619, `clearPlan`&nbsp;3635, `commitDose`&nbsp;3647

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3688, `askConfirm`&nbsp;3753

*INR* — `inrState`&nbsp;3765, `odswiezTerminInr`&nbsp;3778, `addInr`&nbsp;3787, `inrKeysOk`&nbsp;3858

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;3873, `inrTerminKey`&nbsp;3881, `inrDoTerminu`&nbsp;3893, `dniTxt`&nbsp;3902, `renderInr`&nbsp;3904, `inrChart`&nbsp;3976

*STATUS PUDEŁKA* — `relTime`&nbsp;4005, `devDayMon`&nbsp;4014

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4049, `renderBoxLog`&nbsp;4095, `logPrzelacz`&nbsp;4131, `renderNvsFailLog`&nbsp;4140

*DZIENNIK WIECZKA — narzędzie na czas testu terenowego.* — `lidPaczki`&nbsp;4189, `lidWpisy`&nbsp;4195, `renderLidLog`&nbsp;4208

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4290, `oczekWczytaj`&nbsp;4298, `oczekZapisz`&nbsp;4303, `oczekIle`&nbsp;4306, `zapiszPewnie`&nbsp;4316, `zapiszCfg`&nbsp;4358, `bazaOdmowila`&nbsp;4376, `oczekWyslij`&nbsp;4400, `oczekOdmowy`&nbsp;4441

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4457, `ostrzReguly`&nbsp;4479, `lm`&nbsp;4518, `ostrzMilczy`&nbsp;4524, `nvsMalo`&nbsp;4571, `opisNvsFailKey`&nbsp;4583, `stratyDotyczaLeku`&nbsp;4633, `ostrzStraty`&nbsp;4645, `stratyCicho`&nbsp;4679

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4711

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4741, `renderOstrzezenia`&nbsp;4756, `bezPokrycia`&nbsp;4766, `wierszZdarzenia`&nbsp;4772, `renderDiag`&nbsp;4789

*EKRAN ZDARZEN* — `evFiltr`&nbsp;4841, `evPasuje`&nbsp;4846, `renderEvents`&nbsp;4858, `renderOpenWarn`&nbsp;4898, `minutyDoPelna`&nbsp;4951, `opisLadowania`&nbsp;4963, `dni`&nbsp;4983, `opisLadowan`&nbsp;4986, `renderStatus`&nbsp;5003

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5134, `dayAfter`&nbsp;5137, `pillsBaseInfo`&nbsp;5148, `settlePills`&nbsp;5158, `dniZapasu`&nbsp;5198, `renderPills`&nbsp;5211, `savePills`&nbsp;5232, `setPills`&nbsp;5243

*USTAWIENIA* — `renderKafelki`&nbsp;5254, `renderSettings`&nbsp;5281, `tydzienZPol`&nbsp;5325, `renderWeekEditor`&nbsp;5337, `odswiezPodpowiedzTygodnia`&nbsp;5354, `tydzienZmieniony`&nbsp;5368, `rownajTydzien`&nbsp;5369, `renderPlanList`&nbsp;5382, `renderExceptions`&nbsp;5409, `wyslijSiec`&nbsp;5455

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5497, `tgZapytaj`&nbsp;5507, `tgKodParowania`&nbsp;5547, `tgZnajdzCzat`&nbsp;5563, `tgPolacz`&nbsp;5631, `tgProbna`&nbsp;5664, `tgOdlacz`&nbsp;5671, `renderTgStan`&nbsp;5693

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;5808, `pobierzOpisFirmware`&nbsp;5814, `wyslijAktualizacje`&nbsp;5835, `anulujAktualizacje`&nbsp;5880, `renderOta`&nbsp;5886, `renderNetStan`&nbsp;6171

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6256, `renderSkan`&nbsp;6262, `szukajSieci`&nbsp;6314, `wybierzSiec`&nbsp;6322, `wyslijPolecenieSieci`&nbsp;6340, `siecZIndeksu`&nbsp;6350, `tzChanged`&nbsp;6384, `cfgTime`&nbsp;6389, `addSlot`&nbsp;6397, `zapiszPlanDnia`&nbsp;6410, `saveConfig`&nbsp;6428, `inrKrokiZakresu`&nbsp;6489, `opcjeInr`&nbsp;6496, `inrZakresZmieniony`&nbsp;6507, `wypelnijListyZakresu`&nbsp;6520, `saveInrRange`&nbsp;6531, `wypelnijListeOdstepu`&nbsp;6565, `saveInrEvery`&nbsp;6578, `odswiezPodpowiedzInr`&nbsp;6588

*ANALIZA* — `openTimeOf`&nbsp;6613, `openMinutes`&nbsp;6619, `sredniaPora`&nbsp;6643, `kwantyl`&nbsp;6651, `dniMiedzy`&nbsp;6659, `odstepyZPunktow`&nbsp;6673, `analyze`&nbsp;6682, `inrContext`&nbsp;6784

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;6843, `rytmSVG`&nbsp;6855, `poryWCzasieSVG`&nbsp;6917, `iskraSVG`&nbsp;6985, `dowSVG`&nbsp;7013, `dniRytmu`&nbsp;7049, `skutecznoscTygodniami`&nbsp;7070, `renderAnalysis`&nbsp;7098

*RAPORT* — `collectRows`&nbsp;7205, `makeReport`&nbsp;7242

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7388, `tagiPrzed`&nbsp;7396, `tagPrzelacz`&nbsp;7405

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7448, `opisKopii`&nbsp;7458

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7498, `tgCzatKopii`&nbsp;7505, `odswiezKopie`&nbsp;7512, `tgKopiaCzatZapisz`&nbsp;7520, `tgKopiaCzatZnajdz`&nbsp;7540, `tgKopiaWlacz`&nbsp;7570, `tgKopiaWylacz`&nbsp;7589, `kopiaNaTelegram`&nbsp;7598, `kopiaAutomat`&nbsp;7649

*WIEK KOPII* — `dniOdDaty`&nbsp;7696, `wiekKopiiTxt`&nbsp;7702, `renderKopiaStan`&nbsp;7710, `zapiszKopie`&nbsp;7725

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;7759, `wczytajKopie`&nbsp;7789, `kopiaCzytelna`&nbsp;7794, `odtworzKopie`&nbsp;7804, `kopiaWybrana`&nbsp;7827

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;7851, `odtworzZBazy`&nbsp;7883, `exportCsv`&nbsp;7895

*NAWIGACJA* — `wrocZEkranu`&nbsp;7993


---

## `firmware/PillBox/PillBox.ino` — 6259 linii

| od | do | blok |
|---|---|---|
| 1 | 63 | PillBox.ino  -  Inteligentne pudelko na leki / IoT Pill Reminder |
| 64 | 243 | PAMIEC RTC  (przezywa deep sleep, ginie po odlaczeniu zasilania) |
| 244 | 469 | STAN GLOBALNY |
| 470 | 690 | 1.  POMIAR BATERII |
| 691 | 853 | 2.  BUZZER  (pasywny piezo -> PWM przez LEDC) |
| 854 | 915 | 3.  GPIO / WYBUDZANIE |
| 916 | 1001 | 4.  HARMONOGRAM |
| 1002 | 1127 | 4a.  DNI BEZ LEKU |
| 1128 | 1148 | 4b.  PUDELKO ZOSTAWIONE OTWARTE |
| 1149 | 1372 | 4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego |
| 1373 | 1624 | 5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien) |
| 1625 | 1973 | 6.  WiFi |
| 1974 | 2976 | 7.  FIREBASE  (REST: Auth email/haslo + Realtime Database) |
| 2977 | 3341 | 8.  ZDARZENIA |
| 3342 | 3397 | 9.  ALARM |
| 3398 | 3616 | 10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth) |
| 3617 | 4108 | 10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59 |
| 4109 | 4228 | 10b. CZARNA SKRZYNKA |
| 4229 | 4531 | 10c. GESTY SERWISOWE I AUTOTEST |
| 4532 | 4954 | 10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67) |
| 4955 | 5525 | 11.  DEEP SLEEP |
| 5526 | 6259 | 12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany) |

**Funkcje** (173):

*STAN GLOBALNY* — `zanotujNvsFail`&nbsp;322, `nvsPutStr`&nbsp;341, `nvsPutU16`&nbsp;364, `nvsPutU32`&nbsp;387, `nvsWolneWpisy`&nbsp;402, `syncTimeNTP`&nbsp;445, `logbookJson`&nbsp;446, `setTakenDay`&nbsp;447, `note`&nbsp;449, `awakeTooLong`&nbsp;463, `extendAwake`&nbsp;465

*1.  POMIAR BATERII* — `readBatteryRaw`&nbsp;478, `battPercentFromCurve`&nbsp;518, `resetBatteryFilter`&nbsp;550, `zapiszKoniecLadowania`&nbsp;573, `trackCharging`&nbsp;583, `battSmooth`&nbsp;637, `readBattery`&nbsp;668

*2.  BUZZER  (pasywny piezo -> PWM przez LEDC)* — `buzzerInit`&nbsp;694, `buzzerTone`&nbsp;703, `buzzerTonCicho`&nbsp;714, `buzzerOff`&nbsp;723, `beepAck`&nbsp;735, `beepErr`&nbsp;759, `beepQueued`&nbsp;769, `beepAlreadyTaken`&nbsp;779, `beepNowaWersja`&nbsp;804, `beepLowStock`&nbsp;814, `beepLowBattery`&nbsp;823, `beepBoxOpen`&nbsp;839, `beepCharging`&nbsp;847

*3.  GPIO / WYBUDZANIE* — `configureInputs`&nbsp;857, `boxIsOpen`&nbsp;862, `buttonPressed`&nbsp;863, `wakeName`&nbsp;865

*4.  HARMONOGRAM* — `parseSchedule`&nbsp;919, `loadSchedule`&nbsp;932, `saveSchedule`&nbsp;945, `localMinutesOfDay`&nbsp;956, `slotMinutes`&nbsp;963, `localDayNumber`&nbsp;972, `matchSlot`&nbsp;980, `secondsToDayBoundary`&nbsp;995

*4a.  DNI BEZ LEKU* — `localWeekday`&nbsp;1022, `dateKeyToNum`&nbsp;1030, `dawkaNaDobe`&nbsp;1043, `dzisBezLeku`&nbsp;1053, `parseDoseWeek`&nbsp;1062, `parseDoseEx`&nbsp;1080, `saveDosing`&nbsp;1102, `loadDosing`&nbsp;1115

*4b.  PUDELKO ZOSTAWIONE OTWARTE* — `openWarnSecondsLeft`&nbsp;1139

*4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego* — `lidLogAdd`&nbsp;1182, `lidLogCount`&nbsp;1206, `jsonEscape`&nbsp;1216, `lidLogJson`&nbsp;1232, `lidLogClear`&nbsp;1262, `nvsFailLogDoWyslania`&nbsp;1277, `nvsFailLogJson`&nbsp;1287, `nvsFailLogOznaczWyslany`&nbsp;1305, `trackBoxOpen`&nbsp;1309, `secondsToNextSlot`&nbsp;1358

*5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien)* — `rekordTs`&nbsp;1389, `rekordBezDaty`&nbsp;1396, `tsDoBazy`&nbsp;1405, `queuePush`&nbsp;1409, `queueCount`&nbsp;1432, `queuePeek`&nbsp;1439, `queuePop`&nbsp;1454, `queueDrop`&nbsp;1473, `przesunZnaczniki`&nbsp;1497, `queueShiftTimestamps`&nbsp;1512, `queueNadajCzas`&nbsp;1560, `queueEpokaSkasuj`&nbsp;1603

*6.  WiFi* — `netKlucz`&nbsp;1642, `wifiSieciCount`&nbsp;1646, `wifiSiecSsid`&nbsp;1653, `wifiSiecPass`&nbsp;1662, `wifiListeZapisz`&nbsp;1689, `wifiListeCzytaj`&nbsp;1713, `wifiSiecDodaj`&nbsp;1726, `wifiSiecUsun`&nbsp;1757, `wifiSiecPriorytet`&nbsp;1790, `wifiSprobuj`&nbsp;1818, `wifiConnect`&nbsp;1836, `wifiOff`&nbsp;1905, `wifiUspij`&nbsp;1919, `syncTimeNTP`&nbsp;1924

*7.  FIREBASE  (REST: Auth email/haslo + Realtime Database)* — `tokenZPamieci`&nbsp;1991, `zapomnijToken`&nbsp;2000, `hasloJestPrawdziwe`&nbsp;2045, `hasloZPamieci`&nbsp;2050, `hasloWPamieci`&nbsp;2059, `hasloUtrwal`&nbsp;2063, `hasloDoLogowania`&nbsp;2076, `tgTokenZPamieci`&nbsp;2096, `tgChatZPamieci`&nbsp;2103, `tgSkonfigurowany`&nbsp;2112, `tgUtrwal`&nbsp;2119, `tgZapomnij`&nbsp;2131, `firebaseSignIn`&nbsp;2165, `rtdbUrl`&nbsp;2260, `rtdbSend`&nbsp;2282, `rekordKompletny`&nbsp;2309, `pushEventRecord`&nbsp;2318, `pushLidState`&nbsp;2375, `otaSumaZPamieci`&nbsp;2396, `otaSumaWgranej`&nbsp;2418, `pushStatus`&nbsp;2424, `fetchConfig`&nbsp;2619, `trwaleOdrzucony`&nbsp;2934, `flushQueue`&nbsp;2938

*8.  ZDARZENIA* — `makeRecordAt`&nbsp;2980, `makeRecord`&nbsp;3002, `loadDayMarkers`&nbsp;3012, `clearDayMarkers`&nbsp;3031, `setTakenDay`&nbsp;3045, `setRolloverDay`&nbsp;3053, `zapiszDawke`&nbsp;3083, `oznaczAlarmObsluzony`&nbsp;3126, `alarmJuzObsluzony`&nbsp;3143, `ostatniSlotDoby`&nbsp;3169, `juzDzisBrane`&nbsp;3179, `checkDayRollover`&nbsp;3186, `reportEvent`&nbsp;3268

*9.  ALARM* — `alarmPotwierdzony`&nbsp;3365, `runAlarmWindow`&nbsp;3370

*10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth)* — `htmlEscape`&nbsp;3411, `portalPage`&nbsp;3425, `startWifiPortal`&nbsp;3469

*10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59* — `otaOpisDecyzji`&nbsp;3741, `otaZanotujProbe`&nbsp;3767, `otaWyzerujLicznik`&nbsp;3775, `otaZlecenieWBazie`&nbsp;3805, `otaPobierzOpis`&nbsp;3820, `otaWgraj`&nbsp;3864, `otaSprawdzPoStarcie`&nbsp;4022, `otaPotwierdzDzialanie`&nbsp;4055

*10b. CZARNA SKRZYNKA* — `note`&nbsp;4129, `wartoZapisac`&nbsp;4136, `logbookAdd`&nbsp;4148, `logbookPrint`&nbsp;4187, `logbookJson`&nbsp;4211

*10c. GESTY SERWISOWE I AUTOTEST* — `pikNumer`&nbsp;4361, `pikKoniecTestu`&nbsp;4373, `pikBrakSieci`&nbsp;4384, `wynikEtapu`&nbsp;4396, `etapTestu`&nbsp;4415, `autoTest`&nbsp;4420

*10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67)* — `tgWyslijTekst`&nbsp;4567, `tgZglosNieodebrane`&nbsp;4613, `tgSprawdzBaterie`&nbsp;4633, `tgSprawdzZapas`&nbsp;4652, `dniOdEry`&nbsp;4675, `dniDoDaty`&nbsp;4686, `inrPrzypomnienieTeraz`&nbsp;4720, `tgOznaczInrMiniete`&nbsp;4740, `sekundyDoInrPrzypomnienia`&nbsp;4749, `tgSprawdzInr`&nbsp;4771, `tgTekstZapas`&nbsp;4785, `tgTekstInr`&nbsp;4794, `tgTekstNieodebrane`&nbsp;4814, `tgTekstBateria`&nbsp;4823, `tgWyslijZalegle`&nbsp;4842

*11.  DEEP SLEEP* — `otaZglos`&nbsp;4973, `skanujSieci`&nbsp;5001, `otaSprobuj`&nbsp;5050, `kolejnePrzesuniecie`&nbsp;5249, `goToSleep`&nbsp;5254, `planNextSleep`&nbsp;5450

*12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany)* — `petlaLadowania`&nbsp;5538, `setup`&nbsp;5636, `loop`&nbsp;6256


---

## `firmware/PillBoxTest/PillBoxTest.ino` — 681 linii

| od | do | blok |
|---|---|---|
| 1 | 622 | PillBoxTest.ino  -  program DIAGNOSTYCZNY inteligentnego pudelka |
| 623 | 681 | SETUP |

**Funkcje** (27):

*PillBoxTest.ino  -  program DIAGNOSTYCZNY inteligentnego pudelka* — `naglowek`&nbsp;72, `wynik`&nbsp;76, `wynikF`&nbsp;81, `uwagaF`&nbsp;86, `info`&nbsp;92, `adcMediana`&nbsp;99, `napiecieOgniwa`&nbsp;108, `testBaterii`&nbsp;119, `ton`&nbsp;165, `ciszaBuzzera`&nbsp;173, `testBuzzera`&nbsp;184, `pudelkoOtwarte`&nbsp;209, `przyciskWcisniety`&nbsp;210, `testKontaktronu`&nbsp;212, `testPrzycisku`&nbsp;252, `testPamieci`&nbsp;277, `testWifi`&nbsp;307, `testCzasu`&nbsp;356, `wytnijPole`&nbsp;408, `firebaseLogowanie`&nbsp;429, `rtdb`&nbsp;482, `testFirebase`&nbsp;496, `testSnuStart`&nbsp;535, `testSnuKoniec`&nbsp;572, `podsumowanie`&nbsp;607

*SETUP* — `setup`&nbsp;626, `loop`&nbsp;680


---

## `firmware/PillBox/config.h` — 585 linii

| linia | grupa |
|---|---|
| 19 | /* --------------------------------------------------------------------- |
| 25 | /* --------------------------------------------------------------------- |
| 52 | /* --------------------------------------------------------------------- |
| 68 | /* --------------------------------------------------------------------- |
| 109 | /* --------------------------------------------------------------------- |
| 120 | /* --------------------------------------------------------------------- |
| 127 | /* --------------------------------------------------------------------- |
| 161 | /* --------------------------------------------------------------------- |
| 177 | /* --------------------------------------------------------------------- |
| 196 | /* --------------------------------------------------------------------- |
| 201 | /* --------------------------------------------------------------------- |
| 211 | /* --------------------------------------------------------------------- |
| 242 | /* --------------------------------------------------------------------- |
| 250 | /* --------------------------------------------------------------------- |
| 268 | /* --------------------------------------------------------------------- |
| 276 | /* --------------------------------------------------------------------- |
| 288 | /* --------------------------------------------------------------------- |
| 319 | /* --------------------------------------------------------------------- |
| 329 | /* --------------------------------------------------------------------- |
| 348 | /* --------------------------------------------------------------------- |
| 377 | /* --------------------------------------------------------------------- |
| 424 | /* --------------------------------------------------------------------- |
| 441 | /* --------------------------------------------------------------------- |
| 512 | /* --------------------------------------------------------------------- |
| 554 | /* --------------------------------------------------------------------- |
| 561 | /* --------------------------------------------------------------------- |
| 567 | /* --------------------------------------------------------------------- |
