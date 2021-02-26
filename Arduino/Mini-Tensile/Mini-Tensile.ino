#include <HX711.h>


/*########################################
  ##### Mini-Tensile
  ##### Mini tensile load testing machine
  ##### V0
  ##### École de Technologie Supérieure
  ##### Tabas Ilyass, Lengaigne Jacques, Toupin-Guay Valérie, Chabot Olivier
  ##### 26.02.2021
  ##### Libraries:
  ##### HX711 by aguegu: https://github.com/aguegu/ardulibs/tree/master/hx711
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
const int HCSR04_ECHO_PIN = 2;          //Input pin to echo of HC-SR04
const int HCSRO4_TRIG_PIN = 3;          //Output pin to trig of HC-SR04

////// Stepper motor variables
const int STEP_DIR_PIN = 6;             //Output pin for steper direction
const int STEP_ROT_PIN = 7;             //Output pin for steper rotation
int nb_ropes;                          //Number of ropes used in the pulley setup
long lin_speed;                          //Linear test speed (mm/min)  




void setup() {

  Serial.begin(115200);
  
  // Ultrasonic distance sensor
  pinMode(HCSRO4_TRIG_PIN, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(HCSR04_ECHO_PIN, INPUT);  // Sets the echoPin as an INPUT
  // ****** read the initial distance to compute the strain 

  // Steper motor
  pinMode(STEP_DIR_PIN, OUTPUT);     // Sets the direction pin as OUTPUT
  pinMode(STEP_ROT_PIN, OUTPUT);     // Sets the rotation pin as OUTPUT


  // Load cell
  loadcell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  // *************
  // Add process to create tension in the rope before tare
  // *************
  tareValue = loadcell.read_average(32);
  
}



void loop() {
  // Make the motor turn
  stepper_rotate();

  //Read the distance
  read_distance();

}


void stepper_rotate(){
  long step_delay = stepper_delay(nb_ropes, lin_speed); //Time to wait between rotations
  digitalWrite(STEP_ROT_PIN, LOW);
  digitalWrite(STEP_ROT_PIN, HIGH);
  delayMicroseconds(step_delay);
}

long stepper_delay(int nb_ropes,long lin_speed) {
  // Computes the motor delay between steps 
  long step_angle = 1.8;      //degrees per step
  long motor_pulley_dia = 20; //diameter of the motor pulley (mm)
  long pi = 3.1415926535;     //pi approximation
  long RPM_motor = lin_speed*nb_ropes / (pi*motor_pulley_dia);
  long step_minutes = RPM_motor*(360/step_angle);
  return step_minutes/(60*1000000);  //Delay in microseconds between steps
}

void read_distance() {
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
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" mm");
}


