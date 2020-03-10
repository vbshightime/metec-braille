#include <HashMap.h>
#include "metec_db.h"
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)


int datainPin = 3;             //to cell, 'in' from perspective braille cell
int strobePin = 4;             //to cell
int clockPin = 5;              //to cell
int powerPin = 2;              //!goes to DCDC
int braillePin_1 = 6;
int braillePin_2 = 7;
int braillePin_3 = 8;
int braillePin_4 = 9;
int braillePin_5 = 10;
int braillePin_6 = 11;

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
  pinMode(braillePin_6,INPUT);
  pinMode(braillePin_5,INPUT);
  pinMode(braillePin_4,INPUT);
  pinMode(braillePin_3,INPUT);
  pinMode(braillePin_2,INPUT);
  pinMode(braillePin_1,INPUT);
  delay(1000);
  digitalWrite(powerPin, LOW);  // controlpin for DCDC converter (low is on)
  hashMapChar();
  hashMapBraille();
  clearCell("Putting Cells down");
  //checkBraille(CELL_SIZE);
  delay(1000);
}


void loop()                       // switch between two example patterns
{
  /*if (Serial.available() > 0) {
    String serialRead = Serial.readStringUntil(".");
    clearCell();
    serialRead.toCharArray(serialData, serialRead.length() - 1);
    makePatternSerial(serialData);
    writeCells(patternBraille, sizeof(patternBraille), false);
  }else*/ 
  if(keyOR(braillePin_1,braillePin_2,braillePin_3,braillePin_4,braillePin_5,braillePin_6)== 1){
       Serial.println("No Pattern detected");
       clearCell("No pattern detected Nothing to show in Brailles");
    }else{
       Serial.println("Pattern Detected");
       clearCell("Clear Cells and Show the Pattern");
       makePatternButtonBraille(braillePin_1,braillePin_2,braillePin_3,braillePin_4,braillePin_5,braillePin_6);
       writeCells(patternBraille, CELL_SIZE, false);
      }
}

void clearCell(char debugString) {
  digitalWrite(strobePin, LOW);
  for (uint8_t i = CELL_SIZE; i > 0; i-- ) {
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
  Serial.println(debugString);
  delay(1000);
  digitalWrite(strobePin, LOW);
  digitalWrite(datainPin, 1);
  strcpy(serialData, NULL);
  memcpy(patternBraille, 0, sizeof(patternBraille));
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

  if (max_charac <= max_cell) {
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
        clearCell("cellSize overflow");
      }
      else {
        clearCell("cellSize overflow");
      }
    }
  }
}

void writeSingleCell(uint8_t position, uint16_t pattern) {
  if (pattern >= MIN_BRAILLE && pattern <= MAX_BRAILLE) {
    setCell(position, braille_db[pattern - MIN_BRAILLE]);
  }
  else {
    clearCell("Braille unicode passed limits");
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

