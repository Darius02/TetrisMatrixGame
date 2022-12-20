#include "joystick.h"
#include "lcd.h"
#include "highscore.h"
#include "matrix.h"

Joystick joystick;
Joystick moveBlocks;

int contorX = 1;
int contorY = 1;

bool setUpChoice = 1;

int getHighScore = 0;
int highScoreChoice = 1;

int auxStartGame = 0;
int interval = 1000;
int startGameTime = 0;
int previousStartGameTime = 0;

bool hasBegun = 0;

int line = 0;
int column = 0;
int rotation = 0;

int blockCurr = 0;
const int blockNr = 4;
const int blockPos = 5;
const int matrSize = 8;
int deletedLines = 0;

int score = 0;
int lives = 3;

int firstScreenTime = 0;
int lastFirstScreenTime = 0;
int intervalScreen = 5000;
int aux = 0;
int aux1 = 0;

bool starting = 1;

void setup(){
  lcdInit();
  lcInit();
  
  randomSeed(analogRead(0));
  
  Serial.begin(9600);
}
void loop(){
  
 if(starting){
  introduction();
  lcd.clear();
  starting = 0;
 }
 joystick.readValues();
 joystick.checkMove();
 
 if (joystick.displayNo()){
    if (!(joystick.displayNo() == 2 && contorX == 1 && lives)){
      displayLogo();
    }
    if (joystick.displayNo() == 1){
       if(joystick.isLeft()){
        if (contorX > 1){
         contorX--;
        }
       }
     if (joystick.isRight()){
      if(contorX < 4){
        contorX++;
      }
     }
     menuOptions(contorX);
     if(joystick.clickedOn()) lcd.clear();
    }
    if (joystick.displayNo() == 2 && contorX == 2){
      setUpChoice = 1;
      if (joystick.isLeft()) setUpChoice = 1;
      if (joystick.isRight()) setUpChoice = 0;
      if (setUpChoice){
        if (joystick.isUp() && contorY < 3){
          contorY++;
          lcd.clear();
        }
        if (joystick.isDown() && contorY > 1){
          contorY--;
          lcd.clear();
        }
        setUp(contorY, setUpChoice);
      }
      else{
        setUp(contorY, setUpChoice);
        joystick.goBackMenu();
        lcd.clear();
      }
    } 
    
    if(joystick.displayNo() == 1 && contorX == 4){
      joystick.goBackMenu();
      lcd.clear();
      contorX = 1;
    }
    if (joystick.displayNo() == 2 && contorX == 3){
      if (joystick.isRight()) highScoreChoice = 0;
      if (joystick.isLeft()) highScoreChoice = 1;
      highScoreMenu(highScoreChoice);
      if (joystick.isUp()){
        if (!highScoreChoice){
          joystick.goBackMenu();
          lcd.clear();
        }
        else{
          lcd.clear();
          setScore(0);
        }
      }
    }
    if (joystick.displayNo() == 2 && contorX == 1 && lives == 0){

      firstScreenTime = millis();
      if (aux == 0){
        lastFirstScreenTime = firstScreenTime;
        aux = 1;
      }
      if (firstScreenTime - lastFirstScreenTime >= intervalScreen){
        if (aux1 == 0){
          lcd.clear();
          aux1 = 1;
        }
        lcd.setCursor(0, 0);
        lcd.print("Choose D-Go back");
        lcd.setCursor(0, 1);
        lcd.print("U-Try again");
        if (joystick.isLeft()){
          lcd.clear();
          joystick.goBackMenu();
        }
        if (joystick.isRight()){
          lives = 3;
        }
      }
      else{
        lcd.setCursor(0, 0);
        lcd.print("Game Over!");
        lcd.setCursor(0, 1);
        lcd.print("Your score:");
        lcd.print(score);
      }
    }
    else{
      aux = 0;
      aux1 = 0;
    }
    if (joystick.displayNo() == 2 && contorX == 1 && lives){
          startGameTime = millis();
          if (auxStartGame == 0){
            lcd.clear();
            previousStartGameTime = startGameTime;
            lcInit();
            auxStartGame = 1;
            lives = 3;
            score = 0;
          }
          lcdPlay(lives, score);
          
          if (startGameTime - previousStartGameTime >= interval/contorY){
            if (hasBegun == 0){
              deletedLines = removeLines();
              if (deletedLines){
                score += deletedLines * matrSize * contorY;
                getHighScore = getScore();
                if (score > getHighScore){
                  setScore(score);
                }
              }
              line = 0;
              column = random() % blockPos;
              blockCurr = random() % blockNr;
              rotation = random() % blockNr;
              if (checkDraw(blockCurr, line, column, rotation)){
                drawBlock(blockCurr, line, column, true, rotation);
              }
              else{
                lives--;
                resetMatrix();
                hasBegun = 0;
              }
              hasBegun = 1;
            }
            else{
              drawBlock(blockCurr, line, column, false, rotation);
              if (checkDraw(blockCurr, line+1, column, rotation)){
                line++;
              }
              else{
                hasBegun = 0;
              }
              drawBlock(blockCurr, line, column, true, rotation);
            }
            previousStartGameTime = startGameTime;
          }
    
          if (joystick.isRight()){
            drawBlock(blockCurr, line, column, false, rotation);
            if (column - 1 >= 0 && checkDraw(blockCurr, line, column-1, rotation)){
              column--;
            }
            drawBlock(blockCurr, line, column, true, rotation);
          }
    
          if (joystick.isLeft()){
            drawBlock(blockCurr, line, column, false, rotation);
            if (column + 1 < matrSize && checkDraw(blockCurr, line, column+1, rotation)){
              column++;
            }
            drawBlock(blockCurr, line, column, true, rotation);
          }
    
          if (joystick.isDown()){
            drawBlock(blockCurr, line, column, false, rotation);
            if(checkDraw(blockCurr, line+1, column, rotation)){
              line++;
            }
            else{
              hasBegun = 0;
            }
    
            drawBlock(blockCurr, line, column, true, rotation);
          }
          
          displayMatrix();
          Serial.println(score);
      }
    else{
      auxStartGame = 0;
    }
    
  }
   else{
    if (joystick.clickedOn()) lcd.clear();
    startMenu();
    displayLogo();
   }
}