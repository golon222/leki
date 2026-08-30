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

## `index.html` — 8291 linii, ~130 tys. tokenow

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
| 2391 | 2763 | START |
| 2764 | 2824 | OSŁONA RYSOWANIA |
| 2825 | 2883 | REKONCYLIACJA |
| 2884 | 3088 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3089 | 3147 | KALENDARZ |
| 3148 | 3414 | HISTORIA ROZPISANIA DAWKI |
| 3415 | 3596 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3597 | 3748 | ARKUSZ DNIA |
| 3749 | 3848 | WZIĄŁEM TERAZ |
| 3849 | 3963 | INR |
| 3964 | 4103 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4104 | 4116 | STATUS PUDEŁKA |
| 4117 | 4267 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4268 | 4372 | DZIENNIK WIECZKA — narzędzie na czas testu terenowego. |
| 4373 | 4534 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4535 | 4791 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4792 | 4832 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4833 | 4929 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 4930 | 5308 | EKRAN ZDARZEN |
| 5309 | 5427 | ZAPAS TABLETEK |
| 5428 | 5715 | USTAWIENIA |
| 5716 | 6023 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6024 | 6480 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6481 | 6851 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 6852 | 7062 | ANALIZA |
| 7063 | 7448 | WYKRESY ANALIZY |
| 7449 | 7605 | RAPORT |
| 7606 | 7667 | KONTEKST DNIA (TAGI) |
| 7668 | 7713 | KOPIA ZAPASOWA |
| 7714 | 7927 | KOPIA NA TELEGRAM |
| 7928 | 7993 | WIEK KOPII |
| 7994 | 8091 | ODTWARZANIE Z KOPII |
| 8092 | 8166 | KOPIE Z BAZY |
| 8167 | 8249 | NAWIGACJA |
| 8250 | 8291 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (211) — nazwa i linia deklaracji:

*INFORMACJA ZWROTNA* — `toast`&nbsp;1992, `busy`&nbsp;2008, `todayKey`&nbsp;2035, `dzisiajKey`&nbsp;2039, `inNightWindow`&nbsp;2042

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2097, `tzName`&nbsp;2113, `tzLabel`&nbsp;2114, `tzOffsetTxt`&nbsp;2115, `devDate`&nbsp;2121, `devKey`&nbsp;2126, `devHM`&nbsp;2131, `slotMin`&nbsp;2132, `pillColors`&nbsp;2134

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2145

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2182, `cieniuj`&nbsp;2209, `doseGraphic`&nbsp;2226

*LOGOWANIE* — `doLogin`&nbsp;2240

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2260, `wyczyscCache`&nbsp;2355, `fbSignOut`&nbsp;2374

*START* — `boot`&nbsp;2392

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;2791, `rysujWszystkie`&nbsp;2804, `renderAll`&nbsp;2808

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;2831, `reconcileDecyzja`&nbsp;2870

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;2905, `zapiszReconcile`&nbsp;2917, `doReconcile`&nbsp;2956, `doReconcileWewn`&nbsp;2966, `reconcile`&nbsp;3087

*KALENDARZ* — `tydzienDawek`&nbsp;3127

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3173, `dawkaNaDzien`&nbsp;3184, `dzienBezLeku`&nbsp;3205, `wyjatekNaDzien`&nbsp;3210, `opisDawkowania`&nbsp;3216, `dayDose`&nbsp;3226, `dzienZamkniety`&nbsp;3260, `trackingSince`&nbsp;3266, `beforeTracking`&nbsp;3267, `dayStatus`&nbsp;3269, `renderCalendar`&nbsp;3310, `seriaDni`&nbsp;3375, `doNastepnej`&nbsp;3393, `opisCzasu`&nbsp;3408

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3423, `trwanieTxt`&nbsp;3438, `kiedyDawkaTxt`&nbsp;3449, `odswiezOdDawki`&nbsp;3457, `startTikOdDawki`&nbsp;3471, `renderToday`&nbsp;3481

*ARKUSZ DNIA* — `closeSheet`&nbsp;3608, `renderSheet`&nbsp;3610, `resetDose`&nbsp;3688, `resetPlan`&nbsp;3695, `commitPlan`&nbsp;3700, `clearPlan`&nbsp;3716, `commitDose`&nbsp;3728

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3769, `askConfirm`&nbsp;3838

