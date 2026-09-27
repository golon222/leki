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

## `index.html` — 8762 linii, ~130 tys. tokenow

Ekrany (`<section>`) i dwa duze bloki. Zakladki `tab-*` odpowiadaja
pozycjom w pasku nawigacji i podekranom Ustawien.

| od | do | co |
|---|---|---|
| 21 | 21 | CSS — poczatek |
| 22 | 263 | SYSTEM WIZUALNY PillBox |
| 264 | 360 | EKRAN GŁÓWNY — KARTA DNIA |
| 361 | 433 | INFORMACJA ZWROTNA |
| 434 | 898 | TABLETKA W 3D |
| 899 | 1058 | tab-cal |
| 1059 | 1107 | tab-inr |
| 1108 | 1180 | tab-ana |
| 1181 | 1265 | tab-set |
| 1266 | 1337 | tab-lek |
| 1338 | 1347 | tab-pud |
| 1348 | 1367 | tab-sinr |
| 1368 | 1407 | tab-wifi |
| 1408 | 1508 | tab-tg |
| 1509 | 1542 | tab-dev |
| 1543 | 1643 | tab-diag |
| 1644 | 1919 | tab-help |
| 1920 | 1941 | tab-ev |
| 1942 | 2032 | tab-hist |
| 2033 | 2041 | JS — poczatek |
| 2042 | 2262 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2263 | 2337 | INFORMACJA ZWROTNA |
| 2338 | 2432 | STREFY CZASOWE |
| 2433 | 2460 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2461 | 2527 | TABLETKA JAKO BRYŁA |
| 2528 | 2542 | LOGOWANIE |
| 2543 | 2682 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2683 | 3063 | START |
| 3064 | 3124 | OSŁONA RYSOWANIA |
| 3125 | 3183 | REKONCYLIACJA |
| 3184 | 3388 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3389 | 3447 | KALENDARZ |
| 3448 | 3718 | HISTORIA ROZPISANIA DAWKI |
| 3719 | 3908 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3909 | 4060 | ARKUSZ DNIA |
| 4061 | 4160 | WZIĄŁEM TERAZ |
| 4161 | 4275 | INR |
| 4276 | 4415 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4416 | 4428 | STATUS PUDEŁKA |
| 4429 | 4578 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4579 | 4740 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4741 | 5023 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 5024 | 5064 | KOLEJKA, KTÓRA NIE SCHODZI |
| 5065 | 5161 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 5162 | 5680 | EKRAN ZDARZEN |
| 5681 | 5799 | ZAPAS TABLETEK |
| 5800 | 6137 | USTAWIENIA |
| 6138 | 6445 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6446 | 6904 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6905 | 7275 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7276 | 7486 | ANALIZA |
| 7487 | 7872 | WYKRESY ANALIZY |
| 7873 | 8029 | RAPORT |
| 8030 | 8091 | KONTEKST DNIA (TAGI) |
| 8092 | 8137 | KOPIA ZAPASOWA |
| 8138 | 8351 | KOPIA NA TELEGRAM |
| 8352 | 8417 | WIEK KOPII |
| 8418 | 8558 | ODTWARZANIE Z KOPII |
| 8559 | 8633 | KOPIE Z BAZY |
| 8634 | 8720 | NAWIGACJA |
| 8721 | 8762 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (225) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `opisPlikFirmware`&nbsp;2083, `pudelkoZnane`&nbsp;2089, `wybranePudelko`&nbsp;2091, `korzenDanych`&nbsp;2115, `odmowaRegul`&nbsp;2134, `sprawdzDostepPudelek`&nbsp;2139, `wybierzPudelko`&nbsp;2162, `profilTydzien`&nbsp;2211, `komoraDnia`&nbsp;2222, `ustawProfil`&nbsp;2237

*INFORMACJA ZWROTNA* — `toast`&nbsp;2284, `busy`&nbsp;2300, `todayKey`&nbsp;2327, `dzisiajKey`&nbsp;2331, `inNightWindow`&nbsp;2334

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2389, `tzName`&nbsp;2405, `tzLabel`&nbsp;2406, `tzOffsetTxt`&nbsp;2407, `devDate`&nbsp;2413, `devKey`&nbsp;2418, `devHM`&nbsp;2423, `slotMin`&nbsp;2424, `pillColors`&nbsp;2426

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2437

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2474, `cieniuj`&nbsp;2501, `doseGraphic`&nbsp;2518

