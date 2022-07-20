struct pwm {
  float value_pct;
  int16_t counter;
  int pin;
};

const int LED = 38;
bool led_value = LOW;
const float led_frequency = 1.;
int16_t counter = 0;
int value = 0;

pwm led[4];

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
  
  led[0].value_pct = 0.;
  led[1].value_pct = 20.;
  led[2].value_pct = 50.;
  led[3].value_pct = 100.;
  
  sei();//allow interrupts
}


void setup(){
  setup_timer();
  //set pins as outputs
  pinMode(30, OUTPUT);
  Serial.begin(9600);
}

void handle_pwm(pwm *led) {
 float frequency = 50;
 int max_count = int(8000/2/frequency);
 if (led->counter == max_count) {
   led->counter = 0;
 }

 if (led->counter >= int(max_count*led->value_pct/100.))
   digitalWrite(led->pin, 0);
 else
   digitalWrite(led->pin, 1);
 
 led->counter++;
}

ISR(TIMER4_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
  for (int i = 0; i < 4; i++)
    handle_pwm(&led[i]);

  if (counter > 1000) {
    counter = 0;
    led[1].value_pct += 1;
  } else {
    counter++;
  }

  if (led[1].value_pct > 100) {
    led[1].value_pct = 0;
  }
}


// the loop function runs over and over again forever
void loop() {
  if (Serial.available() > 0) {
    led[0].value_pct = Serial.parseFloat();
    Serial.print("Recebido: ");
    Serial.println(led[0].value_pct);
  }
  
}
