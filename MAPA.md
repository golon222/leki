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

## `index.html` — 9081 linii, ~130 tys. tokenow

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
| 3846 | 4045 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 4046 | 4197 | ARKUSZ DNIA |
| 4198 | 4297 | WZIĄŁEM TERAZ |
| 4298 | 4412 | INR |
| 4413 | 4552 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4553 | 4565 | STATUS PUDEŁKA |
| 4566 | 4715 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4716 | 4877 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4878 | 5160 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 5161 | 5201 | KOLEJKA, KTÓRA NIE SCHODZI |
| 5202 | 5298 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 5299 | 5958 | EKRAN ZDARZEN |
| 5959 | 6077 | ZAPAS TABLETEK |
| 6078 | 6432 | USTAWIENIA |
| 6433 | 6740 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6741 | 7216 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 7217 | 7588 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7589 | 7799 | ANALIZA |
| 7800 | 8185 | WYKRESY ANALIZY |
| 8186 | 8342 | RAPORT |
| 8343 | 8404 | KONTEKST DNIA (TAGI) |
| 8405 | 8455 | KOPIA ZAPASOWA |
| 8456 | 8669 | KOPIA NA TELEGRAM |
| 8670 | 8735 | WIEK KOPII |
| 8736 | 8876 | ODTWARZANIE Z KOPII |
| 8877 | 8951 | KOPIE Z BAZY |
| 8952 | 9039 | NAWIGACJA |
| 9040 | 9081 | AUTOMATYCZNA AKTUALIZACJA |

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

*ARKUSZ DNIA* — `closeSheet`&nbsp;4057, `renderSheet`&nbsp;4059, `resetDose`&nbsp;4137, `resetPlan`&nbsp;4144, `commitPlan`&nbsp;4149, `clearPlan`&nbsp;4165, `commitDose`&nbsp;4177

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;4218, `askConfirm`&nbsp;4287

*INR* — `inrState`&nbsp;4299, `odswiezTerminInr`&nbsp;4312, `addInr`&nbsp;4321, `inrKeysOk`&nbsp;4407

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4422, `inrTerminKey`&nbsp;4430, `inrDoTerminu`&nbsp;4442, `dniTxt`&nbsp;4451, `renderInr`&nbsp;4453, `inrChart`&nbsp;4525

*STATUS PUDEŁKA* — `relTime`&nbsp;4554, `devDayMon`&nbsp;4563

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4598, `renderBoxLog`&nbsp;4644, `logPrzelacz`&nbsp;4680, `renderNvsFailLog`&nbsp;4688

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4733, `oczekWczytaj`&nbsp;4741, `oczekZapisz`&nbsp;4746, `oczekIle`&nbsp;4749, `zapiszPewnie`&nbsp;4759, `zapiszCfg`&nbsp;4797, `bazaOdmowila`&nbsp;4815, `oczekWyslij`&nbsp;4839, `oczekOdmowy`&nbsp;4876

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4892, `ostrzReguly`&nbsp;4914, `lm`&nbsp;4953, `ostrzMilczy`&nbsp;4959, `nvsMalo`&nbsp;5039, `opisNvsFailKey`&nbsp;5051, `stratyDotyczaLeku`&nbsp;5104, `ostrzStraty`&nbsp;5116, `stratyCicho`&nbsp;5150

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;5182

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;5212, `renderOstrzezenia`&nbsp;5227, `bezPokrycia`&nbsp;5237, `wierszZdarzenia`&nbsp;5243, `renderDiag`&nbsp;5260

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5315, `evPasuje`&nbsp;5320, `renderEvents`&nbsp;5332, `opisPomiaruBaterii`&nbsp;5392, `nazwaOtwarcia`&nbsp;5404, `renderOpenWarn`&nbsp;5412, `minutyDoPelna`&nbsp;5465, `opisLadowania`&nbsp;5477, `opisLadowaniaCzujnik`&nbsp;5502, `dni`&nbsp;5518, `opisLadowan`&nbsp;5521, `tempoZHistorii`&nbsp;5574, `prognozaDni`&nbsp;5583, `opisPrognozy`&nbsp;5595, `czasKrotko`&nbsp;5617, `opisCzuwania`&nbsp;5625, `renderStatus`&nbsp;5643

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5962, `dayAfter`&nbsp;5965, `pillsBaseInfo`&nbsp;5976, `settlePills`&nbsp;5986, `dniZapasu`&nbsp;6026, `renderPills`&nbsp;6039, `savePills`&nbsp;6060, `setPills`&nbsp;6071

