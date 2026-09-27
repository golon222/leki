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

## `index.html` — 8880 linii, ~130 tys. tokenow

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
| 1112 | 1184 | tab-ana |
| 1185 | 1293 | tab-set |
| 1294 | 1365 | tab-lek |
| 1366 | 1375 | tab-pud |
| 1376 | 1395 | tab-sinr |
| 1396 | 1435 | tab-wifi |
| 1436 | 1536 | tab-tg |
| 1537 | 1570 | tab-dev |
| 1571 | 1680 | tab-diag |
| 1681 | 2003 | tab-help |
| 2004 | 2025 | tab-ev |
| 2026 | 2116 | tab-hist |
| 2117 | 2125 | JS — poczatek |
| 2126 | 2368 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2369 | 2443 | INFORMACJA ZWROTNA |
| 2444 | 2538 | STREFY CZASOWE |
| 2539 | 2566 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2567 | 2633 | TABLETKA JAKO BRYŁA |
| 2634 | 2648 | LOGOWANIE |
| 2649 | 2788 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2789 | 3169 | START |
| 3170 | 3230 | OSŁONA RYSOWANIA |
| 3231 | 3289 | REKONCYLIACJA |
| 3290 | 3494 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3495 | 3553 | KALENDARZ |
| 3554 | 3824 | HISTORIA ROZPISANIA DAWKI |
| 3825 | 4014 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 4015 | 4166 | ARKUSZ DNIA |
| 4167 | 4266 | WZIĄŁEM TERAZ |
| 4267 | 4381 | INR |
| 4382 | 4521 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4522 | 4534 | STATUS PUDEŁKA |
| 4535 | 4684 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4685 | 4846 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4847 | 5129 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 5130 | 5170 | KOLEJKA, KTÓRA NIE SCHODZI |
| 5171 | 5267 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 5268 | 5786 | EKRAN ZDARZEN |
| 5787 | 5905 | ZAPAS TABLETEK |
| 5906 | 6248 | USTAWIENIA |
| 6249 | 6556 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6557 | 7015 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 7016 | 7387 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7388 | 7598 | ANALIZA |
| 7599 | 7984 | WYKRESY ANALIZY |
| 7985 | 8141 | RAPORT |
| 8142 | 8203 | KONTEKST DNIA (TAGI) |
| 8204 | 8254 | KOPIA ZAPASOWA |
| 8255 | 8468 | KOPIA NA TELEGRAM |
| 8469 | 8534 | WIEK KOPII |
| 8535 | 8675 | ODTWARZANIE Z KOPII |
| 8676 | 8750 | KOPIE Z BAZY |
| 8751 | 8838 | NAWIGACJA |
| 8839 | 8880 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (226) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `opisPlikFirmware`&nbsp;2167, `wersjaZTelegramem`&nbsp;2176, `pudelkoZnane`&nbsp;2182, `wybranePudelko`&nbsp;2184, `korzenDanych`&nbsp;2208, `odmowaRegul`&nbsp;2227, `sprawdzDostepPudelek`&nbsp;2232, `wybierzPudelko`&nbsp;2255, `profilTydzien`&nbsp;2317, `komoraDnia`&nbsp;2328, `ustawProfil`&nbsp;2343

*INFORMACJA ZWROTNA* — `toast`&nbsp;2390, `busy`&nbsp;2406, `todayKey`&nbsp;2433, `dzisiajKey`&nbsp;2437, `inNightWindow`&nbsp;2440

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2495, `tzName`&nbsp;2511, `tzLabel`&nbsp;2512, `tzOffsetTxt`&nbsp;2513, `devDate`&nbsp;2519, `devKey`&nbsp;2524, `devHM`&nbsp;2529, `slotMin`&nbsp;2530, `pillColors`&nbsp;2532

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2543

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2580, `cieniuj`&nbsp;2607, `doseGraphic`&nbsp;2624

*LOGOWANIE* — `doLogin`&nbsp;2638

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2658, `wyczyscCache`&nbsp;2753, `fbSignOut`&nbsp;2772

