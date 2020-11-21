//Digital pin 2: Thermo-hydrometer sensor
//Digital pin 8: Fine Dust sensor
//Digital pin 9: Servo motor
//Analog pin 0:  Illuminance sensor
#include <DHT.h>
#include <Servo.h>

//Digital pin define
#define DHTPIN 2
#define DUST_PIN 8
#define SERVO_PIN 9
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
Servo myservo;  // create servo object to control a servo
// Dust sensor related parameters
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 2000;  //Set the sampling time of the dust sensor to 2 seconds.
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
float dustDensity = 0;

void initPin()
{
  pinMode(DUST_PIN, INPUT);
}

void setup()
{
  initPin();
  starttime = millis();
  myservo.attach(SERVO_PIN);  
  Serial.begin(9600);    // initialize serial communication at 9600 bits per second:

}

void loop()
{
  int brightness = analogRead(A0);  // Read Illuminance value
  float t = dht.readTemperature();
  float h = dht.readHumidity();
    while(1)
    {
          calcDustDensity();
          if(dustDensity>0.01)
          break;
    } //For bug fix where fine dust is periodically measured as 0
  Serial.print("brightness: ");
  Serial.println(brightness);
  Serial.print("Dust: ");
  Serial.print(dustDensity);
  Serial.print("ug/m^3, Temperature: "); 
  Serial.print(t);
  Serial.print("℃, Humidity: "); 
  Serial.print(h); 
  Serial.println("%");
     if((brightness>400)&&(dustDensity<30)&&(h>40)&&(h<60)&&(t>15)&&(t<25)){
        myservo.write(100);
        delay(5000);     }
     else{
        myservo.write(145);
        delay(5000);

     }
    /*
      Standard classification of boundary level according to international fine dust concentration
      30ug/m^3 or less : good / 30~80ug/m^3 : normal / 80~150ug/m^3 : bad / 150ug/m^3 or more : very bad

      Room temperature  15℃~25℃
      humidity 40~60%
    */ 
/* 
   Calculate Reliable Dust Density
   In most analog sensors, there is a phenomenon that the value bounces.
   To correct this, accumulate values several times,
   Take the average to get a reliable dust density.
*/
}
void calcDustDensity() {
  duration = pulseIn(DUST_PIN, LOW);
  lowpulseoccupancy = lowpulseoccupancy + duration;
    
  if ((millis() - starttime) > sampletime_ms) {

    ratio = lowpulseoccupancy / (sampletime_ms * 10.0); // Integer percentage 0=>100
    concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62; // using spec sheet curve
    dustDensity = concentration * 100 / 13000;
    lowpulseoccupancy = 0;
  }
}
