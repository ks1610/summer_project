#include <Arduino.h>

// Khai báo chân kết nối cảm biến siêu âm
#define TRIG_FRONT 2
#define ECHO_FRONT 3
#define TRIG_LEFT 4
#define ECHO_LEFT 5
#define TRIG_RIGHT 6
#define ECHO_RIGHT 7

// Khai báo chân điều khiển động cơ (L298N)
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// Kích thước mê cung và ô vuông
#define MAZE_SIZE 16
#define CELL_SIZE 18  // cm

int maze[MAZE_SIZE][MAZE_SIZE];  // Mảng lưu trạng thái mê cung
int x = 0, y = 0;                // Vị trí hiện tại của xe
int direction = 0;               // Hướng của xe (0: Bắc, 1: Đông, 2: Nam, 3: Tây)

// Hàm đo khoảng cách bằng cảm biến siêu âm
long measureDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    return pulseIn(echoPin, HIGH) * 0.034 / 2; // Chuyển đổi thành cm
}

// Hàm điều khiển động cơ
void moveForward() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    delay(500);
}

void turnLeft() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    delay(500);
    direction = (direction + 1) % 4;
}

void turnRight() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    delay(500);
    direction = (direction + 3) % 4;
}

void stopMotors() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

// Thuật toán Flood Fill
void floodFill() {
    // Khởi tạo mê cung với giá trị lớn
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            maze[i][j] = 255;
        }
    }
    // Thiết lập ô đích
    maze[MAZE_SIZE / 2][MAZE_SIZE / 2] = 0;
    maze[MAZE_SIZE / 2 - 1][MAZE_SIZE / 2] = 0;
    maze[MAZE_SIZE / 2][MAZE_SIZE / 2 - 1] = 0;
    maze[MAZE_SIZE / 2 - 1][MAZE_SIZE / 2 - 1] = 0;
    
    // Cập nhật giá trị khoảng cách đến đích
    bool updated = true;
    while (updated) {
        updated = false;
        for (int i = 0; i < MAZE_SIZE; i++) {
            for (int j = 0; j < MAZE_SIZE; j++) {
                if (maze[i][j] != 255) {
                    int currentValue = maze[i][j];
                    if (i > 0 && maze[i - 1][j] > currentValue + 1) {
                        maze[i - 1][j] = currentValue + 1;
                        updated = true;
                    }
                    if (i < MAZE_SIZE - 1 && maze[i + 1][j] > currentValue + 1) {
                        maze[i + 1][j] = currentValue + 1;
                        updated = true;
                    }
                    if (j > 0 && maze[i][j - 1] > currentValue + 1) {
                        maze[i][j - 1] = currentValue + 1;
                        updated = true;
                        }
                    if (j < MAZE_SIZE - 1 && maze[i][j + 1] > currentValue + 1) {
                        maze[i][j + 1] = currentValue + 1;
                        updated = true;
                    }
                }
            }
        }
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(TRIG_FRONT, OUTPUT);
    pinMode(ECHO_FRONT, INPUT);
    pinMode(TRIG_LEFT, OUTPUT);
    pinMode(ECHO_LEFT, INPUT);
    pinMode(TRIG_RIGHT, OUTPUT);
    pinMode(ECHO_RIGHT, INPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    floodFill();  // Tính toán khoảng cách đến đích
}

void loop() {
  
    long frontDist = measureDistance(TRIG_FRONT, ECHO_FRONT);
    long leftDist = measureDistance(TRIG_LEFT, ECHO_LEFT);
    long rightDist = measureDistance(TRIG_RIGHT, ECHO_RIGHT);

    if (frontDist > 10) {
        moveForward();
        y += (direction == 0) ? 1 : (direction == 2) ? -1 : 0;
        x += (direction == 1) ? 1 : (direction == 3) ? -1 : 0;
    } else if (leftDist > 10) {
        turnLeft();
    } else if (rightDist > 10) {
        turnRight();
    } else {
        turnRight();
        turnRight();  // Quay 180 độ nếu không còn đường
    }

    if (maze[x][y] == 0) {
        stopMotors();
        while (true);
    }
}
