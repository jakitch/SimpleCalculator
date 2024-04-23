#include <ArducamSSD1306.h>
#include <Keypad.h>
#include <stdlib.h>
#include <math.h>


// ---Keypad Variables-----------------------------------------------------------

const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','+'},
  {'4','5','6','-'},
  {'7','8','9','*'},
  {'C','0','E','/'}
};

byte rowPins[ROWS] = {11, 10, 9, 8}; 
byte colPins[COLS] = {7, 6, 5, 4}; 

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---Display Variables----------------------------------------------------------

#define OLED_RESET 16
ArducamSSD1306 display(OLED_RESET);

// ---General Variables & Constants----------------------------------------------

const byte title1Y = 0;
const byte title2Y = 8;
const byte body1Y = 16;
const byte body2Y = 24;
const byte body3Y = 32;
const byte body4Y = 40;
const byte body5Y = 48;
const byte body6Y = 56;

String operand1 = "";
String operand2 = "";
char op = "n";
double result = 0.0;

// 0 = Collecting 1st operand
// 1 = Collecting 2nd operand
// 2 = Displaying result/error
// 3 = Finished
byte UIState = 0;

// ------------------------------------------------------------------------------

void setup() {

  // Configure display
  display.begin();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  printTitle();
  display.setCursor(0, body1Y);
  display.display();

  Serial.begin(9600);
}

void loop() {
  if(UIState == 0) {
    collectOperand1();
  }
  else if(UIState == 1) {
    collectOperand2();
  }
  else if(UIState == 2) {
    displayResult();
  }
}

void printTitle() {
  display.setCursor(0, title1Y);
  display.print("Simple Calculator");
  display.setCursor(0, title2Y);
  display.print("=====================");
}

void clearAll() {
  display.clearDisplay();
  printTitle();
  display.setCursor(0, body1Y);
  display.display();
}

void collectOperand1() {
  char key = keypad.getKey();
  if (key) {
    if (key == '+' || key == '-' || key == '*' || key == '/') {
        // Enter "Collecting 2nd operand" state
        display.setCursor(0, body2Y);
        display.print(key);
        display.setCursor(0, body3Y);
        op = key;
        UIState = 1;
    }
    else if(key == 'C') {
      clearAll();
      operand1 = "";
    }
    else if (key == 'E') {
      // Do nothing
    }
    else if(operand1.length() < 16) {
      display.print(key);
      operand1 += key;
      Serial.println(operand1);
    }
    display.display();
  }
}

void collectOperand2() {
  char key = keypad.getKey();
  if (key) {
    if (key == '+' || key == '-' || key == '*' || key == '/') {
      // Do nothing
    }
    else if(key == 'C' && operand2.length() == 0) {
      // 1. Clear operator 
      // 2. Go back to "Collecting 1st operand" state
      clearAll();
      op = "n";
      display.print(operand1);
      UIState = 0;
    }
    else if(key == 'C' && operand2.length() > 0) {
      // Clear operand 2
      clearAll();
      display.print(operand1);
      display.setCursor(0, body2Y);
      display.print(op);
      display.setCursor(0, body3Y);
      operand2 = "";
    }
    else if(key == 'E') {
      // Go to "Displaying result/error" state
      display.setCursor(0, body4Y);
      display.print("=");
      display.setCursor(0, body5Y);
      UIState = 2;
    }
    else if(operand2.length() < 16) {
      display.print(key);
      operand2 += key;
    }
    display.display();
  }
}

void displayResult() {
  // Divide by zero error
  if(operand2 == "0" && op == '/') {
    display.print("ERROR");
    display.setCursor(0, body6Y);
    display.print("Cannot divide by zero");
  }
  else {
    operand1 += ".0001";
    switch (op) {
      case '+':
        result = operand1.toDouble() + operand2.toDouble();
        break;
      case '-':
        result = operand1.toDouble() - operand2.toDouble();
        break;  
      case '*':
        result = operand1.toDouble() * operand2.toDouble();
        break;  
      case '/':
        result = operand1.toDouble() / operand2.toDouble();
        break;     
    }
    display.print(operand1.toDouble()); 
  }
  display.display();
  // Move to "Finished" state
  UIState = 3;
}
