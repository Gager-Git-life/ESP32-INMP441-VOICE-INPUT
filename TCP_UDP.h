#include <Arduino.h>
#include "I2S.h"
//#include "FS.h"
//#include "SPIFFS.h"
#include "TFT_pri.h"
#include <SD.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiMulti.h>


void smart_wifi();
void MUlti_Wifi(const char* ssid1, const char* password1,const char* ssid2, const char* password2);
void connectWifi(const char* ssid, const char* password);
bool tcp_connect(const char* server, int hiPort);
bool Rtcp_conect(const char* server, int hiPort);
void Send_Msg(const char* server,unsigned int hiPort,int modes);
void UDP_File_Tra(const char* server,const char* filename);
int TCP_Data_Tra(const char* server,int hiPort,bool modes);
void Play_Wav(const char* server,int hiPort);
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