*LOGOWANIE* — `doLogin`&nbsp;2532

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2552, `wyczyscCache`&nbsp;2647, `fbSignOut`&nbsp;2666

*START* — `boot`&nbsp;2684

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;3091, `rysujWszystkie`&nbsp;3104, `renderAll`&nbsp;3108

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;3131, `reconcileDecyzja`&nbsp;3170

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3205, `zapiszReconcile`&nbsp;3217, `doReconcile`&nbsp;3256, `doReconcileWewn`&nbsp;3266, `reconcile`&nbsp;3387

*KALENDARZ* — `tydzienDawek`&nbsp;3427

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3473, `dawkaNaDzien`&nbsp;3484, `dzienBezLeku`&nbsp;3505, `wyjatekNaDzien`&nbsp;3510, `opisDawkowania`&nbsp;3516, `dayDose`&nbsp;3530, `dzienZamkniety`&nbsp;3564, `trackingSince`&nbsp;3570, `beforeTracking`&nbsp;3571, `dayStatus`&nbsp;3573, `renderCalendar`&nbsp;3614, `seriaDni`&nbsp;3679, `doNastepnej`&nbsp;3697, `opisCzasu`&nbsp;3712

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3727, `trwanieTxt`&nbsp;3742, `kiedyDawkaTxt`&nbsp;3753, `odswiezOdDawki`&nbsp;3761, `startTikOdDawki`&nbsp;3775, `renderToday`&nbsp;3785

*ARKUSZ DNIA* — `closeSheet`&nbsp;3920, `renderSheet`&nbsp;3922, `resetDose`&nbsp;4000, `resetPlan`&nbsp;4007, `commitPlan`&nbsp;4012, `clearPlan`&nbsp;4028, `commitDose`&nbsp;4040

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;4081, `askConfirm`&nbsp;4150

*INR* — `inrState`&nbsp;4162, `odswiezTerminInr`&nbsp;4175, `addInr`&nbsp;4184, `inrKeysOk`&nbsp;4270

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4285, `inrTerminKey`&nbsp;4293, `inrDoTerminu`&nbsp;4305, `dniTxt`&nbsp;4314, `renderInr`&nbsp;4316, `inrChart`&nbsp;4388

*STATUS PUDEŁKA* — `relTime`&nbsp;4417, `devDayMon`&nbsp;4426

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4461, `renderBoxLog`&nbsp;4507, `logPrzelacz`&nbsp;4543, `renderNvsFailLog`&nbsp;4551

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4596, `oczekWczytaj`&nbsp;4604, `oczekZapisz`&nbsp;4609, `oczekIle`&nbsp;4612, `zapiszPewnie`&nbsp;4622, `zapiszCfg`&nbsp;4660, `bazaOdmowila`&nbsp;4678, `oczekWyslij`&nbsp;4702, `oczekOdmowy`&nbsp;4739

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4755, `ostrzReguly`&nbsp;4777, `lm`&nbsp;4816, `ostrzMilczy`&nbsp;4822, `nvsMalo`&nbsp;4902, `opisNvsFailKey`&nbsp;4914, `stratyDotyczaLeku`&nbsp;4967, `ostrzStraty`&nbsp;4979, `stratyCicho`&nbsp;5013

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;5045

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;5075, `renderOstrzezenia`&nbsp;5090, `bezPokrycia`&nbsp;5100, `wierszZdarzenia`&nbsp;5106, `renderDiag`&nbsp;5123

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5178, `evPasuje`&nbsp;5183, `renderEvents`&nbsp;5195, `renderOpenWarn`&nbsp;5235, `minutyDoPelna`&nbsp;5288, `opisLadowania`&nbsp;5300, `dni`&nbsp;5320, `opisLadowan`&nbsp;5323, `tempoZHistorii`&nbsp;5376, `prognozaDni`&nbsp;5385, `opisPrognozy`&nbsp;5397, `czasKrotko`&nbsp;5419, `opisCzuwania`&nbsp;5427, `renderStatus`&nbsp;5445

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5684, `dayAfter`&nbsp;5687, `pillsBaseInfo`&nbsp;5698, `settlePills`&nbsp;5708, `dniZapasu`&nbsp;5748, `renderPills`&nbsp;5761, `savePills`&nbsp;5782, `setPills`&nbsp;5793

