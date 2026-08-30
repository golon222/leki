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

## `index.html` — 8220 linii, ~130 tys. tokenow

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
| 3749 | 3844 | WZIĄŁEM TERAZ |
| 3845 | 3959 | INR |
| 3960 | 4099 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4100 | 4112 | STATUS PUDEŁKA |
| 4113 | 4263 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4264 | 4368 | DZIENNIK WIECZKA — narzędzie na czas testu terenowego. |
| 4369 | 4530 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4531 | 4787 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4788 | 4828 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4829 | 4925 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 4926 | 5304 | EKRAN ZDARZEN |
| 5305 | 5423 | ZAPAS TABLETEK |
| 5424 | 5653 | USTAWIENIA |
| 5654 | 5961 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 5962 | 6418 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6419 | 6780 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 6781 | 6991 | ANALIZA |
| 6992 | 7377 | WYKRESY ANALIZY |
| 7378 | 7534 | RAPORT |
| 7535 | 7596 | KONTEKST DNIA (TAGI) |
| 7597 | 7642 | KOPIA ZAPASOWA |
| 7643 | 7856 | KOPIA NA TELEGRAM |
| 7857 | 7922 | WIEK KOPII |
| 7923 | 8020 | ODTWARZANIE Z KOPII |
| 8021 | 8095 | KOPIE Z BAZY |
| 8096 | 8178 | NAWIGACJA |
| 8179 | 8220 | AUTOMATYCZNA AKTUALIZACJA |

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

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3769, `askConfirm`&nbsp;3834

*INR* — `inrState`&nbsp;3846, `odswiezTerminInr`&nbsp;3859, `addInr`&nbsp;3868, `inrKeysOk`&nbsp;3954

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;3969, `inrTerminKey`&nbsp;3977, `inrDoTerminu`&nbsp;3989, `dniTxt`&nbsp;3998, `renderInr`&nbsp;4000, `inrChart`&nbsp;4072

*STATUS PUDEŁKA* — `relTime`&nbsp;4101, `devDayMon`&nbsp;4110

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4145, `renderBoxLog`&nbsp;4191, `logPrzelacz`&nbsp;4227, `renderNvsFailLog`&nbsp;4236

*DZIENNIK WIECZKA — narzędzie na czas testu terenowego.* — `lidPaczki`&nbsp;4285, `lidWpisy`&nbsp;4291, `renderLidLog`&nbsp;4304

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4386, `oczekWczytaj`&nbsp;4394, `oczekZapisz`&nbsp;4399, `oczekIle`&nbsp;4402, `zapiszPewnie`&nbsp;4412, `zapiszCfg`&nbsp;4450, `bazaOdmowila`&nbsp;4468, `oczekWyslij`&nbsp;4492, `oczekOdmowy`&nbsp;4529

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4545, `ostrzReguly`&nbsp;4567, `lm`&nbsp;4606, `ostrzMilczy`&nbsp;4612, `nvsMalo`&nbsp;4669, `opisNvsFailKey`&nbsp;4681, `stratyDotyczaLeku`&nbsp;4731, `ostrzStraty`&nbsp;4743, `stratyCicho`&nbsp;4777

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4809

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4839, `renderOstrzezenia`&nbsp;4854, `bezPokrycia`&nbsp;4864, `wierszZdarzenia`&nbsp;4870, `renderDiag`&nbsp;4887

*EKRAN ZDARZEN* — `evFiltr`&nbsp;4942, `evPasuje`&nbsp;4947, `renderEvents`&nbsp;4959, `renderOpenWarn`&nbsp;4999, `minutyDoPelna`&nbsp;5052, `opisLadowania`&nbsp;5064, `dni`&nbsp;5084, `opisLadowan`&nbsp;5087, `renderStatus`&nbsp;5104

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5308, `dayAfter`&nbsp;5311, `pillsBaseInfo`&nbsp;5322, `settlePills`&nbsp;5332, `dniZapasu`&nbsp;5372, `renderPills`&nbsp;5385, `savePills`&nbsp;5406, `setPills`&nbsp;5417

