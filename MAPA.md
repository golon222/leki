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

## `index.html` — 8075 linii, ~130 tys. tokenow

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
| 2825 | 3021 | REKONCYLIACJA |
| 3022 | 3080 | KALENDARZ |
| 3081 | 3347 | HISTORIA ROZPISANIA DAWKI |
| 3348 | 3529 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3530 | 3681 | ARKUSZ DNIA |
| 3682 | 3777 | WZIĄŁEM TERAZ |
| 3778 | 3892 | INR |
| 3893 | 4032 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4033 | 4045 | STATUS PUDEŁKA |
| 4046 | 4196 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4197 | 4301 | DZIENNIK WIECZKA — narzędzie na czas testu terenowego. |
| 4302 | 4471 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4472 | 4718 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4719 | 4759 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4760 | 4853 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 4854 | 5159 | EKRAN ZDARZEN |
| 5160 | 5278 | ZAPAS TABLETEK |
| 5279 | 5508 | USTAWIENIA |
| 5509 | 5816 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 5817 | 6273 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6274 | 6635 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 6636 | 6846 | ANALIZA |
| 6847 | 7232 | WYKRESY ANALIZY |
| 7233 | 7389 | RAPORT |
| 7390 | 7451 | KONTEKST DNIA (TAGI) |
| 7452 | 7497 | KOPIA ZAPASOWA |
| 7498 | 7711 | KOPIA NA TELEGRAM |
| 7712 | 7777 | WIEK KOPII |
| 7778 | 7875 | ODTWARZANIE Z KOPII |
| 7876 | 7950 | KOPIE Z BAZY |
| 7951 | 8033 | NAWIGACJA |
| 8034 | 8075 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (209) — nazwa i linia deklaracji:

*INFORMACJA ZWROTNA* — `toast`&nbsp;1992, `busy`&nbsp;2008, `todayKey`&nbsp;2035, `dzisiajKey`&nbsp;2039, `inNightWindow`&nbsp;2042

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2097, `tzName`&nbsp;2113, `tzLabel`&nbsp;2114, `tzOffsetTxt`&nbsp;2115, `devDate`&nbsp;2121, `devKey`&nbsp;2126, `devHM`&nbsp;2131, `slotMin`&nbsp;2132, `pillColors`&nbsp;2134

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2145

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2182, `cieniuj`&nbsp;2209, `doseGraphic`&nbsp;2226

*LOGOWANIE* — `doLogin`&nbsp;2240

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2260, `wyczyscCache`&nbsp;2355, `fbSignOut`&nbsp;2374

*START* — `boot`&nbsp;2392

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;2791, `rysujWszystkie`&nbsp;2804, `renderAll`&nbsp;2808

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;2831, `reconcileDecyzja`&nbsp;2870, `zapiszReconcile`&nbsp;2887, `doReconcile`&nbsp;2904, `reconcile`&nbsp;3020

*KALENDARZ* — `tydzienDawek`&nbsp;3060

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3106, `dawkaNaDzien`&nbsp;3117, `dzienBezLeku`&nbsp;3138, `wyjatekNaDzien`&nbsp;3143, `opisDawkowania`&nbsp;3149, `dayDose`&nbsp;3159, `dzienZamkniety`&nbsp;3193, `trackingSince`&nbsp;3199, `beforeTracking`&nbsp;3200, `dayStatus`&nbsp;3202, `renderCalendar`&nbsp;3243, `seriaDni`&nbsp;3308, `doNastepnej`&nbsp;3326, `opisCzasu`&nbsp;3341

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3356, `trwanieTxt`&nbsp;3371, `kiedyDawkaTxt`&nbsp;3382, `odswiezOdDawki`&nbsp;3390, `startTikOdDawki`&nbsp;3404, `renderToday`&nbsp;3414

*ARKUSZ DNIA* — `closeSheet`&nbsp;3541, `renderSheet`&nbsp;3543, `resetDose`&nbsp;3621, `resetPlan`&nbsp;3628, `commitPlan`&nbsp;3633, `clearPlan`&nbsp;3649, `commitDose`&nbsp;3661

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3702, `askConfirm`&nbsp;3767

