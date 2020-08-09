// Countdown Timer
// Required HX1230 96x68 LCD FB library and 3 fonts from PropFonts library, both on my GitHub
// (c) 2020 Pawel A. Hernik
// YouTube video: https://youtu.be/rDXpSH2rGX0

/*
  CONNECTIONS:

  HX1230 96x68 LCD connections (header on the bottom, from left):
  #1 RST - D6 or any digital
  #2 CE  - D7 or any digital
  #3 N/C
  #4 DIN - D11/MOSI 
  #5 CLK - D13/SCK
  #6 VCC - 3V3
  #7 BL  - D8 via 100ohm resistor
  #8 GND - GND

  The buzzer is connected directly to D4 via 100ohm resistor, but using extra transistor is recommended
  buzzer + -> D4 via 100ohm resistor
  buzzer - -> GND

  The button is connected between D3 and GND
*/
 
/*
Current draw tests:
In MODE_SET and MODE_PRESET
BL ON:  9.74mA
BL OFF: 7.74mA
SLEEP:  0.01mA/13uA

In MODE_COUNTDOWN
BL ON:  2.37mA
BL OFF: 0.16mA

In MODE_ALARM
2-13mA
*/

#define LCD_RST 6
#define LCD_CS  7
#define LCD_BL  8

#define AUDIO   4

#include "HX1230_FB.h"
#include <SPI.h>
HX1230_FB lcd(LCD_RST, LCD_CS);

#include "bold13x20digtop_font.h"
#include "times_dig_16x24ext_font.h"
#include "c64enh_font.h"

#include "digits.h"

//#define REAL_SLEEP_500MS 568L 
#define REAL_SLEEP_500MS 564L 

#define NUM_PRESETS 7
#define NUM_STYLES  4

#define BACKLIGHT_TIME 15000L
#define LCD_TIME       40000L
#define BAT_MIN        2950
#define BAT_MAX        4150

// examples
char *presetName[NUM_PRESETS]=          { "Eggs","Apple Pie","Sponge Cake","Baked Potato","Pasta","Pizza","Baked Pasta" };
unsigned char presetTime[NUM_PRESETS*4]={ 0,4,3,0, 7,5,0,0,   4,5,0,0,      3,5,0,0,      0,6,3,0, 1,8,0,0, 2,5,3,0 };

char buf[20];
int alarmTime = 0;
int m1=0,m0=0,s1=0,s0=8;
int mode=0,opt=0,pres=0,style=0;
long curTime, loopTime, frameTime=0;
volatile long lightTime=BACKLIGHT_TIME, lcdTime=LCD_TIME;

#define MODE_SET       0
#define MODE_COUNTDOWN 1
#define MODE_ALARM     2
#define MODE_PRESET    3

#define OPT_M1    0
#define OPT_M0    1
#define OPT_S1    2
#define OPT_S0    3
#define OPT_START 4


// ------------------------------------------------
#define MAXSIN 255
const uint8_t sinTab[91] PROGMEM = {
0,4,8,13,17,22,26,31,35,39,44,48,53,57,61,65,70,74,78,83,87,91,95,99,103,107,111,115,119,123,
127,131,135,138,142,146,149,153,156,160,163,167,170,173,177,180,183,186,189,192,195,198,200,203,206,208,211,213,216,218,
220,223,225,227,229,231,232,234,236,238,239,241,242,243,245,246,247,248,249,250,251,251,252,253,253,254,254,254,254,254,
255
};

int fastSin(int i)
{
  while(i<0) i+=360;
  while(i>=360) i-=360;
  if(i<90)  return(pgm_read_byte(&sinTab[i])); else
  if(i<180) return(pgm_read_byte(&sinTab[180-i])); else
  if(i<270) return(-pgm_read_byte(&sinTab[i-180])); else
            return(-pgm_read_byte(&sinTab[360-i]));
}

