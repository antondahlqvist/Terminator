
//#include <gprs.h>
//#include <sim800.h>



#include <avr/wdt.h>
#include <avr/sleep.h>
#include <util/atomic.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
 

#define SIM800_TX_PIN 8
#define SIM800_RX_PIN 7

char r=13;
String ctlZ="\x1A";  
String content="";


SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);

const int SIM800_POWER_PIN=4;     //Power to sim800 via transistor.
const int BTN_PIN = 2;            //Koppla magneten mellan pin 2 och jord.
const int WDT_INTERVALS = 4866;   //Antal WDT timeouts (1 WDT = 16 sec, 4866 WDT ~ 1 dygn), hur ofta den ska vakna.

volatile boolean magnetPosition;
volatile boolean setSleep;        //Sleep or not
volatile boolean extInterrupt;    //external interrupt flag (button)
volatile boolean wdtInterrupt;    //watchdog timer interrupt flag

volatile bool sendstartsms=1;

//SIM800VARS
char* text;
char* number;
char message[160];
bool error; 


void setup() {
  
pinMode(SIM800_POWER_PIN, OUTPUT);
//pinMode(BTN_PIN, INPUT_PULLUP);
pinMode(BTN_PIN, INPUT);           // set pin to input
digitalWrite(BTN_PIN, HIGH);       // turn on pullup resistors

//////////////SETUP WDT INTERRUPT
   

   ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
       wdt_reset();
       MCUSR &= ~bit(WDRF);                            //clear WDRF
       WDTCSR |= bit(WDCE) | bit(WDE);                 //enable WDTCSR change
       WDTCSR =  bit(WDIE) | bit(WDP3) | bit(WDP0);    //~16 sec
       //WDTCSR  = bit(WDIE) | bit(WDP2) | bit(WDP1);    //~1 sec
   }
  delay(100);
//////////////SETUP SIM800



  serialSIM800.begin(9600);
  delay(1000);
  //Serial.begin(9600);
  delay(500);
  //Serial.println("Setup Complete");
  delay(500);
  
  
  
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
void go_to_sleep(void)
{

   byte adcsra = ADCSRA;            //save the ADC Control and Status Register A
   ADCSRA = 0;                      //disable the ADC
   noInterrupts ();                 // timed sequences follow
   EIFR = bit (INTF0);              // clear flag for interrupt 0
   EIFR = bit (INTF1);              // clear flag for interrupt 1
   attachInterrupt (digitalPinToInterrupt(2), wakeOnInterrupt, RISING);
   set_sleep_mode(SLEEP_MODE_PWR_DOWN);
   wdtInterrupt = false;
   extInterrupt = false;
   sleep_enable();
   byte mcucr1 = MCUCR | bit(BODS) | bit(BODSE); //turn off the brown-out detector while sleeping
   byte mcucr2 = mcucr1 & ~bit(BODSE);
   MCUCR = mcucr1;                  //timed sequence
   MCUCR = mcucr2;                  //BODS stays active for 3 cycles, sleep instruction must be executed while it's active
   interrupts ();                   // need interrupts now
   sleep_cpu();                     //go to sleep
   sleep_disable();                 //wake up here
   ADCSRA = adcsra;                 //restore ADCSRA
}

void wakeOnInterrupt (){
noInterrupts();  
extInterrupt = true;
// don't need the external interrupts any more
detachInterrupt (0);  
detachInterrupt (1);  
//detachInterrupt (2); 
}

//handles the Watchdog Time-out Interrupt
ISR(WDT_vect)
{
   wdtInterrupt = true;
}

void text_delay(void){      //HUR LÄNGE DEN VÄNTAR INNAN SMS
  //Serial.println("Börjar delay");
  delay(10000);
  //Serial.println("10s...");
  delay(10000); 
  //Serial.println("20s...");
  delay(10000);
  //Serial.println("30s...");
  //Serial.println("Slutar Delay");
}  

void end_delay(void){       //HUR LÄNGE DEN VÄNTAR EFTER SMS
  //Serial.println("Börjar delay");
  delay(10000);
  //Serial.println("10s...");
  delay(10000); 
  //Serial.println("20s...");
  delay(10000);
  //Serial.println("30s...");
  //Serial.println("Slutar Delay");
}  

void text_function(void){   //SNACK MELLAN ARDUINO OCH SIM800
  serialSIM800.print("AT+CMGF=1");
  delay(500);
  serialSIM800.print(r);
  delay(500);
  serialSIM800.print("AT+CMGS=\"0767675807\""); //Mobilnummer
  delay(500); 
  serialSIM800.print(r);
  delay(500);
  serialSIM800.print(content);
  delay(500);
  serialSIM800.print(ctlZ);
  delay(500);
  serialSIM800.print(r);
  delay(500);
}


  

void send_startup_text(){
  wake_sim_module();
  text_delay();
  magnetPosition = digitalRead(BTN_PIN);
  //SKICKAR UPPSTART-SMS HÄR SAMT KOLLAR SÅ MAGNETEN ÄR OK.
    
  if(magnetPosition){
    content="Check magnet and restart!";
  }
  else{
    content="Trap is alive!";  
  }
  text_function();
  //SLUT
  end_delay();
  set_sleep_sim_module(); 
  
 }


void send_weekly_text(){
  wake_sim_module();
  text_delay();
  magnetPosition = digitalRead(BTN_PIN);
  if(!magnetPosition){

   //SMS VID CHECK OCH INGEN FÅNGST
   content="Trap is alive, no marten!";
   text_function();
   //SLUT
      
  }
  if(magnetPosition){

    //SMS VID CHECK OCH FÅNGST
    content="Trap is alive, go get the marten!";
    text_function();
    //SLUT
  }
  end_delay(); 
  set_sleep_sim_module();  
}


void send_text(){
  wake_sim_module();
  text_delay();
   //SMS NÄR FÄLLAN SLÅR IHOP
   content="Marten trapped!";
   text_function();
   //SLUT
      
  end_delay();
  set_sleep_sim_module();
  
}

void wake_sim_module(void){
  delay(100);
  //Serial.println("Väcker SIM800");
 delay(500);
  digitalWrite(SIM800_POWER_PIN,HIGH);
}
void set_sleep_sim_module(void){
 delay(100);
  //Serial.println("Söver SIM800");
  delay(500);
  digitalWrite(SIM800_POWER_PIN,LOW);
 
}

void loop() {
  // put your main code here, to run repeatedly:
  static int wdtCount;
  
  if (sendstartsms){
    send_startup_text();
    sendstartsms=0;
  }
  
  if (wdtInterrupt) {
    if (++wdtCount >= WDT_INTERVALS) {
           send_weekly_text();
           wdtCount = 0;
           
       }
    else{
     go_to_sleep();    
    }
    
  }

  if (extInterrupt){ //Om fällan lever och magneten har löst ut
    //Skicka sms att fällan löst ut
    send_text();
    extInterrupt=0;
    setSleep=1;  
  }
  setSleep=1;
  if (setSleep){  //Få skiten att sova
      //Serial.println("Going to sleep");
      //delay(100);
      go_to_sleep();
      //Serial.println("Waking Up");
      //delay(100);
  }




}












