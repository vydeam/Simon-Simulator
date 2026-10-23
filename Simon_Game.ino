#include <LiquidCrystal.h>

extern volatile unsigned long timer0_millis;

LiquidCrystal lcd(2, 3, 4, 5, 6, 7); // Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)

const int red_input = 12;
const int green_input = 11;
const int yellow_input = 10;
const int white_input = 9;

const int red_computer = A0;
const int green_computer = A1;
const int yellow_computer = A2;
const int white_computer = A3;

String gameMode[3] = {"Classic", "Rewind", "Stack"};
int highScore[3] = {0, 0, 0};

void setup() {
  Serial.begin(19200);
  // Digital Inputs
  pinMode(red_input, OUTPUT);
  pinMode(green_input, OUTPUT);
  pinMode(yellow_input, OUTPUT);
  pinMode(white_input, OUTPUT);

  // Analog Inputs
  pinMode(red_computer, INPUT);
  pinMode(green_computer, INPUT);
  pinMode(yellow_computer, INPUT);
  pinMode(white_computer, INPUT);
  
  // LCD
  lcd.begin(16, 2);
}


void loop() {
  delay(1000);
  bool gameLose = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     SIMON:");
  lcd.setCursor(0,1);
  lcd.print("Press Any Button");
  
  while(buttonCheck()) {} // WAITS FOR THE USER TO PRESS + HOLD ANY BUTTON TO START THE GAME
  Serial.println("Game will start right now!");
  allOff(500);
  digitalWrite(red_input, HIGH);
  digitalWrite(green_input, HIGH);
  digitalWrite(yellow_input, HIGH);
  digitalWrite(white_input, HIGH);

  pinMode(red_computer, INPUT);
  pinMode(green_computer, INPUT);
  pinMode(yellow_computer, INPUT);
  pinMode(white_computer, INPUT);

  int gameScroll = 0;
  while(true) {
    lcd.setCursor(0,0);
    lcd.clear();
    delay(300);
    lcd.print("GAMEMODE:"); // Classic (Normal Mode), Rewind (Reverse order of Classic), Stack (Add button presses), Corrupt (1/4th chance to change an led to a different one)
    delay(500);
    lcd.setCursor(0,1);
    lcd.print(gameMode[gameScroll]);
    int whiteSpace = 10 - gameMode[gameScroll].length();
    for(int i=0; i<whiteSpace; i++) {
      lcd.print(" ");
    }
    lcd.print("HS: ");
    lcd.print(highScore[gameScroll]);

    while(digitalRead(red_computer) == LOW &&
          digitalRead(green_computer) == LOW && 
          digitalRead(yellow_computer) == LOW && 
          digitalRead(white_computer) == LOW) { delay(50); }
    
    if(digitalRead(red_computer) == HIGH) { // SCROLL LEFT
      while(digitalRead(red_computer) == HIGH) { delay(50); }
      if(gameScroll == 0) {
        gameScroll = 2;
      } else {
        gameScroll -= 1;
      }
    } else if(digitalRead(green_computer) == HIGH) { // STARTS THE GAME
      while(digitalRead(green_computer) == HIGH) { delay(50); }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Ready?");
      lcd.setCursor(0,1);
      delay(1000);
      for(int l=3; l>0; l--) {
        delay(1000);
        lcd.print(l);
        lcd.print("... ");
      }
      delay(500);

      break;
    } else if(digitalRead(yellow_computer) == HIGH) { // SCROLL RIGHT
      while(digitalRead(yellow_computer) == HIGH) { delay(50); }
      if(gameScroll == 2) {
        gameScroll = 0;
      } else {
        gameScroll += 1;
      }
    } else if(digitalRead(white_computer) == HIGH) { // RESET ALL HIGHSCORES
      while(digitalRead(white_computer) == HIGH) { delay(50); }
      for(int i=0; i<=2; i++) {
        highScore[i] = 0;
      }
    }
  }

// Game setup: 
  int round = 0;
  
  int lightArray[32];
  if(gameScroll != 2) {
    randomSeed(analogRead(A5));
    for(int i=0; i<32; i++){
      lightArray[i] = random(0, 4);
    }
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(gameMode[gameScroll]);
  lcd.setCursor(0,1);

// Game in 31 rounds
  while (round != 32 && gameLose == false) {
    Serial.print("ROUND ");
    Serial.println(round);

    // USER STARTS FIRST IF GAMEMODE IS STACK    
    if(gameScroll == 2) {
      lcd.setCursor(0,1);
      lcd.print("Add! | Round: ");
      lcd.print(round+1);
      digitalWrite(red_input, HIGH);
      digitalWrite(green_input, HIGH);
      digitalWrite(yellow_input, HIGH);
      digitalWrite(white_input, HIGH);
      
      while(digitalRead(red_computer) == LOW &&
          digitalRead(green_computer) == LOW && 
          digitalRead(yellow_computer) == LOW && 
          digitalRead(white_computer) == LOW) { delay(50); }

      if(digitalRead(red_computer) == HIGH) {
        lightArray[round] = 0;
        while(digitalRead(red_computer) == HIGH) { delay(50); }
      } else if(digitalRead(green_computer) == HIGH) {
        lightArray[round] = 1;
        while(digitalRead(green_computer) == HIGH) { delay(50); }
      } else if(digitalRead(yellow_computer) == HIGH) {
        lightArray[round] = 2;
        while(digitalRead(yellow_computer) == HIGH) { delay(50); }
      } else if(digitalRead(yellow_computer) == HIGH) {
        lightArray[round] = 3;
        while(digitalRead(white_computer) == HIGH) { delay(50); }
      }
    } else {
      digitalWrite(red_input, LOW);
      digitalWrite(green_input, LOW);
      digitalWrite(yellow_input, LOW);
      digitalWrite(white_input, LOW);
    }
    
    pinMode(red_computer, OUTPUT);
    pinMode(green_computer, OUTPUT);
    pinMode(yellow_computer, OUTPUT);
    pinMode(white_computer, OUTPUT);

    // COMPUTER'S TURN
    lcd.setCursor(0,1);
    lcd.print("STOP | Round: ");
    lcd.print(round+1);
    delay(1000);

    for(int i=0; i<=round; i++) {
      Serial.println(lightArray[i]);
      if(round < 5) { // LEVEL 0 - 600ms
        autoLED(lightArray[i], 600);
      } else if(round < 9) { // LEVEL 5 - 500ms
        autoLED(lightArray[i], 500);
      } else if(round < 13) { // LEVEL 9 - 400ms
        autoLED(lightArray[i], 400);
      } else if(round >= 13) { // LEVEL 13 - 300ms
        autoLED(lightArray[i], 300);
      }
    }
    
    // Changing turns...
    allOff(100);
    digitalWrite(red_input, HIGH);
    digitalWrite(green_input, HIGH);
    digitalWrite(yellow_input, HIGH);
    digitalWrite(white_input, HIGH);

    pinMode(red_computer, INPUT);
    pinMode(green_computer, INPUT);
    pinMode(yellow_computer, INPUT);
    pinMode(white_computer, INPUT);

    // HUMAN'S TURN
    lcd.setCursor(0,1);
    lcd.print("  GO | Round: ");
    lcd.print(round+1);
    if(gameScroll == 0 || gameScroll == 2) { // Classic or Stack
      for(int j=0; j<=round; j++) {
        timerReset();
        if (gameLose == true) {
          break; // Stop the user from playing and immediately end game
          Serial.println("GAME OVER");
        }

        // Waiting for button press
        while(digitalRead(red_computer) == LOW &&
              digitalRead(green_computer) == LOW && 
              digitalRead(yellow_computer) == LOW && 
              digitalRead(white_computer) == LOW && 
              timer0_millis <= timerSet(round)) {
          Serial.println(timer0_millis / 1000.0);
          delay(50);
        }

        // Checking button press
        if(digitalRead(red_computer) == HIGH && timer0_millis <= timerSet(round)) {
          if(lightArray[j] == 0) { // Checks array to see if it matches the LED button pressed
            Serial.print("Red Button Pressed: "); // Replace with actual checking of combo from the array
          Serial.println(timer0_millis / 1000.0);
          timerReset();
        // Waits for user to release button to avoid bouncing while checking that the timer doesn't exceed limit
          while(digitalRead(red_computer) == HIGH && timer0_millis <= timerSet(round)) { delay(50); }
          } else {
            gameLose = true;
            if(round > highScore[gameScroll]) { highScore[gameScroll] = round; }
            creditsLED(A0, lightArray[j], false);
          }
        
        } else if(digitalRead(green_computer) == HIGH && timer0_millis <= timerSet(round)) {
          if(lightArray[j] == 1) {
            Serial.print("Green Button Pressed: ");
            Serial.println(timer0_millis / 1000.0);
            timerReset();
            while(digitalRead(green_computer) == HIGH && timer0_millis <= timerSet(round)) { delay(50); }
          } else {
            gameLose = true;
            if(round > highScore[gameScroll]) { highScore[gameScroll] = round; }
            creditsLED(A1, lightArray[j], false);
          }
          
        } else if(digitalRead(yellow_computer) == HIGH && timer0_millis <= timerSet(round)) {
          if(lightArray[j] == 2) {
            Serial.print("Yellow Button Pressed: ");
            Serial.println(timer0_millis / 1000.0);
            timerReset();
            while(digitalRead(yellow_computer) == HIGH && timer0_millis <= timerSet(round)) { delay(50); }
          } else {
            gameLose = true;
            if(round > highScore[gameScroll]) { highScore[gameScroll] = round; }
            creditsLED(A2, lightArray[j], false);
          }

        } else if(digitalRead(white_computer) == HIGH && timer0_millis <= timerSet(round)) {
          if(lightArray[j] == 3) {
            Serial.print("White Button Pressed: ");
            Serial.println(timer0_millis / 1000.0);
            timerReset();
            while(digitalRead(white_computer) == HIGH && timer0_millis <= timerSet(round)) { delay(50); }
          } else {
            gameLose = true;
            if(round > highScore[gameScroll]) { highScore[gameScroll] = round; }
            creditsLED(A3, lightArray[j], false);
          }

        } else {
          Serial.println("INCORRECT");
          gameLose = true;
          if(round > highScore[gameScroll]) { highScore[gameScroll] = round; }
          creditsLED(-1, lightArray[j], false);
        }
      }
    } else if(gameScroll == 1) { // Rewind
      for(int j=round; j>=0; j--) {
        timerReset();
        if (gameLose == true) {
          break; // Stop the user from playing and immediately end game
          Serial.println("GAME OVER");
        }

        // Waiting for button press
        while(digitalRead(red_computer) == LOW &&
              digitalRead(green_computer) == LOW && 
              digitalRead(yellow_computer) == LOW && 
              digitalRead(white_computer) == LOW && 
              timer0_millis <= timerSet(round)) {
          Serial.println(timer0_millis / 1000.0);
          delay(50);
        }

        // Checking button press
        if(digitalRead(red_computer) == HIGH && timer0_millis <= timerSet(round)) {
          if(lightArray[j] == 0) { // Checks array to see if it matches the LED button pressed
            Serial.print("Red Button Pressed: "); // Replace with actual checking of combo from the array
          Serial.println(timer0_millis / 1000.0);
          timerReset();
        // Waits for user to release button to avoid bouncing while checking that the timer doesn't exceed limit
          while(digitalRead(red_computer) == HIGH && timer0_millis <= timerSet(round)) { delay(50); }
          } else {
            gameLose = true;
            if(round > highScore[gameScroll]) { highScore[gameScroll] = round; }
            creditsLED(A0, lightArray[j], false);
          }
        
        } else if(digitalRead(green_computer) == HIGH && timer0_millis <= timerSet(round)) {
          if(lightArray[j] == 1) {
            Serial.print("Green Button Pressed: ");
            Serial.println(timer0_millis / 1000.0);
            timerReset();
            while(digitalRead(green_computer) == HIGH && timer0_millis <= timerSet(round)) { delay(50); }
          } else {
            gameLose = true;
            if(round > highScore[gameScroll]) { highScore[gameScroll] = round; }
            creditsLED(A1, lightArray[j], false);
          }
          
        } else if(digitalRead(yellow_computer) == HIGH && timer0_millis <= timerSet(round)) {
          if(lightArray[j] == 2) {
            Serial.print("Yellow Button Pressed: ");
            Serial.println(timer0_millis / 1000.0);
            timerReset();
            while(digitalRead(yellow_computer) == HIGH && timer0_millis <= timerSet(round)) { delay(50); }
          } else {
            gameLose = true;
            if(round > highScore[gameScroll]) { highScore[gameScroll] = round; }
            creditsLED(A2, lightArray[j], false);
          }

        } else if(digitalRead(white_computer) == HIGH && timer0_millis <= timerSet(round)) {
          if(lightArray[j] == 3) {
            Serial.print("White Button Pressed: ");
            Serial.println(timer0_millis / 1000.0);
            timerReset();
            while(digitalRead(white_computer) == HIGH && timer0_millis <= timerSet(round)) { delay(50); }
          } else {
            gameLose = true;
            if(round > highScore[gameScroll]) { highScore[gameScroll] = round; }
            creditsLED(A3, lightArray[j], false);
          }

        } else {
          Serial.println("INCORRECT");
          gameLose = true;
          if(round > highScore[gameScroll]) { highScore[gameScroll] = round; }
          creditsLED(-1, lightArray[j], false);
        }
      }
    }

    round++;
  }

  if(gameLose == false) {
    if(round > highScore[gameScroll]) { highScore[gameScroll] = round; }
    creditsLED(-1, -1, true);
  }
}


bool buttonCheck() {
  //Input mode for reading
    allOff(400);
    pinMode(red_computer, INPUT_PULLUP);
    pinMode(green_computer, INPUT_PULLUP);
    pinMode(yellow_computer, INPUT_PULLUP);
    pinMode(white_computer, INPUT_PULLUP);
    delay(400);
    //Check if any button is pressed
    if (digitalRead(red_computer) == LOW ||
        digitalRead(green_computer) == LOW ||
        digitalRead(yellow_computer) == LOW ||
        digitalRead(white_computer) == LOW) {
        
      return false; // Break blinking loop
    } else
      return true;
}

void allOff(int millisecs) {
  digitalWrite(red_computer, LOW);
  digitalWrite(green_computer, LOW);
  digitalWrite(yellow_computer, LOW);
  digitalWrite(white_computer, LOW);
  delay(millisecs);
}

void autoLED(int randomNum, int roundDelay) {
  if(randomNum == 0) { // RED LED
    digitalWrite(red_computer, HIGH);
  } else if(randomNum == 1) { // GREEN LED
    digitalWrite(green_computer, HIGH);
  } else if(randomNum == 2) { // YELLOW LED
    digitalWrite(yellow_computer, HIGH);
  } else if(randomNum == 3) { // WHITE LED
    digitalWrite(white_computer, HIGH);
  }
  delay(roundDelay);
  allOff(roundDelay);
}

long timerSet(int round) {
  if(round < 5) { // 4 secs
    return 4000.0;
  } else if(round < 9) { // 3 secs
    return 3000.0;
  } else if(round <  13) { // 2 secs
    return 2000.0;
  } else if(round >= 13) { // 1 sec
    return 1500.0;
  }
}

void timerReset() {
  noInterrupts();
  timer0_millis = 0; // Resets the Arduino timer to 0
  interrupts();
}

void creditsLED(int incorrectPin, int correctPin, bool winGame) { // LEDs do a little celebration if you win or lose
  if(winGame == false) {
    digitalWrite(incorrectPin, HIGH);
  }
  if(correctPin == 0) {
      correctPin = A0;
    } else if(correctPin == 1) {
      correctPin = A1;
    } else if(correctPin == 2) {
      correctPin = A2;
    } else if(correctPin == 3) {
      correctPin = A3;
    }
  
  if(winGame == true) { // You won the game
    lcd.setCursor(0,0);
    lcd.print("     WINNER     ");
    lcd.setCursor(0,1);
    lcd.print("DONE");

    for(int i=0; i<7; i++) {
      digitalWrite(red_computer, HIGH);
      digitalWrite(green_computer, LOW);
      digitalWrite(yellow_computer, LOW);
      digitalWrite(white_computer, LOW);
      delay(300);
      digitalWrite(red_computer, LOW);
      digitalWrite(green_computer, HIGH);
      digitalWrite(yellow_computer, LOW);
      digitalWrite(white_computer, LOW);
      delay(300);
      digitalWrite(red_computer, LOW);
      digitalWrite(green_computer, LOW);
      digitalWrite(yellow_computer, HIGH);
      digitalWrite(white_computer, LOW);
      delay(300);
      digitalWrite(red_computer, LOW);
      digitalWrite(green_computer, LOW);
      digitalWrite(yellow_computer, LOW);
      digitalWrite(white_computer, HIGH);
      delay(300);
    }
  } else if(winGame == false) { // You lost the game
    lcd.setCursor(0,0);
    lcd.print("   GAME OVER!   ");
    lcd.setCursor(0,1);
    lcd.print("DONE");
    digitalWrite(incorrectPin, HIGH);
    for(int i=0; i<7; i++) {
      digitalWrite(correctPin, HIGH);
      delay(500);
      digitalWrite(correctPin, LOW);
      if(i==6) {
        digitalWrite(incorrectPin, LOW);
      }
      delay(500);
    }
  }
  digitalWrite(red_input, LOW);
  digitalWrite(green_input, LOW);
  digitalWrite(yellow_input, LOW);
  digitalWrite(white_input, LOW);
}