int fastCos(int i)
{
  return fastSin(i+90);
}

// ------------------------------------------------

void drawProgress(int val, int valMax)
{
   style ? drawProgressRect(val, valMax) : drawProgressRound(val, valMax);
}

// ------------------------------------------------
void drawProgressRect(int val, int valMax)
{
  int lenMax = 96+96+68-16+68-16;
  int len = (long)lenMax*val/valMax;
  len&=~3;

  lcd.setDither(4);
  lcd.fillRectD(0,0,96,8,1);
  lcd.fillRectD(0,68-8,96,8,1);
  lcd.fillRectD(0,8,8,68-16,1);
  lcd.fillRectD(96-8,8,8,68-16,1);

  if(len<96/2) { // TR
    lcd.fillRect(96/2,0,len,8,1);
    return;
  }
  lcd.fillRect(96/2,0,96,8,1);
  
  if(len<96/2+68-16) { // R
    lcd.fillRect(96-8,8,8,len-96/2,1);
    return;
  }
  lcd.fillRect(96-8,8,8,68-16,1);

  if(len<96/2+68-16+96) { // B
    lcd.fillRect(96-(len-96/2-68+16),68-8,len-96/2-68+16,8,1);
    return;
  }
  lcd.fillRect(0,68-8,96,8,1);
  
  if(len<96/2+68-16+96+68-16) { // L
    lcd.fillRect(0,68-8-(len-96/2-68+16-96),8,len-96/2-68+16-96,1);
    return;
  }
  lcd.fillRect(0,8,8,68-16,1);

  lcd.fillRect(0,0,len-(96/2+68-16+96+68-16),8,1); // TL
}

// ------------------------------------------------
void drawProgressRound(int val, int valMax)
{
  int cx,cy;
  int sx,sy;
  int xs0,ys0,xe0,ye0;
  int xs1,ys1,xe1,ye1;
  cx=96/2;
  cy=68/2;
  int rx0=47, ry0=33;
  int rx1=rx0-7, ry1=ry0-7;
  int mina=0;
  int maxa=360;
  int st=5;
  lcd.setDither(4);
  for(int i=mina; i<maxa; i+=st) {
    sx = fastCos(i-90);
    sy = fastSin(i-90);
    xs0 = cx+sx*rx0/MAXSIN;
    ys0 = cy+sy*ry0/MAXSIN;
    xe0 = cx+sx*rx1/MAXSIN;
    ye0 = cy+sy*ry1/MAXSIN;
    sx = fastCos(i-90+st);
    sy = fastSin(i-90+st);
    xs1 = cx+sx*rx0/MAXSIN;
    ys1 = cy+sy*ry0/MAXSIN;
    xe1 = cx+sx*rx1/MAXSIN;
    ye1 = cy+sy*ry1/MAXSIN;
    if((long)valMax*(i-mina)/(maxa-mina)<val) {
      lcd.fillTriangle(xs0,ys0,xe0,ye0,xe1,ye1, 1);
      lcd.fillTriangle(xs1,ys1,xe1,ye1,xs0,ys0, 1);
    } else {
      lcd.fillTriangleD(xs0,ys0,xe0,ye0,xe1,ye1, 1);
      lcd.fillTriangleD(xs1,ys1,xe1,ye1,xs0,ys0, 1);
    }
  }
}

// -----------------------------------------------
#define BUTTON 3
int buttonState;
int prevState = HIGH;
long btDebounce    = 30;
long btMultiClick  = 300;
long btLongClick   = 300;
long btLongerClick = 1500;
long btLongerClick2 = 3000;
long btTime = 0, btTime2 = 0;
int clickCnt = 1;

