/* Minimalne atrapy Arduino/ESP32, zeby skompilowac PRAWDZIWE funkcje
   z PillBox.ino na komputerze i uruchomic na nich testy.
   Nie przepisujemy logiki - wycinamy ja skryptem prosto z .ino.        */
#pragma once
/* cstdint MUSI byc jawnie - uint8_t/uint32_t przychodzily tu kiedys tranzytywnie
   przez cstdio, ale od g++ 13 juz nie. Bez tego kompilacja sypie sie lawina
   ~150 bledow "does not name a type", wygladajaca na awarie logiki.          */
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <string>
#include <map>
#include <set>
#include <algorithm>

/* ---------- sterowany czas ---------- */
extern time_t FAKE_NOW;
inline time_t fake_time(void*) { return FAKE_NOW; }
#define time(x) fake_time(x)

/* ---------- Arduino String ---------- */
class String {
public:
  std::string s;
  String() {}
  String(const char* c) : s(c ? c : "") {}
  String(const std::string& c) : s(c) {}
  String(int v) { char b[24]; snprintf(b, sizeof b, "%d", v); s = b; }
  String(unsigned long v) { char b[24]; snprintf(b, sizeof b, "%lu", v); s = b; }
  int length() const { return (int)s.size(); }
  const char* c_str() const { return s.c_str(); }
  char charAt(int i) const { return i >= 0 && i < (int)s.size() ? s[i] : 0; }
  int indexOf(char c, int from = 0) const {
    auto p = s.find(c, from); return p == std::string::npos ? -1 : (int)p; }
  int indexOf(const String& c, int from = 0) const {
    auto p = s.find(c.s, from); return p == std::string::npos ? -1 : (int)p; }
  String substring(int a) const { return String(a >= (int)s.size() ? std::string() : s.substr(a)); }
  String substring(int a, int b) const {
    if (a >= (int)s.size() || b <= a) return String();
    return String(s.substr(a, b - a)); }
  long toInt() const { return atol(s.c_str()); }
  float toFloat() const { return (float)atof(s.c_str()); }
  void trim() {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    s = (a == std::string::npos) ? "" : s.substr(a, b - a + 1); }
  bool startsWith(const String& p) const { return s.rfind(p.s, 0) == 0; }
  String operator+(const String& o) const { return String(s + o.s); }
  String& operator+=(const String& o) { s += o.s; return *this; }
  /* Prawdziwy Arduino ma OSOBNE przeciazenia dla kazdego typu i to ma
     znaczenie: 'char' dokleja ZNAK, a liczby doklejaja swoj zapis dziesietny.
     Sam operator+=(char) nie wystarczy - uint16_t wolalby wtedy konwersje do
     char (wbudowana) niz do String (uzytkownika) i liczba doklejalaby sie
     jako smiec. Dlatego wyliczamy typy jawnie.                            */
  String& operator+=(char c)           { s += c; return *this; }
  String& operator+=(unsigned char v)  { s += std::to_string((unsigned)v); return *this; }
  String& operator+=(short v)          { s += std::to_string((int)v); return *this; }
  String& operator+=(unsigned short v) { s += std::to_string((unsigned)v); return *this; }
  String& operator+=(int v)            { s += std::to_string(v); return *this; }
  String& operator+=(unsigned int v)   { s += std::to_string(v); return *this; }
  String& operator+=(long v)           { s += std::to_string(v); return *this; }
  String& operator+=(unsigned long v)  { s += std::to_string(v); return *this; }
  bool operator==(const String& o) const { return s == o.s; }
  bool operator!=(const String& o) const { return s != o.s; }
};
inline String operator+(const char* a, const String& b) { return String(std::string(a) + b.s); }

/* ---------- Preferences (NVS) ---------- */
class Preferences {
public:
  std::map<std::string, std::string> str;
  std::map<std::string, unsigned short> us;
  std::map<std::string, short> sh;
  std::map<std::string, unsigned long> ul;
  /* --- Otwieranie uchwytu tak, jak robi to PRAWDZIWE Preferences --------
     To nie jest szczegol. Preferences w rdzeniu ESP32 to JEDEN globalny
     obiekt z JEDNYM uchwytem, a jego begin() wyglada tak:

         bool Preferences::begin(...) { if (_started) return false; ... }
         void Preferences::end()      { nvs_close(_handle); _started = false; }

     Czyli: begin() na juz otwartym uchwycie NIC nie robi i zwraca false,
     a end() tego zagniezdzonego wywolania ZAMYKA uchwyt funkcji nadrzednej.
     Wszystko, co ta funkcja zapisze pozniej, przepada.

     Atrapa zwracala dotad zawsze true i miala puste end(), wiec ta cala
     klasa bledow byla dla testow NIEWIDZIALNA - i faktycznie jeden taki
     siedzial w logbookAdd() (B22), przez co czarna skrzynka nie zapisala
     ani jednej linijki. Nikt tego nie zauwazyl, bo przed naprawa B5 zapis
     do NVS nie meldowal niepowodzenia.                                   */
  bool _started = false;
  int  zagniezdzoneBegin = 0;   // ile razy begin() trafil na otwarty uchwyt
  /* Gdy true, zapis przy zamknietym uchwycie zawodzi - jak w NVS. Domyslnie
     false, zeby przygotowanie danych w testach moglo pisac wprost.       */
  bool strict = false;

