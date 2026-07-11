#include <AFMotor.h>
#include <Servo.h>

// Motors
AF_DCMotor motorFL(1);  // Front Left  - M1
AF_DCMotor motorBL(2);  // Back Left   - M2
AF_DCMotor motorFR(3);  // Front Right - M3
AF_DCMotor motorBR(4);  // Back Right  - M4

Servo scanServo;

#define TRIG_PIN          A0
#define ECHO_PIN          A1
#define SERVO_PIN         10   // Shield SERVO1 header

#define MOTOR_SPEED       200  // 0-255 adjust if needed
#define OBSTACLE_DISTANCE 25   // cm
#define SCAN_DELAY        500  // ms
#define TURN_TIME         500  // ms
#define BACKUP_TIME       400  // ms

// ─────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  // Stop all motors at start
  motorFL.setSpeed(0); motorFL.run(RELEASE);
  motorBL.setSpeed(0); motorBL.run(RELEASE);
  motorFR.setSpeed(0); motorFR.run(RELEASE);
  motorBR.setSpeed(0); motorBR.run(RELEASE);

  // Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Servo center
  scanServo.attach(SERVO_PIN);
  scanServo.write(90);
  delay(800);

  Serial.println("System Ready!");
}

// ─────────────────────────────────────────────
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long dur = pulseIn(ECHO_PIN, HIGH, 30000);
  if (dur == 0) return 999;
  return dur * 0.034 / 2;
}

// ─────────────────────────────────────────────
void moveForward() {
  motorFL.setSpeed(MOTOR_SPEED);
  motorBL.setSpeed(MOTOR_SPEED);
  motorFR.setSpeed(MOTOR_SPEED);
  motorBR.setSpeed(MOTOR_SPEED);
  motorFL.run(FORWARD);
  motorBL.run(FORWARD);
  motorFR.run(FORWARD);
  motorBR.run(FORWARD);
}

void moveBackward() {
  motorFL.setSpeed(MOTOR_SPEED);
  motorBL.setSpeed(MOTOR_SPEED);
  motorFR.setSpeed(MOTOR_SPEED);
  motorBR.setSpeed(MOTOR_SPEED);
  motorFL.run(BACKWARD);
  motorBL.run(BACKWARD);
  motorFR.run(BACKWARD);
  motorBR.run(BACKWARD);
}

void turnLeft() {
  motorFL.setSpeed(MOTOR_SPEED);
  motorBL.setSpeed(MOTOR_SPEED);
  motorFR.setSpeed(MOTOR_SPEED);
  motorBR.setSpeed(MOTOR_SPEED);
  motorFL.run(BACKWARD);
  motorBL.run(BACKWARD);
  motorFR.run(FORWARD);
  motorBR.run(FORWARD);
}

void turnRight() {
  motorFL.setSpeed(MOTOR_SPEED);
  motorBL.setSpeed(MOTOR_SPEED);
  motorFR.setSpeed(MOTOR_SPEED);
  motorBR.setSpeed(MOTOR_SPEED);
  motorFL.run(FORWARD);
  motorBL.run(FORWARD);
  motorFR.run(BACKWARD);
  motorBR.run(BACKWARD);
}

void stopMotors() {
  motorFL.run(RELEASE);
  motorBL.run(RELEASE);
  motorFR.run(RELEASE);
  motorBR.run(RELEASE);
}

// ─────────────────────────────────────────────
void loop() {
  long frontDist = getDistance();
  Serial.print("Front: ");
  Serial.print(frontDist);
  Serial.println(" cm");

  if (frontDist > OBSTACLE_DISTANCE) {
    moveForward();

  } else {
    stopMotors();
    delay(300);

    // Scan LEFT
    scanServo.write(150);
    delay(SCAN_DELAY);
    long leftDist = getDistance();
    Serial.print("Left: "); Serial.println(leftDist);

    // Scan RIGHT
    scanServo.write(30);
    delay(SCAN_DELAY);
    long rightDist = getDistance();
    Serial.print("Right: "); Serial.println(rightDist);

    // Return center
    scanServo.write(90);
    delay(300);

    // Back up
    moveBackward();
    delay(BACKUP_TIME);
    stopMotors();
    delay(200);

    // Turn toward clearer side
    if (leftDist >= rightDist) {
      Serial.println("Turning LEFT");
      turnLeft();
    } else {
      Serial.println("Turning RIGHT");
      turnRight();
    }
    delay(TURN_TIME);
    stopMotors();
    delay(200);
  }
}