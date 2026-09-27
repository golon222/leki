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

## `index.html` — 8675 linii, ~130 tys. tokenow

Ekrany (`<section>`) i dwa duze bloki. Zakladki `tab-*` odpowiadaja
pozycjom w pasku nawigacji i podekranom Ustawien.

| od | do | co |
|---|---|---|
| 21 | 21 | CSS — poczatek |
| 22 | 234 | SYSTEM WIZUALNY PillBox |
| 235 | 331 | EKRAN GŁÓWNY — KARTA DNIA |
| 332 | 404 | INFORMACJA ZWROTNA |
| 405 | 836 | TABLETKA W 3D |
| 837 | 996 | tab-cal |
| 997 | 1045 | tab-inr |
| 1046 | 1118 | tab-ana |
| 1119 | 1203 | tab-set |
| 1204 | 1275 | tab-lek |
| 1276 | 1285 | tab-pud |
| 1286 | 1305 | tab-sinr |
| 1306 | 1345 | tab-wifi |
| 1346 | 1446 | tab-tg |
| 1447 | 1480 | tab-dev |
| 1481 | 1581 | tab-diag |
| 1582 | 1857 | tab-help |
| 1858 | 1879 | tab-ev |
| 1880 | 1970 | tab-hist |
| 1971 | 1979 | JS — poczatek |
| 1980 | 2177 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2178 | 2252 | INFORMACJA ZWROTNA |
| 2253 | 2347 | STREFY CZASOWE |
| 2348 | 2375 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2376 | 2442 | TABLETKA JAKO BRYŁA |
| 2443 | 2457 | LOGOWANIE |
| 2458 | 2597 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2598 | 2978 | START |
| 2979 | 3039 | OSŁONA RYSOWANIA |
| 3040 | 3098 | REKONCYLIACJA |
| 3099 | 3303 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3304 | 3362 | KALENDARZ |
| 3363 | 3633 | HISTORIA ROZPISANIA DAWKI |
| 3634 | 3823 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3824 | 3975 | ARKUSZ DNIA |
| 3976 | 4075 | WZIĄŁEM TERAZ |
| 4076 | 4190 | INR |
| 4191 | 4330 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4331 | 4343 | STATUS PUDEŁKA |
| 4344 | 4493 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4494 | 4655 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4656 | 4938 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4939 | 4979 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4980 | 5076 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 5077 | 5595 | EKRAN ZDARZEN |
| 5596 | 5714 | ZAPAS TABLETEK |
| 5715 | 6052 | USTAWIENIA |
| 6053 | 6360 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6361 | 6817 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6818 | 7188 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7189 | 7399 | ANALIZA |
| 7400 | 7785 | WYKRESY ANALIZY |
| 7786 | 7942 | RAPORT |
| 7943 | 8004 | KONTEKST DNIA (TAGI) |
| 8005 | 8050 | KOPIA ZAPASOWA |
| 8051 | 8264 | KOPIA NA TELEGRAM |
| 8265 | 8330 | WIEK KOPII |
| 8331 | 8471 | ODTWARZANIE Z KOPII |
| 8472 | 8546 | KOPIE Z BAZY |
| 8547 | 8633 | NAWIGACJA |
| 8634 | 8675 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (224) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `pudelkoZnane`&nbsp;2011, `wybranePudelko`&nbsp;2013, `korzenDanych`&nbsp;2037, `odmowaRegul`&nbsp;2056, `sprawdzDostepPudelek`&nbsp;2061, `wybierzPudelko`&nbsp;2084, `profilTydzien`&nbsp;2133, `komoraDnia`&nbsp;2144, `ustawProfil`&nbsp;2159

*INFORMACJA ZWROTNA* — `toast`&nbsp;2199, `busy`&nbsp;2215, `todayKey`&nbsp;2242, `dzisiajKey`&nbsp;2246, `inNightWindow`&nbsp;2249

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2304, `tzName`&nbsp;2320, `tzLabel`&nbsp;2321, `tzOffsetTxt`&nbsp;2322, `devDate`&nbsp;2328, `devKey`&nbsp;2333, `devHM`&nbsp;2338, `slotMin`&nbsp;2339, `pillColors`&nbsp;2341

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2352

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2389, `cieniuj`&nbsp;2416, `doseGraphic`&nbsp;2433

