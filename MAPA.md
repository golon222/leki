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

## `index.html` — 8744 linii, ~130 tys. tokenow

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
| 2042 | 2246 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2247 | 2321 | INFORMACJA ZWROTNA |
| 2322 | 2416 | STREFY CZASOWE |
| 2417 | 2444 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2445 | 2511 | TABLETKA JAKO BRYŁA |
| 2512 | 2526 | LOGOWANIE |
| 2527 | 2666 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2667 | 3047 | START |
| 3048 | 3108 | OSŁONA RYSOWANIA |
| 3109 | 3167 | REKONCYLIACJA |
| 3168 | 3372 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3373 | 3431 | KALENDARZ |
| 3432 | 3702 | HISTORIA ROZPISANIA DAWKI |
| 3703 | 3892 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3893 | 4044 | ARKUSZ DNIA |
| 4045 | 4144 | WZIĄŁEM TERAZ |
| 4145 | 4259 | INR |
| 4260 | 4399 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4400 | 4412 | STATUS PUDEŁKA |
| 4413 | 4562 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4563 | 4724 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4725 | 5007 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 5008 | 5048 | KOLEJKA, KTÓRA NIE SCHODZI |
| 5049 | 5145 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 5146 | 5664 | EKRAN ZDARZEN |
| 5665 | 5783 | ZAPAS TABLETEK |
| 5784 | 6121 | USTAWIENIA |
| 6122 | 6429 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6430 | 6886 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6887 | 7257 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7258 | 7468 | ANALIZA |
| 7469 | 7854 | WYKRESY ANALIZY |
| 7855 | 8011 | RAPORT |
| 8012 | 8073 | KONTEKST DNIA (TAGI) |
| 8074 | 8119 | KOPIA ZAPASOWA |
| 8120 | 8333 | KOPIA NA TELEGRAM |
| 8334 | 8399 | WIEK KOPII |
| 8400 | 8540 | ODTWARZANIE Z KOPII |
| 8541 | 8615 | KOPIE Z BAZY |
| 8616 | 8702 | NAWIGACJA |
| 8703 | 8744 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (224) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `pudelkoZnane`&nbsp;2073, `wybranePudelko`&nbsp;2075, `korzenDanych`&nbsp;2099, `odmowaRegul`&nbsp;2118, `sprawdzDostepPudelek`&nbsp;2123, `wybierzPudelko`&nbsp;2146, `profilTydzien`&nbsp;2195, `komoraDnia`&nbsp;2206, `ustawProfil`&nbsp;2221

*INFORMACJA ZWROTNA* — `toast`&nbsp;2268, `busy`&nbsp;2284, `todayKey`&nbsp;2311, `dzisiajKey`&nbsp;2315, `inNightWindow`&nbsp;2318

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2373, `tzName`&nbsp;2389, `tzLabel`&nbsp;2390, `tzOffsetTxt`&nbsp;2391, `devDate`&nbsp;2397, `devKey`&nbsp;2402, `devHM`&nbsp;2407, `slotMin`&nbsp;2408, `pillColors`&nbsp;2410

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2421

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2458, `cieniuj`&nbsp;2485, `doseGraphic`&nbsp;2502

*LOGOWANIE* — `doLogin`&nbsp;2516

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2536, `wyczyscCache`&nbsp;2631, `fbSignOut`&nbsp;2650

*START* — `boot`&nbsp;2668

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;3075, `rysujWszystkie`&nbsp;3088, `renderAll`&nbsp;3092

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;3115, `reconcileDecyzja`&nbsp;3154

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3189, `zapiszReconcile`&nbsp;3201, `doReconcile`&nbsp;3240, `doReconcileWewn`&nbsp;3250, `reconcile`&nbsp;3371

*KALENDARZ* — `tydzienDawek`&nbsp;3411

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3457, `dawkaNaDzien`&nbsp;3468, `dzienBezLeku`&nbsp;3489, `wyjatekNaDzien`&nbsp;3494, `opisDawkowania`&nbsp;3500, `dayDose`&nbsp;3514, `dzienZamkniety`&nbsp;3548, `trackingSince`&nbsp;3554, `beforeTracking`&nbsp;3555, `dayStatus`&nbsp;3557, `renderCalendar`&nbsp;3598, `seriaDni`&nbsp;3663, `doNastepnej`&nbsp;3681, `opisCzasu`&nbsp;3696

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3711, `trwanieTxt`&nbsp;3726, `kiedyDawkaTxt`&nbsp;3737, `odswiezOdDawki`&nbsp;3745, `startTikOdDawki`&nbsp;3759, `renderToday`&nbsp;3769

*ARKUSZ DNIA* — `closeSheet`&nbsp;3904, `renderSheet`&nbsp;3906, `resetDose`&nbsp;3984, `resetPlan`&nbsp;3991, `commitPlan`&nbsp;3996, `clearPlan`&nbsp;4012, `commitDose`&nbsp;4024

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;4065, `askConfirm`&nbsp;4134

*INR* — `inrState`&nbsp;4146, `odswiezTerminInr`&nbsp;4159, `addInr`&nbsp;4168, `inrKeysOk`&nbsp;4254

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4269, `inrTerminKey`&nbsp;4277, `inrDoTerminu`&nbsp;4289, `dniTxt`&nbsp;4298, `renderInr`&nbsp;4300, `inrChart`&nbsp;4372

