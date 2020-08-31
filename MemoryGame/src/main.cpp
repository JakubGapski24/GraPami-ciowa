#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define diode_1 PC7
#define diode_2 PA9
#define diode_3 PA8
#define diode_4 PB10
#define switch_1 PB4
#define switch_2 PB5
#define switch_3 PB3
#define switch_4 PA10
#define easy PB0
#define medium PC1
#define hard PC0

void playGame(String level, int time, byte loops);

void setup() {
  lcd.init();
  lcd.clear();
  lcd.backlight();
  pinMode(diode_1, OUTPUT);
  pinMode(diode_2, OUTPUT);
  pinMode(diode_3, OUTPUT);
  pinMode(diode_4, OUTPUT);
  pinMode(easy, OUTPUT);
  pinMode(medium, OUTPUT);
  pinMode(hard, OUTPUT);
  pinMode(switch_1, INPUT_PULLUP);
  pinMode(switch_2, INPUT_PULLUP);
  pinMode(switch_3, INPUT_PULLUP);
  pinMode(switch_4, INPUT_PULLUP);
}

void loop() {
  digitalWrite(easy, HIGH);
  digitalWrite(medium, HIGH);
  digitalWrite(hard, HIGH);
  digitalWrite(diode_1, LOW);
  digitalWrite(diode_2, LOW);
  digitalWrite(diode_3, LOW);
  digitalWrite(diode_4, LOW);
  lcd.setCursor(0, 0);
  lcd.print("Choose difficulty"); //wybór poziomu trudności
  lcd.setCursor(0, 1);
  lcd.print("1. Easy");
  lcd.setCursor(0, 2);
  lcd.print("2. Medium");
  lcd.setCursor(0, 3);
  lcd.print("3. Hard");

//uruchomienie gry zgodnie z wybranym poziomem trudności
  if (digitalRead(switch_1) == LOW) {
    digitalWrite(medium, LOW);
    digitalWrite(hard, LOW);
    playGame("Easy", 1000, 1);
  }
  if (digitalRead(switch_2) == LOW) {
    digitalWrite(easy, LOW);
    digitalWrite(hard, LOW);
    playGame("Medium", 775, 2);
  }
  if (digitalRead(switch_3) == LOW) {
    digitalWrite(easy, LOW);
    digitalWrite(medium, LOW);
    playGame("Hard", 500, 3);
  }
}

