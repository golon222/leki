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

## `index.html` — 8587 linii, ~130 tys. tokenow

Ekrany (`<section>`) i dwa duze bloki. Zakladki `tab-*` odpowiadaja
pozycjom w pasku nawigacji i podekranom Ustawien.

| od | do | co |
|---|---|---|
| 21 | 21 | CSS — poczatek |
| 22 | 222 | SYSTEM WIZUALNY PillBox |
| 223 | 319 | EKRAN GŁÓWNY — KARTA DNIA |
| 320 | 392 | INFORMACJA ZWROTNA |
| 393 | 781 | TABLETKA W 3D |
| 782 | 941 | tab-cal |
| 942 | 990 | tab-inr |
| 991 | 1063 | tab-ana |
| 1064 | 1148 | tab-set |
| 1149 | 1220 | tab-lek |
| 1221 | 1230 | tab-pud |
| 1231 | 1250 | tab-sinr |
| 1251 | 1290 | tab-wifi |
| 1291 | 1391 | tab-tg |
| 1392 | 1425 | tab-dev |
| 1426 | 1526 | tab-diag |
| 1527 | 1802 | tab-help |
| 1803 | 1824 | tab-ev |
| 1825 | 1915 | tab-hist |
| 1916 | 1924 | JS — poczatek |
| 1925 | 2101 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 2102 | 2176 | INFORMACJA ZWROTNA |
| 2177 | 2271 | STREFY CZASOWE |
| 2272 | 2299 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2300 | 2366 | TABLETKA JAKO BRYŁA |
| 2367 | 2381 | LOGOWANIE |
| 2382 | 2521 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2522 | 2902 | START |
| 2903 | 2963 | OSŁONA RYSOWANIA |
| 2964 | 3022 | REKONCYLIACJA |
| 3023 | 3227 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3228 | 3286 | KALENDARZ |
| 3287 | 3553 | HISTORIA ROZPISANIA DAWKI |
| 3554 | 3735 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3736 | 3887 | ARKUSZ DNIA |
| 3888 | 3987 | WZIĄŁEM TERAZ |
| 3988 | 4102 | INR |
| 4103 | 4242 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4243 | 4255 | STATUS PUDEŁKA |
| 4256 | 4405 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4406 | 4567 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4568 | 4850 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4851 | 4891 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4892 | 4988 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 4989 | 5507 | EKRAN ZDARZEN |
| 5508 | 5626 | ZAPAS TABLETEK |
| 5627 | 5964 | USTAWIENIA |
| 5965 | 6272 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6273 | 6729 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6730 | 7100 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 7101 | 7311 | ANALIZA |
| 7312 | 7697 | WYKRESY ANALIZY |
| 7698 | 7854 | RAPORT |
| 7855 | 7916 | KONTEKST DNIA (TAGI) |
| 7917 | 7962 | KOPIA ZAPASOWA |
| 7963 | 8176 | KOPIA NA TELEGRAM |
| 8177 | 8242 | WIEK KOPII |
| 8243 | 8383 | ODTWARZANIE Z KOPII |
| 8384 | 8458 | KOPIE Z BAZY |
| 8459 | 8545 | NAWIGACJA |
| 8546 | 8587 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (223) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `pudelkoZnane`&nbsp;1956, `wybranePudelko`&nbsp;1958, `korzenDanych`&nbsp;1982, `odmowaRegul`&nbsp;2001, `sprawdzDostepPudelek`&nbsp;2006, `wybierzPudelko`&nbsp;2029, `profilTydzien`&nbsp;2078, `ustawProfil`&nbsp;2083

*INFORMACJA ZWROTNA* — `toast`&nbsp;2123, `busy`&nbsp;2139, `todayKey`&nbsp;2166, `dzisiajKey`&nbsp;2170, `inNightWindow`&nbsp;2173

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2228, `tzName`&nbsp;2244, `tzLabel`&nbsp;2245, `tzOffsetTxt`&nbsp;2246, `devDate`&nbsp;2252, `devKey`&nbsp;2257, `devHM`&nbsp;2262, `slotMin`&nbsp;2263, `pillColors`&nbsp;2265

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2276

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2313, `cieniuj`&nbsp;2340, `doseGraphic`&nbsp;2357