*USTAWIENIA* — `renderKafelki`&nbsp;5428, `renderSettings`&nbsp;5455, `tydzienZPol`&nbsp;5499, `renderWeekEditor`&nbsp;5511, `odswiezPodpowiedzTygodnia`&nbsp;5528, `tydzienZmieniony`&nbsp;5542, `rownajTydzien`&nbsp;5543, `renderPlanList`&nbsp;5556, `renderExceptions`&nbsp;5583, `wyslijSiec`&nbsp;5629

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5671, `tgZapytaj`&nbsp;5681, `tgKodParowania`&nbsp;5721, `tgZnajdzCzat`&nbsp;5737, `tgPolacz`&nbsp;5805, `tgProbna`&nbsp;5838, `tgOdlacz`&nbsp;5845, `renderTgStan`&nbsp;5867

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;5982, `pobierzOpisFirmware`&nbsp;5988, `wyslijAktualizacje`&nbsp;6009, `anulujAktualizacje`&nbsp;6054, `renderOta`&nbsp;6060, `renderNetStan`&nbsp;6345

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6430, `renderSkan`&nbsp;6436, `szukajSieci`&nbsp;6488, `wybierzSiec`&nbsp;6496, `wyslijPolecenieSieci`&nbsp;6514, `siecZIndeksu`&nbsp;6524, `tzChanged`&nbsp;6558, `cfgTime`&nbsp;6563, `addSlot`&nbsp;6571, `zapiszPlanDnia`&nbsp;6584, `saveConfig`&nbsp;6602, `inrKrokiZakresu`&nbsp;6663, `opcjeInr`&nbsp;6670, `inrZakresZmieniony`&nbsp;6681, `wypelnijListyZakresu`&nbsp;6694, `saveInrRange`&nbsp;6705, `wypelnijListeOdstepu`&nbsp;6739, `saveInrEvery`&nbsp;6752, `odswiezPodpowiedzInr`&nbsp;6762

*ANALIZA* — `openTimeOf`&nbsp;6787, `openMinutes`&nbsp;6793, `sredniaPora`&nbsp;6817, `kwantyl`&nbsp;6825, `dniMiedzy`&nbsp;6833, `odstepyZPunktow`&nbsp;6847, `analyze`&nbsp;6856, `inrContext`&nbsp;6958

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7017, `rytmSVG`&nbsp;7029, `poryWCzasieSVG`&nbsp;7091, `iskraSVG`&nbsp;7159, `dowSVG`&nbsp;7187, `dniRytmu`&nbsp;7223, `skutecznoscTygodniami`&nbsp;7244, `renderAnalysis`&nbsp;7272

*RAPORT* — `collectRows`&nbsp;7379, `makeReport`&nbsp;7416

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7567, `tagiPrzed`&nbsp;7575, `tagPrzelacz`&nbsp;7584

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7627, `opisKopii`&nbsp;7637

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7677, `tgCzatKopii`&nbsp;7684, `odswiezKopie`&nbsp;7691, `tgKopiaCzatZapisz`&nbsp;7699, `tgKopiaCzatZnajdz`&nbsp;7719, `tgKopiaWlacz`&nbsp;7749, `tgKopiaWylacz`&nbsp;7768, `kopiaNaTelegram`&nbsp;7777, `kopiaAutomat`&nbsp;7828

*WIEK KOPII* — `dniOdDaty`&nbsp;7875, `wiekKopiiTxt`&nbsp;7881, `renderKopiaStan`&nbsp;7889, `zapiszKopie`&nbsp;7904

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;7938, `wczytajKopie`&nbsp;7968, `kopiaCzytelna`&nbsp;7973, `odtworzKopie`&nbsp;7983, `kopiaWybrana`&nbsp;8006

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8030, `odtworzZBazy`&nbsp;8062, `exportCsv`&nbsp;8074

*NAWIGACJA* — `wrocZEkranu`&nbsp;8177


---

## `firmware/PillBox/PillBox.ino` — 6704 linii

| od | do | blok |
|---|---|---|
| 1 | 64 | PillBox.ino  -  Inteligentne pudelko na leki / IoT Pill Reminder |
| 65 | 252 | PAMIEC RTC  (przezywa deep sleep, ginie po odlaczeniu zasilania) |
| 253 | 478 | STAN GLOBALNY |
| 479 | 699 | 1.  POMIAR BATERII |
| 700 | 862 | 2.  BUZZER  (pasywny piezo -> PWM przez LEDC) |
| 863 | 1027 | 3.  GPIO / WYBUDZANIE |
| 1028 | 1142 | 4.  HARMONOGRAM |
| 1143 | 1268 | 4a.  DNI BEZ LEKU |
| 1269 | 1289 | 4b.  PUDELKO ZOSTAWIONE OTWARTE |
| 1290 | 1513 | 4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego |
| 1514 | 1765 | 5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien) |
| 1766 | 2250 | 6.  WiFi |
| 2251 | 3322 | 7.  FIREBASE  (REST: Auth email/haslo + Realtime Database) |
| 3323 | 3703 | 8.  ZDARZENIA |
| 3704 | 3759 | 9.  ALARM |
| 3760 | 3978 | 10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth) |
| 3979 | 4470 | 10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59 |
| 4471 | 4590 | 10b. CZARNA SKRZYNKA |
| 4591 | 4938 | 10c. GESTY SERWISOWE I AUTOTEST |
| 4939 | 5361 | 10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67) |
| 5362 | 5959 | 11.  DEEP SLEEP |
| 5960 | 6704 | 12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany) |

