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

## `index.html` — 8345 linii, ~130 tys. tokenow

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
| 1052 | 1127 | tab-set |
| 1128 | 1194 | tab-lek |
| 1195 | 1214 | tab-sinr |
| 1215 | 1254 | tab-wifi |
| 1255 | 1355 | tab-tg |
| 1356 | 1389 | tab-dev |
| 1390 | 1490 | tab-diag |
| 1491 | 1766 | tab-help |
| 1767 | 1788 | tab-ev |
| 1789 | 1879 | tab-hist |
| 1880 | 1888 | JS — poczatek |
| 1889 | 1970 | KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu |
| 1971 | 2045 | INFORMACJA ZWROTNA |
| 2046 | 2140 | STREFY CZASOWE |
| 2141 | 2168 | TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin. |
| 2169 | 2235 | TABLETKA JAKO BRYŁA |
| 2236 | 2250 | LOGOWANIE |
| 2251 | 2394 | TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku |
| 2395 | 2759 | START |
| 2760 | 2820 | OSŁONA RYSOWANIA |
| 2821 | 2879 | REKONCYLIACJA |
| 2880 | 3084 | ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ |
| 3085 | 3143 | KALENDARZ |
| 3144 | 3410 | HISTORIA ROZPISANIA DAWKI |
| 3411 | 3592 | ILE MINĘŁO OD POPRZEDNIEJ DAWKI |
| 3593 | 3744 | ARKUSZ DNIA |
| 3745 | 3844 | WZIĄŁEM TERAZ |
| 3845 | 3959 | INR |
| 3960 | 4099 | ODSTĘP MIĘDZY POMIARAMI INR |
| 4100 | 4112 | STATUS PUDEŁKA |
| 4113 | 4262 | DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja |
| 4263 | 4424 | KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej. |
| 4425 | 4707 | OSTRZEŻENIA — celowo NIE schowane w Diagnostyce |
| 4708 | 4748 | KOLEJKA, KTÓRA NIE SCHODZI |
| 4749 | 4845 | EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ |
| 4846 | 5364 | EKRAN ZDARZEN |
| 5365 | 5483 | ZAPAS TABLETEK |
| 5484 | 5771 | USTAWIENIA |
| 5772 | 6079 | POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67) |
| 6080 | 6536 | AKTUALIZACJA PROGRAMU PUDEŁKA (D59) |
| 6537 | 6907 | SIECI WIDZIANE PRZEZ PUDEŁKO |
| 6908 | 7118 | ANALIZA |
| 7119 | 7504 | WYKRESY ANALIZY |
| 7505 | 7661 | RAPORT |
| 7662 | 7723 | KONTEKST DNIA (TAGI) |
| 7724 | 7769 | KOPIA ZAPASOWA |
| 7770 | 7983 | KOPIA NA TELEGRAM |
| 7984 | 8049 | WIEK KOPII |
| 8050 | 8147 | ODTWARZANIE Z KOPII |
| 8148 | 8222 | KOPIE Z BAZY |
| 8223 | 8303 | NAWIGACJA |
| 8304 | 8345 | AUTOMATYCZNA AKTUALIZACJA |

**Funkcje** (215) — nazwa i linia deklaracji:

*KONFIGURACJA — wklej z Firebase Console → Ustawienia projektu* — `deviceIdZEmaila`&nbsp;1911, `profilTydzien`&nbsp;1955

*INFORMACJA ZWROTNA* — `toast`&nbsp;1992, `busy`&nbsp;2008, `todayKey`&nbsp;2035, `dzisiajKey`&nbsp;2039, `inNightWindow`&nbsp;2042

*STREFY CZASOWE* — `tzOffsetFor`&nbsp;2097, `tzName`&nbsp;2113, `tzLabel`&nbsp;2114, `tzOffsetTxt`&nbsp;2115, `devDate`&nbsp;2121, `devKey`&nbsp;2126, `devHM`&nbsp;2131, `slotMin`&nbsp;2132, `pillColors`&nbsp;2134

*TABLETKA — rysowana, z nacięciem krzyżowym jak Warfin.* — `tabletSVG`&nbsp;2145

*TABLETKA JAKO BRYŁA* — `tablet3D`&nbsp;2182, `cieniuj`&nbsp;2209, `doseGraphic`&nbsp;2226

*LOGOWANIE* — `doLogin`&nbsp;2240

*TEST POŁĄCZENIA — przechodzi całą drogę danych krok po kroku* — `testPolaczenia`&nbsp;2260, `wyczyscCache`&nbsp;2355, `fbSignOut`&nbsp;2374

*START* — `boot`&nbsp;2396

*OSŁONA RYSOWANIA* — `rysuj`&nbsp;2787, `rysujWszystkie`&nbsp;2800, `renderAll`&nbsp;2804

*REKONCYLIACJA* — `brakujePokrycia`&nbsp;2827, `reconcileDecyzja`&nbsp;2866

