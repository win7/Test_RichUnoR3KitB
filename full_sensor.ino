
#include <LiquidCrystal_I2C.h>

const int light_pin = A1;
const int water_pin = A2;
#define THERMISTORPIN A3        
// resistance of termistor at 25 degrees C
#define THERMISTORNOMINAL 10000      
#define TEMPERATURENOMINAL 25   
// Accuracy - Higher number is bigger
#define NUMSAMPLES 10
// Beta coefficient from datasheet
#define BCOEFFICIENT 3950
// the value of the R1 resistor
#define SERIESRESISTOR 10000    
//prepare pole 
uint16_t samples[NUMSAMPLES];

const int pir_pin = 3;
const int wire_pin = 5;
const int recv_pin = 6;
const int echo_pin = 7;
const int trigger_pin = 8;
const int led_pin = 13;

int light_value = 0;
int light_map = 0;
int water_value = 0;
int water_map = 0;

int temperature_value = 0;

long time_;
long distance;

uint8_t i;
float average;
  
LiquidCrystal_I2C lcd(0x38, 16, 2); // change 0x38

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  pinMode(pir_pin, INPUT);

  pinMode(trigger_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  digitalWrite(trigger_pin, LOW);

  pinMode(led_pin, OUTPUT);

  lcd.init();       // Initialize the LCD display screen
  lcd.clear(); 
  lcd.backlight();  // Turn on backlight of LCD display screen.
}

void loop() {
  lcd.clear();
  
  // Light
  light_value = analogRead(light_pin);
  light_map = map(light_value, 0, 1023, 0, 100);
  // Serial.println(light_map);
  lcd.setCursor(8, 0);
  lcd.print("Lig:");
  lcd.print(light_map);
  
  // HC-SR04
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10); // pulse of 10us
  digitalWrite(trigger_pin, LOW);
   
  time_ = pulseIn(echo_pin, HIGH);
  distance = time_ / 59; // cm
  /* Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print("cm");
  Serial.println(); */
  lcd.setCursor(8, 1);
  lcd.print("Dis:");
  lcd.print(distance);

  // Water
  water_value = analogRead(water_pin);
  Serial.println(water_value);
  if (water_value >= 150 && water_value <= 200) {
    water_map = map(water_value, 150, 200, 0, 100); 
    water_map = 100 - water_map;
    // Serial.print("Water nivel: ");
    // Serial.println(water_value);
    // Serial.println(water_map);
  } else {
    water_map = 0;
  }
  lcd.setCursor(0, 1);
  lcd.print("Wat:");
  lcd.print(water_map);

  // temperature
  temperature_value = int(get_temperature());
  Serial.print("Temperature: "); 
  Serial.println(temperature_value);
  
  // converter to settle after the last reading:
  delay(1000);
}

float get_temperature() {
  // saving values from input to pole
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
 
   // average value of input
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
 
  //resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;

 //resistence to temperature
  float temperature;
  temperature = average / THERMISTORNOMINAL; 
  temperature = log(temperature);   
  temperature /= BCOEFFICIENT;                  
  temperature += 1.0 / (TEMPERATURENOMINAL + 273.15); 
  temperature = 1.0 / temperature;                 
  temperature -= 273.15;                         // convert to C
  return temperature;
}