*INR* — `inrState`&nbsp;3779, `odswiezTerminInr`&nbsp;3792, `addInr`&nbsp;3801, `inrKeysOk`&nbsp;3887

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;3902, `inrTerminKey`&nbsp;3910, `inrDoTerminu`&nbsp;3922, `dniTxt`&nbsp;3931, `renderInr`&nbsp;3933, `inrChart`&nbsp;4005

*STATUS PUDEŁKA* — `relTime`&nbsp;4034, `devDayMon`&nbsp;4043

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4078, `renderBoxLog`&nbsp;4124, `logPrzelacz`&nbsp;4160, `renderNvsFailLog`&nbsp;4169

*DZIENNIK WIECZKA — narzędzie na czas testu terenowego.* — `lidPaczki`&nbsp;4218, `lidWpisy`&nbsp;4224, `renderLidLog`&nbsp;4237

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4319, `oczekWczytaj`&nbsp;4327, `oczekZapisz`&nbsp;4332, `oczekIle`&nbsp;4335, `zapiszPewnie`&nbsp;4345, `zapiszCfg`&nbsp;4387, `bazaOdmowila`&nbsp;4405, `oczekWyslij`&nbsp;4429, `oczekOdmowy`&nbsp;4470

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4486, `ostrzReguly`&nbsp;4508, `lm`&nbsp;4547, `ostrzMilczy`&nbsp;4553, `nvsMalo`&nbsp;4600, `opisNvsFailKey`&nbsp;4612, `stratyDotyczaLeku`&nbsp;4662, `ostrzStraty`&nbsp;4674, `stratyCicho`&nbsp;4708

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4740

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4770, `renderOstrzezenia`&nbsp;4785, `bezPokrycia`&nbsp;4795, `wierszZdarzenia`&nbsp;4801, `renderDiag`&nbsp;4818

*EKRAN ZDARZEN* — `evFiltr`&nbsp;4870, `evPasuje`&nbsp;4875, `renderEvents`&nbsp;4887, `renderOpenWarn`&nbsp;4927, `minutyDoPelna`&nbsp;4980, `opisLadowania`&nbsp;4992, `dni`&nbsp;5012, `opisLadowan`&nbsp;5015, `renderStatus`&nbsp;5032

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5163, `dayAfter`&nbsp;5166, `pillsBaseInfo`&nbsp;5177, `settlePills`&nbsp;5187, `dniZapasu`&nbsp;5227, `renderPills`&nbsp;5240, `savePills`&nbsp;5261, `setPills`&nbsp;5272

*USTAWIENIA* — `renderKafelki`&nbsp;5283, `renderSettings`&nbsp;5310, `tydzienZPol`&nbsp;5354, `renderWeekEditor`&nbsp;5366, `odswiezPodpowiedzTygodnia`&nbsp;5383, `tydzienZmieniony`&nbsp;5397, `rownajTydzien`&nbsp;5398, `renderPlanList`&nbsp;5411, `renderExceptions`&nbsp;5438, `wyslijSiec`&nbsp;5484

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5526, `tgZapytaj`&nbsp;5536, `tgKodParowania`&nbsp;5576, `tgZnajdzCzat`&nbsp;5592, `tgPolacz`&nbsp;5660, `tgProbna`&nbsp;5693, `tgOdlacz`&nbsp;5700, `renderTgStan`&nbsp;5722

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;5837, `pobierzOpisFirmware`&nbsp;5843, `wyslijAktualizacje`&nbsp;5864, `anulujAktualizacje`&nbsp;5909, `renderOta`&nbsp;5915, `renderNetStan`&nbsp;6200

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6285, `renderSkan`&nbsp;6291, `szukajSieci`&nbsp;6343, `wybierzSiec`&nbsp;6351, `wyslijPolecenieSieci`&nbsp;6369, `siecZIndeksu`&nbsp;6379, `tzChanged`&nbsp;6413, `cfgTime`&nbsp;6418, `addSlot`&nbsp;6426, `zapiszPlanDnia`&nbsp;6439, `saveConfig`&nbsp;6457, `inrKrokiZakresu`&nbsp;6518, `opcjeInr`&nbsp;6525, `inrZakresZmieniony`&nbsp;6536, `wypelnijListyZakresu`&nbsp;6549, `saveInrRange`&nbsp;6560, `wypelnijListeOdstepu`&nbsp;6594, `saveInrEvery`&nbsp;6607, `odswiezPodpowiedzInr`&nbsp;6617

