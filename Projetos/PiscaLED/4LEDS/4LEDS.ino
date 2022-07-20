struct led_blink {
  bool value;
  int16_t counter;
  float frequency;
  int pin;
};

const int LED = 38;
bool led_value = LOW;
const float led_frequency = 1.;
int16_t led_counter = 0;

led_blink led[4];

void setup_timer() {
  cli();//stop interrupts

  //set timer4 interrupt at 1Hz
  TCCR4A = 0;// set entire TCCR1A register to 0
  TCCR4B = 0;// same for TCCR1B
  TCNT4  = 0;//initialize counter value to 0
  OCR4A = 1;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR4B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR4B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK4 |= (1 << OCIE4A);

  led[0].pin = 38;
  led[1].pin = 40;
  led[2].pin = 42;
  led[3].pin = 44;
  
  led[0].frequency = 1.;
  led[1].frequency = 1.5;
  led[2].frequency = 2.;
  led[3].frequency = 2.5;
  
  sei();//allow interrupts
}


void setup(){
  setup_timer();
  //set pins as outputs
  pinMode(30, OUTPUT);
  Serial.begin(9600);

  
}

void handle_blink(led_blink *led) {
 if (led->counter == int(1960*4/2/led->frequency)) { // 10 Hz
   led->value = !led->value;
   digitalWrite(led->pin, led->value);
   led->counter = 0;
 }

 led->counter++;
}

ISR(TIMER4_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
  for (int i = 0; i < 4; i++) {
    handle_blink(&led[i]); 
  }
}


// the loop function runs over and over again forever
void loop() {
}