*ŻADEN ZAPIS DO BAZY NIE CZEKA W NIESKOŃCZONOŚĆ* — `zTerminem`&nbsp;2901, `zapiszReconcile`&nbsp;2913, `doReconcile`&nbsp;2952, `doReconcileWewn`&nbsp;2962, `reconcile`&nbsp;3083

*KALENDARZ* — `tydzienDawek`&nbsp;3123

*HISTORIA ROZPISANIA DAWKI* — `planNaDzien`&nbsp;3169, `dawkaNaDzien`&nbsp;3180, `dzienBezLeku`&nbsp;3201, `wyjatekNaDzien`&nbsp;3206, `opisDawkowania`&nbsp;3212, `dayDose`&nbsp;3222, `dzienZamkniety`&nbsp;3256, `trackingSince`&nbsp;3262, `beforeTracking`&nbsp;3263, `dayStatus`&nbsp;3265, `renderCalendar`&nbsp;3306, `seriaDni`&nbsp;3371, `doNastepnej`&nbsp;3389, `opisCzasu`&nbsp;3404

*ILE MINĘŁO OD POPRZEDNIEJ DAWKI* — `ostatniaDawka`&nbsp;3419, `trwanieTxt`&nbsp;3434, `kiedyDawkaTxt`&nbsp;3445, `odswiezOdDawki`&nbsp;3453, `startTikOdDawki`&nbsp;3467, `renderToday`&nbsp;3477

*ARKUSZ DNIA* — `closeSheet`&nbsp;3604, `renderSheet`&nbsp;3606, `resetDose`&nbsp;3684, `resetPlan`&nbsp;3691, `commitPlan`&nbsp;3696, `clearPlan`&nbsp;3712, `commitDose`&nbsp;3724

*WZIĄŁEM TERAZ* — `wezTeraz`&nbsp;3765, `askConfirm`&nbsp;3834

*INR* — `inrState`&nbsp;3846, `odswiezTerminInr`&nbsp;3859, `addInr`&nbsp;3868, `inrKeysOk`&nbsp;3954

*ODSTĘP MIĘDZY POMIARAMI INR* — `inrOdstep`&nbsp;3969, `inrTerminKey`&nbsp;3977, `inrDoTerminu`&nbsp;3989, `dniTxt`&nbsp;3998, `renderInr`&nbsp;4000, `inrChart`&nbsp;4072

*STATUS PUDEŁKA* — `relTime`&nbsp;4101, `devDayMon`&nbsp;4110

*DIAGNOSTYKA — surowe zdarzenia z pudełka obok tego, co aplikacja* — `renderTesty`&nbsp;4145, `renderBoxLog`&nbsp;4191, `logPrzelacz`&nbsp;4227, `renderNvsFailLog`&nbsp;4235

*KOLEJKA ZAPISÓW — to samo, co pudełko ma w pamięci nieulotnej.* — `magazyn`&nbsp;4280, `oczekWczytaj`&nbsp;4288, `oczekZapisz`&nbsp;4293, `oczekIle`&nbsp;4296, `zapiszPewnie`&nbsp;4306, `zapiszCfg`&nbsp;4344, `bazaOdmowila`&nbsp;4362, `oczekWyslij`&nbsp;4386, `oczekOdmowy`&nbsp;4423

*OSTRZEŻENIA — celowo NIE schowane w Diagnostyce* — `ostrzKolejka`&nbsp;4439, `ostrzReguly`&nbsp;4461, `lm`&nbsp;4500, `ostrzMilczy`&nbsp;4506, `nvsMalo`&nbsp;4586, `opisNvsFailKey`&nbsp;4598, `stratyDotyczaLeku`&nbsp;4651, `ostrzStraty`&nbsp;4663, `stratyCicho`&nbsp;4697

*KOLEJKA, KTÓRA NIE SCHODZI* — `ostrzZatkana`&nbsp;4729

*EKRAN, KTÓRY SIĘ NIE NARYSOWAŁ* — `ostrzRysowanie`&nbsp;4759, `renderOstrzezenia`&nbsp;4774, `bezPokrycia`&nbsp;4784, `wierszZdarzenia`&nbsp;4790, `renderDiag`&nbsp;4807

*EKRAN ZDARZEN* — `evFiltr`&nbsp;4862, `evPasuje`&nbsp;4867, `renderEvents`&nbsp;4879, `renderOpenWarn`&nbsp;4919, `minutyDoPelna`&nbsp;4972, `opisLadowania`&nbsp;4984, `dni`&nbsp;5004, `opisLadowan`&nbsp;5007, `tempoZHistorii`&nbsp;5060, `prognozaDni`&nbsp;5069, `opisPrognozy`&nbsp;5081, `czasKrotko`&nbsp;5103, `opisCzuwania`&nbsp;5111, `renderStatus`&nbsp;5129

