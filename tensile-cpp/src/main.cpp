#include <Arduino.h>
#include <HX711.h>
#include <Wire.h>

/*########################################
  ##### Mini-Tensile
  ##### Mini tensile load testing machine
  ##### V0
  ##### École de Technologie Supérieure
  ##### Tabas Ilyass, Lengaigne Jacques, Toupin-Guay Valérie, Chabot Olivier
  ##### 26.02.2021
  ##### Libraries:
  ##### HX711 by bogde: https://github.com/bogde/HX711/tree/master/
  ##### Wiring is done according the the schematics in the file
  ########################################*/

////// Test variables
int test = 0;

////// Load Cell Variables
HX711 loadcell;
uint8_t dataPin = 6;
uint8_t clockPin = 7;        //Input pin for the load cell ADC
float calbration_factor = 1;                    //Calibration factor (To be determined)
double force;

////// Ultrasonic distance sensor variables
long duration;                          // variable for the duration of sound travel
int distance;                           // variable for the distance measurement
double sample_len;                        // Length measured for unstrained sample
double strain;                            // Strain mesured by the load cell
const int HCSR04_ECHO_PIN = 4;          //Input pin to echo of HC-SR04
const int HCSRO4_TRIG_PIN = 5;          //Output pin to trig of HC-SR04

////// Initiate the functions namespaces

void double_print(double val1, double val2) {
    Serial.print(val1);
    Serial.print(" ");
    Serial.print(val2);
    Serial.print("\n");
}

double initial_length(double n) {
    int i;
    float sum=0;
    for (i = 0; i<=n; i++){
        // Clears the trigPin condition
        digitalWrite(HCSRO4_TRIG_PIN, LOW);
        delayMicroseconds(2);
        // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
        digitalWrite(HCSRO4_TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(HCSRO4_TRIG_PIN, LOW);
        // Reads the echoPin, returns the sound wave travel time in microseconds
        duration = pulseIn(HCSR04_ECHO_PIN, HIGH);
        sample_len = duration * 0.34 / 2; // Speed of sound wave divided by 2 (go and back)
        sum += sample_len;
    }
    return sum/n;
}

double read_strain1(double len) {
    // Clears the trigPin condition
    digitalWrite(HCSRO4_TRIG_PIN, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(HCSRO4_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(HCSRO4_TRIG_PIN, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(HCSR04_ECHO_PIN, HIGH);
    // Calculating the distance
    distance = duration * 0.34 / 2; // Speed of sound wave divided by 2 (go and back)
    return (distance - len)/len;
}

void spin_me_baby(int state) {
    // state = 1 : go up
    // state = 2 : go down
    // state = 0 : chill

    Wire.beginTransmission(9); // transmit to device #9
    Wire.write(state);              // sends x
    Wire.endTransmission();    // stop transmitting
}


void setup() {

    // Begin serial communication
    Serial.begin(115200);

    // Begin IC2 communication
    Wire.begin();

    // Set motor to not spinning
    spin_me_baby(0);

    // Ultrasonic distance sensor
    pinMode(HCSRO4_TRIG_PIN, OUTPUT); // Sets the trigPin as an OUTPUT
    pinMode(HCSR04_ECHO_PIN, INPUT);  // Sets the echoPin as an INPUT
    Serial.println("Calculating initial length");
    sample_len = initial_length(100.0);    // measure the length of the sample
    Serial.println("Done !");

    // Loadcell
    loadcell.begin(dataPin, clockPin);
    loadcell.set_scale(40510/9.12437);  // F = 9.1243791 N
    loadcell.tare();
    Serial.println("Setup done test will start in 10 sec");
    delay(10000);

}


void loop() {

    if (test==0){
        force = loadcell.get_units();
        strain = read_strain1(sample_len);
        double_print(strain, force);
        Serial.println("Testing begin");
        spin_me_baby(1);
        while (strain < 1 && force <60) {
            force = loadcell.get_units();
            strain = read_strain1(sample_len);
            double_print(strain, force);
        }
        spin_me_baby(2);
        while (strain > 0){
            strain = read_strain1(sample_len);
            Serial.println(strain);
        }
        spin_me_baby(0);
        Serial.println("Testing done");
        test = 1;

    }
    delay(1000);
}



