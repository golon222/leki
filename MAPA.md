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

## `index.html` — 9057 linii, ~130 tys. tokenow

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
| 2156 | 2377 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2378 | 2452 | INFORMACJA ZWROTNA |
| 2453 | 2547 | STREFY CZASOWE |
| 2548 | 2575 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2576 | 2642 | TABLETKA JAKO BRYŁA |
| 2643 | 2657 | LOGOWANIE |
| 2658 | 2797 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2798 | 3178 | START |
| 3179 | 3239 | OSŁONA RYSOWANIA |
| 3240 | 3298 | REKONCYLIACJA |
| 3299 | 3503 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3504 | 3562 | KALENDARZ |
| 3563 | 3833 | HISTORIA ROZPISANIA DAWKI |
| 3834 | 4033 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 4034 | 4185 | ARKUSZ DNIA |
| 4186 | 4285 | WZIĄŁEM TERAZ |
| 4286 | 4400 | INR |
| 4401 | 4540 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4541 | 4553 | STATUS PUDEŁKA |
| 4554 | 4703 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4704 | 4865 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4866 | 5148 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 5149 | 5189 | KOLEJKA, KTÓRA NIE SCHODZI |
| 5190 | 5286 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 5287 | 5934 | EKRAN ZDARZEN |
| 5935 | 6053 | ZAPAS TABLETEK |
| 6054 | 6408 | USTAWIENIA |
| 6409 | 6716 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6717 | 7192 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 7193 | 7564 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7565 | 7775 | ANALIZA |
| 7776 | 8161 | WYKRESY ANALIZY |
| 8162 | 8318 | RAPORT |
| 8319 | 8380 | KONTEKST DNIA (TAGI) |
| 8381 | 8431 | KOPIA ZAPASOWA |
| 8432 | 8645 | KOPIA NA TELEGRAM |
| 8646 | 8711 | WIEK KOPII |
| 8712 | 8852 | ODTWARZANIE Z KOPII |
| 8853 | 8927 | KOPIE Z BAZY |
| 8928 | 9015 | NAWIGACJA |
| 9016 | 9057 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (229) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `opisPlikFirmware`&nbsp;2197, `wersjaZTelegramem`&nbsp;2206, `pudelkoZnane`&nbsp;2212, `wybranePudelko`&nbsp;2214, `korzenDanych`&nbsp;2238, `odmowaRegul`&nbsp;2257, `sprawdzDostepPudelek`&nbsp;2262, `wybierzPudelko`&nbsp;2285, `profilTydzien`&nbsp;2347, `ustawProfil`&nbsp;2352

*INFORMACJA ZWROTNA* — `toast`&nbsp;2399, `busy`&nbsp;2415, `todayKey`&nbsp;2442, `dzisiajKey`&nbsp;2446, `inNightWindow`&nbsp;2449

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2504, `tzName`&nbsp;2520, `tzLabel`&nbsp;2521, `tzOffsetTxt`&nbsp;2522, `devDate`&nbsp;2528, `devKey`&nbsp;2533, `devHM`&nbsp;2538, `slotMin`&nbsp;2539, `pillColors`&nbsp;2541

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2552

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2589, `cieniuj`&nbsp;2616, `doseGraphic`&nbsp;2633

*LOGOWANIE* — `doLogin`&nbsp;2647

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2667, `wyczyscCache`&nbsp;2762, `fbSignOut`&nbsp;2781

*START* — `boot`&nbsp;2799

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;3206, `rysujWszystkie`&nbsp;3219, `renderAll`&nbsp;3223

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;3246, `reconcileDecyzja`&nbsp;3285

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3320, `zapiszReconcile`&nbsp;3332, `doReconcile`&nbsp;3371, `doReconcileWewn`&nbsp;3381, `reconcile`&nbsp;3502