**Funkcje** (181):

*STAN GLOBALNY* — `zanotujNvsFail`&nbsp;331, `nvsPutStr`&nbsp;350, `nvsPutU16`&nbsp;373, `nvsPutU32`&nbsp;396, `nvsWolneWpisy`&nbsp;411, `syncTimeNTP`&nbsp;454, `logbookJson`&nbsp;455, `setTakenDay`&nbsp;456, `note`&nbsp;458, `awakeTooLong`&nbsp;472, `extendAwake`&nbsp;474

*1.  POMIAR BATERII* — `readBatteryRaw`&nbsp;487, `battPercentFromCurve`&nbsp;527, `resetBatteryFilter`&nbsp;559, `zapiszKoniecLadowania`&nbsp;582, `trackCharging`&nbsp;592, `battSmooth`&nbsp;646, `readBattery`&nbsp;677

*2.  BUZZER  (pasywny piezo -> PWM przez LEDC)* — `buzzerInit`&nbsp;703, `buzzerTone`&nbsp;712, `buzzerTonCicho`&nbsp;723, `buzzerOff`&nbsp;732, `beepAck`&nbsp;744, `beepErr`&nbsp;768, `beepQueued`&nbsp;778, `beepAlreadyTaken`&nbsp;788, `beepNowaWersja`&nbsp;813, `beepLowStock`&nbsp;823, `beepLowBattery`&nbsp;832, `beepBoxOpen`&nbsp;848, `beepCharging`&nbsp;856

*3.  GPIO / WYBUDZANIE* — `configureInputs`&nbsp;866, `powodResetuOpis`&nbsp;888, `zanotujReset`&nbsp;906, `reedPoziomStabilny`&nbsp;951, `boxIsOpen`&nbsp;972, `buttonPressed`&nbsp;975, `wakeName`&nbsp;977

*4.  HARMONOGRAM* — `godzinaPoprawna`&nbsp;1041, `parseSchedule`&nbsp;1050, `loadSchedule`&nbsp;1063, `saveSchedule`&nbsp;1086, `localMinutesOfDay`&nbsp;1097, `slotMinutes`&nbsp;1104, `localDayNumber`&nbsp;1113, `matchSlot`&nbsp;1121, `secondsToDayBoundary`&nbsp;1136

*4a.  DNI BEZ LEKU* — `localWeekday`&nbsp;1163, `dateKeyToNum`&nbsp;1171, `dawkaNaDobe`&nbsp;1184, `dzisBezLeku`&nbsp;1194, `parseDoseWeek`&nbsp;1203, `parseDoseEx`&nbsp;1221, `saveDosing`&nbsp;1243, `loadDosing`&nbsp;1256

*4b.  PUDELKO ZOSTAWIONE OTWARTE* — `openWarnSecondsLeft`&nbsp;1280

*4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego* — `lidLogAdd`&nbsp;1323, `lidLogCount`&nbsp;1347, `jsonEscape`&nbsp;1357, `lidLogJson`&nbsp;1373, `lidLogClear`&nbsp;1403, `nvsFailLogDoWyslania`&nbsp;1418, `nvsFailLogJson`&nbsp;1428, `nvsFailLogOznaczWyslany`&nbsp;1446, `trackBoxOpen`&nbsp;1450, `secondsToNextSlot`&nbsp;1499

*5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien)* — `rekordTs`&nbsp;1530, `rekordBezDaty`&nbsp;1537, `tsDoBazy`&nbsp;1546, `queuePush`&nbsp;1550, `queueCount`&nbsp;1573, `queuePeek`&nbsp;1580, `queuePop`&nbsp;1595, `queueDrop`&nbsp;1614, `przesunZnaczniki`&nbsp;1638, `queueShiftTimestamps`&nbsp;1653, `queueNadajCzas`&nbsp;1701, `queueEpokaSkasuj`&nbsp;1744

*6.  WiFi* — `netKlucz`&nbsp;1783, `wifiSieciCount`&nbsp;1787, `wifiSiecSsid`&nbsp;1794, `wifiSiecPass`&nbsp;1803, `wifiListeZapisz`&nbsp;1830, `wifiListeCzytaj`&nbsp;1854, `wifiSiecDodaj`&nbsp;1867, `wifiSiecUsun`&nbsp;1898, `wifiSiecPriorytet`&nbsp;1931, `wifiSprobuj`&nbsp;1959, `wifiOdNowa`&nbsp;2003, `wifiZacznijProbe`&nbsp;2014, `wifiStart`&nbsp;2034, `wifiKrokLaczenia`&nbsp;2074, `wifiConnect`&nbsp;2113, `wifiOff`&nbsp;2182, `wifiUspij`&nbsp;2196, `syncTimeNTP`&nbsp;2201

