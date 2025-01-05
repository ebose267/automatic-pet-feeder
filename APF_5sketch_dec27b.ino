#include <Wire.h>
#include <RTClib.h>
#include <Servo.h>


RTC_DS1307 rtc;  // Create an RTC object for managing time
Servo servoMotor; // Create a Servo object for controlling the servo motor

// Define pins
const int servoPin = 9;   // The PWM pin connected to the servo motor
const int buzzerPin = 4; // The pin connected to the buzzer
const int trigPin = 11;   // The pin connected to the TRIG of the distance sensor
const int echoPin = 10;   // The pin connected to the ECHO of the distance sensor

// Variables to keep track of time
DateTime previousTime; // Stores the last time the servo was activated
const int intervalMinutes = 1; // Interval in minutes between feedings
const int feedWarningDistance = 19; // Distance in cm to trigger feed low warning

void setup() {
  Serial.begin(9600); // Initialize serial communication for debugging

  // Initialize the RTC
  if (!rtc.begin()) { // Check if the RTC is connected
    Serial.println("Couldn't find RTC");
    while (1); // Stop execution if RTC is not found
  }

  

  if (!rtc.isrunning()) { // Check if the RTC is running
    Serial.println("RTC is NOT running, setting the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set RTC time to compile time
  }

  // Initialize the servo motor
  servoMotor.attach(servoPin); // Attach the servo motor to the specified pin
  servoMotor.write(0); // Set servo to its initial position (0 degrees)

  // Initialize the buzzer pin
  pinMode(buzzerPin, OUTPUT);
  //digitalWrite(buzzerPin, LOW); // Ensure the buzzer is off initially

  // Initialize the distance sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Record the current time as the initial previous time
  previousTime = rtc.now();
}

void loop() {
  DateTime currentTime = rtc.now(); // Get the current time from the RTC

  // Calculate the time difference in seconds
  int timeDifferenceSeconds = currentTime.unixtime() - previousTime.unixtime();
  int remainingTime = (intervalMinutes * 60) - timeDifferenceSeconds;

  // Countdown display on the serial monitor
  if (remainingTime > 0) {
    Serial.print("Time until next feeding: ");
    Serial.print(remainingTime / 60); // Minutes remaining
    Serial.print(" minutes and ");
    Serial.print(remainingTime % 60); // Seconds remaining
    Serial.println(" seconds.");
  }

  // Check if the interval has passed
  if (timeDifferenceSeconds >= intervalMinutes * 60) {
    // Move the servo for 2 seconds to simulate food dispensing
    Serial.println("Feeding time!");
    servoMotor.write(150); // Rotate the servo to 90 degrees (dispensing position)
    delay(900);          // Wait for 2 seconds to dispense food
    servoMotor.write(0);  // Return the servo to its initial position (0 degrees)

    // Update the previou90time to the current time
    previousTime = currentTime;
  }

  // Check the feed level using the distance sensor
  long duration;
  float distance;
  int j;

  // Trigger the sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the ECHO pin for the duration of the sound wave
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance in cm
  distance = duration * 0.034 / 2;
  Serial.print(distance);
  Serial.println(" cm. ");

  if (distance >= feedWarningDistance && distance > 0) { // If feed level is low
   

    Serial.println("Warning: Feed level is low!");
    for(j = 1; j<5; j++){ //this tells the blink code to run while j<5
    digitalWrite(buzzerPin, HIGH); // Activate the buzzer
    delay(50);                   // Keep the buzzer on for 500 microseconds
    digitalWrite(buzzerPin, LOW);  // Turn off the buzzer
    delay(100);
    }

  }

  delay(800); // Delay to reduce unnecessary processing
}
