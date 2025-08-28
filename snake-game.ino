#include <LedControl.h>  // Library for MAX7219

// Hardware
// MAX7219 Connections (DIN, CLK, CS Pins)
LedControl lc = LedControl(12, 11, 10); 

// Joystick pinleri
const int joyX = A0;
const int joyY = A1;

// Buzzer
const int buzzer = 3;

// --- Game Variables ---
int snakeX[64];   // x coordinates of the snake
int snakeY[64];   // y coordinates of the snake
int snakeLength = 3;

int dirX = 1;   //  starting direction (right)
int dirY = 0;

int foodX, foodY;

bool gameOver = false;

// --- Functions ---
void clearMatrix() {
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, 0);  // Clean all lines
  }
}

void drawSnake() {
  clearMatrix();
  // Snake
  for (int i = 0; i < snakeLength; i++) {
    lc.setLed(0, snakeY[i], snakeX[i], true);
  }
  // Food
  lc.setLed(0, foodY, foodX, true);
}

void spawnFood() {
  bool valid = false;
  while (!valid) {
    foodX = random(0, 8);
    foodY = random(0, 8);
    valid = true;
    for (int i = 0; i < snakeLength; i++) {
      if (snakeX[i] == foodX && snakeY[i] == foodY) {
        valid = false;
        break;
      }
    }
  }
}

void moveSnake() {
  if (gameOver) return;

  // Slide the tail
  for (int i = snakeLength; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  // New head position
  snakeX[0] += dirX;
  snakeY[0] += dirY;

  // Wall -crashing
  if (snakeX[0] < 0 || snakeX[0] > 7 || snakeY[0] < 0 || snakeY[0] > 7) {
    gameOver = true;
    return;
  }

  // Crash on themselves
  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      gameOver = true;
      return;
    }
  }

  // Eating food
  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    snakeLength++;
    tone(buzzer, 1000, 100);
    spawnFood();
  }
}

void readJoystick() {
  int xVal = analogRead(joyX);
  int yVal = analogRead(joyY);

  if (xVal < 400 && dirX == 0) { dirX = -1; dirY = 0; }  // left
  else if (xVal > 600 && dirX == 0) { dirX = 1; dirY = 0; } // right
  else if (yVal < 400 && dirY == 0) { dirX = 0; dirY = -1; } // up
  else if (yVal > 600 && dirY == 0) { dirX = 0; dirY = 1; }  // down
}

void setup() {
  lc.shutdown(0, false);   // Module Open
  lc.setIntensity(0, 8);   // Brightness 0-15
  lc.clearDisplay(0);

  pinMode(buzzer, OUTPUT);

  randomSeed(analogRead(A5)); // random seed

  // Starting Snake
  snakeX[0] = 3; snakeY[0] = 4;
  snakeX[1] = 2; snakeY[1] = 4;
  snakeX[2] = 1; snakeY[2] = 4;

  spawnFood();
}

void loop() {
  if (!gameOver) {
    readJoystick();
    moveSnake();
    drawSnake();
    delay(150);  // snake speed
  } else {
    // Open the light of the whole screen when the game is finished
    for (int i = 0; i < 8; i++) lc.setRow(0, i, B11111111);
    tone(buzzer, 200, 500);
    delay(2000);
    // Reseet
    snakeLength = 3;
    dirX = 1; dirY = 0;
    snakeX[0] = 3; snakeY[0] = 4;
    snakeX[1] = 2; snakeY[1] = 4;
    snakeX[2] = 1; snakeY[2] = 4;
    gameOver = false;
    spawnFood();
  }
}
