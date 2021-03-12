#include <Arduino.h>
#include <HX711.h>

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


////// Load Cell Variables
HX711 loadcell;
const int LOADCELL_DOUT_PIN = 2;        //Output pin for the load cell ADC
const int LOADCELL_SCK_PIN = 3;         //Input pin for the load cell ADC
float gainValue = 1;                    //Calibration factor (To be determined)
float measuringIntervall = 2;           //Measuring interval when IDLE
float measuringIntervallTest = .5;      //Measuring interval during SLOW test
float measuringIntervallTestFast = .15; //Measuring interval during FAST test
long tareValue;                         //Variable to store the tare value

////// Ultrasonic distance sensor variables
long duration;                          // variable for the duration of sound travel
int distance;                           // variable for the distance measurement
long sample_len;                        // Length measured for unstrained sample
long strain;                            // Strain mesured by the load cell
const int HCSR04_ECHO_PIN = 2;          //Input pin to echo of HC-SR04
const int HCSRO4_TRIG_PIN = 3;          //Output pin to trig of HC-SR04

////// Stepper motor variables
const int STEP_DIR_PIN = 6;             //Output pin for steper direction
const int STEP_ROT_PIN = 7;             //Output pin for steper rotation
int nb_ropes;                          //Number of ropes used in the pulley setup
long lin_speed;                          //Linear test speed (mm/min)
long force;                              // Force measured by the load cell

////// Initiate the functions namespaces
long initial_length();
void stepper_rotate();


long read_strain1(long len);

void send_results(long force_r, long strain);

long stepper_delay(int ropes, long speed);

void setup() {

    Serial.begin(115200);

    // Ultrasonic distance sensor
    pinMode(HCSRO4_TRIG_PIN, OUTPUT); // Sets the trigPin as an OUTPUT
    pinMode(HCSR04_ECHO_PIN, INPUT);  // Sets the echoPin as an INPUT
    sample_len = initial_length();    // mesure the length of the sample

    // Steper motor
    pinMode(STEP_DIR_PIN, OUTPUT);     // Sets the direction pin as OUTPUT
    pinMode(STEP_ROT_PIN, OUTPUT);     // Sets the rotation pin as OUTPUT

    // Load cell
    loadcell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    loadcell.set_gain(128);  // Value to determine in future

    // *************
    // Add process to create tension in the rope before tare
    // *************
    tareValue = loadcell.read_average(32);
}

void loop() {
    // Make the motor turn
    stepper_rotate();

    //Read the distance
    strain = read_strain1(sample_len);

    //Read the force
    force  = read_force(tareValue);
    //Write a data point
    send_results(force, strain);
}

long read_force(long tare) {
    force = loadcell.read_average(10) - tare;
    return force;
}

void send_results(long f, long s){
    Serial.println(f);
    Serial.print(',');
    Serial.print(s);
    Serial.println();
}

long read_strain1(long len) {
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
    distance = duration * 0.0034 / 2; // Speed of sound wave divided by 2 (go and back)
    return distance/len;
}


void stepper_rotate(){
    long step_delay = stepper_delay(nb_ropes, lin_speed); //Time to wait between rotations
    digitalWrite(STEP_ROT_PIN, LOW);
    digitalWrite(STEP_ROT_PIN, HIGH);
    delayMicroseconds(step_delay);
}

long stepper_delay(int ropes, long speed) {
    // Computes the motor delay between steps
    double step_angle = 1.8;      //degrees per step
    double motor_pulley_dia = 20; //diameter of the motor pulley (mm)
    double pi = 3.1415926535;     //pi approximation
    double RPM_motor = speed*ropes / (pi*motor_pulley_dia);
    long step_minutes = RPM_motor*(360/step_angle);
    return step_minutes/(60*1000000);  //Delay in microseconds between steps
}

long initial_length() {
    // Clears the trigPin condition
    digitalWrite(HCSRO4_TRIG_PIN, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(HCSRO4_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(HCSRO4_TRIG_PIN, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(HCSR04_ECHO_PIN, HIGH);
    sample_len = duration * 0.0034 / 2; // Speed of sound wave divided by 2 (go and back)
    return sample_len;
}