*ANALIZA* — `openTimeOf`&nbsp;6642, `openMinutes`&nbsp;6648, `sredniaPora`&nbsp;6672, `kwantyl`&nbsp;6680, `dniMiedzy`&nbsp;6688, `odstepyZPunktow`&nbsp;6702, `analyze`&nbsp;6711, `inrContext`&nbsp;6813

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;6872, `rytmSVG`&nbsp;6884, `poryWCzasieSVG`&nbsp;6946, `iskraSVG`&nbsp;7014, `dowSVG`&nbsp;7042, `dniRytmu`&nbsp;7078, `skutecznoscTygodniami`&nbsp;7099, `renderAnalysis`&nbsp;7127

*RAPORT* — `collectRows`&nbsp;7234, `makeReport`&nbsp;7271

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7422, `tagiPrzed`&nbsp;7430, `tagPrzelacz`&nbsp;7439

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7482, `opisKopii`&nbsp;7492

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7532, `tgCzatKopii`&nbsp;7539, `odswiezKopie`&nbsp;7546, `tgKopiaCzatZapisz`&nbsp;7554, `tgKopiaCzatZnajdz`&nbsp;7574, `tgKopiaWlacz`&nbsp;7604, `tgKopiaWylacz`&nbsp;7623, `kopiaNaTelegram`&nbsp;7632, `kopiaAutomat`&nbsp;7683

*WIEK KOPII* — `dniOdDaty`&nbsp;7730, `wiekKopiiTxt`&nbsp;7736, `renderKopiaStan`&nbsp;7744, `zapiszKopie`&nbsp;7759

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;7793, `wczytajKopie`&nbsp;7823, `kopiaCzytelna`&nbsp;7828, `odtworzKopie`&nbsp;7838, `kopiaWybrana`&nbsp;7861

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;7885, `odtworzZBazy`&nbsp;7917, `exportCsv`&nbsp;7929

*NAWIGACJA* — `wrocZEkranu`&nbsp;8032


---

## `firmware/PillBox/PillBox.ino` — 6297 linii

| od | do | blok |
|---|---|---|
| 1 | 63 | PillBox.ino  -  Inteligentne pudelko na leki / IoT Pill Reminder |
| 64 | 243 | PAMIEC RTC  (przezywa deep sleep, ginie po odlaczeniu zasilania) |
| 244 | 469 | STAN GLOBALNY |
| 470 | 690 | 1.  POMIAR BATERII |
| 691 | 853 | 2.  BUZZER  (pasywny piezo -> PWM przez LEDC) |
| 854 | 915 | 3.  GPIO / WYBUDZANIE |
| 916 | 1030 | 4.  HARMONOGRAM |
| 1031 | 1156 | 4a.  DNI BEZ LEKU |
| 1157 | 1177 | 4b.  PUDELKO ZOSTAWIONE OTWARTE |
| 1178 | 1401 | 4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego |
| 1402 | 1653 | 5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien) |
| 1654 | 2002 | 6.  WiFi |
| 2003 | 3014 | 7.  FIREBASE  (REST: Auth email/haslo + Realtime Database) |
| 3015 | 3379 | 8.  ZDARZENIA |
| 3380 | 3435 | 9.  ALARM |
| 3436 | 3654 | 10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth) |
| 3655 | 4146 | 10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59 |
| 4147 | 4266 | 10b. CZARNA SKRZYNKA |
| 4267 | 4569 | 10c. GESTY SERWISOWE I AUTOTEST |
| 4570 | 4992 | 10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67) |
| 4993 | 5563 | 11.  DEEP SLEEP |
| 5564 | 6297 | 12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany) |

**Funkcje** (174):

*STAN GLOBALNY* — `zanotujNvsFail`&nbsp;322, `nvsPutStr`&nbsp;341, `nvsPutU16`&nbsp;364, `nvsPutU32`&nbsp;387, `nvsWolneWpisy`&nbsp;402, `syncTimeNTP`&nbsp;445, `logbookJson`&nbsp;446, `setTakenDay`&nbsp;447, `note`&nbsp;449, `awakeTooLong`&nbsp;463, `extendAwake`&nbsp;465

