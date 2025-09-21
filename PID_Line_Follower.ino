#define LEFT_SENSOR     10
#define CENTER_SENSOR   9
#define RIGHT_SENSOR    8
#define ENA 3   // Left motor PWM
#define IN1 6
#define IN2 2
#define ENB 5   // Right motor PWM
#define IN3 4
#define IN4 7

// PID values
float Kp = 18.0;
float Ki = 0.07;
float Kd = 9.0;

// Increased PID for curves/arcs
float curveKP = 24.0;
float curveKD = 15.0;

// PID for deep spiral/tight curves
float spiralKP = 30.0;
float spiralKD = 20.0;

// Dynamics
float error = 0;
float lastError = 0;
float integral = 0;
float derivative = 0;
float correction = 0;

// Speed settings
int targetBaseSpeed = 100;
int maxSpeed = 220;
float currentBaseSpeed = 0;
float rampStep = 3; // Default ramp step speed

int curveSpeed = 22;     // Slower for curves
int spiralSpeed = 12;    // Very slow for spirals

bool finishLineDetected = false;
int finishLineCount = 0;

// Junction & lost line handling
unsigned long tJunctionTime = 0;
const unsigned long tJunctionDelay = 100;
bool tJunctionLock = false;
unsigned long lostLineTime = 0;
const unsigned long lostLineWiggle = 90;  // Extended for spirals
bool wiggleActive = false;

void setup() {
  pinMode(LEFT_SENSOR, INPUT);
  pinMode(CENTER_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void loop() {
  // Gradual speed ramp-up/down
  if (currentBaseSpeed < targetBaseSpeed) {
    currentBaseSpeed += rampStep;
    if (currentBaseSpeed > targetBaseSpeed) currentBaseSpeed = targetBaseSpeed;
  } else if (currentBaseSpeed > targetBaseSpeed) {
    currentBaseSpeed -= rampStep;
    if (currentBaseSpeed < targetBaseSpeed) currentBaseSpeed = targetBaseSpeed;
  }

  // Sensor reading with noise filter
  int leftRaw = 0, centerRaw = 0, rightRaw = 0;
  for (int i = 0; i < 5; i++) {
    leftRaw   += digitalRead(LEFT_SENSOR);
    centerRaw += digitalRead(CENTER_SENSOR);
    rightRaw  += digitalRead(RIGHT_SENSOR);
    delay(1);
  }
  int left = (leftRaw >= 3) ? 1 : 0;
  int center = (centerRaw >= 3) ? 1 : 0;
  int right = (rightRaw >= 3) ? 1 : 0;

  // Finish line detection
  if (left == 1 && center == 1 && right == 1 && !finishLineDetected) {
    finishLineCount++;
    if (finishLineCount > 5) {
      finishLineDetected = true;
      analogWrite(ENA, 0);
      analogWrite(ENB, 0);
      return;
    }
  } else {
    finishLineCount = 0;
  }

  // Path Handling
  bool isCurve = false;
  bool isSpiral = false;
  static int curveCount = 0;
  static int curveDir = 0;

  // T-junction/Cross
  if ((left == 1 && center == 1 && right == 1)) {
    error = 0;
    targetBaseSpeed = 35;
    isCurve = false;
    tJunctionLock = true;
    tJunctionTime = millis();
  }
  else if (tJunctionLock && (millis() - tJunctionTime) < tJunctionDelay) {
    error = 0;
    targetBaseSpeed = 35;
    isCurve = false;
  } else {
    tJunctionLock = false;
    // Lost line recovery
    if (left == 0 && center == 0 && right == 0) {
      error = lastError;
      targetBaseSpeed = 25;
      isCurve = true;
      if (!wiggleActive) {
        lostLineTime = millis();
        wiggleActive = true;
      }
      // Wiggle based on lastError
      if ((millis() - lostLineTime) < lostLineWiggle) {
        if (lastError <= 0) {
          analogWrite(ENA, maxSpeed);
          analogWrite(ENB, 10);
        } else {
          analogWrite(ENA, 10);
          analogWrite(ENB, maxSpeed);
        }
        delay(18);
      }
    } else {
      wiggleActive = false;
      // Standard line following with arc/spiral detection
      if (left == 1 && center == 0 && right == 0) {    // Hard left
        error = -2;
        targetBaseSpeed = 28;
        isCurve = true;
        curveCount++;
        curveDir = -1;
      } else if (left == 0 && center == 0 && right == 1) { // Hard right
        error = 2;
        targetBaseSpeed = 28;
        isCurve = true;
        curveCount++;
        curveDir = 1;
      } else if ((left == 1 && center == 1 && right == 0) || (left == 0 && center == 1 && right == 1)) {
        // gentle local curve left/right
        error = (left == 1) ? -1 : 1;
        targetBaseSpeed = 60;
        isCurve = false;
        curveCount = 0;
        curveDir = 0;
      } else if (left == 0 && center == 1 && right == 0) {
        // Perfect alignment
        error = 0;
        targetBaseSpeed = 100;
        isCurve = false;
        curveCount = 0;
        curveDir = 0;
      } else if (left == 1 && center == 0 && right == 1) {
        // Fork, ambiguous
        error = 0;
        targetBaseSpeed = 40;
        isCurve = true;
        curveCount = 0;
        curveDir = 0;
      } else {
        error = 0;
        targetBaseSpeed = 100;
        isCurve = false;
        curveCount = 0;
        curveDir = 0;
      }

      // Curve/spiral logic
      if (curveCount >= 3) targetBaseSpeed = curveSpeed;
      if (curveCount >= 6) {
        targetBaseSpeed = spiralSpeed; // deep spiral, very slow
        isSpiral = true;
      }
    }
  }

  // Smooth ramp in curves
  rampStep = (isCurve || isSpiral) ? 1.2 : 3.0;

  // PID selection
  float pidKp = isSpiral ? spiralKP : (isCurve ? curveKP : Kp);
  float pidKd = isSpiral ? spiralKD : (isCurve ? curveKD : Kd);

  // Dampen integral accumulation for spirals
  if (isSpiral && abs(error) > 0) integral *= 0.85;

  integral += error;
  derivative = error - lastError;
  correction = pidKp * error + Ki * integral + pidKd * derivative;
  correction = constrain(correction, -85, 85);

  int leftMotorSpeed = currentBaseSpeed + correction;
  int rightMotorSpeed = currentBaseSpeed - correction;
  leftMotorSpeed = constrain(leftMotorSpeed, 0, maxSpeed);
  rightMotorSpeed = constrain(rightMotorSpeed, 0, maxSpeed);

  // === MOTOR DIRECTION CONTROL ===
  int leftMotorDir = 1;   // 1 = forward, 0 = backward
  int rightMotorDir = 1;

  // For hard left (error == -2): left motor backward, right motor forward
  // For hard right (error == 2): right motor backward, left motor forward
  if (error == -2) {           // Hard left
    leftMotorDir = 0;
    rightMotorDir = 1;
  } else if (error == 2) {     // Hard right
    leftMotorDir = 1;
    rightMotorDir = 0;
  }

  // Set PWM speeds
  analogWrite(ENA, leftMotorSpeed);
  analogWrite(ENB, rightMotorSpeed);

  // Set motor directions
  if (leftMotorDir == 1) {          // Left motor forward
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {                          // Left motor backward
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }

  if (rightMotorDir == 1) {         // Right motor forward
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {                          // Right motor backward
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  // ===============================

  lastError = error;
  delay(10);
}