*LOGOWANIE* — `doLogin`&nbsp;2447

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2467, `wyczyscCache`&nbsp;2562, `fbSignOut`&nbsp;2581

*START* — `boot`&nbsp;2599

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;3006, `rysujWszystkie`&nbsp;3019, `renderAll`&nbsp;3023

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;3046, `reconcileDecyzja`&nbsp;3085

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3120, `zapiszReconcile`&nbsp;3132, `doReconcile`&nbsp;3171, `doReconcileWewn`&nbsp;3181, `reconcile`&nbsp;3302

*KALENDARZ* — `tydzienDawek`&nbsp;3342

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3388, `dawkaNaDzien`&nbsp;3399, `dzienBezLeku`&nbsp;3420, `wyjatekNaDzien`&nbsp;3425, `opisDawkowania`&nbsp;3431, `dayDose`&nbsp;3445, `dzienZamkniety`&nbsp;3479, `trackingSince`&nbsp;3485, `beforeTracking`&nbsp;3486, `dayStatus`&nbsp;3488, `renderCalendar`&nbsp;3529, `seriaDni`&nbsp;3594, `doNastepnej`&nbsp;3612, `opisCzasu`&nbsp;3627

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3642, `trwanieTxt`&nbsp;3657, `kiedyDawkaTxt`&nbsp;3668, `odswiezOdDawki`&nbsp;3676, `startTikOdDawki`&nbsp;3690, `renderToday`&nbsp;3700

*ARKUSZ DNIA* — `closeSheet`&nbsp;3835, `renderSheet`&nbsp;3837, `resetDose`&nbsp;3915, `resetPlan`&nbsp;3922, `commitPlan`&nbsp;3927, `clearPlan`&nbsp;3943, `commitDose`&nbsp;3955

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3996, `askConfirm`&nbsp;4065

*INR* — `inrState`&nbsp;4077, `odswiezTerminInr`&nbsp;4090, `addInr`&nbsp;4099, `inrKeysOk`&nbsp;4185

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4200, `inrTerminKey`&nbsp;4208, `inrDoTerminu`&nbsp;4220, `dniTxt`&nbsp;4229, `renderInr`&nbsp;4231, `inrChart`&nbsp;4303

*STATUS PUDEŁKA* — `relTime`&nbsp;4332, `devDayMon`&nbsp;4341

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4376, `renderBoxLog`&nbsp;4422, `logPrzelacz`&nbsp;4458, `renderNvsFailLog`&nbsp;4466

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4511, `oczekWczytaj`&nbsp;4519, `oczekZapisz`&nbsp;4524, `oczekIle`&nbsp;4527, `zapiszPewnie`&nbsp;4537, `zapiszCfg`&nbsp;4575, `bazaOdmowila`&nbsp;4593, `oczekWyslij`&nbsp;4617, `oczekOdmowy`&nbsp;4654

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4670, `ostrzReguly`&nbsp;4692, `lm`&nbsp;4731, `ostrzMilczy`&nbsp;4737, `nvsMalo`&nbsp;4817, `opisNvsFailKey`&nbsp;4829, `stratyDotyczaLeku`&nbsp;4882, `ostrzStraty`&nbsp;4894, `stratyCicho`&nbsp;4928

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4960

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4990, `renderOstrzezenia`&nbsp;5005, `bezPokrycia`&nbsp;5015, `wierszZdarzenia`&nbsp;5021, `renderDiag`&nbsp;5038

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5093, `evPasuje`&nbsp;5098, `renderEvents`&nbsp;5110, `renderOpenWarn`&nbsp;5150, `minutyDoPelna`&nbsp;5203, `opisLadowania`&nbsp;5215, `dni`&nbsp;5235, `opisLadowan`&nbsp;5238, `tempoZHistorii`&nbsp;5291, `prognozaDni`&nbsp;5300, `opisPrognozy`&nbsp;5312, `czasKrotko`&nbsp;5334, `opisCzuwania`&nbsp;5342, `renderStatus`&nbsp;5360

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5599, `dayAfter`&nbsp;5602, `pillsBaseInfo`&nbsp;5613, `settlePills`&nbsp;5623, `dniZapasu`&nbsp;5663, `renderPills`&nbsp;5676, `savePills`&nbsp;5697, `setPills`&nbsp;5708