// 0=idle, 1,2,3=click, -1,-2=longclick
int checkButton()
{
  //pinMode(BUTTON, INPUT_PULLUP);
  int state = digitalRead(BUTTON);
  if( state == LOW && prevState == HIGH ) { btTime = millis(); prevState = state; return 0; } // button just pressed
  if( state == HIGH && prevState == LOW ) { // button just released
    prevState = state;
    if( millis()-btTime >= btDebounce && millis()-btTime < btLongClick ) { 
      if( millis()-btTime2<btMultiClick ) clickCnt++; else clickCnt=1;
      btTime2 = millis();
      return clickCnt; 
    } 
  }
  if( state == LOW && millis()-btTime >= btLongerClick2 ) { prevState = state; return -3; }
  if( state == LOW && millis()-btTime >= btLongerClick ) { prevState = state; return -2; }
  if( state == LOW && millis()-btTime >= btLongClick ) { prevState = state; return -1; }
  return 0;
}

int prevButtonState=0;

int handleButton()
{
  prevButtonState = buttonState;
  buttonState = checkButton();
  return buttonState;
}

// ------------------------------------------------
long readVcc() 
{
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1125300L / result; // Back-calculate AVcc in mV
  return result;
}
// ------------------------------------------------
#include <avr/sleep.h>
#include <avr/wdt.h>

enum wdt_time {
  SLEEP_15MS,
  SLEEP_30MS, 
  SLEEP_60MS,
  SLEEP_120MS,
  SLEEP_250MS,
  SLEEP_500MS,
  SLEEP_1S,
  SLEEP_2S,
  SLEEP_4S,
  SLEEP_8S,
  SLEEP_FOREVER
};

ISR(WDT_vect) { wdt_disable(); }

void powerDown(uint8_t time)
{
  ADCSRA &= ~(1 << ADEN);  // turn off ADC
  if(time != SLEEP_FOREVER) { // use watchdog timer
    wdt_enable(time);
    WDTCSR |= (1 << WDIE);  
  }
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // most power saving
  cli();
  sleep_enable();
  sleep_bod_disable();
  sei();
  sleep_cpu();
  // ... sleeping here
  sleep_disable();
  ADCSRA |= (1 << ADEN); // turn on ADC
}

// --------------------------------------------------------------------------

long v;
void checkBattery(bool disp)
{
  v = readVcc();
  if(!disp && v>BAT_MIN) return;
  lcd.cls();
  lcd.setFont(c64enh);
  lcd.setDigitMinWd(6);
  dtostrf(v/1000.0,1,3,buf);
  int x=14,y=v<BAT_MIN ? 6*8 : 3*8;
  x=lcd.printStr(x, y, "Vcc: ");
  x=lcd.printStr(x, y, buf);
  lcd.printStr(x+2, y, "V");
  snprintf(buf,4,"%d",map(constrain(v,BAT_MIN,BAT_MAX),BAT_MIN,BAT_MAX,0,100));
  x=lcd.printStr(96/2-10, y+12, buf);
  lcd.printStr(x+2, y+12, "%");
  lcd.display();
  
  if(v>BAT_MIN) return;
  
  lcd.printStr(ALIGN_CENTER, 8, "Low");
  lcd.printStr(ALIGN_CENTER, 24, "Battery !");
  lcd.display();
  for(int i=0;i<12;i++) {
    digitalWrite(LCD_BL, HIGH); powerDown(SLEEP_15MS);
    digitalWrite(LCD_BL, LOW); powerDown(SLEEP_250MS);
  }
  powerDown(SLEEP_8S);
  powerDown(SLEEP_8S);
  // disable LCD controller and power down forever to save battery
  lcd.sleep(true);
  powerDown(SLEEP_FOREVER);
  lcd.sleep(false);
}

// ------------------------------------------------

void buttonInt()
{
  lightTime = BACKLIGHT_TIME;
  lcdTime   = LCD_TIME;
}

// ------------------------------------------------

