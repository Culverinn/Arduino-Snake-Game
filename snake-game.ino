#include <LedControl.h>  // MAX7219 için kütüphane

// --- Donanım ---
// MAX7219 bağlantıları (DIN, CLK, CS pinleri)
LedControl lc = LedControl(12, 11, 10, 1); 

// Joystick pinleri
const int joyX = A0;
const int joyY = A1;

// Buzzer
const int buzzer = 3;

// --- Oyun değişkenleri ---
int snakeX[64];   // yılanın X koordinatları
int snakeY[64];   // yılanın Y koordinatları
int snakeLength = 3;

int dirX = 1;   // başlangıç yönü (sağ)
int dirY = 0;

int foodX, foodY;

bool gameOver = false;

// --- Fonksiyonlar ---
void clearMatrix() {
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, 0);  // bütün satırları temizle
  }
}

void drawSnake() {
  clearMatrix();
  // Yılan
  for (int i = 0; i < snakeLength; i++) {
    lc.setLed(0, snakeY[i], snakeX[i], true);
  }
  // Yem
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

  // Kuyruğu kaydır
  for (int i = snakeLength; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  // Yeni kafa pozisyonu
  snakeX[0] += dirX;
  snakeY[0] += dirY;

  // Duvara çarpma
  if (snakeX[0] < 0 || snakeX[0] > 7 || snakeY[0] < 0 || snakeY[0] > 7) {
    gameOver = true;
    return;
  }

  // Kendi üstüne çarpma
  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      gameOver = true;
      return;
    }
  }

  // Yem yeme
  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    snakeLength++;
    tone(buzzer, 1000, 100);
    spawnFood();
  }
}

void readJoystick() {
  int xVal = analogRead(joyX);
  int yVal = analogRead(joyY);

  if (xVal < 400 && dirX == 0) { dirX = -1; dirY = 0; }  // sol
  else if (xVal > 600 && dirX == 0) { dirX = 1; dirY = 0; } // sağ
  else if (yVal < 400 && dirY == 0) { dirX = 0; dirY = -1; } // yukarı
  else if (yVal > 600 && dirY == 0) { dirX = 0; dirY = 1; }  // aşağı
}

void setup() {
  lc.shutdown(0, false);   // modül aç
  lc.setIntensity(0, 8);   // parlaklık 0–15
  lc.clearDisplay(0);

  pinMode(buzzer, OUTPUT);

  randomSeed(analogRead(A5)); // rastgele için seed

  // Başlangıç yılanı
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
    delay(150);  // yılan hızı
  } else {
    // Oyun bittiğinde tüm ekranı yak
    for (int i = 0; i < 8; i++) lc.setRow(0, i, B11111111);
    tone(buzzer, 200, 500);
    delay(2000);
    // Resetlemek için
    snakeLength = 3;
    dirX = 1; dirY = 0;
    snakeX[0] = 3; snakeY[0] = 4;
    snakeX[1] = 2; snakeY[1] = 4;
    snakeX[2] = 1; snakeY[2] = 4;
    gameOver = false;
    spawnFood();
  }
}