*USTAWIENIA* — `renderKafelki`&nbsp;5804, `renderPudelka`&nbsp;5838, `renderSettings`&nbsp;5880, `tydzienZPol`&nbsp;5936, `renderWeekEditor`&nbsp;5948, `odswiezPodpowiedzTygodnia`&nbsp;5965, `tydzienZmieniony`&nbsp;5979, `rownajTydzien`&nbsp;5980, `renderPlanList`&nbsp;6028, `renderExceptions`&nbsp;6055, `wyslijSiec`&nbsp;6113

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6155, `tgZapytaj`&nbsp;6165, `tgKodParowania`&nbsp;6205, `tgZnajdzCzat`&nbsp;6221, `tgPolacz`&nbsp;6289, `tgProbna`&nbsp;6322, `tgOdlacz`&nbsp;6329, `renderTgStan`&nbsp;6351

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6466, `pobierzOpisFirmware`&nbsp;6472, `wyslijAktualizacje`&nbsp;6495, `anulujAktualizacje`&nbsp;6540, `renderOta`&nbsp;6546, `renderNetStan`&nbsp;6831

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6916, `renderSkan`&nbsp;6922, `szukajSieci`&nbsp;6974, `wybierzSiec`&nbsp;6982, `wyslijPolecenieSieci`&nbsp;7000, `siecZIndeksu`&nbsp;7010, `tzChanged`&nbsp;7044, `cfgTime`&nbsp;7049, `addSlot`&nbsp;7057, `zapiszPlanDnia`&nbsp;7070, `saveConfig`&nbsp;7088, `inrKrokiZakresu`&nbsp;7158, `opcjeInr`&nbsp;7165, `inrZakresZmieniony`&nbsp;7176, `wypelnijListyZakresu`&nbsp;7189, `saveInrRange`&nbsp;7200, `wypelnijListeOdstepu`&nbsp;7234, `saveInrEvery`&nbsp;7247, `odswiezPodpowiedzInr`&nbsp;7257

*ANALIZA* — `openTimeOf`&nbsp;7282, `openMinutes`&nbsp;7288, `sredniaPora`&nbsp;7312, `kwantyl`&nbsp;7320, `dniMiedzy`&nbsp;7328, `odstepyZPunktow`&nbsp;7342, `analyze`&nbsp;7351, `inrContext`&nbsp;7453

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7512, `rytmSVG`&nbsp;7524, `poryWCzasieSVG`&nbsp;7586, `iskraSVG`&nbsp;7654, `dowSVG`&nbsp;7682, `dniRytmu`&nbsp;7718, `skutecznoscTygodniami`&nbsp;7739, `renderAnalysis`&nbsp;7767

*RAPORT* — `collectRows`&nbsp;7874, `makeReport`&nbsp;7911

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;8062, `tagiPrzed`&nbsp;8070, `tagPrzelacz`&nbsp;8079

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;8122, `opisKopii`&nbsp;8132

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8172, `tgCzatKopii`&nbsp;8179, `odswiezKopie`&nbsp;8186, `tgKopiaCzatZapisz`&nbsp;8194, `tgKopiaCzatZnajdz`&nbsp;8214, `tgKopiaWlacz`&nbsp;8244, `tgKopiaWylacz`&nbsp;8263, `kopiaNaTelegram`&nbsp;8272, `kopiaAutomat`&nbsp;8323

*WIEK KOPII* — `dniOdDaty`&nbsp;8370, `wiekKopiiTxt`&nbsp;8376, `renderKopiaStan`&nbsp;8384, `zapiszKopie`&nbsp;8399

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8433, `ustawieniaDoOdtworzenia`&nbsp;8481, `wczytajKopie`&nbsp;8496, `kopiaCzytelna`&nbsp;8501, `odtworzKopie`&nbsp;8511, `kopiaWybrana`&nbsp;8544

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8568, `odtworzZBazy`&nbsp;8600, `exportCsv`&nbsp;8612

*NAWIGACJA* — `wrocZEkranu`&nbsp;8719


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