*1.  POMIAR BATERII* — `readBatteryRaw`&nbsp;478, `battPercentFromCurve`&nbsp;518, `resetBatteryFilter`&nbsp;550, `zapiszKoniecLadowania`&nbsp;573, `trackCharging`&nbsp;583, `battSmooth`&nbsp;637, `readBattery`&nbsp;668

*2.  BUZZER  (pasywny piezo -> PWM przez LEDC)* — `buzzerInit`&nbsp;694, `buzzerTone`&nbsp;703, `buzzerTonCicho`&nbsp;714, `buzzerOff`&nbsp;723, `beepAck`&nbsp;735, `beepErr`&nbsp;759, `beepQueued`&nbsp;769, `beepAlreadyTaken`&nbsp;779, `beepNowaWersja`&nbsp;804, `beepLowStock`&nbsp;814, `beepLowBattery`&nbsp;823, `beepBoxOpen`&nbsp;839, `beepCharging`&nbsp;847

*3.  GPIO / WYBUDZANIE* — `configureInputs`&nbsp;857, `boxIsOpen`&nbsp;862, `buttonPressed`&nbsp;863, `wakeName`&nbsp;865

*4.  HARMONOGRAM* — `godzinaPoprawna`&nbsp;929, `parseSchedule`&nbsp;938, `loadSchedule`&nbsp;951, `saveSchedule`&nbsp;974, `localMinutesOfDay`&nbsp;985, `slotMinutes`&nbsp;992, `localDayNumber`&nbsp;1001, `matchSlot`&nbsp;1009, `secondsToDayBoundary`&nbsp;1024

*4a.  DNI BEZ LEKU* — `localWeekday`&nbsp;1051, `dateKeyToNum`&nbsp;1059, `dawkaNaDobe`&nbsp;1072, `dzisBezLeku`&nbsp;1082, `parseDoseWeek`&nbsp;1091, `parseDoseEx`&nbsp;1109, `saveDosing`&nbsp;1131, `loadDosing`&nbsp;1144

*4b.  PUDELKO ZOSTAWIONE OTWARTE* — `openWarnSecondsLeft`&nbsp;1168

*4c.  DZIENNIK WIECZKA  -  narzedzie do testu terenowego* — `lidLogAdd`&nbsp;1211, `lidLogCount`&nbsp;1235, `jsonEscape`&nbsp;1245, `lidLogJson`&nbsp;1261, `lidLogClear`&nbsp;1291, `nvsFailLogDoWyslania`&nbsp;1306, `nvsFailLogJson`&nbsp;1316, `nvsFailLogOznaczWyslany`&nbsp;1334, `trackBoxOpen`&nbsp;1338, `secondsToNextSlot`&nbsp;1387

*5.  KOLEJKA OFFLINE  (Preferences / NVS - pierscien)* — `rekordTs`&nbsp;1418, `rekordBezDaty`&nbsp;1425, `tsDoBazy`&nbsp;1434, `queuePush`&nbsp;1438, `queueCount`&nbsp;1461, `queuePeek`&nbsp;1468, `queuePop`&nbsp;1483, `queueDrop`&nbsp;1502, `przesunZnaczniki`&nbsp;1526, `queueShiftTimestamps`&nbsp;1541, `queueNadajCzas`&nbsp;1589, `queueEpokaSkasuj`&nbsp;1632

*6.  WiFi* — `netKlucz`&nbsp;1671, `wifiSieciCount`&nbsp;1675, `wifiSiecSsid`&nbsp;1682, `wifiSiecPass`&nbsp;1691, `wifiListeZapisz`&nbsp;1718, `wifiListeCzytaj`&nbsp;1742, `wifiSiecDodaj`&nbsp;1755, `wifiSiecUsun`&nbsp;1786, `wifiSiecPriorytet`&nbsp;1819, `wifiSprobuj`&nbsp;1847, `wifiConnect`&nbsp;1865, `wifiOff`&nbsp;1934, `wifiUspij`&nbsp;1948, `syncTimeNTP`&nbsp;1953

