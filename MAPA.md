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

## `index.html` — 9089 linii, ~130 tys. tokenow

Ekrany (`<section>`) i dwa duze bloki. Zakladki `tab-*` odpowiadaja
pozycjom w pasku nawigacji i podekranom Ustawien.

| od | do | co |
|---|---|---|
| 21 | 21 | CSS — poczatek |
| 22 | 263 | SYSTEM WIZUALNY PillBox |
| 264 | 360 | EKRAN GŁÓWNY — KARTA DNIA |
| 361 | 433 | INFORMACJA ZWROTNA |
| 434 | 904 | TABLETKA W 3D |
| 905 | 1065 | tab-cal |
| 1066 | 1114 | tab-inr |
| 1115 | 1205 | tab-ana |
| 1206 | 1314 | tab-set |
| 1315 | 1395 | tab-lek |
| 1396 | 1405 | tab-pud |
| 1406 | 1425 | tab-sinr |
| 1426 | 1465 | tab-wifi |
| 1466 | 1566 | tab-tg |
| 1567 | 1600 | tab-dev |
| 1601 | 1710 | tab-diag |
| 1711 | 2033 | tab-help |
| 2034 | 2055 | tab-ev |
| 2056 | 2146 | tab-hist |
| 2147 | 2155 | JS — poczatek |
| 2156 | 2389 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2390 | 2464 | INFORMACJA ZWROTNA |
| 2465 | 2559 | STREFY CZASOWE |
| 2560 | 2587 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2588 | 2654 | TABLETKA JAKO BRYŁA |
| 2655 | 2669 | LOGOWANIE |
| 2670 | 2809 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2810 | 3190 | START |
| 3191 | 3251 | OSŁONA RYSOWANIA |
| 3252 | 3310 | REKONCYLIACJA |
| 3311 | 3515 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3516 | 3574 | KALENDARZ |
| 3575 | 3845 | HISTORIA ROZPISANIA DAWKI |
| 3846 | 4053 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 4054 | 4205 | ARKUSZ DNIA |
| 4206 | 4305 | WZIĄŁEM TERAZ |
| 4306 | 4420 | INR |
| 4421 | 4560 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4561 | 4573 | STATUS PUDEŁKA |
| 4574 | 4723 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4724 | 4885 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4886 | 5168 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 5169 | 5209 | KOLEJKA, KTÓRA NIE SCHODZI |
| 5210 | 5306 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 5307 | 5966 | EKRAN ZDARZEN |
| 5967 | 6085 | ZAPAS TABLETEK |
| 6086 | 6440 | USTAWIENIA |
| 6441 | 6748 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6749 | 7224 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 7225 | 7596 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7597 | 7807 | ANALIZA |
| 7808 | 8193 | WYKRESY ANALIZY |
| 8194 | 8350 | RAPORT |
| 8351 | 8412 | KONTEKST DNIA (TAGI) |
| 8413 | 8463 | KOPIA ZAPASOWA |
| 8464 | 8677 | KOPIA NA TELEGRAM |
| 8678 | 8743 | WIEK KOPII |
| 8744 | 8884 | ODTWARZANIE Z KOPII |
| 8885 | 8959 | KOPIE Z BAZY |
| 8960 | 9047 | NAWIGACJA |
| 9048 | 9089 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (230) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `opisPlikFirmware`&nbsp;2197, `wersjaZTelegramem`&nbsp;2206, `wersjaZOtwarciem`&nbsp;2218, `pudelkoZnane`&nbsp;2224, `wybranePudelko`&nbsp;2226, `korzenDanych`&nbsp;2250, `odmowaRegul`&nbsp;2269, `sprawdzDostepPudelek`&nbsp;2274, `wybierzPudelko`&nbsp;2297, `profilTydzien`&nbsp;2359, `ustawProfil`&nbsp;2364

*INFORMACJA ZWROTNA* — `toast`&nbsp;2411, `busy`&nbsp;2427, `todayKey`&nbsp;2454, `dzisiajKey`&nbsp;2458, `inNightWindow`&nbsp;2461

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2516, `tzName`&nbsp;2532, `tzLabel`&nbsp;2533, `tzOffsetTxt`&nbsp;2534, `devDate`&nbsp;2540, `devKey`&nbsp;2545, `devHM`&nbsp;2550, `slotMin`&nbsp;2551, `pillColors`&nbsp;2553

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2564

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2601, `cieniuj`&nbsp;2628, `doseGraphic`&nbsp;2645

