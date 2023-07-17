#include <SoftwareSerial.h>

#define Ard1 13
#define Ard2 12

SoftwareSerial sim(10, 11); // RX & TX pinout between parenthesis. If it doesn't work change the pins to: RX #10 & TX #11

int _timeout, state = 0;

char order;

short OK = -1;

String _buffer;
String number = "+917003104504"; // The Phone Number of the reciever
String password = "sjhs";        // Password for turning on and off the security


void setup() {
  delay(2000);        // delay for 2 seconds to make sure the modules get the signal
  Serial.begin(9600); // To help contact the serial monitor
  pinMode(Ard1, OUTPUT);
  pinMode(Ard2, INPUT);
  digitalWrite(Ard1, 1);
  _buffer.reserve(50);
  Serial.println("The program ");
  sim.begin(9600);
  delay(1000);
  Serial.println("Ready"); // Wait for this message to start testing
  SendMessage("SMS from the laser alarm system!");
  RecieveMessage();
}


void loop() {
  String RSMS;
  switch (state) {
  case 0:
    while (sim.available() > 0) {
      order = sim.read();
      RSMS.concat(order);
      OK = RSMS.indexOf("On");
    }
    if (OK != -1) {
      SendMessage("Type the password to activate the alarm:");
      state = 1;
      OK = -1;
      RecieveMessage();
    }
    break;

  case 1:
    while (sim.available() > 0) {
      order = sim.read();
      RSMS.concat(order);
      OK = RSMS.indexOf(password);
    }
    if (OK != -1) {
      SendMessage("Turned On!. To deactivate the alarm. Type 'Off'");
      state = 2;
      digitalWrite(Ard1, 0);
      OK = -1;
      RecieveMessage();
    }
    break;

  case 2:
    if (digitalRead(Ard2) == 0 && digitalRead(Ard1) == 0) {
      Serial.println("Detected suspisous movement!");
      delay(500);
      callNumber();
      RecieveMessage();
    }
    while (sim.available() > 0) {
      order = sim.read();
      RSMS.concat(order);
      OK = RSMS.indexOf("Off");
    }
    if (OK != -1) {
      SendMessage("To deactivate the alarm type off:");
      state = 3;
      OK = -1;
      RecieveMessage();
    }
    break;

  case 3:
    if (digitalRead(Ard2) == 0 && digitalRead(Ard1) == 0) {
      Serial.println("Alert! We detected Suspisous Movements. Please Check for any Intruder.");
      delay(500);
      callNumber();
      RecieveMessage();
    }
    while (sim.available() > 0) {
      order = sim.read();
      RSMS.concat(order);
      OK = RSMS.indexOf(password);
    }
    if (OK != -1) {
      SendMessage("Turned Off!");
      digitalWrite(Ard1, 1);
      state = 0;
      OK = -1;
      RecieveMessage();
    }
    break;
  }
  if (sim.available() > 0)
    Serial.write(sim.read());
}


void SendMessage(String SMS) {
  // Serial.println ("Sending Message");
  sim.println("AT+CMGF=1"); // Sets the GSM Module in Text Mode
  delay(1000);
  Serial.println("Set SMS Number");
  sim.println("AT+CMGS=\"" + number + "\"\r"); // Mobile phone number to send message
  delay(1000);
  sim.println(SMS);
  delay(100);
  sim.println((char)26); // ASCII code of CTRL+Z
  delay(1000);
  _buffer = _readSerial();
}


void RecieveMessage() {
  Serial.println("SIM800L Read an SMS");
  delay(1000);
  sim.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
  delay(1000);
  Serial.write("Unread Message done");
}


String _readSerial()
{
  _timeout = 0;
  while (!sim.available() && _timeout < 12000)
  {
    delay(13);
    _timeout++;
  }
  if (sim.available())
  {
    return sim.readString();
  }
}


void callNumber()
{
  sim.print(F("ATD"));
  sim.print(number);
  sim.print(F(";\r\n"));
  _buffer = _readSerial();
  Serial.println(_buffer);
}