*STATUS PUDEŁKA* — `relTime`&nbsp;4401, `devDayMon`&nbsp;4410

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4445, `renderBoxLog`&nbsp;4491, `logPrzelacz`&nbsp;4527, `renderNvsFailLog`&nbsp;4535

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4580, `oczekWczytaj`&nbsp;4588, `oczekZapisz`&nbsp;4593, `oczekIle`&nbsp;4596, `zapiszPewnie`&nbsp;4606, `zapiszCfg`&nbsp;4644, `bazaOdmowila`&nbsp;4662, `oczekWyslij`&nbsp;4686, `oczekOdmowy`&nbsp;4723

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4739, `ostrzReguly`&nbsp;4761, `lm`&nbsp;4800, `ostrzMilczy`&nbsp;4806, `nvsMalo`&nbsp;4886, `opisNvsFailKey`&nbsp;4898, `stratyDotyczaLeku`&nbsp;4951, `ostrzStraty`&nbsp;4963, `stratyCicho`&nbsp;4997

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;5029

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;5059, `renderOstrzezenia`&nbsp;5074, `bezPokrycia`&nbsp;5084, `wierszZdarzenia`&nbsp;5090, `renderDiag`&nbsp;5107

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5162, `evPasuje`&nbsp;5167, `renderEvents`&nbsp;5179, `renderOpenWarn`&nbsp;5219, `minutyDoPelna`&nbsp;5272, `opisLadowania`&nbsp;5284, `dni`&nbsp;5304, `opisLadowan`&nbsp;5307, `tempoZHistorii`&nbsp;5360, `prognozaDni`&nbsp;5369, `opisPrognozy`&nbsp;5381, `czasKrotko`&nbsp;5403, `opisCzuwania`&nbsp;5411, `renderStatus`&nbsp;5429

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5668, `dayAfter`&nbsp;5671, `pillsBaseInfo`&nbsp;5682, `settlePills`&nbsp;5692, `dniZapasu`&nbsp;5732, `renderPills`&nbsp;5745, `savePills`&nbsp;5766, `setPills`&nbsp;5777

*USTAWIENIA* — `renderKafelki`&nbsp;5788, `renderPudelka`&nbsp;5822, `renderSettings`&nbsp;5864, `tydzienZPol`&nbsp;5920, `renderWeekEditor`&nbsp;5932, `odswiezPodpowiedzTygodnia`&nbsp;5949, `tydzienZmieniony`&nbsp;5963, `rownajTydzien`&nbsp;5964, `renderPlanList`&nbsp;6012, `renderExceptions`&nbsp;6039, `wyslijSiec`&nbsp;6097

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6139, `tgZapytaj`&nbsp;6149, `tgKodParowania`&nbsp;6189, `tgZnajdzCzat`&nbsp;6205, `tgPolacz`&nbsp;6273, `tgProbna`&nbsp;6306, `tgOdlacz`&nbsp;6313, `renderTgStan`&nbsp;6335

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6450, `pobierzOpisFirmware`&nbsp;6456, `wyslijAktualizacje`&nbsp;6477, `anulujAktualizacje`&nbsp;6522, `renderOta`&nbsp;6528, `renderNetStan`&nbsp;6813

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6898, `renderSkan`&nbsp;6904, `szukajSieci`&nbsp;6956, `wybierzSiec`&nbsp;6964, `wyslijPolecenieSieci`&nbsp;6982, `siecZIndeksu`&nbsp;6992, `tzChanged`&nbsp;7026, `cfgTime`&nbsp;7031, `addSlot`&nbsp;7039, `zapiszPlanDnia`&nbsp;7052, `saveConfig`&nbsp;7070, `inrKrokiZakresu`&nbsp;7140, `opcjeInr`&nbsp;7147, `inrZakresZmieniony`&nbsp;7158, `wypelnijListyZakresu`&nbsp;7171, `saveInrRange`&nbsp;7182, `wypelnijListeOdstepu`&nbsp;7216, `saveInrEvery`&nbsp;7229, `odswiezPodpowiedzInr`&nbsp;7239

*ANALIZA* — `openTimeOf`&nbsp;7264, `openMinutes`&nbsp;7270, `sredniaPora`&nbsp;7294, `kwantyl`&nbsp;7302, `dniMiedzy`&nbsp;7310, `odstepyZPunktow`&nbsp;7324, `analyze`&nbsp;7333, `inrContext`&nbsp;7435

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7494, `rytmSVG`&nbsp;7506, `poryWCzasieSVG`&nbsp;7568, `iskraSVG`&nbsp;7636, `dowSVG`&nbsp;7664, `dniRytmu`&nbsp;7700, `skutecznoscTygodniami`&nbsp;7721, `renderAnalysis`&nbsp;7749

*RAPORT* — `collectRows`&nbsp;7856, `makeReport`&nbsp;7893

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;8044, `tagiPrzed`&nbsp;8052, `tagPrzelacz`&nbsp;8061

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;8104, `opisKopii`&nbsp;8114

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8154, `tgCzatKopii`&nbsp;8161, `odswiezKopie`&nbsp;8168, `tgKopiaCzatZapisz`&nbsp;8176, `tgKopiaCzatZnajdz`&nbsp;8196, `tgKopiaWlacz`&nbsp;8226, `tgKopiaWylacz`&nbsp;8245, `kopiaNaTelegram`&nbsp;8254, `kopiaAutomat`&nbsp;8305

*WIEK KOPII* — `dniOdDaty`&nbsp;8352, `wiekKopiiTxt`&nbsp;8358, `renderKopiaStan`&nbsp;8366, `zapiszKopie`&nbsp;8381

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8415, `ustawieniaDoOdtworzenia`&nbsp;8463, `wczytajKopie`&nbsp;8478, `kopiaCzytelna`&nbsp;8483, `odtworzKopie`&nbsp;8493, `kopiaWybrana`&nbsp;8526

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8550, `odtworzZBazy`&nbsp;8582, `exportCsv`&nbsp;8594

*NAWIGACJA* — `wrocZEkranu`&nbsp;8701


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
