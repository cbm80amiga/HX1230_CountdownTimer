// 7-segment and outline digit font rendering module
// (c) 2020 by Pawel A. Hernik

int digWd, digHt;
int segWd, segHt;
int segThick,segSt;
int colOn=1,colOff=0;
int spacing=2;
 
/*
 Segment bits:
   00
  5  1
  5  1
   66
  4  2
  4  2
   33
*/

static unsigned char digits[10]={
  0b0111111, 0b0000110, 0b1011011, 0b1001111, 0b1100110, 0b1101101, 0b1111101, 0b0000111, 0b1111111, 0b1101111};

void setSize7(int wd, int ht, int th, int st)
{
  digWd = wd;
  digHt = ht;
  segThick = th;
  segSt = st;
  segWd = wd-(segSt+1)*2;
  segHt = (ht-segSt*2-3)/2;
  //Serial.print(segWd); Serial.print("x"); Serial.println(segHt);
}

void setSizeF(int wd, int ht, int th)
{
  digWd = wd;
  digHt = ht;
  segThick = th;
  segWd = wd;
  segHt = (ht-th*3)/2;
  //Serial.print(segWd); Serial.print("x"); Serial.println(segHt);
}

void setSizeO(int wd, int ht, int th)
{
  digWd = wd;
  digHt = ht;
  segThick = th;
  segWd = wd;
  segHt = (ht-th*3)/2;
  //Serial.print(segWd); Serial.print("x"); Serial.println(segHt);
}

void drawSeg7(int seg, int x, int y, int c)
{
  int i;
  switch(seg) {
    case 0: // top
      for(i=0;i<segThick;i++) lcd.drawLineHfast(x+(i<segSt?segSt-i:i-segSt),x+segWd-1-(i<segSt?segSt-i:i-segSt), y+i, c ? colOn : colOff);
      break;
    case 3: // bottom
      for(i=0;i<segThick;i++) lcd.drawLineHfast(x+(i<segSt?segSt-i:i-segSt),x+segWd-1-(i<segSt?segSt-i:i-segSt), y+segThick-1-i, c ? colOn : colOff);
      break;
    case 6: // middle
      for(i=0;i<segThick/2+1;i++) lcd.drawLineHfast(x+i,x+segWd-1-i, y-i, c ? colOn : colOff);
      for(i=1;i<segThick/2+1;i++) lcd.drawLineHfast(x+i,x+segWd-1-i, y+i, c ? colOn : colOff);
      break;
    case 1: // right
    case 2: // right
      for(i=0;i<segThick;i++) lcd.drawLineVfast(x+segThick-1-i,y+(i<segSt?segSt-i:i-segSt),y+segHt-1-(i<segSt?segSt-i:i-segSt), c ? colOn : colOff);
      break;
    case 4: // left
    case 5: // left
      for(i=0;i<segThick;i++) lcd.drawLineVfast(x+i,y+(i<segSt?segSt-i:i-segSt),y+segHt-1-(i<segSt?segSt-i:i-segSt), c ? colOn : colOff);
      break;
  }
}


int drawDigit7(int ch, int x, int y)
{
  int s,offs=segSt+1;
  switch(ch) {
    case '.':
      //lcd.fillRect(x,y+segHt+segHt+2,segThick,segThick,colOn); return segThick;
    case ':':
      //lcd.fillRect(x,y+ofs+(segHt-segThick)/2,segThick,segThick,colOn);
      //lcd.fillRect(x,y+segHt+1+ofs+(segHt-segThick)/2,segThick,segThick,colOn);
      //lcd.fillRect(x,y+      1+offs+(segHt-segThick)/2,segThick,segThick,colOn);
      //lcd.fillRect(x,y+segHt+1+offs+(segHt-segThick)/2,segThick,segThick,colOn);
      lcd.fillRect(x,y+      offs+1+(segHt-segThick)/2,segThick,segThick,colOn);
      lcd.fillRect(x,y+segHt+offs+1+(segHt-segThick)/2,segThick,segThick,colOn);
      return segThick; 
    case ' ':
      s=0; break;
    case '-':
      s=0b1000000; break;
    default:
      s=digits[ch&0xf];
  }
  drawSeg7(0,x+offs,           y,                  s&0b0000001);
  drawSeg7(1,x+digWd-segThick, y+offs,          s&0b0000010);
  drawSeg7(2,x+digWd-segThick, y+offs+segHt+1,  s&0b0000100);
  drawSeg7(3,x+offs,           y+segHt+segHt+offs-segThick+segSt+2,  s&0b0001000);
  drawSeg7(4,x,                y+offs+segHt+1,        s&0b0010000);
  drawSeg7(5,x,                y+offs,                s&0b0100000);
  drawSeg7(6,x+offs,           y+segHt+offs,        s&0b1000000);
  return digWd; 
}

