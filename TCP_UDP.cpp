#include "TCP_UDP.h"


File file1;
WiFiUDP Uclient;
WiFiClient Tclient;
WiFiClient TRclient;
WiFiServer Server(23);
WiFiMulti wifiMulti;

int count = 0;
bool port_send_flg = false,wificont_flg = false;
bool first_init_send = true;
bool first_init_recv = true;
bool deep_init = true;

//const int numCommunicationData = 4096;
//const int numPartWavData = numCommunicationData/4;
//char communicationData[numCommunicationData];
//char partWavData[numPartWavData];

extern i2s_config_t i2s_config;

//wifi连接
void MUlti_Wifi(const char* ssid1, const char* password1,const char* ssid2, const char* password2){
  // 先断开所以连接
  WiFi.disconnect();
  wifiMulti.addAP(ssid1, password1);
  wifiMulti.addAP(ssid1, password2);
  Serial.println("[*]>>> Connecting Wifi...");
  unsigned long startTime = millis();
  if(wifiMulti.run() == WL_CONNECTED) {
      Serial.println("");
      Serial.println("[√]>>> WiFi connected");
      Serial.println("[√]>>> IP address: ");
      Serial.println(WiFi.localIP());
      wificont_flg = true;
  } 
  else if(millis() - startTime < 5000){
    delay(100);
    Serial.print(".");
  }
  else{
    Serial.println("\n connect error");
  }
}

void connectWifi(const char* ssid, const char* password) {
  Serial.print("Connecting as wifi client to SSID: ");
  Serial.println(ssid);

  // 先断开所以连接
  WiFi.disconnect();
  //选择连接模式
  if (WiFi.getMode() != WIFI_STA) {
    WiFi.mode(WIFI_STA);
  }
  
  unsigned long startTime = millis();
  //连接诶wifi
  WiFi.begin ( ssid, password );
  TFT_pri_txt(0.5,0,0,"[*]>>> Begin WiFi");
  WiFi.printDiag(Serial);

  // ... Give ESP 10 seconds to connect to station.
  //wifi保持持续连接
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 5000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  // wifi是否连接
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected; IP address: ");
    Serial.println(WiFi.localIP());
    wificont_flg = true;
    TFT_pri_txt(0.5,0,10,"[√]>>> WiFi connected");
  } else {
    Serial.print("WiFi connect failed to ssid: ");
    Serial.println(ssid);
    Serial.print("WiFi password <");
    Serial.print(password);
    Serial.println(">");
    Serial.println("Check for wrong typing!");
    TFT_pri_txt(0.5,0,10,"[X]>>> WiFi connecte error");
  }
} 

