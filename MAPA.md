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

## `index.html` — 8901 linii, ~130 tys. tokenow

Ekrany (`<section>`) i dwa duze bloki. Zakladki `tab-*` odpowiadaja
pozycjom w pasku nawigacji i podekranom Ustawien.

| od | do | co |
|---|---|---|
| 21 | 21 | CSS — poczatek |
| 22 | 263 | SYSTEM WIZUALNY PillBox |
| 264 | 360 | EKRAN GŁÓWNY — KARTA DNIA |
| 361 | 433 | INFORMACJA ZWROTNA |
| 434 | 902 | TABLETKA W 3D |
| 903 | 1062 | tab-cal |
| 1063 | 1111 | tab-inr |
| 1112 | 1185 | tab-ana |
| 1186 | 1294 | tab-set |
| 1295 | 1375 | tab-lek |
| 1376 | 1385 | tab-pud |
| 1386 | 1405 | tab-sinr |
| 1406 | 1445 | tab-wifi |
| 1446 | 1546 | tab-tg |
| 1547 | 1580 | tab-dev |
| 1581 | 1690 | tab-diag |
| 1691 | 2013 | tab-help |
| 2014 | 2035 | tab-ev |
| 2036 | 2126 | tab-hist |
| 2127 | 2135 | JS — poczatek |
| 2136 | 2378 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2379 | 2453 | INFORMACJA ZWROTNA |
| 2454 | 2548 | STREFY CZASOWE |
| 2549 | 2576 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2577 | 2643 | TABLETKA JAKO BRYŁA |
| 2644 | 2658 | LOGOWANIE |
| 2659 | 2798 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2799 | 3179 | START |
| 3180 | 3240 | OSŁONA RYSOWANIA |
| 3241 | 3299 | REKONCYLIACJA |
| 3300 | 3504 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3505 | 3563 | KALENDARZ |
| 3564 | 3834 | HISTORIA ROZPISANIA DAWKI |
| 3835 | 4023 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 4024 | 4175 | ARKUSZ DNIA |
| 4176 | 4275 | WZIĄŁEM TERAZ |
| 4276 | 4390 | INR |
| 4391 | 4530 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4531 | 4543 | STATUS PUDEŁKA |
| 4544 | 4693 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4694 | 4855 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4856 | 5138 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 5139 | 5179 | KOLEJKA, KTÓRA NIE SCHODZI |
| 5180 | 5276 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 5277 | 5795 | EKRAN ZDARZEN |
| 5796 | 5914 | ZAPAS TABLETEK |
| 5915 | 6269 | USTAWIENIA |
| 6270 | 6577 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6578 | 7036 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 7037 | 7408 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7409 | 7619 | ANALIZA |
| 7620 | 8005 | WYKRESY ANALIZY |
| 8006 | 8162 | RAPORT |
| 8163 | 8224 | KONTEKST DNIA (TAGI) |
| 8225 | 8275 | KOPIA ZAPASOWA |
| 8276 | 8489 | KOPIA NA TELEGRAM |
| 8490 | 8555 | WIEK KOPII |
| 8556 | 8696 | ODTWARZANIE Z KOPII |
| 8697 | 8771 | KOPIE Z BAZY |
| 8772 | 8859 | NAWIGACJA |
| 8860 | 8901 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (227) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `opisPlikFirmware`&nbsp;2177, `wersjaZTelegramem`&nbsp;2186, `pudelkoZnane`&nbsp;2192, `wybranePudelko`&nbsp;2194, `korzenDanych`&nbsp;2218, `odmowaRegul`&nbsp;2237, `sprawdzDostepPudelek`&nbsp;2242, `wybierzPudelko`&nbsp;2265, `profilTydzien`&nbsp;2327, `komoraDnia`&nbsp;2338, `ustawProfil`&nbsp;2353

*INFORMACJA ZWROTNA* — `toast`&nbsp;2400, `busy`&nbsp;2416, `todayKey`&nbsp;2443, `dzisiajKey`&nbsp;2447, `inNightWindow`&nbsp;2450

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2505, `tzName`&nbsp;2521, `tzLabel`&nbsp;2522, `tzOffsetTxt`&nbsp;2523, `devDate`&nbsp;2529, `devKey`&nbsp;2534, `devHM`&nbsp;2539, `slotMin`&nbsp;2540, `pillColors`&nbsp;2542

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2553

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2590, `cieniuj`&nbsp;2617, `doseGraphic`&nbsp;2634