// ------------------

int drawDigitF(int ch, int x, int y)
{
  int hc=(digHt-segThick*3)/2;
  ch = ch>='0' && ch<='9' ? ch-'0' : ch;
  switch(ch) {
    case '.':
      ///lcd.fillRectD(x,y+digHt-segThick,segThick,segThick,colOn);
       return segThick;
    case ':':
      lcd.fillRectD(x,y+(digHt/2-segThick)/2+2,segThick,segThick,colOn);
      lcd.fillRectD(x,y+digHt-1-(digHt/2-segThick)/2-segThick-1,segThick,segThick,colOn);
      return segThick;
    case ' ':
      break;
    case '-':
      lcd.fillRectD(x,y+hc+segThick,   digWd,segThick,colOn);
      break;
    case 0:
      lcd.fillRectD(x,               y,segThick,digHt,colOn);
      lcd.fillRectD(x+digWd-segThick,y,segThick,digHt,colOn);
      lcd.fillRectD(x+segThick,y,               digWd-segThick*2,segThick,colOn);
      lcd.fillRectD(x+segThick,y+digHt-segThick,digWd-segThick*2,segThick,colOn);
      break;
    case 1:
      lcd.fillRectD(x+digWd-segThick,y,segThick,digHt,colOn);
      break;
    case 2:
      lcd.fillRectD(x,y,               digWd,segThick,colOn);
      lcd.fillRectD(x,y+hc+segThick,   digWd,segThick,colOn);
      lcd.fillRectD(x,y+digHt-segThick,digWd,segThick,colOn);
      lcd.fillRectD(x,               y+segThick*2+hc,segThick,digHt-hc-segThick*3,colOn);
      lcd.fillRectD(x+digWd-segThick,y+segThick,     segThick,hc,colOn);
      break;
    case 3:
      lcd.fillRectD(x,y,               digWd-segThick,segThick,colOn);
      //lcd.fillRectD(x+segThick,y+hc+segThick,   digWd-segThick*1,segThick,colOn);
      //lcd.fillRectD(x,y+hc+segThick,   digWd-segThick,segThick,colOn); // full width
      lcd.fillRectD(x+digWd/3,y+hc+segThick,   digWd-segThick-digWd/3,segThick,colOn); // 2/3 width
      lcd.fillRectD(x,y+digHt-segThick,digWd-segThick,segThick,colOn);
      lcd.fillRectD(x+digWd-segThick,y,         segThick,digHt,colOn);
      break;
    case 4:
      lcd.fillRectD(x+digWd-segThick,y,             segThick,digHt,colOn);
      lcd.fillRectD(x,               y,             segThick,hc+segThick*2,colOn);
      lcd.fillRectD(x+segThick,      y+hc+segThick, digWd-segThick*2,segThick,colOn);
      //lcd.fillRectD(x+digWd-segThick,y+segThick,         segThick,digHt-segThick,colOn);
      //lcd.fillRectD(x,y,         segThick,hc+segThick*3,colOn);
      //lcd.fillRectD(x+segThick,y+hc+segThick*2,   digWd-segThick*2,segThick,colOn);
      break;
    case 5:
      lcd.fillRectD(x,y,               digWd,segThick,colOn);
      lcd.fillRectD(x,y+hc+segThick,   digWd,segThick,colOn);
      lcd.fillRectD(x,y+digHt-segThick,digWd,segThick,colOn);
      lcd.fillRectD(x,               y+segThick,     segThick,hc,colOn);
      lcd.fillRectD(x+digWd-segThick,y+segThick*2+hc,segThick,digHt-hc-segThick*3,colOn);
      break;
    case 6:
      lcd.fillRectD(x,y,                      digWd,segThick,colOn);
      lcd.fillRectD(x+segThick,y+hc+segThick, digWd-segThick,segThick,colOn);
      lcd.fillRectD(x,y+digHt-segThick,       digWd,segThick,colOn);
      lcd.fillRectD(x,               y+segThick,       segThick,digHt-segThick*2,colOn);
      lcd.fillRectD(x+digWd-segThick,y+hc+segThick*2,  segThick,digHt-hc-segThick*3,colOn);
      break;
    case 7:
      lcd.fillRectD(x+digWd-segThick,y,segThick,      digHt,colOn);
      lcd.fillRectD(x,               y,digWd-segThick,segThick,colOn);
      break;
    case 8:
      lcd.fillRectD(x,               y,segThick,digHt,colOn);
      lcd.fillRectD(x+digWd-segThick,y,segThick,digHt,colOn);
      lcd.fillRectD(x+segThick,y,               digWd-segThick*2,segThick,colOn);
      lcd.fillRectD(x+segThick,y+digHt-segThick,digWd-segThick*2,segThick,colOn);
      lcd.fillRectD(x+segThick,y+hc+segThick,            digWd-segThick*2,segThick,colOn);
      break;
    case 9:
      lcd.fillRectD(x,               y,                segThick,hc+segThick*2,colOn);
      lcd.fillRectD(x+digWd-segThick,y,                segThick,digHt,colOn);
      lcd.fillRectD(x+segThick,      y,                digWd-segThick*2,segThick,colOn);
      lcd.fillRectD(x+segThick,      y+hc+segThick,    digWd-segThick*2,segThick,colOn);
      lcd.fillRectD(x,               y+digHt-segThick, digWd-segThick,segThick,colOn);
      break;
  }
  return digWd; 
}