*INR* — `inrState`&nbsp;3850, `odswiezTerminInr`&nbsp;3863, `addInr`&nbsp;3872, `inrKeysOk`&nbsp;3958

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;3973, `inrTerminKey`&nbsp;3981, `inrDoTerminu`&nbsp;3993, `dniTxt`&nbsp;4002, `renderInr`&nbsp;4004, `inrChart`&nbsp;4076

*STATUS PUDEŁKA* — `relTime`&nbsp;4105, `devDayMon`&nbsp;4114

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4149, `renderBoxLog`&nbsp;4195, `logPrzelacz`&nbsp;4231, `renderNvsFailLog`&nbsp;4240

*DZIENNIK WIECZKA — narzędzie na czas testu terenowego.* — `lidPaczki`&nbsp;4289, `lidWpisy`&nbsp;4295, `renderLidLog`&nbsp;4308

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4390, `oczekWczytaj`&nbsp;4398, `oczekZapisz`&nbsp;4403, `oczekIle`&nbsp;4406, `zapiszPewnie`&nbsp;4416, `zapiszCfg`&nbsp;4454, `bazaOdmowila`&nbsp;4472, `oczekWyslij`&nbsp;4496, `oczekOdmowy`&nbsp;4533

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4549, `ostrzReguly`&nbsp;4571, `lm`&nbsp;4610, `ostrzMilczy`&nbsp;4616, `nvsMalo`&nbsp;4673, `opisNvsFailKey`&nbsp;4685, `stratyDotyczaLeku`&nbsp;4735, `ostrzStraty`&nbsp;4747, `stratyCicho`&nbsp;4781

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4813

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4843, `renderOstrzezenia`&nbsp;4858, `bezPokrycia`&nbsp;4868, `wierszZdarzenia`&nbsp;4874, `renderDiag`&nbsp;4891

*EKRAN ZDARZEN* — `evFiltr`&nbsp;4946, `evPasuje`&nbsp;4951, `renderEvents`&nbsp;4963, `renderOpenWarn`&nbsp;5003, `minutyDoPelna`&nbsp;5056, `opisLadowania`&nbsp;5068, `dni`&nbsp;5088, `opisLadowan`&nbsp;5091, `renderStatus`&nbsp;5108

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5312, `dayAfter`&nbsp;5315, `pillsBaseInfo`&nbsp;5326, `settlePills`&nbsp;5336, `dniZapasu`&nbsp;5376, `renderPills`&nbsp;5389, `savePills`&nbsp;5410, `setPills`&nbsp;5421

*USTAWIENIA* — `renderKafelki`&nbsp;5432, `renderSettings`&nbsp;5459, `tydzienZPol`&nbsp;5514, `renderWeekEditor`&nbsp;5526, `odswiezPodpowiedzTygodnia`&nbsp;5543, `tydzienZmieniony`&nbsp;5557, `rownajTydzien`&nbsp;5558, `renderPlanList`&nbsp;5606, `renderExceptions`&nbsp;5633, `wyslijSiec`&nbsp;5691

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5733, `tgZapytaj`&nbsp;5743, `tgKodParowania`&nbsp;5783, `tgZnajdzCzat`&nbsp;5799, `tgPolacz`&nbsp;5867, `tgProbna`&nbsp;5900, `tgOdlacz`&nbsp;5907, `renderTgStan`&nbsp;5929

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6044, `pobierzOpisFirmware`&nbsp;6050, `wyslijAktualizacje`&nbsp;6071, `anulujAktualizacje`&nbsp;6116, `renderOta`&nbsp;6122, `renderNetStan`&nbsp;6407

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6492, `renderSkan`&nbsp;6498, `szukajSieci`&nbsp;6550, `wybierzSiec`&nbsp;6558, `wyslijPolecenieSieci`&nbsp;6576, `siecZIndeksu`&nbsp;6586, `tzChanged`&nbsp;6620, `cfgTime`&nbsp;6625, `addSlot`&nbsp;6633, `zapiszPlanDnia`&nbsp;6646, `saveConfig`&nbsp;6664, `inrKrokiZakresu`&nbsp;6734, `opcjeInr`&nbsp;6741, `inrZakresZmieniony`&nbsp;6752, `wypelnijListyZakresu`&nbsp;6765, `saveInrRange`&nbsp;6776, `wypelnijListeOdstepu`&nbsp;6810, `saveInrEvery`&nbsp;6823, `odswiezPodpowiedzInr`&nbsp;6833

