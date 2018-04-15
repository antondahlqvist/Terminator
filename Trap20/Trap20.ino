#include <AltSoftSerial.h>
#include <BareBoneSim800.h>

#include <EEPROM.h>

#define SENSOR_PIN 2




void setup_sensor(void){
  const int sensorpin = SENSOR_PIN;            //Koppla magneten mellan pin 2 och jord.
  pinMode(sensorpin, INPUT_PULLUP);   //Input, Pullup motstånd
  attachInterrupt(digitalPinToInterrupt(sensorpin), ISR_Sensor, FALLING);
}

ISR_Sensor(){
  
}

void setup() {

  setup_sensor();  //Konfigruerar sensorns inpupinne och interrupt

}

void loop() {
  // put your main code here, to run repeatedly:

}