*START* — `boot`&nbsp;2790

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;3197, `rysujWszystkie`&nbsp;3210, `renderAll`&nbsp;3214

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;3237, `reconcileDecyzja`&nbsp;3276

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3311, `zapiszReconcile`&nbsp;3323, `doReconcile`&nbsp;3362, `doReconcileWewn`&nbsp;3372, `reconcile`&nbsp;3493

*KALENDARZ* — `tydzienDawek`&nbsp;3533

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3579, `dawkaNaDzien`&nbsp;3590, `dzienBezLeku`&nbsp;3611, `wyjatekNaDzien`&nbsp;3616, `opisDawkowania`&nbsp;3622, `dayDose`&nbsp;3636, `dzienZamkniety`&nbsp;3670, `trackingSince`&nbsp;3676, `beforeTracking`&nbsp;3677, `dayStatus`&nbsp;3679, `renderCalendar`&nbsp;3720, `seriaDni`&nbsp;3785, `doNastepnej`&nbsp;3803, `opisCzasu`&nbsp;3818

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3833, `trwanieTxt`&nbsp;3848, `kiedyDawkaTxt`&nbsp;3859, `odswiezOdDawki`&nbsp;3867, `startTikOdDawki`&nbsp;3881, `renderToday`&nbsp;3891

*ARKUSZ DNIA* — `closeSheet`&nbsp;4026, `renderSheet`&nbsp;4028, `resetDose`&nbsp;4106, `resetPlan`&nbsp;4113, `commitPlan`&nbsp;4118, `clearPlan`&nbsp;4134, `commitDose`&nbsp;4146

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;4187, `askConfirm`&nbsp;4256

*INR* — `inrState`&nbsp;4268, `odswiezTerminInr`&nbsp;4281, `addInr`&nbsp;4290, `inrKeysOk`&nbsp;4376

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4391, `inrTerminKey`&nbsp;4399, `inrDoTerminu`&nbsp;4411, `dniTxt`&nbsp;4420, `renderInr`&nbsp;4422, `inrChart`&nbsp;4494

*STATUS PUDEŁKA* — `relTime`&nbsp;4523, `devDayMon`&nbsp;4532

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4567, `renderBoxLog`&nbsp;4613, `logPrzelacz`&nbsp;4649, `renderNvsFailLog`&nbsp;4657

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4702, `oczekWczytaj`&nbsp;4710, `oczekZapisz`&nbsp;4715, `oczekIle`&nbsp;4718, `zapiszPewnie`&nbsp;4728, `zapiszCfg`&nbsp;4766, `bazaOdmowila`&nbsp;4784, `oczekWyslij`&nbsp;4808, `oczekOdmowy`&nbsp;4845

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4861, `ostrzReguly`&nbsp;4883, `lm`&nbsp;4922, `ostrzMilczy`&nbsp;4928, `nvsMalo`&nbsp;5008, `opisNvsFailKey`&nbsp;5020, `stratyDotyczaLeku`&nbsp;5073, `ostrzStraty`&nbsp;5085, `stratyCicho`&nbsp;5119

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;5151

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;5181, `renderOstrzezenia`&nbsp;5196, `bezPokrycia`&nbsp;5206, `wierszZdarzenia`&nbsp;5212, `renderDiag`&nbsp;5229

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5284, `evPasuje`&nbsp;5289, `renderEvents`&nbsp;5301, `renderOpenWarn`&nbsp;5341, `minutyDoPelna`&nbsp;5394, `opisLadowania`&nbsp;5406, `dni`&nbsp;5426, `opisLadowan`&nbsp;5429, `tempoZHistorii`&nbsp;5482, `prognozaDni`&nbsp;5491, `opisPrognozy`&nbsp;5503, `czasKrotko`&nbsp;5525, `opisCzuwania`&nbsp;5533, `renderStatus`&nbsp;5551

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5790, `dayAfter`&nbsp;5793, `pillsBaseInfo`&nbsp;5804, `settlePills`&nbsp;5814, `dniZapasu`&nbsp;5854, `renderPills`&nbsp;5867, `savePills`&nbsp;5888, `setPills`&nbsp;5899