*KALENDARZ* — `tydzienDawek`&nbsp;3542

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3588, `dawkaNaDzien`&nbsp;3599, `dzienBezLeku`&nbsp;3620, `wyjatekNaDzien`&nbsp;3625, `opisDawkowania`&nbsp;3631, `dayDose`&nbsp;3645, `dzienZamkniety`&nbsp;3679, `trackingSince`&nbsp;3685, `beforeTracking`&nbsp;3686, `dayStatus`&nbsp;3688, `renderCalendar`&nbsp;3729, `seriaDni`&nbsp;3794, `doNastepnej`&nbsp;3812, `opisCzasu`&nbsp;3827

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3842, `trwanieTxt`&nbsp;3857, `kiedyDawkaTxt`&nbsp;3868, `odswiezOdDawki`&nbsp;3876, `startTikOdDawki`&nbsp;3890, `renderToday`&nbsp;3900

*ARKUSZ DNIA* — `closeSheet`&nbsp;4045, `renderSheet`&nbsp;4047, `resetDose`&nbsp;4125, `resetPlan`&nbsp;4132, `commitPlan`&nbsp;4137, `clearPlan`&nbsp;4153, `commitDose`&nbsp;4165

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;4206, `askConfirm`&nbsp;4275

*INR* — `inrState`&nbsp;4287, `odswiezTerminInr`&nbsp;4300, `addInr`&nbsp;4309, `inrKeysOk`&nbsp;4395

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4410, `inrTerminKey`&nbsp;4418, `inrDoTerminu`&nbsp;4430, `dniTxt`&nbsp;4439, `renderInr`&nbsp;4441, `inrChart`&nbsp;4513

*STATUS PUDEŁKA* — `relTime`&nbsp;4542, `devDayMon`&nbsp;4551

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4586, `renderBoxLog`&nbsp;4632, `logPrzelacz`&nbsp;4668, `renderNvsFailLog`&nbsp;4676

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4721, `oczekWczytaj`&nbsp;4729, `oczekZapisz`&nbsp;4734, `oczekIle`&nbsp;4737, `zapiszPewnie`&nbsp;4747, `zapiszCfg`&nbsp;4785, `bazaOdmowila`&nbsp;4803, `oczekWyslij`&nbsp;4827, `oczekOdmowy`&nbsp;4864

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4880, `ostrzReguly`&nbsp;4902, `lm`&nbsp;4941, `ostrzMilczy`&nbsp;4947, `nvsMalo`&nbsp;5027, `opisNvsFailKey`&nbsp;5039, `stratyDotyczaLeku`&nbsp;5092, `ostrzStraty`&nbsp;5104, `stratyCicho`&nbsp;5138

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;5170

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;5200, `renderOstrzezenia`&nbsp;5215, `bezPokrycia`&nbsp;5225, `wierszZdarzenia`&nbsp;5231, `renderDiag`&nbsp;5248

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5303, `evPasuje`&nbsp;5308, `renderEvents`&nbsp;5320, `opisPomiaruBaterii`&nbsp;5380, `nazwaOtwarcia`&nbsp;5392, `renderOpenWarn`&nbsp;5400, `minutyDoPelna`&nbsp;5453, `opisLadowania`&nbsp;5465, `opisLadowaniaCzujnik`&nbsp;5490, `dni`&nbsp;5506, `opisLadowan`&nbsp;5509, `tempoZHistorii`&nbsp;5562, `prognozaDni`&nbsp;5571, `opisPrognozy`&nbsp;5583, `czasKrotko`&nbsp;5605, `opisCzuwania`&nbsp;5613, `renderStatus`&nbsp;5631

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5938, `dayAfter`&nbsp;5941, `pillsBaseInfo`&nbsp;5952, `settlePills`&nbsp;5962, `dniZapasu`&nbsp;6002, `renderPills`&nbsp;6015, `savePills`&nbsp;6036, `setPills`&nbsp;6047