  bool begin(const char*, bool = false) {
    if (_started) { zagniezdzoneBegin++; return false; }
    _started = true; return true;
  }
  void end() { _started = false; }
  String getString(const char* k, const char* d = "") {
    auto it = str.find(k); return String(it == str.end() ? std::string(d) : it->second); }
  String getString(const char* k, const String& d) {
    auto it = str.find(k); return it == str.end() ? d : String(it->second); }
  /* Prawdziwe NVS zwraca liczbe zapisanych bajtow, a ZERO gdy zapis sie
     nie udal. Atrapa musi to odwzorowac, inaczej testy nie dotknelyby
     jedynej sciezki, w ktorej dane znikaja po cichu.
     failKeys pozwala udawac awarie konkretnego klucza.                 */
  std::set<std::string> failKeys;
  size_t putString(const char* k, const String& v) {
    if (failKeys.count(k)) return 0;
    if (strict && !_started) return 0;      // NVS: zapis bez uchwytu nie przechodzi
    str[k] = v.s;
    /* PRAWDZIWE putString zwraca liczbe ZAPISANYCH BAJTOW, wiec dla pustego
       napisu ZERO - czyli tyle samo, co zapis nieudany. Atrapa zwracala tu
       `size()+1` i przez to pusty zapis wygladal na udany, a cala klasa
       bledow byla dla testow NIEWIDZIALNA. Kosztowalo to falszywy alarm
       "utrata danych" u Kuby i siec WiFi, ktora nie chciala sie przyjac.
       Trzeci raz ta sama lekcja co w D30: atrapa lagodniejsza od oryginalu
       mierzy inna rzecz niz sie mysli.                                   */
    return v.s.size(); }
  unsigned short getUShort(const char* k, unsigned short d = 0) {
    auto it = us.find(k); return it == us.end() ? d : it->second; }
  size_t putUShort(const char* k, unsigned short v) {
    if (failKeys.count(k)) return 0;
    if (strict && !_started) return 0;
    us[k] = v; return sizeof(unsigned short); }
  /* Licznik znanych sieci WiFi. Jak putUShort: prawdziwe NVS zwraca liczbe
     zapisanych bajtow, a ZERO przy niepowodzeniu - i na tym zerze opiera sie
     cala decyzja, czy wolno skasowac haslo z bazy.                       */
  std::map<std::string, unsigned char> uc;
  unsigned char getUChar(const char* k, unsigned char d = 0) {
    auto it = uc.find(k); return it == uc.end() ? d : it->second; }
  size_t putUChar(const char* k, unsigned char v) {
    if (failKeys.count(k)) return 0;
    if (strict && !_started) return 0;
    uc[k] = v; return sizeof(unsigned char); }
  /* Daty ladowania trzymamy jako 32-bitowe znaczniki czasu. */
  std::map<std::string, unsigned int> ui;
  unsigned int getUInt(const char* k, unsigned int d = 0) {
    auto it = ui.find(k); return it == ui.end() ? d : it->second;
  }
  void putUInt(const char* k, unsigned int v) { ui[k] = v; }
  unsigned long getULong(const char* k, unsigned long d = 0) {
    auto it = ul.find(k); return it == ul.end() ? d : it->second; }
  void putULong(const char* k, unsigned long v) { ul[k] = v; }
  short getShort(const char* k, short d = 0) {
    auto it = sh.find(k); return it == sh.end() ? d : it->second; }
  void putShort(const char* k, short v) { sh[k] = v; }
  /* Kasowanie pojedynczego klucza - uzywa go lidLogClear(). Musi czyscic
     WSZYSTKIE mapy, bo prawdziwe NVS nie wie, jakiego typu byl wpis.    */
  bool remove(const char* k) {
    bool bylo = str.erase(k) || us.erase(k) || sh.erase(k) || ul.erase(k) || ui.erase(k);
    return bylo; }
  void wipe() { str.clear(); us.clear(); sh.clear(); ul.clear(); ui.clear();
                uc.clear();
                failKeys.clear(); _started = false; zagniezdzoneBegin = 0; }
};

/* ---------- ADC / GPIO ---------- */
extern int FAKE_ADC;
inline int  analogRead(int) { return FAKE_ADC; }
inline void analogReadResolution(int) {}
inline void analogSetPinAttenuation(int, int) {}
inline void pinMode(int, int) {}
inline void delay(unsigned long) {}
extern unsigned long FAKE_MILLIS;
inline unsigned long millis() { return FAKE_MILLIS; }
#define INPUT 0
#define ADC_11db 3

/* ---------- nazwy pinow plytki XIAO ESP32-C3 ---------- */
#define D0 2
#define D1 3
#define D2 4
#define D3 5

/* ---------- makra ---------- */
#define RTC_DATA_ATTR

/* Wyciszamy diagnostyke z config.h (chcemy czysty wynik testow). */
struct FakeSerial {
  template<class... A> void printf(A&&...) {}
  template<class A>    void println(A&&)   {}
};
extern FakeSerial Serial;

/* ---------- atrapa WiFi ----------
   Tylko tyle, ile potrzebuje logika listy sieci: czy jestesmy polaczeni
   i z czym. Zabezpieczenie "nie usuwaj jedynej sieci, przez ktora wlasnie
   gadamy" opiera sie dokladnie na tych dwoch odpowiedziach, wiec bez nich
   nie dalo by sie go przetestowac.                                      */
#define WL_IDLE_STATUS 0
#define WL_CONNECTED   3

struct FakeWiFi {
  int _status = WL_IDLE_STATUS;
  String _ssid = String("");
  int status() const { return _status; }
  String SSID() const { return _ssid; }
  /* Wygodne ustawianie stanu w testach. */
  void _polacz(const char* ssid) { _status = WL_CONNECTED; _ssid = String(ssid); }
  void _rozlacz() { _status = WL_IDLE_STATUS; _ssid = String(""); }
};
inline FakeWiFi WiFi;
