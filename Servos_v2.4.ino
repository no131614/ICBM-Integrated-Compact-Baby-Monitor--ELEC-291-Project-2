//included libraries
#include <NewPing.h>
#include <Servo.h>
#include <Wire.h>
#include <DHT.h>

/*makes servos for up/down, and left/right*/
Servo LeftRight;
Servo UpDown;

/*Pin constants*/
const int LR_PIN = 3;
const int UD_PIN = 5;
const int TRIGGER_PIN = 12;
const int ECHO_PIN = 11;
const int DHTPIN = 2;

#define DHTTYPE DHT11

/*functions*/
void receiveEvent(int);
void requestData();

/*device number for I2C connection*/
const int device = 0x05;

/*variables*/
double lastMillis = 0;
int posLR = 90;
int posUD = 90;
float humidity, temp;
int sonar_cm;

//range sensor and humidity sensor initialization
DHT dht(DHTPIN, DHTTYPE);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, 200);

void setup() {
  /*set pins*/
  LeftRight.attach(LR_PIN);
  UpDown.attach(UD_PIN);

  /*starts I2C connection*/
  Wire.begin(device);
  Wire.onReceive(receiveEvent);   //registers an event
  Wire.onRequest(requestData);
  Serial.begin(9600);           //start serial for output
  dht.begin();

//put the servoes in starting positions
  LeftRight.write(posLR);
  UpDown.write(posUD);

}

void loop() {

//update the readings every second
  double newMillis = millis();
  if ( (lastMillis + 1000) < newMillis) {
    lastMillis = newMillis;
    temp = dht.readTemperature();
    humidity = dht.readHumidity();
    sonar_cm = sonar.ping_cm();
  }
}

/**
 * When data is recieved from the master this function starts automatically
 */
void receiveEvent(int bytesIn) {

  int input;

  while (Wire.available() > 0) {

    //read the first byte to see which servo needs to be rotated
    switch ( Wire.read()) {

      // 0 : Up-Down servo
      case 0:
        Serial.println("case 0");
        // read the new position for the servo
        input = Wire.read();
        if (input != posUD) {
          turnUpDownServo(input);
        }
        break;
      // 1 : Left-Right servo
      case 1:
        Serial.println("case 1");
        // read the new position for the servo
        input = Wire.read();
        if (input != posLR) {
          turnLeftRightServo(input);
        }
        break;
    }
  }

}

/**
 * When the master is requesing data this function starts automatically.
 */
void requestData() {
  // format : 6 bytes in this order [1 , 0 to 200 range , 2 , 0 to 50 temprature , 3 , 0 to 100 humidity ]
  byte data[6] = {0b1, (byte) sonar_cm , 0b10, (byte) temp, 0b11, (byte)humidity };
  // send data back
  Wire.write(data, 6);
}

/*
   For operating up-down servo motor
   Parameters: value -- the desired angle the servo motor should turn
   Does: turns the servo motor to angle value gradually
   Returns: nothing
*/
void turnUpDownServo(int value)
{
  if (value < 0 ) {
    return;
  }
  if (value > posUD) {
    for (posUD; posUD <= value; posUD++) { // goes from 180 degrees to 0 degrees
      UpDown.write(posUD);              // tell servo to go to position in variable 'pos'
    }
  } else if (value < posUD) {
    for (posUD; posUD >= value; posUD--) {
      UpDown.write(posUD);
    }

  } else if (value == posUD) {
    return;
  }
}


/**
    For operating up-down servo motor
    Parameters: value -- the desired angle the servo motor should turn
    Does: turns the servo motor to angle value gradually
    Returns: nothing
*/
void turnLeftRightServo(int value)
{
  if (value < 0 ) {
    return;
  }
  if (value > posLR) {
    for (posLR; posLR <= value; posLR++) { // goes from 180 degrees to 0 degrees
      LeftRight.write(posLR);              // tell servo to go to position in variable 'pos'
    }
  } else if (value < posLR) {
    for (posLR; posLR >= value; posLR--) {
      LeftRight.write(posLR);
    }

  } else if (value == posLR) {
    return;
  }
}

