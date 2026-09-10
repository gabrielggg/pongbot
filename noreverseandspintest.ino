#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);

// PCA9685 channels
#define M1_IN1  0
#define M1_IN2  1

#define M2_IN1  2
#define M2_IN2  3

#define PWM_MAX 4095

// Motor 1 stays at 100%
#define MOTOR1_SPEED 4095

// Motor 2 speed levels
int motor2Speeds[] = {
  3072,  // 75%
  2048,  // 50%
  1024   // 25%
};

const int NUM_SPEEDS = sizeof(motor2Speeds) / sizeof(motor2Speeds[0]);

// How long each speed runs
const int RUN_TIME = 5000;


// --------------------------------------------------
// Set motor speed
// Positive = forward
// Zero = stop
// --------------------------------------------------
void setMotor(int in1, int in2, int speed)
{
  speed = constrain(speed, 0, PWM_MAX);

  if (speed > 0)
  {
    // Inverted rotation direction
    pca.setPWM(in1, 0, 0);
    pca.setPWM(in2, 0, speed);
  }
  else
  {
    // Stop / coast
    pca.setPWM(in1, 0, 0);
    pca.setPWM(in2, 0, 0);
  }
}


// --------------------------------------------------
// Set both motors
// --------------------------------------------------
void setMotors(int motor1Speed, int motor2Speed)
{
  setMotor(M1_IN1, M1_IN2, motor1Speed);
  setMotor(M2_IN1, M2_IN2, motor2Speed);
}


void setup()
{
  Wire.begin();

  pca.begin();

  // 1 kHz PWM
  pca.setPWMFreq(1000);

  // Start stopped
  setMotors(0, 0);
}


void loop()
{
  // Motor 1 = 100% constantly
  // Motor 2 = 75% -> 50% -> 25%

  for (int i = 0; i < NUM_SPEEDS; i++)
  {
    int motor2Speed = motor2Speeds[i];

    setMotors(MOTOR1_SPEED, motor2Speed);

    // Hold speed for 5 seconds
    delay(RUN_TIME);
  }

  // Repeat forever:
  // 75% -> 50% -> 25% -> 75% -> ...
}
