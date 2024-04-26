// Sketch created by Jared Kitchen

#include <ArducamSSD1306.h>
#include <Keypad.h>
#include <stdlib.h>


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

// ---LED Constants--------------------------------------------------------------

const byte whiteLED = 52;
const byte yellowLED = 50;
const byte blueLED = 48;
const byte greenLED = 46;
const byte redLED = 44;


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
// 3 = Wait for reset
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

  // Configure LEDs
  pinMode(whiteLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  // Start in "Collecting 1st operand" state
  digitalWrite(whiteLED, HIGH);
  digitalWrite(yellowLED, LOW);
  digitalWrite(blueLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);

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
  else if(UIState == 3) {
    waitForReset();
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
    if (key == '+' || key == '-' || key == '*' || key == '/' && operand1.length() > 0) {
        // Enter "Collecting 2nd operand" state
        display.setCursor(0, body2Y);
        display.print(key);
        display.setCursor(0, body3Y);
        op = key;
        UIState = 1;
        digitalWrite(whiteLED, LOW);
        digitalWrite(yellowLED, HIGH);
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
      digitalWrite(yellowLED, LOW);
      digitalWrite(whiteLED, HIGH);
    }
    else if(key == 'C' && operand2.length() > 0) {
      // Clear operand 2
      clearAll();
      display.print(operand1);
      display.setCursor(0, body2Y);
      display.print(op);
      display.setCursor(0, body3Y);
      operand2 = "";
      digitalWrite(blueLED, LOW);
      digitalWrite(yellowLED, HIGH);

    }
    else if(key == 'E' && operand2.length() > 0) {
      // Go to "Displaying result/error" state
      display.setCursor(0, body4Y);
      display.print("=");
      display.setCursor(0, body5Y);
      UIState = 2;
    }
    else if(operand2.length() < 16) {
      if(operand2.length() == 0) {
        // Update LEDs
        digitalWrite(yellowLED, LOW);
        digitalWrite(blueLED, HIGH);
      }
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
    digitalWrite(blueLED, LOW);
    digitalWrite(redLED, HIGH);
  }
  else {
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
    display.print(result);
    digitalWrite(blueLED, LOW);
    digitalWrite(greenLED, HIGH);
  }
  
  display.display();

  // Move to "Wait for reset" state
  UIState = 3;
}

void waitForReset() {
  char key = keypad.getKey();
  if(key) {
    if(key == 'C' || key == 'E') {
      clearAll();
      operand1 = "";
      operand2 = "";
      op = "n";
      result = 0.0;
      // Move to  "Collecting 1st operand" state
      UIState = 0;
      digitalWrite(greenLED, LOW);
      digitalWrite(redLED, LOW);
      digitalWrite(whiteLED, HIGH);
    }
  }
}
