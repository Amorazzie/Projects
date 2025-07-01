#include <Adafruit_NeoPixel.h>
#define DISPLAY 6
#define PIXELS 64

#define LEFT 3
#define RIGHT 4

Adafruit_NeoPixel strip(PIXELS, DISPLAY, NEO_GRB + NEO_KHZ800);

unsigned long lastmove = 0;
const int moveinterval = 400;

struct Point {
  int x;
  int y;
};

Point snake[64];
int snakeLength = 3;

int dx = 1;
int dy = 0;

int getPixelIndex(int x, int y) {
  if (y % 2 == 0) {
    return y * 8 + x;
  } else {  //y % 2 != 0
    return y * 8 + (7 - x);
  }
}

void moveSnake() {
  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }
  snake[0].x += dx;
  snake[0].y += dy;

  if (snake[0].x > 7) snake[0].x = 0;
  if (snake[0].x < 0) snake[0].x = 7;
  if (snake[0].y > 7) snake[0].y = 0;
  if (snake[0].y < 0) snake[0].y = 7;

  strip.clear();

  //Zeichne die Schlange
  for (int i = 0; i < snakeLength; i++) {
    int index = getPixelIndex(snake[i].x, snake[i].y);
    strip.setPixelColor(index, strip.Color(0, 255, 0));
  }

  strip.show();
}

void spawnPoint() {
  snake[0] = { 3, 4 };  //Head
  snake[1] = { 2, 4 };  //Body
  snake[2] = { 1, 4 };  //Tail
}

void placeFood() {
  int foodx, foody;
  bool valid = false;

  while (!valid) {
    foodx = random(0, 8);
    foody = random(0, 8);

    valid = true;

    for (int i = 0; i < snakeLength; i++) {
      if (foodx == snake[i].x && foody == snake[i].y) {
        valid = false;
        break;
      }
    }
  }

  int foodIndex = getPixelIndex(foodx, foody);
  strip.setPixelColor(foodIndex, strip.Color(255, 255, 0));
  strip.show();
}
Point food;

void collision() {
  // Check for collision with self
  for (int i = 1; i < snakeLength; i++) {  // Start from 1 to avoid checking head against itself
    if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
      Serial.println("GAME OVER!");

      for (int flash = 0; flash < 3; flash++) {
        strip.clear();
        strip.show();
        delay(200);

        snakeLength = 3;
        spawnPoint();
        strip.clear();
        strip.show();

        delay(2000);
        placeFood();
        return;
      }
    }

    // Check for collision with food
    if (snake[0].x == food.x && snake[0].y == food.y) {
      snakeLength++;  // Increase snake length
      if (snakeLength > PIXELS) {
        snakeLength = PIXELS;
      }
      Serial.print(" ");
      Serial.println(snakeLength);
      placeFood();  // Place new food
    }
  }
}

void Left_Button() {
  //If moving right, turn up
  if (dx == 1 && dy == 0) {
    dx = 0;
    dy = -1;
  }
  // If moving up, turn left
  else if (dx == 0 && dy == -1) {
    dx = -1;
    dy = 0;
  }
  // If moving left, turn down
  else if (dx == -1 && dy == 0) {
    dx = 0;
    dy = 1;
  }
  // If moving down, turn right
  else if (dx == 0 && dy == 1) {
    dx = 1;
    dy = 0;
  }
}

void Right_Button() {
  // If moving right, turn down
  if (dx == 1 && dy == 0) {
    dx = 0;
    dy = 1;
  }
  // If moving down, turn left
  else if (dx == 0 && dy == 1) {
    dx = -1;
    dy = 0;
  }
  // If moving left, turn up
  else if (dx == -1 && dy == 0) {
    dx = 0;
    dy = -1;
  }
  // If moving up, turn rightz
  else if (dx == 0 && dy == -1) {
    dx = 1;
    dy = 0;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);

  strip.begin();
  strip.clear();
  strip.setBrightness(100);
  strip.show();

  spawnPoint();
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long now = millis();
  if (now - lastmove >= moveinterval) {
    moveSnake();
    collision();
    lastmove = now;
  }
  int buttonState = digitalRead(LEFT);

  int button2State = digitalRead(RIGHT);
  if (buttonState == 0) {
    Serial.println("LEFT");
    delay(100);
  }
  if (button2State == 0) {
    Serial.println("RIGHT");
    delay(100);
  }
  delay(10);
}