void smart_wifi(){
  WiFi.mode(WIFI_AP_STA);
  WiFi.beginSmartConfig();

  //Wait for SmartConfig packet from mobile
  Serial.println("Waiting for SmartConfig.");
  TFT_pri_txt(0.5,0,10,"[*]>>> Begin Smart WiFi ");
  while (!WiFi.smartConfigDone()) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n SmartConfig received.");
  //Wait for WiFi to connect to AP
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  TFT_pri_txt(0.5,0,15,"[*]>>> Smart WiFi Connected");
  Serial.println("WiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

bool tcp_connect(const char* server, int hiPort){
  if(Tclient.connect(server, hiPort)){
    Serial.printf("\n[√]>>> TCP Server %s connect success\n",server);
    TFT_pri_txt(0.5,0,40,"[*]>>> TCP Server connect success");
    return true;
  }
  else{
      Serial.printf("\n[X]>>> TCP Server %s connect error\n",server);
      TFT_pri_txt(0.5,0,40,"[*]>>> TCP Server connect error");
      return false;
  }  
}

void Send_Msg(const char* server,unsigned int hiPort,int modes){
  if(modes == 0){
    if (Tclient.connect(server, hiPort)) {
      Serial.println(F("[√]>>> TCP port connected"));
      Tclient.write("1234",4);
      port_send_flg = true;
      Tclient.stop();  
    }
    else {
      Serial.println(F("[X]>>> TCP connection failed"));
      file1.close();
    }
  }
  else{
    Uclient.beginPacket(server, hiPort);
    Uclient.println("1234");
    Uclient.endPacket();
  }
}


void UDP_File_Tra(const char* server,const char* filename){
  file1 = SPIFFS.open(filename, "r");
  if (!file1) {
    Serial.println(F("[X]>>> SD open fail"));
  }
  if (!file1.seek(0)){
    Serial.println(F("[X]>>> Rewind fail"));
    file1.close();
  } 
  Uclient.beginPacket(server, 1234);
  Serial.println(F("[√]>>> Data Writing........."));
  #define bufSizeFTP 1024
  uint8_t clientBuf[bufSizeFTP];
  size_t clientCount = 0;
  //将文件读取上传
  while (file1.available()) {
    clientBuf[clientCount] = file1.read();
    clientCount++;
    if (clientCount > (bufSizeFTP - 1)) {
      Uclient.write((const uint8_t *) &clientBuf[0], bufSizeFTP);
      clientCount = 0;
      delay(1);
    }
  }
  if (clientCount > 0) Uclient.write((const uint8_t *) &clientBuf[0], clientCount);
   //关闭数据传输连接
  Uclient.endPacket();
  Serial.println(F("[√]>>> Data transmission success"));
  //文件读写操作结束
  file1.close();
  Serial.println(F("[√]>>> file closed"));
}

int TCP_Data_Tra(const char* server,int hiPort,bool modes){
    if(first_init_send){
      I2S_Init(I2S_MODE_RX, I2S_BITS_PER_SAMPLE_32BIT);
      //i2s_start(I2S_NUM_0);
      tcp_connect(server, hiPort);
      first_init_send = !first_init_send;
    }
    const int numCommunicationData = 4096;
    const int numPartWavData = numCommunicationData/4;
    char communicationData[numCommunicationData];
    char partWavData[numPartWavData];
    if(modes == 0){
      if(count == 0) {
        Serial.print("[√]>>> ");
        //TFT_pri_txt(0.5,0,60,"[*]>>> Data coming ...");
      }
      //将文件读取上传
      I2S_Read(communicationData, numCommunicationData);
      //01234567
      for (int i = 0; i < numCommunicationData/8; ++i) {
        partWavData[2*i] = communicationData[8*i + 2];
        partWavData[2*i + 1] = communicationData[8*i + 3];
      }
      Tclient.write((const byte*)partWavData, numPartWavData);
      count ++;
      if(count >10){
        Serial.print(".");
        count = 1;
      }
    }
    else if (Tclient.available()){
      #define bufSize 1024
      uint8_t clientBuf[bufSize];
      size_t clientCount = 0;
      if(!SD.begin()){
        Serial.println("[X]>>> SPIFFS Mount Failed");
        return 0;
      }
      file1 = SD.open("/sound.wav", "w");
      Serial.println("[*]>>> Data is coming.....\n");
      while (Tclient.available()){
        clientBuf[clientCount++] = Tclient.read();
        //if(clientCount/10%10 == 0) Serial.print(".");
        if (clientCount > (bufSizeFTP - 1)){
          file1.write((const uint8_t *) &clientBuf[0], bufSize);
          clientCount = 0;
          delay(1);
        }
      }
      if (clientCount > 0) file1.write((const uint8_t *) &clientBuf[0], clientCount);
      //文件读写操作结束
      Serial.println(F("[√]>>> file closed"));
      file1.close();
    }
}

bool Rtcp_conect(const char* server, int hiPort){
  if(TRclient.connect(server, hiPort)){
    Serial.printf("\n[√]>>> TCP Wav Server %s connect success\n",server);
    TFT_pri_txt(0.5,0,90,"[*]>>> TCP Wav Server connect success");
    return true;
  }
  else{
      Serial.printf("\n[X]>>> TCP Wav Server %s connect error\n",server);
      TFT_pri_txt(0.5,0,90,"[*]>>> TCP Wav Server connect error");
      return false;
  }
}

void Play_Wav(const char* server,int hiPort){
  if(first_init_recv){
    Play_I2S_Init(12000);
    delay(10);
    Server.begin();
    Server.setNoDelay(true);
    first_init_recv = !first_init_recv;
  }
  
  //i2s_driver_uninstall(I2S_NUM_0);
//  delay(10);
//  Server.begin();
//  Server.setNoDelay(true);
  //i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  //i2s_set_pin(I2S_NUM_0, NULL);
  //set sample rates of i2s to sample rate of wav file
  //i2s_set_sample_rates(I2S_NUM_0, 12000);
  while(!Server.hasClient()){;}
  if(Server.hasClient()){
    TRclient = Server.available();
    #define bufSize 1024
    char clientBuf[bufSize];
    size_t clientCount = 0;
    while(!TRclient.available()){;}
    while(TRclient.available()){
      clientBuf[clientCount++] = TRclient.read();
      if (clientCount > (bufSizeFTP - 1)){
        I2S0_Write(clientBuf,bufSize); 
        clientCount = 0;
        //delay(1);
      }
    }
    if (clientCount > 0) I2S0_Write(clientBuf,clientCount);
    TRclient.stop(); 
    Serial.print("[√]>>> play ok");
    //TFT_pri_txt(0.5,0,110,"[*]>>> play ok");
  }
  else {
    Serial.print("[X]>>> play error");
    //TFT_pri_txt(0.5,0,110,"[*]>>> play error");
  }
  count = 0;
  first_init_send = !first_init_send;
  i2s_zero_dma_buffer(I2S_NUM_1);
  //i2s_stop(I2S_NUM_0);
  i2s_driver_uninstall(I2S_NUM_1); //stop & destroy i2s driver 
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}
