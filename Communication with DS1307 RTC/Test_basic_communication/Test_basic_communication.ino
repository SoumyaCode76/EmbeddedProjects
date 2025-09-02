#include <Wire.h>

#define DS1307_I2C_ADDRESS      (0x68)

unsigned char c = 0x00;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
  while(!Serial); // Wait until serial monitor is opened
  Serial.println("Test\n");
  Wire.beginTransmission(DS1307_I2C_ADDRESS);     // Send START with slave address
  Wire.write(0x0A);                               // Send write address
  Wire.write(0x5A);                               // Send data to write
  Wire.endTransmission(false);                    // Send STOP
  Wire.beginTransmission(DS1307_I2C_ADDRESS);     // Send START with slave address
  Wire.write(0x0A);                               // Send read address as write data to register
  Wire.endTransmission(true);                     // Send STOP for repeated START
  Wire.requestFrom(DS1307_I2C_ADDRESS, 1);        // Send REPEATED START
  c = Wire.read();                                // Read 1 byte sent from slave
  Serial.print("0x");
  Serial.println(c, HEX);
  Wire.endTransmission(false);                    // Send STOP
}

void loop() {
  // put your main code here, to run repeatedly:
  // Wire.beginTransmission(DS1307_I2C_ADDRESS);
  // Wire.write(byte(0x00));
  // Wire.endTransmission(false);
  // Wire.requestFrom(DS1307_I2C_ADDRESS, 1);
  // unsigned int c = Wire.read();
  // Serial.print("Received: ");
  // Serial.println(c);  
  // Wire.endTransmission(true);  
  // delay(1000);

}