*LOGOWANIE* — `doLogin`&nbsp;2371

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2391, `wyczyscCache`&nbsp;2486, `fbSignOut`&nbsp;2505

*START* — `boot`&nbsp;2523

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;2930, `rysujWszystkie`&nbsp;2943, `renderAll`&nbsp;2947

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;2970, `reconcileDecyzja`&nbsp;3009

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;3044, `zapiszReconcile`&nbsp;3056, `doReconcile`&nbsp;3095, `doReconcileWewn`&nbsp;3105, `reconcile`&nbsp;3226

*KALENDARZ* — `tydzienDawek`&nbsp;3266

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3312, `dawkaNaDzien`&nbsp;3323, `dzienBezLeku`&nbsp;3344, `wyjatekNaDzien`&nbsp;3349, `opisDawkowania`&nbsp;3355, `dayDose`&nbsp;3365, `dzienZamkniety`&nbsp;3399, `trackingSince`&nbsp;3405, `beforeTracking`&nbsp;3406, `dayStatus`&nbsp;3408, `renderCalendar`&nbsp;3449, `seriaDni`&nbsp;3514, `doNastepnej`&nbsp;3532, `opisCzasu`&nbsp;3547

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3562, `trwanieTxt`&nbsp;3577, `kiedyDawkaTxt`&nbsp;3588, `odswiezOdDawki`&nbsp;3596, `startTikOdDawki`&nbsp;3610, `renderToday`&nbsp;3620

*ARKUSZ DNIA* — `closeSheet`&nbsp;3747, `renderSheet`&nbsp;3749, `resetDose`&nbsp;3827, `resetPlan`&nbsp;3834, `commitPlan`&nbsp;3839, `clearPlan`&nbsp;3855, `commitDose`&nbsp;3867

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3908, `askConfirm`&nbsp;3977

*INR* — `inrState`&nbsp;3989, `odswiezTerminInr`&nbsp;4002, `addInr`&nbsp;4011, `inrKeysOk`&nbsp;4097

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;4112, `inrTerminKey`&nbsp;4120, `inrDoTerminu`&nbsp;4132, `dniTxt`&nbsp;4141, `renderInr`&nbsp;4143, `inrChart`&nbsp;4215

*STATUS PUDEŁKA* — `relTime`&nbsp;4244, `devDayMon`&nbsp;4253

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4288, `renderBoxLog`&nbsp;4334, `logPrzelacz`&nbsp;4370, `renderNvsFailLog`&nbsp;4378

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4423, `oczekWczytaj`&nbsp;4431, `oczekZapisz`&nbsp;4436, `oczekIle`&nbsp;4439, `zapiszPewnie`&nbsp;4449, `zapiszCfg`&nbsp;4487, `bazaOdmowila`&nbsp;4505, `oczekWyslij`&nbsp;4529, `oczekOdmowy`&nbsp;4566

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4582, `ostrzReguly`&nbsp;4604, `lm`&nbsp;4643, `ostrzMilczy`&nbsp;4649, `nvsMalo`&nbsp;4729, `opisNvsFailKey`&nbsp;4741, `stratyDotyczaLeku`&nbsp;4794, `ostrzStraty`&nbsp;4806, `stratyCicho`&nbsp;4840

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4872

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4902, `renderOstrzezenia`&nbsp;4917, `bezPokrycia`&nbsp;4927, `wierszZdarzenia`&nbsp;4933, `renderDiag`&nbsp;4950

*EKRAN ZDARZEN* — `evFiltr`&nbsp;5005, `evPasuje`&nbsp;5010, `renderEvents`&nbsp;5022, `renderOpenWarn`&nbsp;5062, `minutyDoPelna`&nbsp;5115, `opisLadowania`&nbsp;5127, `dni`&nbsp;5147, `opisLadowan`&nbsp;5150, `tempoZHistorii`&nbsp;5203, `prognozaDni`&nbsp;5212, `opisPrognozy`&nbsp;5224, `czasKrotko`&nbsp;5246, `opisCzuwania`&nbsp;5254, `renderStatus`&nbsp;5272

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5511, `dayAfter`&nbsp;5514, `pillsBaseInfo`&nbsp;5525, `settlePills`&nbsp;5535, `dniZapasu`&nbsp;5575, `renderPills`&nbsp;5588, `savePills`&nbsp;5609, `setPills`&nbsp;5620

