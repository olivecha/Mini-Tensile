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
int test = 1;

////// Load Cell Variables
HX711 loadcell;
uint8_t dataPin = 6;
uint8_t clockPin = 7;        //Input pin for the load cell ADC
double force;

////// Ultrasonic distance sensor variables
long duration;                          // variable for the duration of sound travel
int distance;                           // variable for the distance measurement
double sample_len;                        // Length measured for unstrained sample
double strain;                            // Strain mesured by the load cell
const int HCSR04_ECHO_PIN = 4;          //Input pin to echo of HC-SR04
const int HCSRO4_TRIG_PIN = 5;          //Output pin to trig of HC-SR04

//// Toggle button variables
const int BUTTON_PIN = 8;
int ButtonState;    // the previous state of button
int condition = true;

////// Functions

int button(){
    return digitalRead(BUTTON_PIN);
}

double read_strain(double len) {
    //// measure the strain from the initial length
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
    //// Send instructions to the slave arduino to rotate the stepper
    // state = 1 : go up
    // state = 2 : go down
    // state = 0 : chill

    Wire.beginTransmission(9); // transmit to device #9
    Wire.write(state);              // sends x
    Wire.endTransmission();    // stop transmitting
}

double read_distance(){
    ///// Direct read of the distance with distance sensor
    // Clears the trigPin condition
    digitalWrite(HCSRO4_TRIG_PIN, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(HCSRO4_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(HCSRO4_TRIG_PIN, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(HCSR04_ECHO_PIN, HIGH);
    return duration * 0.34 / 2; // Speed of sound wave divided by 2 (go and back)
}

void double_print(double val1, double val2) {
    //// Print 2 values to terminal
    Serial.print(val1);
    Serial.print(" ");
    Serial.print(val2);
    Serial.print("\n");
}

void come_down(){
    spin_me_baby(2);
    while (strain >0.1){
        strain = read_strain(sample_len);
        ButtonState = digitalRead(BUTTON_PIN);
        // Button interrupt
        if (ButtonState == LOW){
            strain = 0;
        }
    }
    spin_me_baby(0);
}

void do_test(int test_number) {
    // Test 1 : Tensile test with turning motor
    // Test 2 : Measure with sensors without turning the motor
    // Test 3 : Measure distance and time to compute the speed

    if (test_number==1){
        // measure a point before starting the motor
        force = loadcell.get_units();
        strain = read_strain(sample_len);
        double_print(strain, force);
        spin_me_baby(1);
        // test stops before hitting the top of the machine
        while (strain < 1.4 && force <70 && button()==HIGH) {
            force = loadcell.get_units();
            strain = read_strain(sample_len);
            double_print(strain, force);
        }
        come_down();
        spin_me_baby(0);
    }

    if (test_number==2){
        // user interrupt by applying a 100N force
        while (force < 100){
            force = loadcell.get_units();
            strain = read_strain(sample_len);
            double_print(strain, force);
        }
    }

    if (test_number==3){
        spin_me_baby(1);
        // Stop before hitting the top
        while (strain<1.4){
            strain = read_strain(sample_len);
            distance = read_distance();
            double time = millis();
            double_print(distance, time);
            delay(100);
        }
    }
}

double initial_length(double n) {
    //// Compute the initial length with n measurements
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

    // Measure the initial length (maybe do in Loop)
    Serial.println("Calculating initial length");
    sample_len = initial_length(100.0);    // measure the length of the sample
    Serial.println("Done !");

    // Loadcell
    loadcell.begin(dataPin, clockPin);
    loadcell.set_scale(40510/9.12437);  // F = 9.1243791 N
    loadcell.tare();

    // Button
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Delay to plug stuff
    Serial.println("Setup done test will start in 1 sec");
    delay(1000);
    Serial.println("Lets go !");
}


void loop() {

    ButtonState = button();
    // If the button is pressed
    if (ButtonState == LOW) {

        // Start going up
        delay(200);
        spin_me_baby(1);
        Serial.println("Test started");

        // Go up while the button is not pushed and the strain is below the top of the machine
        while (button() == HIGH && strain < 1.4) {
            strain = read_strain(sample_len);
            force = loadcell.get_units();
            double_print(strain, force);
        }

        // When the top is reached or the button is pushed the machine stops
        spin_me_baby(0);
        Serial.println("Test Stopped");
        delay(100);
        // do nothing while the button is not pushed
        while(button()== HIGH){
            delay(100);
        }

        // when the button is pushed we go back down
        Serial.println("Coming down");
        strain = read_strain(sample_len);
        delay(100);
        spin_me_baby(2);
        while(button()==HIGH && strain>0){
            strain = read_strain(sample_len);
            Serial.println(strain);
        }
        spin_me_baby(0);
        if(strain <= 0){
            Serial.println("Went back down");
        }
        else{
            Serial.println("User interrupt");
        }

        delay(100);

    }


}