*LOGOWANIE* — `doLogin`&nbsp;2648

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2668, `wyczyscCache`&nbsp;2763, `fbSignOut`&nbsp;2782

*START* — `boot`&nbsp;2800

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;3207, `rysujWszystkie`&nbsp;3220, `renderAll`&nbsp;3224

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;3247, `reconcileDecyzja`&nbsp;3286

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3321, `zapiszReconcile`&nbsp;3333, `doReconcile`&nbsp;3372, `doReconcileWewn`&nbsp;3382, `reconcile`&nbsp;3503

*KALENDARZ* — `tydzienDawek`&nbsp;3543

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3589, `dawkaNaDzien`&nbsp;3600, `dzienBezLeku`&nbsp;3621, `wyjatekNaDzien`&nbsp;3626, `opisDawkowania`&nbsp;3632, `dayDose`&nbsp;3646, `dzienZamkniety`&nbsp;3680, `trackingSince`&nbsp;3686, `beforeTracking`&nbsp;3687, `dayStatus`&nbsp;3689, `renderCalendar`&nbsp;3730, `seriaDni`&nbsp;3795, `doNastepnej`&nbsp;3813, `opisCzasu`&nbsp;3828

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3843, `trwanieTxt`&nbsp;3858, `kiedyDawkaTxt`&nbsp;3869, `odswiezOdDawki`&nbsp;3877, `startTikOdDawki`&nbsp;3891, `renderToday`&nbsp;3901

*ARKUSZ DNIA* — `closeSheet`&nbsp;4035, `renderSheet`&nbsp;4037, `resetDose`&nbsp;4115, `resetPlan`&nbsp;4122, `commitPlan`&nbsp;4127, `clearPlan`&nbsp;4143, `commitDose`&nbsp;4155

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;4196, `askConfirm`&nbsp;4265

*INR* — `inrState`&nbsp;4277, `odswiezTerminInr`&nbsp;4290, `addInr`&nbsp;4299, `inrKeysOk`&nbsp;4385

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4400, `inrTerminKey`&nbsp;4408, `inrDoTerminu`&nbsp;4420, `dniTxt`&nbsp;4429, `renderInr`&nbsp;4431, `inrChart`&nbsp;4503

*STATUS PUDEŁKA* — `relTime`&nbsp;4532, `devDayMon`&nbsp;4541

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4576, `renderBoxLog`&nbsp;4622, `logPrzelacz`&nbsp;4658, `renderNvsFailLog`&nbsp;4666

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4711, `oczekWczytaj`&nbsp;4719, `oczekZapisz`&nbsp;4724, `oczekIle`&nbsp;4727, `zapiszPewnie`&nbsp;4737, `zapiszCfg`&nbsp;4775, `bazaOdmowila`&nbsp;4793, `oczekWyslij`&nbsp;4817, `oczekOdmowy`&nbsp;4854

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4870, `ostrzReguly`&nbsp;4892, `lm`&nbsp;4931, `ostrzMilczy`&nbsp;4937, `nvsMalo`&nbsp;5017, `opisNvsFailKey`&nbsp;5029, `stratyDotyczaLeku`&nbsp;5082, `ostrzStraty`&nbsp;5094, `stratyCicho`&nbsp;5128

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;5160

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;5190, `renderOstrzezenia`&nbsp;5205, `bezPokrycia`&nbsp;5215, `wierszZdarzenia`&nbsp;5221, `renderDiag`&nbsp;5238

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5293, `evPasuje`&nbsp;5298, `renderEvents`&nbsp;5310, `renderOpenWarn`&nbsp;5350, `minutyDoPelna`&nbsp;5403, `opisLadowania`&nbsp;5415, `dni`&nbsp;5435, `opisLadowan`&nbsp;5438, `tempoZHistorii`&nbsp;5491, `prognozaDni`&nbsp;5500, `opisPrognozy`&nbsp;5512, `czasKrotko`&nbsp;5534, `opisCzuwania`&nbsp;5542, `renderStatus`&nbsp;5560

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5799, `dayAfter`&nbsp;5802, `pillsBaseInfo`&nbsp;5813, `settlePills`&nbsp;5823, `dniZapasu`&nbsp;5863, `renderPills`&nbsp;5876, `savePills`&nbsp;5897, `setPills`&nbsp;5908

