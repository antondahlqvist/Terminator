
#include "AltSoftSerial.h"
#include "BareBoneSim800.h"

#include "tasks.h"


//***Variabler***//
const int sensorpin = 2;            //Koppla magneten mellan pin 2 och jord.
const int SIM800_POWER_PIN=4;     //Power to sim800 via transistor.
const char* number = "+46730432404";
char* message = "";

BareBoneSim800 sim800;

//***Programflaggor***///
volatile bool sensorInterruptRecieved = 0;  //Flaggar om externa interrupten är utvärderad

//***Programstart***///
void setup() {
  Serial.begin(9600);
  setup_sensor();  //Konfigruerar sensorns inpupinne och interrupt
  setup_SIM800();
  tasks_init();   //Initierar task hanteraren
}

void setup_SIM800(){
  sim800.begin();
  power_sim_module();
}

void setup_sensor(void){
  pinMode(sensorpin, INPUT_PULLUP);   //Input, Pullup motstånd
  attachInterrupt(digitalPinToInterrupt(sensorpin), ISR_Sensor, CHANGE);
}

void ISR_Sensor(){
 if (not sensorInterruptRecieved){
 add_task_to_queue(evaluate_sensor);      //Lägger till skicka sms i kön
 }
 sensorInterruptRecieved=true;
}

void evaluate_sensor(){
  delay(100); //debounce på sensor
  bool currentVal=digitalRead(sensorpin);
  if (currentVal){
    add_task_to_queue(send_trapped_text);
  }
  else{
    add_task_to_queue(send_triggered_text);
  }
  sensorInterruptRecieved = false;
}

void send_status_text(void) {
 Serial.println("Weekly text");
}

void send_trapped_text(void) {
 char* message = "The Marten is trapped";
 bool messageSent = sim800.sendSMS(number, message);
 if(messageSent)
      Serial.println("Message Sent");
   else
      Serial.println("Not Sent, Something happened");     
}

void send_triggered_text(){
 Serial.println("triggered text"); 
}

void go_to_sleep(){
  
}

void power_sim_module(void){
  digitalWrite(SIM800_POWER_PIN,HIGH);
}
void unpower_sim_module(void){
  digitalWrite(SIM800_POWER_PIN,LOW);
}

void loop() {
  run_tasks();
  go_to_sleep();
}