*LOGOWANIE* — `doLogin`&nbsp;2659

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2679, `wyczyscCache`&nbsp;2774, `fbSignOut`&nbsp;2793

*START* — `boot`&nbsp;2811

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;3218, `rysujWszystkie`&nbsp;3231, `renderAll`&nbsp;3235

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;3258, `reconcileDecyzja`&nbsp;3297

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3332, `zapiszReconcile`&nbsp;3344, `doReconcile`&nbsp;3383, `doReconcileWewn`&nbsp;3393, `reconcile`&nbsp;3514

*KALENDARZ* — `tydzienDawek`&nbsp;3554

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3600, `dawkaNaDzien`&nbsp;3611, `dzienBezLeku`&nbsp;3632, `wyjatekNaDzien`&nbsp;3637, `opisDawkowania`&nbsp;3643, `dayDose`&nbsp;3657, `dzienZamkniety`&nbsp;3691, `trackingSince`&nbsp;3697, `beforeTracking`&nbsp;3698, `dayStatus`&nbsp;3700, `renderCalendar`&nbsp;3741, `seriaDni`&nbsp;3806, `doNastepnej`&nbsp;3824, `opisCzasu`&nbsp;3839

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3854, `trwanieTxt`&nbsp;3869, `kiedyDawkaTxt`&nbsp;3880, `odswiezOdDawki`&nbsp;3888, `startTikOdDawki`&nbsp;3902, `renderToday`&nbsp;3912

*ARKUSZ DNIA* — `closeSheet`&nbsp;4065, `renderSheet`&nbsp;4067, `resetDose`&nbsp;4145, `resetPlan`&nbsp;4152, `commitPlan`&nbsp;4157, `clearPlan`&nbsp;4173, `commitDose`&nbsp;4185

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;4226, `askConfirm`&nbsp;4295

*INR* — `inrState`&nbsp;4307, `odswiezTerminInr`&nbsp;4320, `addInr`&nbsp;4329, `inrKeysOk`&nbsp;4415

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4430, `inrTerminKey`&nbsp;4438, `inrDoTerminu`&nbsp;4450, `dniTxt`&nbsp;4459, `renderInr`&nbsp;4461, `inrChart`&nbsp;4533

*STATUS PUDEŁKA* — `relTime`&nbsp;4562, `devDayMon`&nbsp;4571

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4606, `renderBoxLog`&nbsp;4652, `logPrzelacz`&nbsp;4688, `renderNvsFailLog`&nbsp;4696

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4741, `oczekWczytaj`&nbsp;4749, `oczekZapisz`&nbsp;4754, `oczekIle`&nbsp;4757, `zapiszPewnie`&nbsp;4767, `zapiszCfg`&nbsp;4805, `bazaOdmowila`&nbsp;4823, `oczekWyslij`&nbsp;4847, `oczekOdmowy`&nbsp;4884

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4900, `ostrzReguly`&nbsp;4922, `lm`&nbsp;4961, `ostrzMilczy`&nbsp;4967, `nvsMalo`&nbsp;5047, `opisNvsFailKey`&nbsp;5059, `stratyDotyczaLeku`&nbsp;5112, `ostrzStraty`&nbsp;5124, `stratyCicho`&nbsp;5158

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;5190

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;5220, `renderOstrzezenia`&nbsp;5235, `bezPokrycia`&nbsp;5245, `wierszZdarzenia`&nbsp;5251, `renderDiag`&nbsp;5268

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5323, `evPasuje`&nbsp;5328, `renderEvents`&nbsp;5340, `opisPomiaruBaterii`&nbsp;5400, `nazwaOtwarcia`&nbsp;5412, `renderOpenWarn`&nbsp;5420, `minutyDoPelna`&nbsp;5473, `opisLadowania`&nbsp;5485, `opisLadowaniaCzujnik`&nbsp;5510, `dni`&nbsp;5526, `opisLadowan`&nbsp;5529, `tempoZHistorii`&nbsp;5582, `prognozaDni`&nbsp;5591, `opisPrognozy`&nbsp;5603, `czasKrotko`&nbsp;5625, `opisCzuwania`&nbsp;5633, `renderStatus`&nbsp;5651

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5970, `dayAfter`&nbsp;5973, `pillsBaseInfo`&nbsp;5984, `settlePills`&nbsp;5994, `dniZapasu`&nbsp;6034, `renderPills`&nbsp;6047, `savePills`&nbsp;6068, `setPills`&nbsp;6079