*ANALIZA* — `openTimeOf`&nbsp;6858, `openMinutes`&nbsp;6864, `sredniaPora`&nbsp;6888, `kwantyl`&nbsp;6896, `dniMiedzy`&nbsp;6904, `odstepyZPunktow`&nbsp;6918, `analyze`&nbsp;6927, `inrContext`&nbsp;7029

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7088, `rytmSVG`&nbsp;7100, `poryWCzasieSVG`&nbsp;7162, `iskraSVG`&nbsp;7230, `dowSVG`&nbsp;7258, `dniRytmu`&nbsp;7294, `skutecznoscTygodniami`&nbsp;7315, `renderAnalysis`&nbsp;7343

*RAPORT* — `collectRows`&nbsp;7450, `makeReport`&nbsp;7487

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7638, `tagiPrzed`&nbsp;7646, `tagPrzelacz`&nbsp;7655

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7698, `opisKopii`&nbsp;7708

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7748, `tgCzatKopii`&nbsp;7755, `odswiezKopie`&nbsp;7762, `tgKopiaCzatZapisz`&nbsp;7770, `tgKopiaCzatZnajdz`&nbsp;7790, `tgKopiaWlacz`&nbsp;7820, `tgKopiaWylacz`&nbsp;7839, `kopiaNaTelegram`&nbsp;7848, `kopiaAutomat`&nbsp;7899

*WIEK KOPII* — `dniOdDaty`&nbsp;7946, `wiekKopiiTxt`&nbsp;7952, `renderKopiaStan`&nbsp;7960, `zapiszKopie`&nbsp;7975

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8009, `wczytajKopie`&nbsp;8039, `kopiaCzytelna`&nbsp;8044, `odtworzKopie`&nbsp;8054, `kopiaWybrana`&nbsp;8077

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8101, `odtworzZBazy`&nbsp;8133, `exportCsv`&nbsp;8145

*NAWIGACJA* — `wrocZEkranu`&nbsp;8248


---

## `firmware/PillBox/PillBox.ino` — 6737 linii

| od | do | blok |
|---|---|---|
| 1 | 64 | PillBox.ino  -  Inteligentne pudelko na leki / IoT Pill Reminder |
| 65 | 252 | PAMIEC RTC  (przezywa deep sleep, ginie po odlaczeniu zasilania) |
| 253 | 511 | STAN GLOBALNY |
| 512 | 732 | 1.  POMIAR BATERII |
| 733 | 895 | 2.  BUZZER  (pasywny piezo -> PWM przez LEDC) |
| 896 | 1060 | 3.  GPIO / WYBUDZANIE |
| 1061 | 1175 | 4.  HARMONOGRAM |
| 1176 | 1301 | 4a.  DNI BEZ LEKU |
| 1302 | 1322 | 4b.  PUDELKO ZOSTAWIONE OTWARTE |
| 1323 | 1546 | 4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego |
| 1547 | 1798 | 5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien) |
| 1799 | 2283 | 6.  WiFi |
| 2284 | 3355 | 7.  FIREBASE  (REST: Auth email/haslo + Realtime Database) |
| 3356 | 3736 | 8.  ZDARZENIA |
| 3737 | 3792 | 9.  ALARM |
| 3793 | 4011 | 10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth) |
| 4012 | 4503 | 10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59 |
| 4504 | 4623 | 10b. CZARNA SKRZYNKA |
| 4624 | 4971 | 10c. GESTY SERWISOWE I AUTOTEST |
| 4972 | 5394 | 10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67) |
| 5395 | 5992 | 11.  DEEP SLEEP |
| 5993 | 6737 | 12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany) |

**Funkcje** (183):

*STAN GLOBALNY* — `zanotujNvsFail`&nbsp;331, `nvsPutStr`&nbsp;350, `nvsPutU16`&nbsp;373, `nvsPutU32`&nbsp;396, `nvsPutI16`&nbsp;422, `nvsPutU8`&nbsp;429, `nvsWolneWpisy`&nbsp;444, `syncTimeNTP`&nbsp;487, `logbookJson`&nbsp;488, `setTakenDay`&nbsp;489, `note`&nbsp;491, `awakeTooLong`&nbsp;505, `extendAwake`&nbsp;507

*1.  POMIAR BATERII* — `readBatteryRaw`&nbsp;520, `battPercentFromCurve`&nbsp;560, `resetBatteryFilter`&nbsp;592, `zapiszKoniecLadowania`&nbsp;615, `trackCharging`&nbsp;625, `battSmooth`&nbsp;679, `readBattery`&nbsp;710

