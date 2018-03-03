#include "Adafruit_GFX/Adafruit_GFX.h"    // Core graphics library
#include "Adafruit_TFTLCD/Adafruit_TFTLCD.h" // Hardware-specific library

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define BLACK       0X0000
#define NAVY        0X000F
#define DARKGREEN   0X03E0
#define DARKCYAN    0X03EF
#define MAROON      0X7800
#define PURPLE      0X780F
#define OLIVE       0X7BE0
#define LIGHTGREY   0XC618
#define DARKGREY    0X7BEF
#define BLUE        0X001F
#define GREEN       0X07E0
#define CYAN        0X07FF
#define RED         0XF800
#define MAGENTA     0XF81F
#define YELLOW      0XFFE0
#define WHITE       0XFFFF
#define ORANGE      0XFD20
#define GREENYELLOW 0XAFE5

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

String crypto[2]={"", ""};
String unit[2]={"", ""};
unsigned long uptime=0;
char hours[3];
char minutes[3];
unsigned long hr[2]={0, 0};
float hrf[2]={0, 0};
unsigned long accepted[2]={0, 0};
unsigned long rejected[2]={0, 0};
unsigned long invalid[2]={0, 0};
int temp = 0;
int fan = 0;

char str[100];
int strIndex;
bool changes = false;

void setup(void) {
  Serial.begin(115200);
  tft.reset();
  uint16_t identifier = tft.readID();
  if(identifier==0x0101)
      identifier=0x9341;
  tft.begin(identifier);
  tft.setRotation(2);
  Serial.println("Boot");
}

void printCentered(String str, int color, int sizeStr, bool ret){
  int strSize = str.length()*sizeStr*6;
  int remaining = tft.width()-strSize;
  tft.setTextSize(sizeStr);
  tft.setTextColor(color);
  tft.setCursor(remaining/2, tft.getCursorY());
  tft.print(str);
  if((ret) && ((remaining/2)>15))
    tft.println();
}

void loop(void) {
  if(changes){
    tft.setCursor(0, 0);
    
    for(int i=0; i<2; i++){
      hrf[i] = (float)hr[i]/1000;
  
      printCentered(crypto[i], WHITE, 4, true);
  
      printCentered(String(hrf[i], 1) + unit[i], WHITE, 4, true);
  
      printCentered(String(accepted[i]) + " " + String(rejected[i]) + " " + String(invalid[i]), GREEN, 3, true);
      tft.println();
    }
  
    tft.setCursor(0, 230);
    sprintf(hours, "%02d", int(uptime/60));
    sprintf(minutes, "%02d", int(uptime%60));
    printCentered(String(hours) + ":" + String(minutes), WHITE, 4, true);
    
    tft.setCursor(0, 290);
    if(temp>=70){
      printCentered(String(temp) + "C " + String(fan) + "%", ORANGE, 4, true);
    } else {
      printCentered(String(temp) + "C " + String(fan) + "%", GREEN, 4, true); 
    }
    changes = false;
  }
  
  if(Serial.available()>0) {
    char byteIn = Serial.read();
    str[strIndex] = byteIn;
    
    if(byteIn=='\n'){ //end of string
      str[strIndex] = '\0';
      strIndex = 0;

      char * var = strtok(str,";");
      char * value = strtok(NULL, ";");
      if(strcmp(var, "")!=0 && strcmp(value, "")!=0){
        Serial.println(var);
        Serial.println(value);
        if(strcmp(var, "0")==0){
          crypto[0]=String(value);
        }else if(strcmp(var, "1")==0){
          crypto[1]=String(value);
        }else if(strcmp(var, "2")==0){
          unit[0]=String(value);
        }else if(strcmp(var, "3")==0){
          unit[1]=String(value);
        }else if(strcmp(var, "4")==0){
          uptime=atol(value);
        }else if(strcmp(var, "5")==0){
          hr[0]=atol(value);
        }else if(strcmp(var, "6")==0){
          hr[1]=atol(value);
        }else if(strcmp(var, "7")==0){
          accepted[0]=atol(value);
        }else if(strcmp(var, "8")==0){
          accepted[1]=atol(value);
        }else if(strcmp(var, "9")==0){
          rejected[0]=atol(value);
        }else if(strcmp(var, "10")==0){
          rejected[1]=atol(value);
        }else if(strcmp(var, "11")==0){
          invalid[0]=atol(value);
        }else if(strcmp(var, "12")==0){
          invalid[1]=atol(value);
        }else if(strcmp(var, "13")==0){
          temp=atoi(value);
        }else if(strcmp(var, "14")==0){
          fan=atoi(value);
        }else if(strcmp(var, "15")==0){
          tft.fillScreen(BLACK);
          changes = true;
        }
      }
    }else{
      strIndex++;
    }
  }
}