*USTAWIENIA* — `renderKafelki`&nbsp;5631, `renderPudelka`&nbsp;5665, `renderSettings`&nbsp;5707, `tydzienZPol`&nbsp;5763, `renderWeekEditor`&nbsp;5775, `odswiezPodpowiedzTygodnia`&nbsp;5792, `tydzienZmieniony`&nbsp;5806, `rownajTydzien`&nbsp;5807, `renderPlanList`&nbsp;5855, `renderExceptions`&nbsp;5882, `wyslijSiec`&nbsp;5940

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5982, `tgZapytaj`&nbsp;5992, `tgKodParowania`&nbsp;6032, `tgZnajdzCzat`&nbsp;6048, `tgPolacz`&nbsp;6116, `tgProbna`&nbsp;6149, `tgOdlacz`&nbsp;6156, `renderTgStan`&nbsp;6178

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6293, `pobierzOpisFirmware`&nbsp;6299, `wyslijAktualizacje`&nbsp;6320, `anulujAktualizacje`&nbsp;6365, `renderOta`&nbsp;6371, `renderNetStan`&nbsp;6656

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6741, `renderSkan`&nbsp;6747, `szukajSieci`&nbsp;6799, `wybierzSiec`&nbsp;6807, `wyslijPolecenieSieci`&nbsp;6825, `siecZIndeksu`&nbsp;6835, `tzChanged`&nbsp;6869, `cfgTime`&nbsp;6874, `addSlot`&nbsp;6882, `zapiszPlanDnia`&nbsp;6895, `saveConfig`&nbsp;6913, `inrKrokiZakresu`&nbsp;6983, `opcjeInr`&nbsp;6990, `inrZakresZmieniony`&nbsp;7001, `wypelnijListyZakresu`&nbsp;7014, `saveInrRange`&nbsp;7025, `wypelnijListeOdstepu`&nbsp;7059, `saveInrEvery`&nbsp;7072, `odswiezPodpowiedzInr`&nbsp;7082

*ANALIZA* — `openTimeOf`&nbsp;7107, `openMinutes`&nbsp;7113, `sredniaPora`&nbsp;7137, `kwantyl`&nbsp;7145, `dniMiedzy`&nbsp;7153, `odstepyZPunktow`&nbsp;7167, `analyze`&nbsp;7176, `inrContext`&nbsp;7278

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7337, `rytmSVG`&nbsp;7349, `poryWCzasieSVG`&nbsp;7411, `iskraSVG`&nbsp;7479, `dowSVG`&nbsp;7507, `dniRytmu`&nbsp;7543, `skutecznoscTygodniami`&nbsp;7564, `renderAnalysis`&nbsp;7592

*RAPORT* — `collectRows`&nbsp;7699, `makeReport`&nbsp;7736

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7887, `tagiPrzed`&nbsp;7895, `tagPrzelacz`&nbsp;7904

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7947, `opisKopii`&nbsp;7957

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7997, `tgCzatKopii`&nbsp;8004, `odswiezKopie`&nbsp;8011, `tgKopiaCzatZapisz`&nbsp;8019, `tgKopiaCzatZnajdz`&nbsp;8039, `tgKopiaWlacz`&nbsp;8069, `tgKopiaWylacz`&nbsp;8088, `kopiaNaTelegram`&nbsp;8097, `kopiaAutomat`&nbsp;8148

*WIEK KOPII* — `dniOdDaty`&nbsp;8195, `wiekKopiiTxt`&nbsp;8201, `renderKopiaStan`&nbsp;8209, `zapiszKopie`&nbsp;8224

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8258, `ustawieniaDoOdtworzenia`&nbsp;8306, `wczytajKopie`&nbsp;8321, `kopiaCzytelna`&nbsp;8326, `odtworzKopie`&nbsp;8336, `kopiaWybrana`&nbsp;8369

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8393, `odtworzZBazy`&nbsp;8425, `exportCsv`&nbsp;8437

*NAWIGACJA* — `wrocZEkranu`&nbsp;8544


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
