#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);

// PCA9685 channels
#define M1_IN1  0
#define M1_IN2  1

#define M2_IN1  2
#define M2_IN2  3

#define PWM_MAX 4095

// Speed levels
int speeds[] = {
  3000,   // ~75%
  3350,   // ~83%
  3700,   // ~91%
  4095    // 100%
};

const int NUM_SPEEDS = sizeof(speeds) / sizeof(speeds[0]);

// Brake time when changing direction
const int BRAKE_TIME = 500;


// --------------------------------------------------
// Set motor speed
// Positive = forward
// Negative = reverse
// Zero = coast/stop
// --------------------------------------------------
void setMotor(int in1, int in2, int speed)
{
  speed = constrain(speed, -PWM_MAX, PWM_MAX);

  if (speed > 0)
  {
    // Forward
    pca.setPWM(in2, 0, 0);
    pca.setPWM(in1, 0, speed);
  }
  else if (speed < 0)
  {
    // Reverse
    pca.setPWM(in1, 0, 0);
    pca.setPWM(in2, 0, -speed);
  }
  else
  {
    // Coast
    pca.setPWM(in1, 0, 0);
    pca.setPWM(in2, 0, 0);
  }
}


// --------------------------------------------------
// Active brake
// DRV8871 IN1 = HIGH
// DRV8871 IN2 = HIGH
// --------------------------------------------------
void brakeMotor(int in1, int in2)
{
  pca.setPWM(in1, 0, PWM_MAX);
  pca.setPWM(in2, 0, PWM_MAX);
}


// --------------------------------------------------
// Brake both motors
// --------------------------------------------------
void brakeBothMotors()
{
  brakeMotor(M1_IN1, M1_IN2);
  brakeMotor(M2_IN1, M2_IN2);

  delay(BRAKE_TIME);

  // Release brake
  setMotor(M1_IN1, M1_IN2, 0);
  setMotor(M2_IN1, M2_IN2, 0);
}


// --------------------------------------------------
// Set both motors to same speed/direction
// --------------------------------------------------
void setBothMotors(int speed)
{
  setMotor(M1_IN1, M1_IN2, speed);
  setMotor(M2_IN1, M2_IN2, speed);
}


void setup()
{
  Wire.begin();

  pca.begin();

  // 1 kHz PWM
  pca.setPWMFreq(1000);

  // Start stopped
  setBothMotors(0);
}


void loop()
{
  bool forward = true;
  int speedIndex = 0;

  while (true)
  {
    int speed = speeds[speedIndex];

    // Set direction
    if (forward)
      setBothMotors(speed);
    else
      setBothMotors(-speed);

    // Run for 5 seconds
    delay(5000);

    // Active brake before changing direction
    brakeBothMotors();

    // Change direction
    forward = !forward;

    // Change speed every 10 seconds
    if (forward)
    {
      speedIndex++;

      if (speedIndex >= NUM_SPEEDS)
        speedIndex = 0;
    }
  }
}
