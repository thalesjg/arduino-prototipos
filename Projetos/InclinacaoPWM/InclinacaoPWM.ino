#include <Wire.h> //I2C Arduino Library

struct vec3d {
    int x;
    int y;
    int z;
};

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

int estado_do_portao = 0;

pwm led[4];
#define address 0x1E //00011110b, I2C 7bit address of HMC5883

void setup_i2c(){  //Initialize Serial and I2C communications
  Serial.begin(9600);
  Wire.begin();  
  //Put the HMC5883 IC into the correct operating mode
  Wire.beginTransmission(address); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();
  pinMode(30, OUTPUT);
}

void setup_timer() {
  cli();//stop interrupts

  //set timer4 interrupt at 1Hz
  TCCR4A = 0;// set entire TCCR1A register to 0
  TCCR4B = 0;// same for TCCR1B
  TCNT4  = 0;//initialize counter value to 0
  OCR4A = 8;// = (16*10^6) / (1*1024) - 1 (must be <65536)
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
  pinMode(22, INPUT);
  Serial.begin(9600);
}

void handle_pwm(pwm *led) {
 int frequency = 50;
 int max_count = int(1000/2/frequency);
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

vec3d read_inclinacao() {
  vec3d vec;
  int xmin,xmax,ymin,ymax,zmin,zmax;
  xmin=0; xmax=0; ymax=0; ymin = 0; zmin=0;zmax=0;
  //Tell the HMC5883 where to begin reading data
  Wire.beginTransmission(address);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();
  
 
 //Read data from each axis, 2 registers per axis
  Wire.requestFrom(address, 6);
  if(6<=Wire.available()){
    vec.x = Wire.read()<<8; //X msb
    vec.x |= Wire.read(); //X lsb
    vec.z = Wire.read()<<8; //Z msb
    vec.z |= Wire.read(); //Z lsb
    vec.y = Wire.read()<<8; //Y msb
    vec.y |= Wire.read(); //Y lsb
  }
  
  //Print out values of each axis
  Serial.print("x: ");
  Serial.print(vec.x);
  Serial.print("  y: ");
  Serial.print(vec.y);
  Serial.print("  z: ");
  Serial.println(vec.z); 

  return vec;
}

// the loop function runs over and over again forever
void loop() {
//  if (Serial.available() > 0) {
//    led[0].value_pct = Serial.parseFloat();
//    Serial.print("Recebido: ");
//    Serial.println(led[0].value_pct);
//  }
  
  vec3d out = read_inclinacao();
//  if (out.z < 400 && out.z > -200)
    led[0].value_pct = -out.z/2 + 65;

  switch (estado_do_portao) {
    case 0:
      if (digitalRead(22)) {
        if (led[0].value_pct > 90) {
          estado_do_portao = -1;
        } else {
          estado_do_portao = 1;
        }
      }
      break;

    case 1:
      if (led[0].value_pct > 90) {
        estado_do_portao = 0;
      }
      break;

    case -1:
      if (led[0].value_pct <= 0) {
        estado_do_portao = 0;
      }
      break;
  }

  led[2].value_pct = (estado_do_portao == 1) ? 100 : 0;
  led[3].value_pct = (estado_do_portao == -1) ? 100 : 0;
}