*ZAPAS TABLETEK* — `yesterdayKey`&nbsp;5368, `dayAfter`&nbsp;5371, `pillsBaseInfo`&nbsp;5382, `settlePills`&nbsp;5392, `dniZapasu`&nbsp;5432, `renderPills`&nbsp;5445, `savePills`&nbsp;5466, `setPills`&nbsp;5477

*USTAWIENIA* — `renderKafelki`&nbsp;5488, `renderSettings`&nbsp;5515, `tydzienZPol`&nbsp;5570, `renderWeekEditor`&nbsp;5582, `odswiezPodpowiedzTygodnia`&nbsp;5599, `tydzienZmieniony`&nbsp;5613, `rownajTydzien`&nbsp;5614, `renderPlanList`&nbsp;5662, `renderExceptions`&nbsp;5689, `wyslijSiec`&nbsp;5747

*POWIADOMIENIA NA TELEFON — BOT TELEGRAM (D67)* — `tgTokenPoprawny`&nbsp;5789, `tgZapytaj`&nbsp;5799, `tgKodParowania`&nbsp;5839, `tgZnajdzCzat`&nbsp;5855, `tgPolacz`&nbsp;5923, `tgProbna`&nbsp;5956, `tgOdlacz`&nbsp;5963, `renderTgStan`&nbsp;5985

*AKTUALIZACJA PROGRAMU PUDEŁKA (D59)* — `sprawdzAktualizacje`&nbsp;6100, `pobierzOpisFirmware`&nbsp;6106, `wyslijAktualizacje`&nbsp;6127, `anulujAktualizacje`&nbsp;6172, `renderOta`&nbsp;6178, `renderNetStan`&nbsp;6463

*SIECI WIDZIANE PRZEZ PUDEŁKO* — `opisSygnalu`&nbsp;6548, `renderSkan`&nbsp;6554, `szukajSieci`&nbsp;6606, `wybierzSiec`&nbsp;6614, `wyslijPolecenieSieci`&nbsp;6632, `siecZIndeksu`&nbsp;6642, `tzChanged`&nbsp;6676, `cfgTime`&nbsp;6681, `addSlot`&nbsp;6689, `zapiszPlanDnia`&nbsp;6702, `saveConfig`&nbsp;6720, `inrKrokiZakresu`&nbsp;6790, `opcjeInr`&nbsp;6797, `inrZakresZmieniony`&nbsp;6808, `wypelnijListyZakresu`&nbsp;6821, `saveInrRange`&nbsp;6832, `wypelnijListeOdstepu`&nbsp;6866, `saveInrEvery`&nbsp;6879, `odswiezPodpowiedzInr`&nbsp;6889

*ANALIZA* — `openTimeOf`&nbsp;6914, `openMinutes`&nbsp;6920, `sredniaPora`&nbsp;6944, `kwantyl`&nbsp;6952, `dniMiedzy`&nbsp;6960, `odstepyZPunktow`&nbsp;6974, `analyze`&nbsp;6983, `inrContext`&nbsp;7085

*WYKRESY ANALIZY* — `komorkaRytmu`&nbsp;7144, `rytmSVG`&nbsp;7156, `poryWCzasieSVG`&nbsp;7218, `iskraSVG`&nbsp;7286, `dowSVG`&nbsp;7314, `dniRytmu`&nbsp;7350, `skutecznoscTygodniami`&nbsp;7371, `renderAnalysis`&nbsp;7399

*RAPORT* — `collectRows`&nbsp;7506, `makeReport`&nbsp;7543

*KONTEKST DNIA (TAGI)* — `tagiDnia`&nbsp;7694, `tagiPrzed`&nbsp;7702, `tagPrzelacz`&nbsp;7711

*KOPIA ZAPASOWA* — `zbierzKopie`&nbsp;7754, `opisKopii`&nbsp;7764

*KOPIA NA TELEGRAM* — `tgKopiaUst`&nbsp;7804, `tgCzatKopii`&nbsp;7811, `odswiezKopie`&nbsp;7818, `tgKopiaCzatZapisz`&nbsp;7826, `tgKopiaCzatZnajdz`&nbsp;7846, `tgKopiaWlacz`&nbsp;7876, `tgKopiaWylacz`&nbsp;7895, `kopiaNaTelegram`&nbsp;7904, `kopiaAutomat`&nbsp;7955

*WIEK KOPII* — `dniOdDaty`&nbsp;8002, `wiekKopiiTxt`&nbsp;8008, `renderKopiaStan`&nbsp;8016, `zapiszKopie`&nbsp;8031

*ODTWARZANIE Z KOPII* — `policzOdtworzenie`&nbsp;8065, `wczytajKopie`&nbsp;8095, `kopiaCzytelna`&nbsp;8100, `odtworzKopie`&nbsp;8110, `kopiaWybrana`&nbsp;8133

*KOPIE Z BAZY* — `kopieZBazy`&nbsp;8157, `odtworzZBazy`&nbsp;8189, `exportCsv`&nbsp;8201

*NAWIGACJA* — `wrocZEkranu`&nbsp;8302


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
