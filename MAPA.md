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

## `index.html` — 8557 linii, ~130 tys. tokenow

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
| 1052 | 1136 | tab-set |
| 1137 | 1203 | tab-lek |
| 1204 | 1213 | tab-pud |
| 1214 | 1233 | tab-sinr |
| 1234 | 1273 | tab-wifi |
| 1274 | 1374 | tab-tg |
| 1375 | 1408 | tab-dev |
| 1409 | 1509 | tab-diag |
| 1510 | 1785 | tab-help |
| 1786 | 1807 | tab-ev |
| 1808 | 1898 | tab-hist |
| 1899 | 1907 | JS — poczatek |
| 1908 | 2076 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2077 | 2151 | INFORMACJA ZWROTNA |
| 2152 | 2246 | STREFY CZASOWE |
| 2247 | 2274 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2275 | 2341 | TABLETKA JAKO BRYŁA |
| 2342 | 2356 | LOGOWANIE |
| 2357 | 2496 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2497 | 2876 | START |
| 2877 | 2937 | OSŁONA RYSOWANIA |
| 2938 | 2996 | REKONCYLIACJA |
| 2997 | 3201 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3202 | 3260 | KALENDARZ |
| 3261 | 3527 | HISTORIA ROZPISANIA DAWKI |
| 3528 | 3709 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3710 | 3861 | ARKUSZ DNIA |
| 3862 | 3961 | WZIĄŁEM TERAZ |
| 3962 | 4076 | INR |
| 4077 | 4216 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4217 | 4229 | STATUS PUDEŁKA |
| 4230 | 4379 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4380 | 4541 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4542 | 4824 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4825 | 4865 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4866 | 4962 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 4963 | 5481 | EKRAN ZDARZEN |
| 5482 | 5600 | ZAPAS TABLETEK |
| 5601 | 5938 | USTAWIENIA |
| 5939 | 6246 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6247 | 6703 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6704 | 7074 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7075 | 7285 | ANALIZA |
| 7286 | 7671 | WYKRESY ANALIZY |
| 7672 | 7828 | RAPORT |
| 7829 | 7890 | KONTEKST DNIA (TAGI) |
| 7891 | 7936 | KOPIA ZAPASOWA |
| 7937 | 8150 | KOPIA NA TELEGRAM |
| 8151 | 8216 | WIEK KOPII |
| 8217 | 8357 | ODTWARZANIE Z KOPII |
| 8358 | 8432 | KOPIE Z BAZY |
| 8433 | 8515 | NAWIGACJA |
| 8516 | 8557 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (222) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `pudelkoZnane`&nbsp;1939, `wybranePudelko`&nbsp;1941, `korzenDanych`&nbsp;1965, `odmowaRegul`&nbsp;1984, `sprawdzDostepPudelek`&nbsp;1989, `wybierzPudelko`&nbsp;2012, `profilTydzien`&nbsp;2061

*INFORMACJA ZWROTNA* — `toast`&nbsp;2098, `busy`&nbsp;2114, `todayKey`&nbsp;2141, `dzisiajKey`&nbsp;2145, `inNightWindow`&nbsp;2148

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2203, `tzName`&nbsp;2219, `tzLabel`&nbsp;2220, `tzOffsetTxt`&nbsp;2221, `devDate`&nbsp;2227, `devKey`&nbsp;2232, `devHM`&nbsp;2237, `slotMin`&nbsp;2238, `pillColors`&nbsp;2240

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2251

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2288, `cieniuj`&nbsp;2315, `doseGraphic`&nbsp;2332

*LOGOWANIE* — `doLogin`&nbsp;2346

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2366, `wyczyscCache`&nbsp;2461, `fbSignOut`&nbsp;2480

*START* — `boot`&nbsp;2498

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;2904, `rysujWszystkie`&nbsp;2917, `renderAll`&nbsp;2921

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;2944, `reconcileDecyzja`&nbsp;2983

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3018, `zapiszReconcile`&nbsp;3030, `doReconcile`&nbsp;3069, `doReconcileWewn`&nbsp;3079, `reconcile`&nbsp;3200