*7.  FIREBASE  (REST: Auth email/haslo + Realtime Database)* — `tokenZPamieci`&nbsp;2020, `zapomnijToken`&nbsp;2029, `hasloJestPrawdziwe`&nbsp;2074, `hasloZPamieci`&nbsp;2079, `hasloWPamieci`&nbsp;2088, `hasloUtrwal`&nbsp;2092, `hasloDoLogowania`&nbsp;2105, `tgTokenZPamieci`&nbsp;2125, `tgChatZPamieci`&nbsp;2132, `tgSkonfigurowany`&nbsp;2141, `tgUtrwal`&nbsp;2148, `tgZapomnij`&nbsp;2160, `firebaseSignIn`&nbsp;2194, `rtdbUrl`&nbsp;2289, `rtdbSend`&nbsp;2311, `rekordKompletny`&nbsp;2338, `pushEventRecord`&nbsp;2347, `pushLidState`&nbsp;2404, `otaSumaZPamieci`&nbsp;2425, `otaSumaWgranej`&nbsp;2447, `pushStatus`&nbsp;2453, `fetchConfig`&nbsp;2648, `trwaleOdrzucony`&nbsp;2972, `flushQueue`&nbsp;2976

*8.  ZDARZENIA* — `makeRecordAt`&nbsp;3018, `makeRecord`&nbsp;3040, `loadDayMarkers`&nbsp;3050, `clearDayMarkers`&nbsp;3069, `setTakenDay`&nbsp;3083, `setRolloverDay`&nbsp;3091, `zapiszDawke`&nbsp;3121, `oznaczAlarmObsluzony`&nbsp;3164, `alarmJuzObsluzony`&nbsp;3181, `ostatniSlotDoby`&nbsp;3207, `juzDzisBrane`&nbsp;3217, `checkDayRollover`&nbsp;3224, `reportEvent`&nbsp;3306

*9.  ALARM* — `alarmPotwierdzony`&nbsp;3403, `runAlarmWindow`&nbsp;3408

*10.  PORTAL KONFIGURACJI WiFi  (zamiast Bluetooth)* — `htmlEscape`&nbsp;3449, `portalPage`&nbsp;3463, `startWifiPortal`&nbsp;3507

*10a2. AKTUALIZACJA PROGRAMU PRZEZ WIFI  (OTA)   -  D59* — `otaOpisDecyzji`&nbsp;3779, `otaZanotujProbe`&nbsp;3805, `otaWyzerujLicznik`&nbsp;3813, `otaZlecenieWBazie`&nbsp;3843, `otaPobierzOpis`&nbsp;3858, `otaWgraj`&nbsp;3902, `otaSprawdzPoStarcie`&nbsp;4060, `otaPotwierdzDzialanie`&nbsp;4093

*10b. CZARNA SKRZYNKA* — `note`&nbsp;4167, `wartoZapisac`&nbsp;4174, `logbookAdd`&nbsp;4186, `logbookPrint`&nbsp;4225, `logbookJson`&nbsp;4249

*10c. GESTY SERWISOWE I AUTOTEST* — `pikNumer`&nbsp;4399, `pikKoniecTestu`&nbsp;4411, `pikBrakSieci`&nbsp;4422, `wynikEtapu`&nbsp;4434, `etapTestu`&nbsp;4453, `autoTest`&nbsp;4458

*10b. POWIADOMIENIA NA TELEFON  (bot Telegram, D67)* — `tgWyslijTekst`&nbsp;4605, `tgZglosNieodebrane`&nbsp;4651, `tgSprawdzBaterie`&nbsp;4671, `tgSprawdzZapas`&nbsp;4690, `dniOdEry`&nbsp;4713, `dniDoDaty`&nbsp;4724, `inrPrzypomnienieTeraz`&nbsp;4758, `tgOznaczInrMiniete`&nbsp;4778, `sekundyDoInrPrzypomnienia`&nbsp;4787, `tgSprawdzInr`&nbsp;4809, `tgTekstZapas`&nbsp;4823, `tgTekstInr`&nbsp;4832, `tgTekstNieodebrane`&nbsp;4852, `tgTekstBateria`&nbsp;4861, `tgWyslijZalegle`&nbsp;4880

*11.  DEEP SLEEP* — `otaZglos`&nbsp;5011, `skanujSieci`&nbsp;5039, `otaSprobuj`&nbsp;5088, `kolejnePrzesuniecie`&nbsp;5287, `goToSleep`&nbsp;5292, `planNextSleep`&nbsp;5488

*12.  SETUP  =  cala logika (loop() nigdy nie jest osiagany)* — `petlaLadowania`&nbsp;5576, `setup`&nbsp;5674, `loop`&nbsp;6294


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
