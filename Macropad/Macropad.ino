#include <ClickEncoder.h>
#include <TimerOne.h>
#include <HID-Project.h>

#define ENCODER1_CLK A0 // Change A0 to, for example, A5 if you want to use analog pin 5 instead
#define ENCODER1_DT A1
#define ENCODER1_SW 16
#define ENCODER2_CLK A2 // Change A0 to, for example, A5 if you want to use analog pin 5 instead
#define ENCODER2_DT A3
#define ENCODER2_SW 10

int ledPin[] = {2, 3, 4, 5};

ClickEncoder *encoder1; // variable representing the rotary encoder
ClickEncoder *encoder2;

int16_t last1, value1; // variables for current and last rotation value
int16_t last2, value2;
byte count = 1;

void timerIsr() {
  encoder1->service();
  encoder2->service();
}

void setup() {
  Serial.begin(9600); // Opens the serial connection used for communication with the PC.
  Consumer.begin(); // Initializes the media keyboard
  encoder1 = new ClickEncoder(ENCODER1_DT, ENCODER1_CLK, ENCODER1_SW); // Initializes the rotary encoder with the mentioned pins
  encoder2 = new ClickEncoder(ENCODER2_DT, ENCODER2_CLK, ENCODER2_SW);

  Timer1.initialize(1000); // Initializes the timer, which the rotary encoder uses to detect rotation
  Timer1.attachInterrupt(timerIsr);
  last1 = -1;
  last2 = -1;

  for (int i = 0; i < 4; i++)
  {
    pinMode(ledPin[i], OUTPUT);
  }
}

void loop() {
  displayLight(count);

  value1 += encoder1->getValue();
  // This part of the code is responsible for the actions when you rotate the encoder
  if (value1 != last1) { // New value is different than the last one, that means to encoder was rotated
    if (last1 < value1) // Detecting the direction of rotation
      Consumer.write(HID_CONSUMER_AC_SCROLL_UP); // Replace this line to have a different function when rotating counter-clockwise
    else
      Consumer.write(HID_CONSUMER_AC_SCROLL_DOWN); // Replace this line to have a different function when rotating clockwise
    last1 = value1; // Refreshing the "last" varible for the next loop with the current value
    Serial.print("Encoder Value: "); // Text output of the rotation value used manily for debugging (open Tools - Serial Monitor to see it)
    Serial.println(value1);
  }

  value2 += encoder2->getValue();
  // This part of the code is responsible for the actions when you rotate the encoder
  if (value2 != last2) { // New value is different than the last one, that means to encoder was rotated
    if (last2 < value2) // Detecting the direction of rotation
      Consumer.write(HID_CONSUMER_AC_SCROLL_UP); // Replace this line to have a different function when rotating counter-clockwise
    else
      Consumer.write(HID_CONSUMER_AC_SCROLL_DOWN); // Replace this line to have a different function when rotating clockwise
    last2 = value2; // Refreshing the "last" varible for the next loop with the current value
    Serial.print("Encoder Value: "); // Text output of the rotation value used manily for debugging (open Tools - Serial Monitor to see it)
    Serial.println(value2);
  }

  // This next part handles the rotary encoder BUTTON
  ClickEncoder::Button b = encoder1->getButton();
  if (b != ClickEncoder::Open) {
    //Serial.print("Button: ");
    //#define VERBOSECASE(label) case label: Serial.println(#label); break;
    switch (b) {
      case ClickEncoder::Clicked:
        increment();
        //Consumer.write(MEDIA_PLAY_PAUSE);
        break;

      case ClickEncoder::DoubleClicked:
        decrement();
        //Consumer.write(MEDIA_NEXT);
        break;
    }
  }

  delay(10);
}

void increment() {
  if (count < 15)
    count++;
  else
    count = 1;
  displayLight(count);
  Serial.println(count);
}

void decrement() {
  if (count >= 2)
    count--;
  else
    count = 15;
  displayLight(count);
  Serial.println(count);
}

void displayLight(byte numToShow) {
  for (int i = 0; i < 4; i++)
  {
    if (bitRead(numToShow, i) == 1)
    {
      digitalWrite(ledPin[i], HIGH);
    }
    else
    {
      digitalWrite(ledPin[i], LOW);
    }
  }
}

/*
    This is just a long comment
    Here are some fun functions you can use to replace the default behaviour
    Consumer.write(CONSUMER_BRIGHTNESS_UP);
    Consumer.write(CONSUMER_BRIGHTNESS_DOWN);
    Consumer.write(CONSUMER_BROWSER_HOME);
    Consumer.write(CONSUMER_SCREENSAVER);
    Consumer.write(HID_CONSUMER_AL_CALCULATOR); //launch calculator :)
    Consumer.write(HID_CONSUMER_AC_ZOOM_IN);
    Consumer.write(HID_CONSUMER_AC_SCROLL_UP);
    CONSUMER_SLEEP = 0x32,

    FULL LIST CAN BE FOUND HERE:
    https://github.com/NicoHood/HID/blob/master/src/HID-APIs/ConsumerAPI.h
*/