*KALENDARZ* — `tydzienDawek`&nbsp;3240

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3286, `dawkaNaDzien`&nbsp;3297, `dzienBezLeku`&nbsp;3318, `wyjatekNaDzien`&nbsp;3323, `opisDawkowania`&nbsp;3329, `dayDose`&nbsp;3339, `dzienZamkniety`&nbsp;3373, `trackingSince`&nbsp;3379, `beforeTracking`&nbsp;3380, `dayStatus`&nbsp;3382, `renderCalendar`&nbsp;3423, `seriaDni`&nbsp;3488, `doNastepnej`&nbsp;3506, `opisCzasu`&nbsp;3521

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3536, `trwanieTxt`&nbsp;3551, `kiedyDawkaTxt`&nbsp;3562, `odswiezOdDawki`&nbsp;3570, `startTikOdDawki`&nbsp;3584, `renderToday`&nbsp;3594

*ARKUSZ DNIA* — `closeSheet`&nbsp;3721, `renderSheet`&nbsp;3723, `resetDose`&nbsp;3801, `resetPlan`&nbsp;3808, `commitPlan`&nbsp;3813, `clearPlan`&nbsp;3829, `commitDose`&nbsp;3841

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3882, `askConfirm`&nbsp;3951

*INR* — `inrState`&nbsp;3963, `odswiezTerminInr`&nbsp;3976, `addInr`&nbsp;3985, `inrKeysOk`&nbsp;4071

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4086, `inrTerminKey`&nbsp;4094, `inrDoTerminu`&nbsp;4106, `dniTxt`&nbsp;4115, `renderInr`&nbsp;4117, `inrChart`&nbsp;4189

*STATUS PUDEŁKA* — `relTime`&nbsp;4218, `devDayMon`&nbsp;4227

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4262, `renderBoxLog`&nbsp;4308, `logPrzelacz`&nbsp;4344, `renderNvsFailLog`&nbsp;4352

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4397, `oczekWczytaj`&nbsp;4405, `oczekZapisz`&nbsp;4410, `oczekIle`&nbsp;4413, `zapiszPewnie`&nbsp;4423, `zapiszCfg`&nbsp;4461, `bazaOdmowila`&nbsp;4479, `oczekWyslij`&nbsp;4503, `oczekOdmowy`&nbsp;4540

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4556, `ostrzReguly`&nbsp;4578, `lm`&nbsp;4617, `ostrzMilczy`&nbsp;4623, `nvsMalo`&nbsp;4703, `opisNvsFailKey`&nbsp;4715, `stratyDotyczaLeku`&nbsp;4768, `ostrzStraty`&nbsp;4780, `stratyCicho`&nbsp;4814

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4846

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4876, `renderOstrzezenia`&nbsp;4891, `bezPokrycia`&nbsp;4901, `wierszZdarzenia`&nbsp;4907, `renderDiag`&nbsp;4924

*EKRAN ZDARZEN* — `evFiltr`&nbsp;4979, `evPasuje`&nbsp;4984, `renderEvents`&nbsp;4996, `renderOpenWarn`&nbsp;5036, `minutyDoPelna`&nbsp;5089, `opisLadowania`&nbsp;5101, `dni`&nbsp;5121, `opisLadowan`&nbsp;5124, `tempoZHistorii`&nbsp;5177, `prognozaDni`&nbsp;5186, `opisPrognozy`&nbsp;5198, `czasKrotko`&nbsp;5220, `opisCzuwania`&nbsp;5228, `renderStatus`&nbsp;5246

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5485, `dayAfter`&nbsp;5488, `pillsBaseInfo`&nbsp;5499, `settlePills`&nbsp;5509, `dniZapasu`&nbsp;5549, `renderPills`&nbsp;5562, `savePills`&nbsp;5583, `setPills`&nbsp;5594