*USTAWIENIA* — `renderKafelki`&nbsp;5910, `renderPudelka`&nbsp;5946, `renderSettings`&nbsp;5988, `tydzienZPol`&nbsp;6047, `renderWeekEditor`&nbsp;6059, `odswiezPodpowiedzTygodnia`&nbsp;6076, `tydzienZmieniony`&nbsp;6090, `rownajTydzien`&nbsp;6091, `renderPlanList`&nbsp;6139, `renderExceptions`&nbsp;6166, `wyslijSiec`&nbsp;6224

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;6266, `tgZapytaj`&nbsp;6276, `tgKodParowania`&nbsp;6316, `tgZnajdzCzat`&nbsp;6332, `tgPolacz`&nbsp;6400, `tgProbna`&nbsp;6433, `tgOdlacz`&nbsp;6440, `renderTgStan`&nbsp;6462

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6577, `pobierzOpisFirmware`&nbsp;6583, `wyslijAktualizacje`&nbsp;6606, `anulujAktualizacje`&nbsp;6651, `renderOta`&nbsp;6657, `renderNetStan`&nbsp;6942

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;7027, `renderSkan`&nbsp;7033, `szukajSieci`&nbsp;7085, `wybierzSiec`&nbsp;7093, `wyslijPolecenieSieci`&nbsp;7111, `siecZIndeksu`&nbsp;7121, `tzChanged`&nbsp;7155, `cfgTime`&nbsp;7160, `addSlot`&nbsp;7168, `zapiszPlanDnia`&nbsp;7181, `saveConfig`&nbsp;7199, `inrKrokiZakresu`&nbsp;7270, `opcjeInr`&nbsp;7277, `inrZakresZmieniony`&nbsp;7288, `wypelnijListyZakresu`&nbsp;7301, `saveInrRange`&nbsp;7312, `wypelnijListeOdstepu`&nbsp;7346, `saveInrEvery`&nbsp;7359, `odswiezPodpowiedzInr`&nbsp;7369

*ANALIZA* — `openTimeOf`&nbsp;7394, `openMinutes`&nbsp;7400, `sredniaPora`&nbsp;7424, `kwantyl`&nbsp;7432, `dniMiedzy`&nbsp;7440, `odstepyZPunktow`&nbsp;7454, `analyze`&nbsp;7463, `inrContext`&nbsp;7565

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7624, `rytmSVG`&nbsp;7636, `poryWCzasieSVG`&nbsp;7698, `iskraSVG`&nbsp;7766, `dowSVG`&nbsp;7794, `dniRytmu`&nbsp;7830, `skutecznoscTygodniami`&nbsp;7851, `renderAnalysis`&nbsp;7879

*RAPORT* — `collectRows`&nbsp;7986, `makeReport`&nbsp;8023

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;8174, `tagiPrzed`&nbsp;8182, `tagPrzelacz`&nbsp;8191

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;8234, `opisKopii`&nbsp;8244

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;8289, `tgCzatKopii`&nbsp;8296, `odswiezKopie`&nbsp;8303, `tgKopiaCzatZapisz`&nbsp;8311, `tgKopiaCzatZnajdz`&nbsp;8331, `tgKopiaWlacz`&nbsp;8361, `tgKopiaWylacz`&nbsp;8380, `kopiaNaTelegram`&nbsp;8389, `kopiaAutomat`&nbsp;8440

*WIEK KOPII* — `dniOdDaty`&nbsp;8487, `wiekKopiiTxt`&nbsp;8493, `renderKopiaStan`&nbsp;8501, `zapiszKopie`&nbsp;8516

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8550, `ustawieniaDoOdtworzenia`&nbsp;8598, `wczytajKopie`&nbsp;8613, `kopiaCzytelna`&nbsp;8618, `odtworzKopie`&nbsp;8628, `kopiaWybrana`&nbsp;8661

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8685, `odtworzZBazy`&nbsp;8717, `exportCsv`&nbsp;8729

*NAWIGACJA* — `wrocZEkranu`&nbsp;8837


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
