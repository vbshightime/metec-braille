#include <HashMap.h>
#include "metec_db.h"
#define PATTERN_LENGTH 36
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

char serialData[CELL_SIZE];
uint16_t patternBraille[CELL_SIZE];

const byte HASH_SIZE = PATTERN_LENGTH; 

HashType<char*, uint16_t> hashRawArray[HASH_SIZE]; 
HashMap<char*, uint16_t> hashMap = HashMap<char*,uint16_t>( hashRawArray , HASH_SIZE ); 

int datainPin = 3;             //to cell, 'in' from perspective braille cell
int strobePin = 4;             //to cell
int clockPin = 5;              //to cell
int powerPin = 2;              //!goes to DCDC
unsigned int i;

uint8_t* cells = 0;

float delayTime = 0.25;
    
      
/*uint16_t patternBraille[PATTERN_LENGTH]={
  0x2801,0x2803,0x2811,0x2831,0x2821,0x2813,0x2833,0x2823,0x2812,
  //a     b     c      d      e      f      g      h      i 
  0x2832,0x2805,0x2807,0x2815,0x2835,0x2825,0x2817,0x2837,0x2827,
  //j    k      l      m      n      o      p      q      r
  0x2816,0x2836,0x2845,0x2847,0x2872,0x2855,0x2875,0x2865,0x2841,
  //s    t      u      v      w      x      y      z      one
  0x2843,0x2811,0x2871,0x2861,0x2853,0x2873,0x2863,0x2852,0x2874,
  //two  three  four   five   six    seven  eight  nine   zero
  };*/
  
void setup() {          
  Serial.begin(115200);     
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);  // controlpin for DCDC converter (low is on)
  pinMode(datainPin, OUTPUT);
  pinMode(strobePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  delay(1000);
  digitalWrite(powerPin, LOW);  // controlpin for DCDC converter (low is on)
  hashMap[0]("a", 0x2801);
  hashMap[1]("b", 0x2803);
  hashMap[2]("c", 0x2811);
  hashMap[3]("d", 0x2831);
  hashMap[4]("e", 0x2821);
  hashMap[5]("f", 0x2813);
  hashMap[6]("g", 0x2833);
  hashMap[7]("h", 0x2823);
  hashMap[8]("i", 0x2812);
  hashMap[9]("j", 0x2832);
  hashMap[10]("k", 0x2805);
  hashMap[11]("l", 0x2807);
  hashMap[12]("m", 0x2815);
  hashMap[13]("n", 0x2835);
  hashMap[14]("o", 0x2815);
  hashMap[15]("p", 0x2817);
  hashMap[16]("q", 0x2837);
  hashMap[17]("r", 0x2827);
  hashMap[18]("s", 0x2816);
  hashMap[19]("t", 0x2836);
  hashMap[20]("u", 0x2845);
  hashMap[21]("v", 0x2847);
  hashMap[22]("w", 0x2872);
  hashMap[23]("x", 0x2855);
  hashMap[24]("y", 0x2875);
  hashMap[25]("z", 0x2865);
  hashMap[26]("1", 0x2841);
  hashMap[27]("2", 0x2843);
  hashMap[28]("3", 0x2811);
  hashMap[29]("4", 0x2871);
  hashMap[30]("5", 0x2861);
  hashMap[31]("6", 0x2853);
  hashMap[32]("7", 0x2873);
  hashMap[33]("8", 0x2863);
  hashMap[34]("9", 0x2852);
  hashMap[35]("0", 0x2874);
  clearCell();
  //checkBraille(CELL_SIZE);
  delay(50);
}


void loop()                       // switch between two example patterns
{
  if(Serial.available()>0){
       String serialRead = Serial.readStringUntil(".");
       clearCell();
       serialRead.toCharArray(serialData,serialRead.length()-1);
       makePattern(serialData);
       writeCells(patternBraille,sizeof(patternBraille),false);
    }  
}

