int pre_flash_ms = 1000; //milliseconds delay between frame start and LED on
int frame_duration_ms = 1000; //milliseconds from LED on to camera off
int inter_frame_interval_min = 1; //minutes between frame capture episodes
unsigned long voltage_minimum = 1000.0; //mV ...value TBD

boolean battery_okay = 1; //state parameter for remaining battery life

// Digital IO symbolic definitions (All are Outputs except those with 'IN' comments)
#define ARD_LED        13    // same as SCK (conflicts with DS3234 SPI bus)
#define RTC_INT_L      2     // IN  //WAKEUP_PIN = 2;
#define LIGHT_BUTTON   4     // was PIN4
#define RTC_RESET_L    5
#define UART_SHDN_L    6
#define LIGHT_POWER    7
#define CAM_ON         8
#define CAM_POWER      9
#define RTC_CS_L       10

//working under the assumption that GoPro can ping back (TBD)
#define CAM_PING       11 //?

// Analog INPUTs
#define BAT_VOLT    A0
#define BAT_CURR    A1

void setup() {
  //pinMode(ARD_LED, OUTPUT);      //  = 13;  (port 13 is also SPI bus SCK
  pinMode(RTC_INT_L, INPUT);       //  = 2;
  pinMode(RTC_RESET_L, OUTPUT);    //  = 5;
  pinMode(UART_SHDN_L, OUTPUT);    //  = 6;
  pinMode(LIGHT_POWER, OUTPUT);    //  = 7;
  pinMode(CAM_ON, OUTPUT);       //  = 8;
  pinMode(CAM_POWER, OUTPUT);    //  = 9;
  pinMode(RTC_CS_L, OUTPUT);       //  = 10;

  //working under the assumption that GoPro can ping back (TBD)
  pinMode(CAM_PING, INPUT);       //  = ?;

  // Note: dont use Arduino LED (pin 13) and DS34322 RTC
  //digitalWrite(ARD_LED, LOW);      // LED off
  digitalWrite(RTC_RESET_L, HIGH);  // active lo uart reset
  digitalWrite(UART_SHDN_L, HIGH);  // active lo uart reset  (Make this low for deployment)
  digitalWrite(LIGHT_POWER, LOW);   // active hi
  digitalWrite(CAM_ON, LOW);        // active hi
  digitalWrite(CAM_POWER, LOW);     // active hi
  digitalWrite(RTC_CS_L, HIGH);     // active lo  

}

//main loop, while battery isn't below threshold voltage, periodically capture video frames
void loop() {
  while (battery_okay) {
    record_frame();
    sleep(inter_frame_interval_min);
  }
  sleep(infinite);
}

//coordinate GoPro and LED to record one frame of video
void record_frame(void) {
  digitalWrite(CAM_POWER, HIGH); //power up GoPro
  
  //wait for GoPro "ready" ping (TBD if this is possible)
  while(digitalRead(CAM_PING,LOW)) { 
    //twiddle thumbs
  }
  digitalWrite(LIGHT_POWER, HIGH); //turn on LED
  poll_voltage(); //check battery during full voltage load (LED and camera) 
  sleep(frame_duration_ms); //wait until frame is over 
  digitalWrite(LIGHT_POWER, LOW); //turn off LED
  digitalWrite(CAM_POWER, LOW); //power down GoPro
}

//store battery life state in "battery_okay"
void poll_voltage(void) {
  if (read_battery() < voltage_minimum) {
    battery_okay = 0
  }
}

//from lines 738-777, method to query battery voltage
unsigned long read_battery()
{
  unsigned long ulBat, batPower;
  unsigned int voltCnt, currCnt;
  float batVolt;

  voltCnt = (unsigned int) analogRead(A0);
  currCnt = (unsigned int) analogRead(A1);

  ulBat = (unsigned long) voltCnt;
  ulBat *= 2687;
  ulBat /= 100;
  batVoltage = (unsigned int) ulBat;

  batCurrent = currCnt * 49;
  batCurrent /= 10;            // 4.9

  batPower = (unsigned long) batVoltage;
  batPower *= (unsigned long) batCurrent;
  return (batPower);
}