*USTAWIENIA* — `renderKafelki`&nbsp;5605, `renderPudelka`&nbsp;5639, `renderSettings`&nbsp;5681, `tydzienZPol`&nbsp;5737, `renderWeekEditor`&nbsp;5749, `odswiezPodpowiedzTygodnia`&nbsp;5766, `tydzienZmieniony`&nbsp;5780, `rownajTydzien`&nbsp;5781, `renderPlanList`&nbsp;5829, `renderExceptions`&nbsp;5856, `wyslijSiec`&nbsp;5914

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5956, `tgZapytaj`&nbsp;5966, `tgKodParowania`&nbsp;6006, `tgZnajdzCzat`&nbsp;6022, `tgPolacz`&nbsp;6090, `tgProbna`&nbsp;6123, `tgOdlacz`&nbsp;6130, `renderTgStan`&nbsp;6152

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6267, `pobierzOpisFirmware`&nbsp;6273, `wyslijAktualizacje`&nbsp;6294, `anulujAktualizacje`&nbsp;6339, `renderOta`&nbsp;6345, `renderNetStan`&nbsp;6630

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6715, `renderSkan`&nbsp;6721, `szukajSieci`&nbsp;6773, `wybierzSiec`&nbsp;6781, `wyslijPolecenieSieci`&nbsp;6799, `siecZIndeksu`&nbsp;6809, `tzChanged`&nbsp;6843, `cfgTime`&nbsp;6848, `addSlot`&nbsp;6856, `zapiszPlanDnia`&nbsp;6869, `saveConfig`&nbsp;6887, `inrKrokiZakresu`&nbsp;6957, `opcjeInr`&nbsp;6964, `inrZakresZmieniony`&nbsp;6975, `wypelnijListyZakresu`&nbsp;6988, `saveInrRange`&nbsp;6999, `wypelnijListeOdstepu`&nbsp;7033, `saveInrEvery`&nbsp;7046, `odswiezPodpowiedzInr`&nbsp;7056

*ANALIZA* — `openTimeOf`&nbsp;7081, `openMinutes`&nbsp;7087, `sredniaPora`&nbsp;7111, `kwantyl`&nbsp;7119, `dniMiedzy`&nbsp;7127, `odstepyZPunktow`&nbsp;7141, `analyze`&nbsp;7150, `inrContext`&nbsp;7252

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7311, `rytmSVG`&nbsp;7323, `poryWCzasieSVG`&nbsp;7385, `iskraSVG`&nbsp;7453, `dowSVG`&nbsp;7481, `dniRytmu`&nbsp;7517, `skutecznoscTygodniami`&nbsp;7538, `renderAnalysis`&nbsp;7566

*RAPORT* — `collectRows`&nbsp;7673, `makeReport`&nbsp;7710

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7861, `tagiPrzed`&nbsp;7869, `tagPrzelacz`&nbsp;7878

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7921, `opisKopii`&nbsp;7931

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7971, `tgCzatKopii`&nbsp;7978, `odswiezKopie`&nbsp;7985, `tgKopiaCzatZapisz`&nbsp;7993, `tgKopiaCzatZnajdz`&nbsp;8013, `tgKopiaWlacz`&nbsp;8043, `tgKopiaWylacz`&nbsp;8062, `kopiaNaTelegram`&nbsp;8071, `kopiaAutomat`&nbsp;8122

*WIEK KOPII* — `dniOdDaty`&nbsp;8169, `wiekKopiiTxt`&nbsp;8175, `renderKopiaStan`&nbsp;8183, `zapiszKopie`&nbsp;8198

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8232, `ustawieniaDoOdtworzenia`&nbsp;8280, `wczytajKopie`&nbsp;8295, `kopiaCzytelna`&nbsp;8300, `odtworzKopie`&nbsp;8310, `kopiaWybrana`&nbsp;8343

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8367, `odtworzZBazy`&nbsp;8399, `exportCsv`&nbsp;8411

*NAWIGACJA* — `wrocZEkranu`&nbsp;8514


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