*7.  FIREBASE  (REST: Auth email/haslo + Realtime Database)* — `tokenZPamieci`&nbsp;2268, `zapomnijToken`&nbsp;2277, `hasloJestPrawdziwe`&nbsp;2322, `hasloZPamieci`&nbsp;2327, `hasloWPamieci`&nbsp;2336, `hasloUtrwal`&nbsp;2340, `hasloDoLogowania`&nbsp;2353, `tgTokenZPamieci`&nbsp;2373, `tgChatZPamieci`&nbsp;2380, `tgSkonfigurowany`&nbsp;2389, `tgUtrwal`&nbsp;2396, `tgZapomnij`&nbsp;2408, `firebaseSignIn`&nbsp;2442, `rtdbUrl`&nbsp;2537, `rtdbSend`&nbsp;2559, `rekordKompletny`&nbsp;2586, `pushEventRecord`&nbsp;2595, `pushLidState`&nbsp;2652, `otaSumaZPamieci`&nbsp;2703, `otaSumaWgranej`&nbsp;2725, `pushStatus`&nbsp;2731, `fetchConfig`&nbsp;2956, `trwaleOdrzucony`&nbsp;3280, `flushQueue`&nbsp;3284

*8.  ZDARZENIA* — `makeRecordAt`&nbsp;3326, `makeRecord`&nbsp;3348, `loadDayMarkers`&nbsp;3358, `clearDayMarkers`&nbsp;3377, `setTakenDay`&nbsp;3391, `setRolloverDay`&nbsp;3399, `zapiszDawke`&nbsp;3429, `oznaczAlarmObsluzony`&nbsp;3472, `alarmJuzObsluzony`&nbsp;3505, `ostatniSlotDoby`&nbsp;3531, `juzDzisBrane`&nbsp;3541, `checkDayRollover`&nbsp;3548, `reportEvent`&nbsp;3630

*9.  ALARM* — `alarmPotwierdzony`&nbsp;3727, `runAlarmWindow`&nbsp;3732

*10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth)* — `htmlEscape`&nbsp;3773, `portalPage`&nbsp;3787, `startWifiPortal`&nbsp;3831

*10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59* — `otaOpisDecyzji`&nbsp;4103, `otaZanotujProbe`&nbsp;4129, `otaWyzerujLicznik`&nbsp;4137, `otaZlecenieWBazie`&nbsp;4167, `otaPobierzOpis`&nbsp;4182, `otaWgraj`&nbsp;4226, `otaSprawdzPoStarcie`&nbsp;4384, `otaPotwierdzDzialanie`&nbsp;4417

*10b. CZARNA SKRZYNKA* — `note`&nbsp;4491, `wartoZapisac`&nbsp;4498, `logbookAdd`&nbsp;4510, `logbookPrint`&nbsp;4549, `logbookJson`&nbsp;4573

*10c. GESTY SERWISOWE I AUTOTEST* — `pikNumer`&nbsp;4768, `pikKoniecTestu`&nbsp;4780, `pikBrakSieci`&nbsp;4791, `wynikEtapu`&nbsp;4803, `etapTestu`&nbsp;4822, `autoTest`&nbsp;4827

*10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67)* — `tgWyslijTekst`&nbsp;4974, `tgZglosNieodebrane`&nbsp;5020, `tgSprawdzBaterie`&nbsp;5040, `tgSprawdzZapas`&nbsp;5059, `dniOdEry`&nbsp;5082, `dniDoDaty`&nbsp;5093, `inrPrzypomnienieTeraz`&nbsp;5127, `tgOznaczInrMiniete`&nbsp;5147, `sekundyDoInrPrzypomnienia`&nbsp;5156, `tgSprawdzInr`&nbsp;5178, `tgTekstZapas`&nbsp;5192, `tgTekstInr`&nbsp;5201, `tgTekstNieodebrane`&nbsp;5221, `tgTekstBateria`&nbsp;5230, `tgWyslijZalegle`&nbsp;5249

*11.  DEEP SLEEP* — `otaZglos`&nbsp;5380, `skanujSieci`&nbsp;5408, `otaSprobuj`&nbsp;5457, `kolejnePrzesuniecie`&nbsp;5656, `goToSleep`&nbsp;5661, `planNextSleep`&nbsp;5879

*12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany)* — `petlaLadowania`&nbsp;5972, `setup`&nbsp;6070, `loop`&nbsp;6701


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
