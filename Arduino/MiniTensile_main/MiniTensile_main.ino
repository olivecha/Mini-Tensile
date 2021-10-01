#include <HX711.h>
#include <Wire.h>

/*
 * Mini-Tensile
 * Mini tensile load testing machine
 * Version 0.0.1
 * École de Technologie Supérieure
 * Chabot Olivier, Toupin-Guay Valérie, Lengaigne Jacques, Tabas Ilyass, 
 * 2021
 * Libraries :
 * HX711 by bogde: https://github.com/bogde/HX711/tree/master/
 * Wiring is done according the the schematics in the file Electronics/Schematics
 */

/**********************************************************************************
 ****************************** Variable definitions ******************************
 **********************************************************************************/

// Load Cell Variables
HX711 loadcell;
uint8_t dataPin = 6;
uint8_t clockPin = 7;        //Input pin for the load cell ADC
double force;

// Ultrasonic distance sensor variables
long duration;                          // variable for the duration of sound travel
double distance;                                 // variable for the distance measurement
double sample_len;                        // Length measured for unstrained sample
double strain;                            // Strain mesured by the load cell
const int HCSR04_ECHO_PIN = 4;          //Input pin to echo of HC-SR04
const int HCSRO4_TRIG_PIN = 5;          //Output pin to trig of HC-SR04

// Toggle button variables
const int BUTTON_PIN = 8;
int ButtonState;    // the previous state of button

// Other test variables
double t; // for time

/***********************************************************************************
 ************************************** Functions **********************************
 ***********************************************************************************/

/*
 * Function: button
 * ----------------------------
 *   returns: the value of the button pin 
 *   LOW : if the button is pressend
 *   HIGH : if the button is not pressend
 */
int button(){
    return digitalRead(BUTTON_PIN);
}

/*
 * Function: read_strain
 * ----------------------------
 *   Returns the strain value according to the inital length and the distance sensor
 *
 *   len: initial length
 *
 *   returns: the measured strain 
 */
double read_strain(double len) {
    // Clears the trigPin condition
    digitalWrite(HCSRO4_TRIG_PIN, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(HCSRO4_TRIG_PIN, HIGH);
    delayMicroseconds(100);
    digitalWrite(HCSRO4_TRIG_PIN, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(HCSR04_ECHO_PIN, HIGH);
    // Calculating the distance
    distance = duration * 0.34 / 2; // Speed of sound wave divided by 2 (go and back)
    // return the strain defined as change in initial length
    return (distance - len)/len;
}

/*
 * Function: spin_me_baby
 * ----------------------------
 *   Sends an instruction to the Arduino board controling the stepper motor
 *
 *   state: desired state for the stepper motor
 *   1 : go up (slow)
 *   2 : go down (fast)
 *   3 : chill (do nothing)
 */
void spin_me_baby(int state) {
    Wire.beginTransmission(9); // transmit to device #9
    Wire.write(state);              // sends x
    Wire.endTransmission();    // stop transmitting
}

/*
 * Function: read_distance
 * ----------------------------
 *   Returns the mesured distance according to the distance sensor
 *
 *   returns: the measured distance (mm)
 */
double read_distance(){
    // Clears the trigger pin condition
    digitalWrite(HCSRO4_TRIG_PIN, LOW);
    delayMicroseconds(2);
    // Sets the trigger pin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(HCSRO4_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(HCSRO4_TRIG_PIN, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(HCSR04_ECHO_PIN, HIGH);
    return duration * 0.34 / 2; // Speed of sound wave divided by 2 (go and back)
}


/*
 * Function: double_print
 * ----------------------------
 *   Prints two values separeted by a space in the serial terminal
 */
void double_print(double val1, double val2) {
    Serial.print(val1);
    Serial.print(" ");
    Serial.print(val2);
    Serial.print("\n");
}

/*
 * Function: come_down
 * ----------------------------
 *    Sends instructions to the Arduino board to lower the carrier until it reaches
 *    its initial position.
 */
void come_down(){
    // Go down
    spin_me_baby(2);
    // While the strain is greater than 0.1 (deformation exists)
    while (strain > 0.1){
        strain = read_strain(sample_len);
        ButtonState = digitalRead(BUTTON_PIN);
        // Button interrupt
        if (ButtonState == LOW){
            strain = 0;
        }
    }
    spin_me_baby(0);
}

/*
 * Function: initial_length
 * ----------------------------
 *   Computes the initial length of the distance sensor with n measurements
 *
 *   n: number of measurements to use in calculating the average initial length
 *   
 *   return: the initial length (mm)
 */
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

/***********************************************************************************
 ************************************** Setup **************************************
 **********************************************************************************/

void setup() {

    // Begin serial communication
    Serial.begin(115200);

    // Begin IC2 communication
    Wire.begin();

    // Set motor to not spinning
    spin_me_baby(0);

    // Setup the ultrasonic distance sensor
    pinMode(HCSRO4_TRIG_PIN, OUTPUT); // Sets the trigPin as an OUTPUT
    pinMode(HCSR04_ECHO_PIN, INPUT);  // Sets the echoPin as an INPUT

    // Setup the loadcell
    loadcell.begin(dataPin, clockPin);
    loadcell.set_scale(40510/9.12437);  // Calibration was made with F = 9.1243791 N
    loadcell.tare();

    // Setup the push button
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Tell the user the setup is done
    Serial.println("Setup done");
}

/***********************************************************************************
 *************************************** Loop **************************************
 **********************************************************************************/

void loop() {

    // Measure the initial length Everytime a loop is started
    Serial.println("Calculating initial length...");
    sample_len = initial_length(100.0);    // measure the length of the sample with 100 reads
    Serial.println("Done !");
    Serial.println("Press button to begin testing");

    int test = 1;
    while (test == 1) {
        // Read the button state
        ButtonState = button();
        
        // If the button is pressed
        if (ButtonState == LOW) {
            // Start going up
            delay(200);
            Serial.println("Test started");
            // Go up while the button is not pushed and the strain is below the top of the machine
            spin_me_baby(1);
            while (button() == HIGH && strain < 550) {
                // Read distance, force and time
                distance = read_distance();
                strain = read_strain(sample_len);
                force = loadcell.get_units();
                t = millis();

                /**********************************************************
                 ** Change this line to choose the output of the machine **
                 **********************************************************/
                double_print(distance, force); // Example : double_print(distance, time) 
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
            delay(500);
            spin_me_baby(2);
            while(button()==HIGH && strain>0){
                strain = read_strain(sample_len);
                Serial.println(strain);
                delay(100);
            }
            spin_me_baby(0);
            if(strain <= 0){
                Serial.println("Went back down");
                test = 0;
            }
            else{
                Serial.println("User interrupt");
                test = 0;
            }

            delay(100);
        } // end if ButtonState
    } // end while test
} // end loop