void setup(void) 
{
  Serial.begin(9600);
  pinMode(LCD_BL, OUTPUT);
  pinMode(AUDIO, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  digitalWrite(LCD_BL, HIGH);
  //attachInterrupt(digitalPinToInterrupt(3), buttonInt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), buttonInt, FALLING);
  lcd.init();
  lcd.cls();
  checkBattery(true);
  playSound(AUDIO,3000,300);
  playSound(AUDIO,4000,400);
  playSound(AUDIO,5000,300);
}

// ------------------------------------------------

void drawTime(int val,int mode=0)
{
  int m=val/60;
  int s=val-m*60;
  int x;
  if(mode) snprintf(buf,10,"%02d:%02d",m,s); else snprintf(buf,10,"%d:%02d",m,s);
  switch(style) {
    case 0: lcd.setFont(Bold13x20); lcd.setDigitMinWd(13); break;
    case 1: lcd.setFont(times_dig_16x24); lcd.setDigitMinWd(16); break;
    case 2: setSize7(17,35,5,1); spacing=m<20||mode?2:1; break;
    case 3: setSizeO(17,35,5); spacing=m<20||mode?2:1; break;
  }
  if(style>1) {
    x = (*buf=='1'&&!mode?-(digWd-segThick)/2:0)+(SCR_WD-numberWidth(buf))/2;
    style==2?printNumber7(buf,x,(SCR_HT-36)/2):printNumberO(buf,x,(SCR_HT-36)/2);
    return;
  }
  lcd.setCharMinWd(lcd.charWidth(':',false)+2);
  lcd.printStr(ALIGN_CENTER,(SCR_HT-lcd.fontHeight())/2,buf);
}

// ------------------------------------------------

void drawSel(int o)
{
  int x,y=SCR_HT-6;
  if(o>=4) { lcd.fillTriangle(SCR_WD/2-10,2+y,SCR_WD/2-10,2+y-10,SCR_WD/2+10,2+y-5,1); return; }
  static int xs[4*4]={
    22,22+14,57,57+14,
    17,17+17,59,59+17,
    15,15+19,60,60+19,
    15,15+19,60,60+19};
  x=xs[o+style*4];
  lcd.fillTriangle(x-5,y,x+5,y,x,y-10,1);
}

// ------------------------------------------------
// play simple tones
void playSound(int pin, long freq, long len) 
{
  int dutyH = 500; // 10 to silent, 500-regular
  long del = 1000000L/freq;
  for(long i=freq*len/1000L; i--;) {
    digitalWrite(pin,HIGH);
    delayMicroseconds(del*dutyH/1000);
    digitalWrite(pin,LOW);
    delayMicroseconds(del*(1000-dutyH)/1000);
  }
}

void playSoundBt(int pin, long freq, long len) 
{
  int dutyH = 500; // 10 to silent, 500-regular
  long del = 1000000L/freq;
  for(long i=freq*len/1000L; i--;) {
    digitalWrite(pin,HIGH);
    delayMicroseconds(del*dutyH/1000);
    digitalWrite(pin,LOW);
    delayMicroseconds(del*(1000-dutyH)/1000);
    if(digitalRead(BUTTON)==LOW) return;
  }
}

// ------------------------------------------------

void loop() 
{
  loopTime = frameTime<=0 ? 0 : millis()-frameTime;
  frameTime = millis();
  checkBattery(false);

  int bt = handleButton();

  if(mode==MODE_SET) { // set time
    lcd.cls();
    if(bt==-1 && prevButtonState>=0) {
      if(++opt>OPT_START) opt=OPT_M1;
      playSound(AUDIO,3000,100);
    }
    if(bt==-2) mode=MODE_PRESET;
    if(bt>0) {
      switch(opt) {
        case OPT_M1: if(++m1>9) m1=0; break;
        case OPT_M0: if(++m0>9) m0=0; break;
        case OPT_S1: if(++s1>5) s1=0; break;
        case OPT_S0: if(++s0>9) s0=0; break;
      }       
      playSound(AUDIO,4000,50);
    }
    alarmTime = (m1*10+m0)*60+s1*10+s0;
    drawSel(opt);
    drawTime(alarmTime,1);
    lcd.display();
    if(bt>0 && opt==OPT_START) {
      mode = MODE_COUNTDOWN;
      playSound(AUDIO,3000,500);
      curTime = alarmTime*1000L; 
    }
  }

  if(mode==MODE_PRESET) {  // presets menu
    lcd.cls();
    lcd.setFont(c64enh);
    for(int i=NUM_PRESETS;i--;) lcd.printStr(10,1+10*i,presetName[i]);
    if(bt>0) {
      if(++pres>=NUM_PRESETS) pres=0;
      playSound(AUDIO,3000,100);
    }
    if(bt==-1 && prevButtonState>=0) {
      m1=presetTime[pres*4+0];
      m0=presetTime[pres*4+1];
      s1=presetTime[pres*4+2];
      s0=presetTime[pres*4+3];
      mode = MODE_SET;
      opt = OPT_START;
      playSound(AUDIO,4000,500);
    }
    if(bt==-3) {
      if(++style>=NUM_STYLES) style=0;
      mode = MODE_SET;
    }
    lcd.fillTriangle(0,pres*10,0,pres*10+8,8,pres*10+4,1);
    lcd.display();
  }

  if(mode==MODE_SET || mode==MODE_PRESET) { // common for 0 and 3
    lightTime-=loopTime;
    if(lightTime<=0) {
      digitalWrite(LCD_BL, LOW);
      lightTime=0;
    } else digitalWrite(LCD_BL, HIGH);
 
    lcdTime-=loopTime;
    if(lcdTime<=0) {
      lcd.sleep(true);
      playSound(AUDIO,3000,200);
      powerDown(SLEEP_60MS);
      playSound(AUDIO,3000,200);
      powerDown(SLEEP_FOREVER);
      playSound(AUDIO,2000,200);
      while(digitalRead(BUTTON)==LOW);
      lcd.sleep(false);
    }
  }

  if(mode==MODE_COUNTDOWN) {  // countdown
    lcd.cls();
    drawTime(curTime/1000);
    drawProgress(curTime/1000,alarmTime);
    lcd.display();

    powerDown(SLEEP_500MS); loopTime+=REAL_SLEEP_500MS;
    powerDown(SLEEP_500MS); loopTime+=REAL_SLEEP_500MS;
    //delay(500);
    curTime-=loopTime;
    if(curTime<=0) mode=MODE_ALARM;
    if(curTime<6000) playSound(AUDIO,5000,50);
    long bt=millis();
    while(digitalRead(BUTTON)==LOW);
    if(millis()-bt>800) {
      mode=opt=0;
      playSound(AUDIO,4000,100);
      powerDown(SLEEP_60MS);
      playSound(AUDIO,4000,100);
    }
    lightTime-=loopTime;
    if(lightTime<=0 && curTime>10*1000L) {
      lightTime=0;
      digitalWrite(LCD_BL, LOW);
    } else digitalWrite(LCD_BL, HIGH);
  } else

  if(mode==MODE_ALARM) { // alarm
    lcd.cls();
    drawProgress(0,alarmTime);
    drawTime(0);
    lcd.display();
    digitalWrite(LCD_BL, LOW);
    playSoundBt(AUDIO,4000,150);
    powerDown(SLEEP_60MS);
    playSoundBt(AUDIO,5000,150);
    powerDown(SLEEP_60MS);
    playSoundBt(AUDIO,4000,150);
    powerDown(SLEEP_60MS);

    drawProgress(alarmTime,alarmTime);
    drawTime(0);
    lcd.display();
    digitalWrite(LCD_BL, HIGH);
    powerDown(SLEEP_250MS);

    if(digitalRead(BUTTON)==LOW) {
      while(digitalRead(BUTTON)==LOW);
      mode=opt=0;
    }
  }
}


