#include <AltSoftSerial.h>
#include <BareBoneSim800.h>

#include "tasks.h"

const int sensorpin = 2;            //Koppla magneten mellan pin 2 och jord.

void setup() {
  Serial.begin(9600);

  
  setup_sensor();  //Konfigruerar sensorns inpupinne och interrupt
  tasks_init();   //Initierar task hanteraren
}

void setup_sensor(void){
  
  pinMode(sensorpin, INPUT_PULLUP);   //Input, Pullup motstånd
  attachInterrupt(digitalPinToInterrupt(sensorpin), ISR_Sensor, CHANGE);
}

void ISR_Sensor(){
  detachInterrupt(digitalPinToInterrupt(sensorpin));
  add_task_to_queue(evaluate_sensor);      //Lägger till skicka sms i kön
  
}

void evaluate_sensor(){
  delay(500); //debounce på sensor
  bool currentVal=digitalRead(sensorpin);
  if (currentVal){
    add_task_to_queue(send_trapped_text);
  }
  else{
    add_task_to_queue(send_triggered_text);
  }
  attachInterrupt(digitalPinToInterrupt(sensorpin), ISR_Sensor, CHANGE);

}

void send_status_text(void) {
 Serial.println("Weekly text");
}

void send_trapped_text(void) {
 Serial.println("trapped text");
}

void send_triggered_text(){
 Serial.println("triggered text"); 
}

void go_to_sleep(){
  
}

void loop() {
  run_tasks();
  go_to_sleep();
}
