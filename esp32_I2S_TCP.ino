#include "TCP_UDP.h"

extern WiFiClient Tclient,TRclient;

//const char* ssid = "Xiaomi_A15B";
const char* ssid1 = "Xiaomi_A15B";
const char* password1 = "********";
const char* ssid2 = "TP-LINK_3ADB";
const char* password2 = "********";
const char* server = "192.168.2.23";
int hiPort = 6543;
int RhiPort = 6544;
int modes = 0;
int button_count,status_count=0;
int Button_Pin=19,LED_Pin=4;
int numPartWavData_count = 0;
extern bool wificont_flg,first_init_send,deep_init;
extern int count;
bool End_flg = false;


void setup() {
  Serial.begin(115200);
  pinMode(Button_Pin, INPUT_PULLUP);
  pinMode(LED_Pin,OUTPUT);
  digitalWrite(LED_Pin, HIGH);
  MUlti_Wifi(ssid1, password1, ssid2, password2);
  if(wificont_flg) digitalWrite(LED_Pin, LOW);
  else if(!wificont_flg) smart_wifi();
  Serial.println("[√]>>> please set button to record");
}



void loop(){
  while(!digitalRead(Button_Pin) && !End_flg){
    if(modes != 2){
      while(!digitalRead(Button_Pin)){
        digitalWrite(LED_Pin, HIGH);
        TCP_Data_Tra(server,hiPort,modes);
      }
    }
    End_flg = !End_flg;
    
  }
  while(digitalRead(Button_Pin) && End_flg){
    if(modes != 2){
      digitalWrite(LED_Pin, LOW);
      //关闭数据传输连接
      Tclient.stop();
      Serial.println(F("\n[√]>>> Data transmission success")); 
      Play_Wav(server,RhiPort);
      End_flg = !End_flg;
    }
  }
}
