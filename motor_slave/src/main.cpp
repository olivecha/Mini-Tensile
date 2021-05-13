#include <Arduino.h>
#include <Wire.h>

/// Stepper motor variables
const int STEP_DIR_PIN = 2;             //Output pin for steper direction
const int STEP_ROT_PIN = 3;             //Output pin for steper rotation
const int LED = 13;
int nb_ropes = 4;                          //Number of ropes used in the pulley setup
long lin_speed = 1000;                          //Linear test speed (mm/min)

/// I2C communication
int motor_instruction = 0;

/// Functions
void stepper_rotate() {
    //long step_delay = stepper_delay(nb_ropes, lin_speed); //Time to wait between rotations
    digitalWrite(STEP_ROT_PIN, LOW);
    digitalWrite(STEP_ROT_PIN, HIGH);
    delayMicroseconds(2000);
}
long stepper_delay(int ropes, long speed) {
    // Computes the motor delay between steps
    double step_angle = 1.8;      //degrees per step
    double motor_pulley_dia = 20; //diameter of the motor pulley (mm)
    double pi = 3.1415926535;     //pi approximation
    double RPM_motor = speed * ropes / (pi * motor_pulley_dia);
    long step_minutes = RPM_motor * (360 / step_angle);
    return step_minutes / (60 * 1000000);  //Delay in microseconds between steps
}

void receive_instructions(int bytes) {
    motor_instruction = Wire.read();    // read one character from the I2C
}

void setup() {

    /// Motor variables initialisation
    pinMode(STEP_DIR_PIN, OUTPUT);
    pinMode(STEP_ROT_PIN, OUTPUT);

    /// I2C protocol
    // Start the I2C Bus as Slave on address 9
    Wire.begin(9);
    // Attach a function to trigger when something is received.
    Wire.onReceive(receive_instructions);

}

void loop() {
    // If value received is 0 rotate the motor
    if (motor_instruction == 1) {
        stepper_rotate();
    }
    //If value received is 3 stop the motor
    if (motor_instruction == 0) {
        delayMicroseconds(1000);
    }

}