*USTAWIENIA* — `opisLeku`&nbsp;6097, `renderKafelki`&nbsp;6103, `renderPudelka`&nbsp;6138, `renderSettings`&nbsp;6180, `tydzienZPol`&nbsp;6239, `renderWeekEditor`&nbsp;6251, `odswiezPodpowiedzTygodnia`&nbsp;6268, `tydzienZmieniony`&nbsp;6282, `rownajTydzien`&nbsp;6283, `renderPlanList`&nbsp;6331, `renderExceptions`&nbsp;6358, `wyslijSiec`&nbsp;6416

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6458, `tgZapytaj`&nbsp;6468, `tgKodParowania`&nbsp;6508, `tgZnajdzCzat`&nbsp;6524, `tgPolacz`&nbsp;6592, `tgProbna`&nbsp;6625, `tgOdlacz`&nbsp;6632, `renderTgStan`&nbsp;6654

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6769, `pobierzOpisFirmware`&nbsp;6775, `wyslijAktualizacje`&nbsp;6798, `anulujAktualizacje`&nbsp;6843, `renderOta`&nbsp;6849, `renderNetStan`&nbsp;7151

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;7236, `renderSkan`&nbsp;7242, `szukajSieci`&nbsp;7294, `wybierzSiec`&nbsp;7302, `wyslijPolecenieSieci`&nbsp;7320, `siecZIndeksu`&nbsp;7330, `tzChanged`&nbsp;7364, `cfgTime`&nbsp;7369, `addSlot`&nbsp;7377, `zapiszPlanDnia`&nbsp;7390, `saveConfig`&nbsp;7408, `inrKrokiZakresu`&nbsp;7479, `opcjeInr`&nbsp;7486, `inrZakresZmieniony`&nbsp;7497, `wypelnijListyZakresu`&nbsp;7510, `saveInrRange`&nbsp;7521, `wypelnijListeOdstepu`&nbsp;7555, `saveInrEvery`&nbsp;7568, `odswiezPodpowiedzInr`&nbsp;7578

*ANALIZA* — `openTimeOf`&nbsp;7603, `openMinutes`&nbsp;7609, `sredniaPora`&nbsp;7633, `kwantyl`&nbsp;7641, `dniMiedzy`&nbsp;7649, `odstepyZPunktow`&nbsp;7663, `analyze`&nbsp;7672, `inrContext`&nbsp;7774

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7833, `rytmSVG`&nbsp;7845, `poryWCzasieSVG`&nbsp;7907, `iskraSVG`&nbsp;7975, `dowSVG`&nbsp;8003, `dniRytmu`&nbsp;8039, `skutecznoscTygodniami`&nbsp;8060, `renderAnalysis`&nbsp;8088

*RAPORT* — `collectRows`&nbsp;8195, `makeReport`&nbsp;8232

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;8383, `tagiPrzed`&nbsp;8391, `tagPrzelacz`&nbsp;8400

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;8443, `opisKopii`&nbsp;8453

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8498, `tgCzatKopii`&nbsp;8505, `odswiezKopie`&nbsp;8512, `tgKopiaCzatZapisz`&nbsp;8520, `tgKopiaCzatZnajdz`&nbsp;8540, `tgKopiaWlacz`&nbsp;8570, `tgKopiaWylacz`&nbsp;8589, `kopiaNaTelegram`&nbsp;8598, `kopiaAutomat`&nbsp;8649

*WIEK KOPII* — `dniOdDaty`&nbsp;8696, `wiekKopiiTxt`&nbsp;8702, `renderKopiaStan`&nbsp;8710, `zapiszKopie`&nbsp;8725

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8759, `ustawieniaDoOdtworzenia`&nbsp;8807, `wczytajKopie`&nbsp;8822, `kopiaCzytelna`&nbsp;8827, `odtworzKopie`&nbsp;8837, `kopiaWybrana`&nbsp;8870

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8894, `odtworzZBazy`&nbsp;8926, `exportCsv`&nbsp;8938

*NAWIGACJA* — `wrocZEkranu`&nbsp;9046


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
