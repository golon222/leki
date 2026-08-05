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
  bool begin(const char*, bool = false) { return true; }
  void end() {}
  String getString(const char* k, const char* d = "") {
    auto it = str.find(k); return String(it == str.end() ? std::string(d) : it->second); }
  String getString(const char* k, const String& d) {
    auto it = str.find(k); return it == str.end() ? d : String(it->second); }
  void putString(const char* k, const String& v) { str[k] = v.s; }
  unsigned short getUShort(const char* k, unsigned short d = 0) {
    auto it = us.find(k); return it == us.end() ? d : it->second; }
  void putUShort(const char* k, unsigned short v) { us[k] = v; }
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
  void wipe() { str.clear(); us.clear(); sh.clear(); ul.clear(); ui.clear(); }
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
