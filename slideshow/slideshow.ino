/*******************************

Minitel Slideshow Template

Tested on Arduino UNO

Kevin Driscoll, 2012-2013

See README.md for more information
on constructing a serial cable to
connect your Arduino to a Minitel.

********************************/

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

void serial_mirror() {
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
        Serial.print(char(buffer[buffindex]));
      }
  }
}

const int grave_length = 1028;
const char grave[grave_length] PROGMEM = {
0x1b,0x43,0xe,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1b,0x46,0xe,0x20,0x20,0x78,0x7e,0x7c,
0x30,0x20,0x20,0x20,0x20,0x20,0x1b,0x47,0xe,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x60,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1b,0x46,
0xe,0x20,0x6a,0x7f,0x7f,0x7f,0x7f,0x20,0x20,0x20,0x20,0x20,0x1b,0x47,0xe,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x60,0x7c,0x7f,0x7f,0x7f,0x7f,0x7f,0x7d,0x74,
0x20,0x20,0x20,0x20,0x1b,0x46,0xe,0x20,0x6b,0x7f,0x7f,0x7f,0x7f,0x21,0x20,0x20,0x20,0x20,0x1b,0x47,
0xe,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x78,0x7f,0x7f,0x7f,
0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7d,0x30,0x20,0x20,0x1b,0x46,0xe,0x20,0x22,0x6f,0x7f,0x7f,0x27,0x20,
0x20,0x20,0x20,0x20,0x1b,0x47,0xe,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x7a,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x30,0x20,0x1b,0x46,0xe,0x20,
0x20,0x20,0x22,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1b,0x47,0xe,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
0x35,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1b,0x47,0xe,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x28,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
0x7f,0x7f,0x7f,0x3d,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1b,0x47,0xe,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x6a,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1b,
0x47,0xe,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x6a,0x7f,0x7f,0x7f,0x7f,
0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x1b,0x47,0xe,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x6a,0x7f,
0x7f,0x7f,0x1b,0x47,0xf,0x20,0x52,0x2e,0x49,0x2e,0x50,0x2e,0x1b,0x47,0xe,0x20,0x7f,0x7f,0x7f,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1b,0x47,0xe,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x6a,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
0x7f,0x7f,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1b,0x47,0xe,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x6a,0x7f,0x7f,0x1b,0x47,0xf,0x20,0x4d,0x49,0x4e,
0x49,0x54,0x45,0x4c,0x1b,0x47,0xe,0x20,0x7f,0x7f,0x7f,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x1b,0x47,0xe,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x6a,
0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x1b,0x47,0xe,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x6a,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1b,0x47,0xe,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x6a,0x7f,0x7f,0x1b,0x47,0xf,0x20,0x31,0x39,0x38,0x31,0x2d,0x1b,0x47,0xe,
0x20,0x7f,0x7f,0x7f,0x7f,0x7f,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1b,0x47,
0xe,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x6a,0x7f,0x7f,0x7f,0x7f,0x7f,
0x1b,0x47,0xf,0x20,0x32,0x30,0x31,0x32,0x1b,0x47,0xe,0x20,0x7f,0x7f,0x7f,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1b,0x47,0xe,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x6a,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1b,0x47,0xe,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x6a,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
0x7f,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1b,0x47,0xe,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x6a,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
0x7f,0x7f,0x7f,0x7f,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1b,0x42,0xe,0x20,
0x7c,0x7c,0x7c,0x7c,0x7c,0x7c,0x7c,0x7c,0x7c,0x7c,0x7c,0x7c,0x7e,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7c,0x7c,0x7c,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1b,
0x42,0xe,0x20,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
0x7f,0x7f,0x1b,0x42,0xe,0x20,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
0x7f,0x7f,0x7f,0x7f,0x7f,0x1b,0x42,0xe,0x20,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f
};

void show_grave() {
  serialprint7(CLRSCR);
  serialprint7(VIDEOTEX);
  for (int i = 0; i < grave_length; i++) {
    byte b = pgm_read_byte_near(grave + i);
    serialprint7(b);
  }
}

/*
Setup
Runs once
*/

void setup() {
  Serial.begin(BAUDRATE);
  delay(500);
  Serial.println("Hello, Laptop");
  
  _minitel.begin(1200); // port serie vers le minitel
  delay(500);
  
  serialprint7(TEXT);
  serialprint7(CLRSCR);
  
  // Tests

  colortest();

  serialprint7(VIDEOTEX);
  chartest();
  
  positiontest();
  movecursor(0,21);
  positiontest();
  
  serialprint7(TEXT);
  serialprint7(BACKGROUND[4]);
  chartest();

} 


/*
Main loop
*/

void loop()
{
    delay(SLIDEWAIT);
    show_grave(); // keep
}

