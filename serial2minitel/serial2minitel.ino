/*******************************

serial2minitel.ino

Simple Arduino program to pass 
bytes from serial over to a 
Minitel terminal.

Use with twitter2minitel.py.

Kevin Driscoll, 2012-2013

*******************************/
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
SoftwareSerial _minitel(6, 7);

const int BAUDRATE = 9600;

const int SCREENWIDTH = 39;
const int SCREENHEIGHT = 23;

const int SLIDEWAIT = 5000;

/*
Minitel Videotex Bytecodes 
*/

const byte VIDEOTEX = 0x0E;
const byte TEXT = 0x0F;
const byte CLRSCR = 0x0C;
const byte HOME = 0x1e;
const byte SPACE = 0xa0;

const byte CHKPOS = 0x61;

const byte LEFT = 0x08; // curseur à gauche
const byte RIGHT = 0x09; // curseur à droite
const byte DOWN = 0x0A; // curseur en bas
const byte UP = 0x0B; // curseur en haut

void cursorleft() { serialprint7(LEFT); };
void cursorright() { serialprint7(RIGHT); };
void cursorup() { serialprint7(UP); };
void cursordown() { serialprint7(DOWN); };

// ESC required for the rest of these 
const byte ESC = 0x1B;
const byte NORMALSIZE = 0x4C;
const byte DOUBLEHEIGHT = 0x4D;
const byte DOUBLEWIDTH = 0x4E;
const byte DOUBLEBOTH = 0x4F;
const byte FLASHING = 0x30; // Not working on Minitel 1

// Colors in sequence from dark to light
const byte FOREGROUND[] = {
  0x40,
  0x44,
  0x41,
  0x45,
  0x42,
  0x46,
  0x43,
  0x47
};
const byte BACKGROUND[] = {
  0x50,
  0x54,
  0x51,
  0x55,
  0x52,
  0x56,
  0x53,
  0x57
};

void serialprint7(byte b) // permet d'ecrire en 7 bits + parité sur le software serial
{
  boolean  i = false;
  for(int j = 0; j<8;j++)
  {
    if (bitRead(b,j)==1) i =!i; //calcul de la parité
  }
  if (i) bitWrite(b,7,1); //ecriture de la partié
  else bitWrite(b,7,0);//ecriture de la partié
  _minitel.write(b);//ecriture du byte sur le software serial
};

void drawbox(int x, int y, int width, int height, byte c = 0x7f) {
  movecursor(x, y);
  for (int i = 0; i < width; i++) {
    serialprint7(ESC);
    serialprint7(FOREGROUND[i]);
    serialprint7(ESC);
    serialprint7(BACKGROUND[(7-i)]);
    movecursor((x + i), y);
    serialprint7(c);
    movecursor((x + i), (y + (height-1)));
    serialprint7(c);
  }
  movecursor(x, y);
  for (int i = 0; i < height; i++) {
    serialprint7(ESC);
    serialprint7(FOREGROUND[i]);
    serialprint7(ESC);
    serialprint7(BACKGROUND[(7-i)]);
    movecursor(x, (y + i));
    serialprint7(c);
    movecursor((x + (width-1)), (y + i));
    serialprint7(c);
  }
}


void positiontest() {
  Serial.println("Requesting position...");
  serialprint7(ESC);
  serialprint7(0x61);
  delay(250);
  while (_minitel.available() > 0) {
    byte b = _minitel.read();
    Serial.println(b);
    delay(50);
  }
}


void fixposition() {
  serialprint7(ESC);
  serialprint7(0x61);
  delay(250);
  byte b = -1;
  while ((_minitel.available() > 0) or (b == -1)) {
    b = _minitel.read();
    Serial.write(b);
  }  
  byte row = -1;
  while ((_minitel.available() > 0) or (row == -1)) {
    row = _minitel.read();
    Serial.write(row);
  }
  delay(50);
  byte col = -1;
  while ((_minitel.available() > 0) or (col == -1)) {
    col = _minitel.read();
    Serial.write(col);
  }
  if (col == 0) {
    delay(50);
    serialprint7(0x1f);
    delay(50);
    serialprint7(row);
    delay(50);
    serialprint7(0);
  }
}


void movecursor(int x, int y) {
  serialprint7(0x1f);
  serialprint7(byte(65+y));
  serialprint7(byte(65+x));  
};

void printtestrow(byte m, byte f, byte b) {
      serialprint7(m);
      serialprint7(ESC);
      serialprint7(FOREGROUND[f]);
      serialprint7(ESC);
      serialprint7(BACKGROUND[b]);
      serialprint7(byte(40));
      serialprint7(byte(48+f));
      serialprint7(byte(44));
      serialprint7(byte(48+b));
      serialprint7(byte(41));
      for (int i = 0; i < 35; i++) {
        serialprint7(byte(32));
      }
};

void colortest() {
  Serial.println("Color test...");
  for (int color = 0; color < 8; color++) {
    printtestrow(TEXT, color, (7-color));
  }
};

void chartest() {
  Serial.println("Character test...");
  for (int c = 32; c < 128; c++) {
    serialprint7(byte(c));
  }
} 

const int BUFFMAX = 64;
byte buffer[BUFFMAX];

void serial_mirror_old() {
  int i = 0;
  while ((Serial.available()) && (i < BUFFMAX)) {
    delay(10);
    byte b = Serial.read();
    if (b != 0xFF) {
      buffer[i] = byte(b);
      i++;
    }
  }
  if (i > 0) {
    for (int buffindex = 0; buffindex < i; buffindex++) {
        serialprint7(buffer[buffindex]);
        // Serial.print(char(buffer[buffindex]));
      }
  }
}

void serial_mirror() {
}

/*
Setup
Runs once
*/

void setup() {

  Serial.begin(BAUDRATE);
  delay(1000);
  Serial.println("Hello, Laptop");
  
  _minitel.begin(1200); // port serie vers le minitel
  delay(500);
  
  serialprint7(TEXT);
  serialprint7(CLRSCR);
  
  Serial.println("OK");

} 


/*
Main loop
*/

void loop()
{
    delay(500);
    if (Serial.available() > 0) {
      int count = 0;
      while (Serial.available() > 0) {
        byte b = Serial.read();
        delay(20);
        //if (b != 0xFF) {
          serialprint7(b);
          count += 1;
        //}
      }
      Serial.println("OK");
    }
}