*USTAWIENIA* — `opisLeku`&nbsp;6089, `renderKafelki`&nbsp;6095, `renderPudelka`&nbsp;6130, `renderSettings`&nbsp;6172, `tydzienZPol`&nbsp;6231, `renderWeekEditor`&nbsp;6243, `odswiezPodpowiedzTygodnia`&nbsp;6260, `tydzienZmieniony`&nbsp;6274, `rownajTydzien`&nbsp;6275, `renderPlanList`&nbsp;6323, `renderExceptions`&nbsp;6350, `wyslijSiec`&nbsp;6408

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6450, `tgZapytaj`&nbsp;6460, `tgKodParowania`&nbsp;6500, `tgZnajdzCzat`&nbsp;6516, `tgPolacz`&nbsp;6584, `tgProbna`&nbsp;6617, `tgOdlacz`&nbsp;6624, `renderTgStan`&nbsp;6646

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6761, `pobierzOpisFirmware`&nbsp;6767, `wyslijAktualizacje`&nbsp;6790, `anulujAktualizacje`&nbsp;6835, `renderOta`&nbsp;6841, `renderNetStan`&nbsp;7143

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;7228, `renderSkan`&nbsp;7234, `szukajSieci`&nbsp;7286, `wybierzSiec`&nbsp;7294, `wyslijPolecenieSieci`&nbsp;7312, `siecZIndeksu`&nbsp;7322, `tzChanged`&nbsp;7356, `cfgTime`&nbsp;7361, `addSlot`&nbsp;7369, `zapiszPlanDnia`&nbsp;7382, `saveConfig`&nbsp;7400, `inrKrokiZakresu`&nbsp;7471, `opcjeInr`&nbsp;7478, `inrZakresZmieniony`&nbsp;7489, `wypelnijListyZakresu`&nbsp;7502, `saveInrRange`&nbsp;7513, `wypelnijListeOdstepu`&nbsp;7547, `saveInrEvery`&nbsp;7560, `odswiezPodpowiedzInr`&nbsp;7570

*ANALIZA* — `openTimeOf`&nbsp;7595, `openMinutes`&nbsp;7601, `sredniaPora`&nbsp;7625, `kwantyl`&nbsp;7633, `dniMiedzy`&nbsp;7641, `odstepyZPunktow`&nbsp;7655, `analyze`&nbsp;7664, `inrContext`&nbsp;7766

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7825, `rytmSVG`&nbsp;7837, `poryWCzasieSVG`&nbsp;7899, `iskraSVG`&nbsp;7967, `dowSVG`&nbsp;7995, `dniRytmu`&nbsp;8031, `skutecznoscTygodniami`&nbsp;8052, `renderAnalysis`&nbsp;8080

*RAPORT* — `collectRows`&nbsp;8187, `makeReport`&nbsp;8224

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;8375, `tagiPrzed`&nbsp;8383, `tagPrzelacz`&nbsp;8392

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;8435, `opisKopii`&nbsp;8445

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8490, `tgCzatKopii`&nbsp;8497, `odswiezKopie`&nbsp;8504, `tgKopiaCzatZapisz`&nbsp;8512, `tgKopiaCzatZnajdz`&nbsp;8532, `tgKopiaWlacz`&nbsp;8562, `tgKopiaWylacz`&nbsp;8581, `kopiaNaTelegram`&nbsp;8590, `kopiaAutomat`&nbsp;8641

*WIEK KOPII* — `dniOdDaty`&nbsp;8688, `wiekKopiiTxt`&nbsp;8694, `renderKopiaStan`&nbsp;8702, `zapiszKopie`&nbsp;8717

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8751, `ustawieniaDoOdtworzenia`&nbsp;8799, `wczytajKopie`&nbsp;8814, `kopiaCzytelna`&nbsp;8819, `odtworzKopie`&nbsp;8829, `kopiaWybrana`&nbsp;8862

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8886, `odtworzZBazy`&nbsp;8918, `exportCsv`&nbsp;8930

*NAWIGACJA* — `wrocZEkranu`&nbsp;9038


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