void rect(int x, int y, int w, int h, int col)
{
  lcd.drawLineHfast(x, x+w-1, y+h-1,col);
  lcd.drawLineHfast(x, x+w-1, y,    col);
  lcd.drawLineVfast(x,    y+1, y+h-2,col);
  lcd.drawLineVfast(x+w-1,y+1, y+h-2,col);
}

int drawDigitO(int ch, int x, int y)
{
  setSizeF(17-2,35-2,5-2);
  lcd.setDither(8);
  drawDigitF(ch,x+1,y+1);
  setSizeO(17,35,5);
  int hc=(digHt-segThick*3)/2;
  ch = ch>='0' && ch<='9' ? ch-'0' : ch;
  switch(ch) {
    case '.':
       ///rect(x,y+digHt-segThick,segThick,segThick,colOn);
       return segThick;
    case ':':
      rect(x,y+(digHt/2-segThick)/2+2,segThick,segThick,colOn);
      rect(x,y+digHt-1-(digHt/2-segThick)/2-segThick-1,segThick,segThick,colOn);
      return segThick;
    case ' ':
      break;
    case '-':
      rect(x,y+hc+segThick,   digWd,segThick,colOn);
      break;
    case 0:
      rect(x,y,digWd,digHt,colOn);
      rect(x+segThick-1,y+segThick-1,digWd-segThick*2+2,digHt-segThick*2+2,colOn);
      break;
    case 1:
      rect(x+digWd-segThick,y,segThick,digHt,colOn);
      break;
    case 2:
      lcd.drawLineHfast(x,          x+digWd-1,        y,  colOn); // top
      lcd.drawLineHfast(x,          x+digWd-segThick, y+segThick-1,  colOn); // top2
      lcd.drawLineHfast(x,          x+digWd-1,        y+digHt-1,  colOn); // bottom
      lcd.drawLineHfast(x+segThick, x+digWd-1,        y+digHt-segThick,  colOn); // bottom2
      lcd.drawLineHfast(x,          x+digWd-segThick, y+segThick+hc,  colOn); // mid
      lcd.drawLineHfast(x+segThick, x+digWd-1,        y+segThick+hc+segThick-1,  colOn); // mid2
      lcd.drawLineVfast(x+digWd-1,        y+1,               y+hc+segThick*2-1, colOn); // rt
      lcd.drawLineVfast(x+digWd-segThick, y+segThick-1,       y+hc+segThick, colOn); // rt le
      lcd.drawLineVfast(x,                y+hc+segThick+1,    y+digHt-2, colOn); // le
      lcd.drawLineVfast(x+segThick-1,     y+hc+segThick*2-1,  y+digHt-segThick, colOn); // le rt
      lcd.drawLineVfast(x,                y+1,                y+segThick-2, colOn); // le short
      lcd.drawLineVfast(x+digWd-1,        y+digHt-segThick+1, y+digHt-2, colOn); // rt short
      break;
    case 3:
      lcd.drawLineHfast(x,          x+digWd-1,        y,  colOn); // top
      lcd.drawLineHfast(x,          x+digWd-1,        y+digHt-1,  colOn); // bottom
      lcd.drawLineHfast(x,          x+digWd-segThick, y+segThick-1,  colOn); // top2
      lcd.drawLineHfast(x,          x+digWd-segThick, y+digHt-segThick,  colOn); // bottom2
      //lcd.drawLineHfast(x+segThick, x+digWd-segThick, y+segThick+hc,  colOn); // mid1
      //lcd.drawLineHfast(x+segThick, x+digWd-segThick, y+segThick*2+hc-1,  colOn); // mid2
      //lcd.drawLineHfast(x, x+digWd-segThick, y+segThick+hc,  colOn); // mid1 full width
      //lcd.drawLineHfast(x, x+digWd-segThick, y+segThick*2+hc-1,  colOn); // mid2 full width
      lcd.drawLineHfast(x+digWd/3, x+digWd-segThick, y+segThick+hc,  colOn); // mid1 2/3 width
      lcd.drawLineHfast(x+digWd/3, x+digWd-segThick, y+segThick*2+hc-1,  colOn); // mid2 2/3 width
      lcd.drawLineVfast(x+digWd-1,  y+1,                y+digHt-2, colOn); // rt-long
      lcd.drawLineVfast(x,          y+1, y+segThick-2, colOn); // le-top-short
      lcd.drawLineVfast(x,          y+digHt-segThick+1, y+digHt-2, colOn); // le-bot-short
      //lcd.drawLineVfast(x+segThick, y+segThick+hc+1,    y+segThick+hc+1+segThick-2, colOn); // le-mid-short
      //lcd.drawLineVfast(x, y+segThick+hc+1,    y+segThick+hc+1+segThick-2, colOn); // le-mid-short full width
      lcd.drawLineVfast(x+digWd/3, y+segThick+hc+0,    y+segThick+hc+1+segThick-2, colOn); // le-mid-short 2/3 width
      lcd.drawLineVfast(x+digWd-segThick,  y+segThick,           y+segThick+1+hc-1, colOn); //le-top
      lcd.drawLineVfast(x+digWd-segThick,  y+segThick*2+hc,      y+segThick*2+hc+1+hc-1, colOn); //le-bot
      break;
    case 4:
      lcd.drawLineHfast(x,                  x+segThick-1,     y, colOn); // top short1
      lcd.drawLineHfast(x+digWd-segThick,   x+digWd-1,        y, colOn); // top short2
      lcd.drawLineHfast(x+digWd-segThick,   x+digWd-1,        y+digHt-1,  colOn); // bottom short
      lcd.drawLineHfast(x+segThick,         x+digWd-segThick, y+segThick+hc,  colOn); // mid1
      lcd.drawLineHfast(x,                  x+digWd-segThick, y+segThick+hc+segThick-1,  colOn); // mid2      
      lcd.drawLineVfast(x+digWd-1,         y+1,             y+digHt-2, colOn); // rt-long
      lcd.drawLineVfast(x+digWd-segThick,  y+1,             y+segThick+hc-1, colOn); //rt-left-top
      lcd.drawLineVfast(x+digWd-segThick,  y+hc+segThick*2, y+digHt-2, colOn); //rt-left-bot
      lcd.drawLineVfast(x,                 y+1,             y+segThick*2+hc-1, colOn); //le-le
      lcd.drawLineVfast(x+segThick-1,      y+1,             y+segThick+hc, colOn); //le-rt
      break;
    case 5:
      lcd.drawLineHfast(x,          x+digWd-1,        y,  colOn); // top
      lcd.drawLineHfast(x+segThick, x+digWd-1,        y+segThick-1,  colOn); // top2
      lcd.drawLineHfast(x,          x+digWd-1,        y+digHt-1,  colOn); // bottom
      lcd.drawLineHfast(x,          x+digWd-segThick, y+digHt-segThick,  colOn); // bottom2
      lcd.drawLineHfast(x+segThick, x+digWd-1,        y+segThick+hc,  colOn); // mid
      lcd.drawLineHfast(x,          x+digWd-segThick, y+segThick+hc+segThick-1,  colOn); // mid2
      lcd.drawLineVfast(x,                y+1,             y+hc+segThick*2-1, colOn); // le
      lcd.drawLineVfast(x+segThick-1,     y+segThick-1,    y+hc+segThick, colOn); // le rt
      lcd.drawLineVfast(x+digWd-1,        y+hc+segThick+1, y+digHt-2, colOn); // rt
      lcd.drawLineVfast(x+digWd-segThick, y+hc+segThick*2, y+digHt-segThick-1, colOn); // rt le
      lcd.drawLineVfast(x+digWd-1,        y+1,             y+segThick-2, colOn); // rt short
      lcd.drawLineVfast(x,                y+digHt-segThick+1, y+digHt-2, colOn); // le short
      break;
    case 6:
      rect(x+segThick-1,y+hc+segThick*2-1,digWd-segThick*2+2,digHt-hc-segThick*3+2,colOn);
      lcd.drawLineHfast(x,          x+digWd-1, y,  colOn); // top
      lcd.drawLineHfast(x+segThick, x+digWd-1, y+segThick-1,  colOn); // top2
      lcd.drawLineHfast(x,          x+digWd-1, y+digHt-1,  colOn); // bottom
      lcd.drawLineHfast(x+segThick, x+digWd-1, y+segThick+hc,  colOn); // mid
      lcd.drawLineVfast(x,            y+1, y+digHt-2, colOn); // le long
      lcd.drawLineVfast(x+segThick-1, y+segThick-1, y+hc+segThick, colOn); // le rt
      lcd.drawLineVfast(x+digWd-1,    y+hc+segThick+1, y+digHt-2, colOn); // rt
      lcd.drawLineVfast(x+digWd-1,    y+1, y+segThick-2, colOn); // rt short
      break;
    case 7:
      lcd.drawLineHfast(x,                x+digWd-1,          y,  colOn); // top
      lcd.drawLineHfast(x+digWd-segThick, x+digWd-1,          y+digHt-1,  colOn); // bottom
      lcd.drawLineHfast(x,                x+digWd-1-segThick, y+segThick-1,  colOn); // bottom2
      lcd.drawLineVfast(x+digWd-1,        y+1,                y+digHt-2, colOn); // rt-long
      lcd.drawLineVfast(x,                y+1,                y+segThick-2, colOn);  // le-short
      lcd.drawLineVfast(x+digWd-segThick, y+segThick-1,       y+digHt-1, colOn); // rt-le-long
      break;
    case 8:
      rect(x,y,digWd,digHt,colOn);
      rect(x+segThick-1,y+segThick-1,     digWd-segThick*2+2,hc+2,colOn);
      rect(x+segThick-1,y+hc+segThick*2-1,digWd-segThick*2+2,digHt-hc-segThick*3+2,colOn);
      break;
    case 9:
      rect(x+segThick-1,y+segThick-1, digWd-segThick*2+2,hc+2,colOn);
      lcd.drawLineHfast(x, x+digWd-1, y, colOn); // top
      lcd.drawLineHfast(x, x+digWd-1, y+digHt-1, colOn); // bot
      lcd.drawLineHfast(x, x+digWd-1-segThick, y+segThick*2+hc-1,  colOn); // mid
      lcd.drawLineHfast(x, x+digWd-1-segThick, y+digHt-segThick,  colOn); // bot-higher
      lcd.drawLineVfast(x+digWd-1,        y+1, y+digHt-2, colOn); // rt-long
      lcd.drawLineVfast(x,                y+1, y+hc+segThick*2-2, colOn);  // le-top
      lcd.drawLineVfast(x,                y+digHt-segThick+1, y+digHt-2, colOn); // le-bot-short
      lcd.drawLineVfast(x+digWd-segThick, y+hc+segThick*2-1, y+digHt-segThick, colOn); // rt-le-bot
      break;
  }
  return digWd; 
}

int printNumber7(char *txt, int x, int y)
{
  while(*txt) x+=drawDigit7(*txt++,x,y)+spacing;
}

int printNumberF(char *txt, int x, int y)
{
  while(*txt) x+=drawDigitO(*txt++,x,y)+spacing;
}

int printNumberO(char *txt, int x, int y)
{
  while(*txt) x+=drawDigitO(*txt++,x,y)+spacing;
}

int numberWidth(char *txt)
{
  int wd=0;
  while(*txt) { wd+=((*txt=='.' || *txt==':')?segThick:digWd)+spacing; txt++; }
  return wd-spacing;
}