*USTAWIENIA* — `opisLeku`&nbsp;5926, `renderKafelki`&nbsp;5932, `renderPudelka`&nbsp;5967, `renderSettings`&nbsp;6009, `tydzienZPol`&nbsp;6068, `renderWeekEditor`&nbsp;6080, `odswiezPodpowiedzTygodnia`&nbsp;6097, `tydzienZmieniony`&nbsp;6111, `rownajTydzien`&nbsp;6112, `renderPlanList`&nbsp;6160, `renderExceptions`&nbsp;6187, `wyslijSiec`&nbsp;6245

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6287, `tgZapytaj`&nbsp;6297, `tgKodParowania`&nbsp;6337, `tgZnajdzCzat`&nbsp;6353, `tgPolacz`&nbsp;6421, `tgProbna`&nbsp;6454, `tgOdlacz`&nbsp;6461, `renderTgStan`&nbsp;6483

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6598, `pobierzOpisFirmware`&nbsp;6604, `wyslijAktualizacje`&nbsp;6627, `anulujAktualizacje`&nbsp;6672, `renderOta`&nbsp;6678, `renderNetStan`&nbsp;6963

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;7048, `renderSkan`&nbsp;7054, `szukajSieci`&nbsp;7106, `wybierzSiec`&nbsp;7114, `wyslijPolecenieSieci`&nbsp;7132, `siecZIndeksu`&nbsp;7142, `tzChanged`&nbsp;7176, `cfgTime`&nbsp;7181, `addSlot`&nbsp;7189, `zapiszPlanDnia`&nbsp;7202, `saveConfig`&nbsp;7220, `inrKrokiZakresu`&nbsp;7291, `opcjeInr`&nbsp;7298, `inrZakresZmieniony`&nbsp;7309, `wypelnijListyZakresu`&nbsp;7322, `saveInrRange`&nbsp;7333, `wypelnijListeOdstepu`&nbsp;7367, `saveInrEvery`&nbsp;7380, `odswiezPodpowiedzInr`&nbsp;7390

*ANALIZA* — `openTimeOf`&nbsp;7415, `openMinutes`&nbsp;7421, `sredniaPora`&nbsp;7445, `kwantyl`&nbsp;7453, `dniMiedzy`&nbsp;7461, `odstepyZPunktow`&nbsp;7475, `analyze`&nbsp;7484, `inrContext`&nbsp;7586

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7645, `rytmSVG`&nbsp;7657, `poryWCzasieSVG`&nbsp;7719, `iskraSVG`&nbsp;7787, `dowSVG`&nbsp;7815, `dniRytmu`&nbsp;7851, `skutecznoscTygodniami`&nbsp;7872, `renderAnalysis`&nbsp;7900

*RAPORT* — `collectRows`&nbsp;8007, `makeReport`&nbsp;8044

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;8195, `tagiPrzed`&nbsp;8203, `tagPrzelacz`&nbsp;8212

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;8255, `opisKopii`&nbsp;8265

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8310, `tgCzatKopii`&nbsp;8317, `odswiezKopie`&nbsp;8324, `tgKopiaCzatZapisz`&nbsp;8332, `tgKopiaCzatZnajdz`&nbsp;8352, `tgKopiaWlacz`&nbsp;8382, `tgKopiaWylacz`&nbsp;8401, `kopiaNaTelegram`&nbsp;8410, `kopiaAutomat`&nbsp;8461

*WIEK KOPII* — `dniOdDaty`&nbsp;8508, `wiekKopiiTxt`&nbsp;8514, `renderKopiaStan`&nbsp;8522, `zapiszKopie`&nbsp;8537

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8571, `ustawieniaDoOdtworzenia`&nbsp;8619, `wczytajKopie`&nbsp;8634, `kopiaCzytelna`&nbsp;8639, `odtworzKopie`&nbsp;8649, `kopiaWybrana`&nbsp;8682

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8706, `odtworzZBazy`&nbsp;8738, `exportCsv`&nbsp;8750

*NAWIGACJA* — `wrocZEkranu`&nbsp;8858


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
