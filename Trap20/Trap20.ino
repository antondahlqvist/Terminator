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
  attachInterrupt(digitalPinToInterrupt(sensorpin), ISR_Sensor, RISING);
}

void ISR_Sensor(){
  add_task_to_queue(send_status_text);      //Lägger till skicka sms i kön
  detachInterrupt(digitalPinToInterrupt(sensorpin));
}

void send_status_text(void) {
 Serial.println("Weekly text");
}

void send_triggered_trap_text(){
 Serial.println("Weekly text"); 
}

void go_to_sleep(){
  
}

void loop() {
  run_tasks();
  go_to_sleep();
}