*2.  BUZZER  (pasywny piezo -> PWM przez LEDC)* — `buzzerInit`&nbsp;736, `buzzerTone`&nbsp;745, `buzzerTonCicho`&nbsp;756, `buzzerOff`&nbsp;765, `beepAck`&nbsp;777, `beepErr`&nbsp;801, `beepQueued`&nbsp;811, `beepAlreadyTaken`&nbsp;821, `beepNowaWersja`&nbsp;846, `beepLowStock`&nbsp;856, `beepLowBattery`&nbsp;865, `beepBoxOpen`&nbsp;881, `beepCharging`&nbsp;889

*3.  GPIO / WYBUDZANIE* — `configureInputs`&nbsp;899, `powodResetuOpis`&nbsp;921, `zanotujReset`&nbsp;939, `reedPoziomStabilny`&nbsp;984, `boxIsOpen`&nbsp;1005, `buttonPressed`&nbsp;1008, `wakeName`&nbsp;1010

*4.  HARMONOGRAM* — `godzinaPoprawna`&nbsp;1074, `parseSchedule`&nbsp;1083, `loadSchedule`&nbsp;1096, `saveSchedule`&nbsp;1119, `localMinutesOfDay`&nbsp;1130, `slotMinutes`&nbsp;1137, `localDayNumber`&nbsp;1146, `matchSlot`&nbsp;1154, `secondsToDayBoundary`&nbsp;1169

*4a.  DNI BEZ LEKU* — `localWeekday`&nbsp;1196, `dateKeyToNum`&nbsp;1204, `dawkaNaDobe`&nbsp;1217, `dzisBezLeku`&nbsp;1227, `parseDoseWeek`&nbsp;1236, `parseDoseEx`&nbsp;1254, `saveDosing`&nbsp;1276, `loadDosing`&nbsp;1289

*4b.  PUDELKO ZOSTAWIONE OTWARTE* — `openWarnSecondsLeft`&nbsp;1313

*4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego* — `lidLogAdd`&nbsp;1356, `lidLogCount`&nbsp;1380, `jsonEscape`&nbsp;1390, `lidLogJson`&nbsp;1406, `lidLogClear`&nbsp;1436, `nvsFailLogDoWyslania`&nbsp;1451, `nvsFailLogJson`&nbsp;1461, `nvsFailLogOznaczWyslany`&nbsp;1479, `trackBoxOpen`&nbsp;1483, `secondsToNextSlot`&nbsp;1532

*5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien)* — `rekordTs`&nbsp;1563, `rekordBezDaty`&nbsp;1570, `tsDoBazy`&nbsp;1579, `queuePush`&nbsp;1583, `queueCount`&nbsp;1606, `queuePeek`&nbsp;1613, `queuePop`&nbsp;1628, `queueDrop`&nbsp;1647, `przesunZnaczniki`&nbsp;1671, `queueShiftTimestamps`&nbsp;1686, `queueNadajCzas`&nbsp;1734, `queueEpokaSkasuj`&nbsp;1777

*6.  WiFi* — `netKlucz`&nbsp;1816, `wifiSieciCount`&nbsp;1820, `wifiSiecSsid`&nbsp;1827, `wifiSiecPass`&nbsp;1836, `wifiListeZapisz`&nbsp;1863, `wifiListeCzytaj`&nbsp;1887, `wifiSiecDodaj`&nbsp;1900, `wifiSiecUsun`&nbsp;1931, `wifiSiecPriorytet`&nbsp;1964, `wifiSprobuj`&nbsp;1992, `wifiOdNowa`&nbsp;2036, `wifiZacznijProbe`&nbsp;2047, `wifiStart`&nbsp;2067, `wifiKrokLaczenia`&nbsp;2107, `wifiConnect`&nbsp;2146, `wifiOff`&nbsp;2215, `wifiUspij`&nbsp;2229, `syncTimeNTP`&nbsp;2234

*7.  FIREBASE  (REST: Auth email/haslo + Realtime Database)* — `tokenZPamieci`&nbsp;2301, `zapomnijToken`&nbsp;2310, `hasloJestPrawdziwe`&nbsp;2355, `hasloZPamieci`&nbsp;2360, `hasloWPamieci`&nbsp;2369, `hasloUtrwal`&nbsp;2373, `hasloDoLogowania`&nbsp;2386, `tgTokenZPamieci`&nbsp;2406, `tgChatZPamieci`&nbsp;2413, `tgSkonfigurowany`&nbsp;2422, `tgUtrwal`&nbsp;2429, `tgZapomnij`&nbsp;2441, `firebaseSignIn`&nbsp;2475, `rtdbUrl`&nbsp;2570, `rtdbSend`&nbsp;2592, `rekordKompletny`&nbsp;2619, `pushEventRecord`&nbsp;2628, `pushLidState`&nbsp;2685, `otaSumaZPamieci`&nbsp;2736, `otaSumaWgranej`&nbsp;2758, `pushStatus`&nbsp;2764, `fetchConfig`&nbsp;2989, `trwaleOdrzucony`&nbsp;3313, `flushQueue`&nbsp;3317

