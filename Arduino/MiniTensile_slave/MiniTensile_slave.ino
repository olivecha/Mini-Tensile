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

/// Stepper motor variables
const int STEP_DIR_PIN = 2;             //Output pin for steper direction
const int STEP_ROT_PIN = 3;             //Output pin for steper rotation

/// I2C communication
int motor_instruction = 0;

/***********************************************************************************
 ************************************** Functions **********************************
 ***********************************************************************************/

/*
 * Function: stepper_rotate
 * ----------------------------
 *   d: delay in microseconds between steps
 */
void stepper_rotate(int d) {
    digitalWrite(STEP_ROT_PIN, LOW);
    digitalWrite(STEP_ROT_PIN, HIGH);
    delayMicroseconds(d);
}

/*
 * Function: receive_instructions
 * ----------------------------
 *  Reads the motor instruction from the main Arduino board
 *  
 *  bytes: bytes to read according to I2C proctocoll
 */
void receive_instructions(int bytes) {
    motor_instruction = Wire.read();    // read one character from the I2C
}

/***********************************************************************************
 ************************************** Setup **************************************
 ***********************************************************************************/

void setup() {
    // Setup the motor pins
    pinMode(STEP_DIR_PIN, OUTPUT);
    pinMode(STEP_ROT_PIN, OUTPUT);

    // Setup the I2C protocol 
    // Start the I2C Bus as Slave on address 9)
    Wire.begin(9);
    // Attach a function to trigger when something is received.
    Wire.onReceive(receive_instructions);
}

/***********************************************************************************
 *************************************** Loop **************************************
 **********************************************************************************/

void loop() {

    //If value received is 0 stop the motor
    if (motor_instruction == 0) {
        delayMicroseconds(1000);
    }
    
    // If value received is 1 rotate the motor to go up
    if (motor_instruction == 1) {
        digitalWrite(STEP_DIR_PIN, HIGH);
        stepper_rotate(3000 * 28.87); // To have 100 micro-m / s
    }
    
    // If value received is 2 we go down
    if (motor_instruction == 2) {
        digitalWrite(STEP_DIR_PIN, LOW);
        stepper_rotate(100);
    }

}