void playGame(String level, int time, byte loops) {
  //ustawienia początkowe
  lcd.clear();
  int position = 0, randomArray[6], arrayToComplete[6] = {0,0,0,0,0,0};
  String message = "Repeat ";
  bool started = false, mistake = false;
  byte seconds = 6;
  unsigned long timeToAnotherLoop = 0, rememberedTimeToAnotherLoop = 0;

  //wyświetlanie wybranego poziomu trudności
  if (level == "Easy" || level == "Hard") {
    position = 8;
  }
  else {
    position = 7;
  }
  lcd.setCursor(position, 0);
  lcd.print(level);

  //wyświetlanie odpowiedniej instrukcji wygrania gry
  if (level == "Easy") {
    message += "only once";
    position = 2;
  }
  else if (level == "Medium") {
    message += "twice";
    position = 4;
  }
  else {
    message += "three times";
    position = 1;
  }
  lcd.setCursor(position, 1);
  lcd.print(message);
  lcd.setCursor(3,2);
  lcd.print("to win a game");
  lcd.setCursor(7,3);
  lcd.print("START");

  //oczekiwanie na rozpoczęcie gry (wciśnięcie przycisku)
  while (!started) {
    if (digitalRead(switch_4) == LOW) {
      started = true;
    }
  }

  //losowa sekwencja zapalania diod (na razie na liczbach)
  for (int i=1; i<=loops; i++) {
    randomSeed(analogRead(PA0));

    //uzupełnianie tabeli losową sekwencją
    for (int j=0; j<6; j++) {
      randomArray[j] = random(1, 5);
    }

    //sprawdzenie, czy poszczególna dioda nie zapala się 3 razy pod rząd (lub więcej)
    for (int j=0; j<4; j++) {
      while (randomArray[j] == randomArray[j+1] && randomArray[j] == randomArray[j+2]) {
        randomArray[j+2] = random(1, 5);
      }
    }

    //zapalanie diod, zgodnie z wylosowaną sekwencją
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Remember a sequence");
    for (int j=0; j<6; j++) {
      switch (randomArray[j]) {
        case 1:
              digitalWrite(diode_1, HIGH);
              delay(time);
              digitalWrite(diode_1, LOW);
              delay(350);
        break;
        case 2:
              digitalWrite(diode_2, HIGH);
              delay(time);
              digitalWrite(diode_2, LOW);
              delay(350);
        break;
        case 3:
              digitalWrite(diode_3, HIGH);
              delay(time);
              digitalWrite(diode_3, LOW);
              delay(350);
        break;
        case 4:
              digitalWrite(diode_4, HIGH);
              delay(time);
              digitalWrite(diode_4, LOW);
              delay(350);
        break;
      }
    }
  
    //wypełnianie tabeli zgodnie z wyborem użytkownika - powtarzanie sekwencji
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Repeat the sequence");
  
    for (int j=0; j<6; j++) {
      //czekaj, dopóki któryś z przycisków przypisanych diodom nie zostanie wciśnięty
      while (digitalRead(switch_1) == HIGH && digitalRead(switch_2) == HIGH && digitalRead(switch_3) == HIGH && 
      digitalRead(switch_4) == HIGH) {}

      //wciśnięci przycisku 1
      if (digitalRead(switch_1) == LOW) {
        digitalWrite(diode_1, HIGH);
        arrayToComplete[j] = 1;
        while (digitalRead(switch_1) == LOW) {}
        digitalWrite(diode_1, LOW);
        delay(100);
      }

      //wciśnięcie przycisku 2
      else if (digitalRead(switch_2) == LOW) {
        digitalWrite(diode_2, HIGH);
        arrayToComplete[j] = 2;
        while (digitalRead(switch_2) == LOW) {}
        digitalWrite(diode_2, LOW);
        delay(100);
      }
          
      //wciśnięcie przycisku 3
      else if (digitalRead(switch_3) == LOW) {
        digitalWrite(diode_3, HIGH);
        arrayToComplete[j] = 3;
        while (digitalRead(switch_3) == LOW) {}
        digitalWrite(diode_3, LOW);
        delay(100);
      }

      //wciśnięcie przycisku 4
      else if (digitalRead(switch_4) == LOW) {
        digitalWrite(diode_4, HIGH);
        arrayToComplete[j] = 4;
        while (digitalRead(switch_4) == LOW) {}
        digitalWrite(diode_4, LOW);
        delay(100);
      }
      
      //sprawdzenie, czy wciśnięty przez użytkownika przycisk jest zgodny z sekwencją
      if (arrayToComplete[j] == randomArray[j]) {
        continue; //jeśli tak, to kontynnuj sprawdzanie sekwencji
      }
      else { //jeśli nie, to zakończ grę i pokaż odpowiedni komunikat
        mistake = true;
        lcd.clear();
        lcd.setCursor(1,0);
        lcd.print("You made a mistake");
        lcd.setCursor(5,1);
        lcd.print("Game Over");
        lcd.setCursor(4,2);
        lcd.print("Press START");
        while (digitalRead(switch_4) == HIGH) {}
        lcd.clear();
        break;
      }
    }

    //sprawdzenie, czy użytkownik popełnił błąd i zakończenie gry
    if (mistake) {
      break;
    }

    //kontynuacja gry, jeśli użytkownik nie popełnił błędu w danej sekwencji
    else {
      if (loops >= 2 && i != loops) { //wyświetlenie informacji o pozostałych sekwencjach do powtórzenia do ukończenia gry
        lcd.clear();
        lcd.setCursor(5,0);
        lcd.print("Well done!");
        lcd.setCursor(2,1);
        if ((i == 1 && loops == 2) || (i == 2 && loops == 3)) {
          message = "one";
        }
        else if (i == 1 && loops == 3) {
          message = "two";
        }
        lcd.print("Only ");
        lcd.print(message);
        lcd.print(" to win");
        lcd.setCursor(4,2);
        lcd.print("Press START");
        while (digitalRead(switch_4) == HIGH) {} //oczekiwanie na wciśnięcie przycisku START
        lcd.clear();
        while (seconds != 0) {
          timeToAnotherLoop = millis();
          lcd.setCursor(2,0);
          lcd.print("Repeat again in"); //odliczanie czasu do rozpoczęcia wyświetlania nowej sekwencji
          lcd.setCursor(7,1);
          lcd.print("00:0");
          lcd.print(seconds);
          if (timeToAnotherLoop - rememberedTimeToAnotherLoop >= 1000UL) {
            rememberedTimeToAnotherLoop = timeToAnotherLoop;
            seconds--;
          }
        }
        seconds = 6;
      }
    }
  }

  //pozytywne ukończenie gry
  if (!mistake) {
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Congratulations!");
    lcd.setCursor(2,1);
    lcd.print("Game is finished");
    lcd.setCursor(4,2);
    lcd.print("Press START");
    while (digitalRead(switch_4) == HIGH) {}
    lcd.clear();
  }
}