*USTAWIENIA* — `opisLeku`&nbsp;6065, `renderKafelki`&nbsp;6071, `renderPudelka`&nbsp;6106, `renderSettings`&nbsp;6148, `tydzienZPol`&nbsp;6207, `renderWeekEditor`&nbsp;6219, `odswiezPodpowiedzTygodnia`&nbsp;6236, `tydzienZmieniony`&nbsp;6250, `rownajTydzien`&nbsp;6251, `renderPlanList`&nbsp;6299, `renderExceptions`&nbsp;6326, `wyslijSiec`&nbsp;6384

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6426, `tgZapytaj`&nbsp;6436, `tgKodParowania`&nbsp;6476, `tgZnajdzCzat`&nbsp;6492, `tgPolacz`&nbsp;6560, `tgProbna`&nbsp;6593, `tgOdlacz`&nbsp;6600, `renderTgStan`&nbsp;6622

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6737, `pobierzOpisFirmware`&nbsp;6743, `wyslijAktualizacje`&nbsp;6766, `anulujAktualizacje`&nbsp;6811, `renderOta`&nbsp;6817, `renderNetStan`&nbsp;7119

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;7204, `renderSkan`&nbsp;7210, `szukajSieci`&nbsp;7262, `wybierzSiec`&nbsp;7270, `wyslijPolecenieSieci`&nbsp;7288, `siecZIndeksu`&nbsp;7298, `tzChanged`&nbsp;7332, `cfgTime`&nbsp;7337, `addSlot`&nbsp;7345, `zapiszPlanDnia`&nbsp;7358, `saveConfig`&nbsp;7376, `inrKrokiZakresu`&nbsp;7447, `opcjeInr`&nbsp;7454, `inrZakresZmieniony`&nbsp;7465, `wypelnijListyZakresu`&nbsp;7478, `saveInrRange`&nbsp;7489, `wypelnijListeOdstepu`&nbsp;7523, `saveInrEvery`&nbsp;7536, `odswiezPodpowiedzInr`&nbsp;7546

*ANALIZA* — `openTimeOf`&nbsp;7571, `openMinutes`&nbsp;7577, `sredniaPora`&nbsp;7601, `kwantyl`&nbsp;7609, `dniMiedzy`&nbsp;7617, `odstepyZPunktow`&nbsp;7631, `analyze`&nbsp;7640, `inrContext`&nbsp;7742

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7801, `rytmSVG`&nbsp;7813, `poryWCzasieSVG`&nbsp;7875, `iskraSVG`&nbsp;7943, `dowSVG`&nbsp;7971, `dniRytmu`&nbsp;8007, `skutecznoscTygodniami`&nbsp;8028, `renderAnalysis`&nbsp;8056

*RAPORT* — `collectRows`&nbsp;8163, `makeReport`&nbsp;8200

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;8351, `tagiPrzed`&nbsp;8359, `tagPrzelacz`&nbsp;8368

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;8411, `opisKopii`&nbsp;8421

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8466, `tgCzatKopii`&nbsp;8473, `odswiezKopie`&nbsp;8480, `tgKopiaCzatZapisz`&nbsp;8488, `tgKopiaCzatZnajdz`&nbsp;8508, `tgKopiaWlacz`&nbsp;8538, `tgKopiaWylacz`&nbsp;8557, `kopiaNaTelegram`&nbsp;8566, `kopiaAutomat`&nbsp;8617

*WIEK KOPII* — `dniOdDaty`&nbsp;8664, `wiekKopiiTxt`&nbsp;8670, `renderKopiaStan`&nbsp;8678, `zapiszKopie`&nbsp;8693

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8727, `ustawieniaDoOdtworzenia`&nbsp;8775, `wczytajKopie`&nbsp;8790, `kopiaCzytelna`&nbsp;8795, `odtworzKopie`&nbsp;8805, `kopiaWybrana`&nbsp;8838

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8862, `odtworzZBazy`&nbsp;8894, `exportCsv`&nbsp;8906

*NAWIGACJA* — `wrocZEkranu`&nbsp;9014


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