void clearCell(){
  digitalWrite(strobePin, LOW);
        for(uint8_t i = CELL_SIZE; i>0; i-- ){
        digitalWrite(datainPin, 1); digitalWrite(clockPin, 1); digitalWrite(clockPin, 0);
        digitalWrite(datainPin, 1); digitalWrite(clockPin, 1); digitalWrite(clockPin, 0);
        digitalWrite(datainPin, 1); digitalWrite(clockPin, 1); digitalWrite(clockPin, 0);
        digitalWrite(datainPin, 1); digitalWrite(clockPin, 1); digitalWrite(clockPin, 0);
        digitalWrite(datainPin, 1); digitalWrite(clockPin, 1); digitalWrite(clockPin, 0);
        digitalWrite(datainPin, 1); digitalWrite(clockPin, 1); digitalWrite(clockPin, 0);
        digitalWrite(datainPin, 1); digitalWrite(clockPin, 1); digitalWrite(clockPin, 0);
        digitalWrite(datainPin, 1); digitalWrite(clockPin, 1); digitalWrite(clockPin, 0);
        }
        digitalWrite(strobePin, HIGH);
        Serial.println("Pattern Set");
        delay(1000);
        digitalWrite(strobePin, LOW);
        digitalWrite(datainPin, 1);
        strcpy(serialData,NULL);
        memcpy(patternBraille,0,sizeof(patternBraille));
        free(cells);
        //free(patternBraille);
        cells = (uint8_t *)malloc(CELL_SIZE * sizeof(uint8_t));
  }


void setCell(uint8_t position, uint8_t value) {
    Serial.println(position);
    Serial.println(value);
    cells[position] = value;
    if (delayTime != 0) {
        writeAllCell();
        delay(delayTime);
    }
}

void writeCells(uint16_t *pattern, uint8_t max_charac, bool reverse) {
    uint8_t max_cell = CELL_SIZE;

    if (max_charac < max_cell) {
        max_cell = max_charac;
    }

    for (uint8_t i = 0; i < max_cell; i++) {
        if (!reverse) {
            writeSingleCell(CELL_SIZE - 1 - i, pattern[i]);
        }
        else {
            writeSingleCell(i, pattern[i]);
        }
    }
    if (max_cell < CELL_SIZE) {
        for (uint8_t i = max_cell; i < CELL_SIZE; i++) {
            if (!reverse) {
                clearCell();
            }
            else {
                clearCell();
            }
        }
    }
}

void writeSingleCell(uint8_t position, uint16_t pattern) {
    if (pattern >= MIN_BRAILLE && pattern <= MAX_BRAILLE) {
        setCell(position, braille_db[pattern - MIN_BRAILLE]);
    }
    else {
        setCell(position, NOTHING);
    }
}

void writeAllCell() {
    for (uint8_t i = 0; i < CELL_SIZE; i++) {
        digitalWrite(datainPin, bitRead(cells[i], 6) ? 1 : 0); digitalWrite(clockPin, 1); digitalWrite(clockPin, 0);
        digitalWrite(datainPin, bitRead(cells[i], 7) ? 1 : 0); digitalWrite(clockPin, 1); digitalWrite(clockPin, 0);
        digitalWrite(datainPin, bitRead(cells[i], 2) ? 1 : 0); digitalWrite(clockPin, 1); digitalWrite(clockPin, 0);
        digitalWrite(datainPin, bitRead(cells[i], 1) ? 1 : 0); digitalWrite(clockPin, 1); digitalWrite(clockPin, 0);
        digitalWrite(datainPin, bitRead(cells[i], 0) ? 1 : 0); digitalWrite(clockPin, 1); digitalWrite(clockPin, 0);
        digitalWrite(datainPin, bitRead(cells[i], 5) ? 1 : 0); digitalWrite(clockPin, 1); digitalWrite(clockPin, 0);
        digitalWrite(datainPin, bitRead(cells[i], 4) ? 1 : 0); digitalWrite(clockPin, 1); digitalWrite(clockPin, 0);
        digitalWrite(datainPin, bitRead(cells[i], 3) ? 1 : 0); digitalWrite(clockPin, 1); digitalWrite(clockPin, 0);
    }
    digitalWrite(strobePin, 1);
    digitalWrite(strobePin, 0);
    digitalWrite(datainPin, 1);
}

void makePattern(char* data){
    for(int i=0;i<=CELL_SIZE;i++){
       if(data[i] ==NULL){
          break;
        }
        patternBraille[i] = hashMap.getValueOf(data[i]);
        Serial.println(data[i]);
        Serial.println(patternBraille[i],HEX);
      }
  }