*8.  ZDARZENIA* — `makeRecordAt`&nbsp;3359, `makeRecord`&nbsp;3381, `loadDayMarkers`&nbsp;3391, `clearDayMarkers`&nbsp;3410, `setTakenDay`&nbsp;3424, `setRolloverDay`&nbsp;3432, `zapiszDawke`&nbsp;3462, `oznaczAlarmObsluzony`&nbsp;3505, `alarmJuzObsluzony`&nbsp;3538, `ostatniSlotDoby`&nbsp;3564, `juzDzisBrane`&nbsp;3574, `checkDayRollover`&nbsp;3581, `reportEvent`&nbsp;3663

*9.  ALARM* — `alarmPotwierdzony`&nbsp;3760, `runAlarmWindow`&nbsp;3765

*10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth)* — `htmlEscape`&nbsp;3806, `portalPage`&nbsp;3820, `startWifiPortal`&nbsp;3864

*10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59* — `otaOpisDecyzji`&nbsp;4136, `otaZanotujProbe`&nbsp;4162, `otaWyzerujLicznik`&nbsp;4170, `otaZlecenieWBazie`&nbsp;4200, `otaPobierzOpis`&nbsp;4215, `otaWgraj`&nbsp;4259, `otaSprawdzPoStarcie`&nbsp;4417, `otaPotwierdzDzialanie`&nbsp;4450

*10b. CZARNA SKRZYNKA* — `note`&nbsp;4524, `wartoZapisac`&nbsp;4531, `logbookAdd`&nbsp;4543, `logbookPrint`&nbsp;4582, `logbookJson`&nbsp;4606

*10c. GESTY SERWISOWE I AUTOTEST* — `pikNumer`&nbsp;4801, `pikKoniecTestu`&nbsp;4813, `pikBrakSieci`&nbsp;4824, `wynikEtapu`&nbsp;4836, `etapTestu`&nbsp;4855, `autoTest`&nbsp;4860

*10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67)* — `tgWyslijTekst`&nbsp;5007, `tgZglosNieodebrane`&nbsp;5053, `tgSprawdzBaterie`&nbsp;5073, `tgSprawdzZapas`&nbsp;5092, `dniOdEry`&nbsp;5115, `dniDoDaty`&nbsp;5126, `inrPrzypomnienieTeraz`&nbsp;5160, `tgOznaczInrMiniete`&nbsp;5180, `sekundyDoInrPrzypomnienia`&nbsp;5189, `tgSprawdzInr`&nbsp;5211, `tgTekstZapas`&nbsp;5225, `tgTekstInr`&nbsp;5234, `tgTekstNieodebrane`&nbsp;5254, `tgTekstBateria`&nbsp;5263, `tgWyslijZalegle`&nbsp;5282

*11.  DEEP SLEEP* — `otaZglos`&nbsp;5413, `skanujSieci`&nbsp;5441, `otaSprobuj`&nbsp;5490, `kolejnePrzesuniecie`&nbsp;5689, `goToSleep`&nbsp;5694, `planNextSleep`&nbsp;5912

*12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany)* — `petlaLadowania`&nbsp;6005, `setup`&nbsp;6103, `loop`&nbsp;6734


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

## `firmware/PillBox/config.h` — 620 linii

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
| 277 | /* --------------------------------------------------------------------- |
| 285 | /* --------------------------------------------------------------------- |
| 303 | /* --------------------------------------------------------------------- |
| 311 | /* --------------------------------------------------------------------- |
| 323 | /* --------------------------------------------------------------------- |
| 354 | /* --------------------------------------------------------------------- |
| 364 | /* --------------------------------------------------------------------- |
| 383 | /* --------------------------------------------------------------------- |
| 412 | /* --------------------------------------------------------------------- |
| 459 | /* --------------------------------------------------------------------- |
| 476 | /* --------------------------------------------------------------------- |
| 547 | /* --------------------------------------------------------------------- |
| 589 | /* --------------------------------------------------------------------- |
| 596 | /* --------------------------------------------------------------------- |
| 602 | /* --------------------------------------------------------------------- |
