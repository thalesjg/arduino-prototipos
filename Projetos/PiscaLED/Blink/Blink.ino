const int LED = 30;
bool led_value = LOW;
const float led_frequency = 1.;
int16_t led_counter = 0;

void setup_timer() {
  cli();//stop interrupts

  //set timer4 interrupt at 1Hz
  TCCR4A = 0;// set entire TCCR1A register to 0
  TCCR4B = 0;// same for TCCR1B
  TCNT4  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR4A = 8;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR4B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR4B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK4 |= (1 << OCIE4A);
  
  sei();//allow interrupts
}


void setup(){
  setup_timer();
  //set pins as outputs
  pinMode(30, OUTPUT);
}

ISR(TIMER4_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
 if (led_counter == int(1960/2/led_frequency)) { // 10 Hz
   led_value = !led_value;
   digitalWrite(LED, led_value);
   led_counter = 0;
 }

 led_counter++;
}


// the loop function runs over and over again forever
void loop() {
}