*USTAWIENIA* — `renderKafelki`&nbsp;5719, `renderPudelka`&nbsp;5753, `renderSettings`&nbsp;5795, `tydzienZPol`&nbsp;5851, `renderWeekEditor`&nbsp;5863, `odswiezPodpowiedzTygodnia`&nbsp;5880, `tydzienZmieniony`&nbsp;5894, `rownajTydzien`&nbsp;5895, `renderPlanList`&nbsp;5943, `renderExceptions`&nbsp;5970, `wyslijSiec`&nbsp;6028

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6070, `tgZapytaj`&nbsp;6080, `tgKodParowania`&nbsp;6120, `tgZnajdzCzat`&nbsp;6136, `tgPolacz`&nbsp;6204, `tgProbna`&nbsp;6237, `tgOdlacz`&nbsp;6244, `renderTgStan`&nbsp;6266

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6381, `pobierzOpisFirmware`&nbsp;6387, `wyslijAktualizacje`&nbsp;6408, `anulujAktualizacje`&nbsp;6453, `renderOta`&nbsp;6459, `renderNetStan`&nbsp;6744

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6829, `renderSkan`&nbsp;6835, `szukajSieci`&nbsp;6887, `wybierzSiec`&nbsp;6895, `wyslijPolecenieSieci`&nbsp;6913, `siecZIndeksu`&nbsp;6923, `tzChanged`&nbsp;6957, `cfgTime`&nbsp;6962, `addSlot`&nbsp;6970, `zapiszPlanDnia`&nbsp;6983, `saveConfig`&nbsp;7001, `inrKrokiZakresu`&nbsp;7071, `opcjeInr`&nbsp;7078, `inrZakresZmieniony`&nbsp;7089, `wypelnijListyZakresu`&nbsp;7102, `saveInrRange`&nbsp;7113, `wypelnijListeOdstepu`&nbsp;7147, `saveInrEvery`&nbsp;7160, `odswiezPodpowiedzInr`&nbsp;7170

*ANALIZA* — `openTimeOf`&nbsp;7195, `openMinutes`&nbsp;7201, `sredniaPora`&nbsp;7225, `kwantyl`&nbsp;7233, `dniMiedzy`&nbsp;7241, `odstepyZPunktow`&nbsp;7255, `analyze`&nbsp;7264, `inrContext`&nbsp;7366

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7425, `rytmSVG`&nbsp;7437, `poryWCzasieSVG`&nbsp;7499, `iskraSVG`&nbsp;7567, `dowSVG`&nbsp;7595, `dniRytmu`&nbsp;7631, `skutecznoscTygodniami`&nbsp;7652, `renderAnalysis`&nbsp;7680

*RAPORT* — `collectRows`&nbsp;7787, `makeReport`&nbsp;7824

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7975, `tagiPrzed`&nbsp;7983, `tagPrzelacz`&nbsp;7992

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;8035, `opisKopii`&nbsp;8045

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8085, `tgCzatKopii`&nbsp;8092, `odswiezKopie`&nbsp;8099, `tgKopiaCzatZapisz`&nbsp;8107, `tgKopiaCzatZnajdz`&nbsp;8127, `tgKopiaWlacz`&nbsp;8157, `tgKopiaWylacz`&nbsp;8176, `kopiaNaTelegram`&nbsp;8185, `kopiaAutomat`&nbsp;8236

*WIEK KOPII* — `dniOdDaty`&nbsp;8283, `wiekKopiiTxt`&nbsp;8289, `renderKopiaStan`&nbsp;8297, `zapiszKopie`&nbsp;8312

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8346, `ustawieniaDoOdtworzenia`&nbsp;8394, `wczytajKopie`&nbsp;8409, `kopiaCzytelna`&nbsp;8414, `odtworzKopie`&nbsp;8424, `kopiaWybrana`&nbsp;8457

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8481, `odtworzZBazy`&nbsp;8513, `exportCsv`&nbsp;8525

*NAWIGACJA* — `wrocZEkranu`&nbsp;8632


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
