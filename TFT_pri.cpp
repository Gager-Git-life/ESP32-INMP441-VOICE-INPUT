#include "TFT_pri.h"

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

void TFT_Init(){
   //Set up the display
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK); 
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(50, 55);
  tft.println(F("Hello Gager"));
  //delay(1000);
  tft.fillScreen(TFT_BLACK); 
}

void TFT_pri_txt(float sizt,int x_txt,int y_txt,String txt){
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(sizt);
  tft.setCursor(x_txt, y_txt);
  tft.println(txt);
}
