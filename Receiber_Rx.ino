/**
 * By Ramiro Gutierrez Alaniz
 * Date  October 31st, 2016
 * Router Master receiver arduino code
 * Note : xbee code was obtained by the example from the xbee library documentation
 */

// include the library
#include <XBee.h>

// Init all the xbee use variables
XBee xbee = XBee();

// Xbee response from xbee library
XBeeResponse response = XBeeResponse();

// create reusable response objects for responses we expect to handle 
Rx16Response rx16 = Rx16Response();

// signal outputs
const int s0 = 9;
const int s1 = 10;
const int s2 = 5;
const int s3 = 6;

// state leds
const int SUCCESS_LED = 2;
const int ERROR_LED = 3;

// Voltage control
const int MIN_AO = 0; // Minimum analog output
const int MAX_AO = 255; // Maximum analog output

// Motor variables 
typedef enum  { 
      LEFT_MOTOR,
      RIGHT_MOTOR
   } motor_type;
  
// instruction variable; will get the instruction and proceed to get verify
int instruction = 0;

// Directions 
enum { 
    FRONT = 5, 
    BACK = 10, 
    RIGHT_FRONT = 4, 
    LEFT_FRONT = 1, 
    LEFT_BACK = 2, 
    RIGHT_BACK = 8, 
    STOP = 0
  } direction_type;
  
/*
 * Setup function
 */
void setup() {
  // start serial
  Serial.begin(9600);
  // state led init
  pinMode( SUCCESS_LED, OUTPUT );
  pinMode( ERROR_LED, OUTPUT );
  // Signal init
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  // set xbee serial
  xbee.setSerial(Serial);
}// End of setup function

/*
 * Loop function
 * It will countinously look for packages
 */
void loop() {
    // Start the packe reading
    xbee.readPacket();
    // Verify if it is available on the network
    if (xbee.getResponse().isAvailable()) {
      // Check if there ir a tx package
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE) {
        // Success status
        set_state( 1 );
        if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
          xbee.getResponse().getRx16Response(rx16);
          instruction = rx16.getData( 0 );
        }
        verify_motor_instruction( instruction );
      } else {
      	// set error status
        set_state( 0 );
      }
    } else if (xbee.getResponse().isError()) {
      // Set error status
      set_state( 0 );
    }
}// End of loop function

/*
 * set state
 * set the led state tu success if true else error
 */
void set_state( int state ) {
  if( state ) {
    digitalWrite( SUCCESS_LED, HIGH );
    digitalWrite( ERROR_LED, LOW );
  } else {
    digitalWrite( SUCCESS_LED, LOW );
    digitalWrite( ERROR_LED, HIGH );
  }
} // End of set state function
/*
 * This will give the instructions of the motor instructions
 */
void verify_motor_instruction( int ins ) {
  switch( ins ) {
    case STOP : 
      // stop motor right
      break_motor( RIGHT_MOTOR );
      // stop motor left  
      break_motor( LEFT_MOTOR );
      break;
    case FRONT :
      // Turn front all motors
      right_control( 1 );
      left_control( 1 );
      break;
    case BACK :
      // Turns back all motors
      right_control( 0 );
      left_control( 0 );
      break;
    case RIGHT_FRONT :
      // stops right motor
      break_motor( RIGHT_MOTOR );
      // turns front left motor
      left_control( 1 );
      break;
    case LEFT_FRONT :
      // stops left motor
      break_motor( LEFT_MOTOR );
      // turns front right motor
      right_control( 1 );
      break;
    case RIGHT_BACK :
      // Stops right motor
      break_motor( RIGHT_MOTOR );
      // turns back left motor
      left_control( 0 );
      break;
    case LEFT_BACK :
      // Stops left motor
      break_motor( LEFT_MOTOR );
      // turns back right motor
      right_control( 0 );
      break;
  } // End of switch statement
} // End of verify_motor_instruction function

/*
 *  right control
 * This function controls the right motor
 * if control is true then turns front else turns back
 */
void right_control( int control ) {
  if ( control ) { // Turns front motor
    analogWrite( s1, MIN_AO );
    analogWrite( s0, MAX_AO );  
  } else { // Turns back the motor
    analogWrite( s0, MIN_AO );
    analogWrite( s1, MAX_AO );
  }
} // End of rigth_control function

/*
 * left control
 * this function controls the left motor
 * if control is true then turns front else turns back
 */
void left_control( int control ) {
  if ( control ) { // Turns front motor
    analogWrite( s3, MIN_AO );
    analogWrite( s2, MAX_AO );  
  } else { // Turns back motor
    analogWrite( s2, MIN_AO );  
    analogWrite( s3, MAX_AO );
  }
} // End of left control function

/*
 * break_motor
 * stops a motor
 * if motor true then stops motor right else stops motor left
 */
void break_motor( motor_type motor ) {
  if( motor == RIGHT_MOTOR ) { // Right motor
    analogWrite( s0, MIN_AO );
    analogWrite( s1, MIN_AO );  
  } else if( motor == LEFT_MOTOR ){ // Left motor
    analogWrite( s2, MIN_AO );
    analogWrite( s3, MIN_AO );
  }
}// End of break_motor function
