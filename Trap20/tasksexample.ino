#include "tasks.h"

void kalle_task(void) {
  Serial.println("Kalle task");
}

void setup() {
  Serial.begin(9600);
  Serial.println("Startup");
  
  tasks_init();
  add_task_to_queue(kalle_task);
  
}


void go_to_sleep() {
  
}

void loop() {
  // put your main code here, to run repeatedly:

  run_tasks();
  go_to_sleep();

}



