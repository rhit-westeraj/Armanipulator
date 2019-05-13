/*
 * Controller.cpp
 *
 *  Created on: May 11, 2019
 *      Author: FaceF
 */

//Include Me pls
#include "Controller.h"
#include "cpu_map.h"

Controller::Arm_Command* currentCmd;
	BasicStepperDriver* rotateDriver;
	BasicStepperDriver* grabDriver;
	BasicStepperDriver* extendDriver;
//Bob the builder
Controller::Controller() {
	//Init the motors
	//Declare Motor Pinouts
		rotateDriver = new BasicStepperDriver(MotorConfig::MOTOR_STEPS, Pinout::WRIST_ROT_DIR, Pinout::WRIST_ROT_STEP);
		grabDriver = new BasicStepperDriver(MotorConfig::MOTOR_STEPS, Pinout::GRIP_DIR, Pinout::GRIP_STEP);
		extendDriver =  new BasicStepperDriver(MotorConfig::MOTOR_STEPS, Pinout::EXTEND_DIR, Pinout::EXTEND_STEP);

		//Start the motors
		rotateDriver->begin(MotorConfig::MOTOR_RPM, MotorConfig::MICROSTEPS);
		grabDriver->begin(MotorConfig::MOTOR_RPM, MotorConfig::MICROSTEPS);
		extendDriver->begin(MotorConfig::MOTOR_RPM, MotorConfig::MICROSTEPS);

	//Init the struct as an error struct (If executeCMD is called before parseSerial, error is returned intentionally)

}

//DESTROY THE CHILD. CORRUPT THEM ALL
Controller::~Controller() {

}

//Getters and Setters
//Arm_Command
Controller::Arm_Command* Controller::getCommand(){
	return currentCmd;
}

void Controller::setCommand(Controller::Arm_Command* cmd){
	currentCmd = cmd;
}


void Controller::parseSerial(String rawinput) {
	Arm_Command* out = new Arm_Command();

	//This is what we expect in terms of input
	//Any commands related to rotation will start with an 'r' or 'R', then followed by a double indicating revolutions
	//Any commands related to gripping will start with a 'g' or 'G', then followed by a 0 or 1. Other values will trigger an exception
	//Any commands related to extending the arm will start with an 'e' or 'E', then followed by a double indicating motor rotation. We will assert that the value is between -1 and 1 for now
	//Any unrecognized letter will return the ERROR enum and 1 as the value. Serial should display an error message, or trigger an exception
	if (rawinput.charAt(0) == 'r' || rawinput.charAt(0) == 'R') {
		out->op = Controller::Arm_Operation::ROTATE;
		//Extract Value from command string
		out->value = atof(rawinput.substring(1, rawinput.length()).c_str());
	} else if (rawinput.charAt(0) == 'g' || rawinput.charAt(0) == 'G') {
		out->op = Controller::Arm_Operation::GRAB;
		//Extract Value from command string
		out->value = atof(rawinput.substring(1, rawinput.length()).c_str());
	} else if (rawinput.charAt(0) == 'e' || rawinput.charAt(0) == 'E') {
		out->op = Controller::Arm_Operation::EXTEND;
		//Extract Value from command string
		out->value = atof(rawinput.substring(1, rawinput.length()).c_str());
	} else if (rawinput.charAt(0) == 'm' || rawinput.charAt(0) == 'M') {
		out->op = Controller::Arm_Operation::MICROSTEPS;
		//Special Extract Value related to enum
		//Ev
	} else {

		out->op = Controller::Arm_Operation::ERROR;
		out->value = 1;
	}
	currentCmd = out;
}

void Controller::printExec() {
	Controller::Arm_Operation plsdo = this->getCommand()->op;
	switch (plsdo) {
	case Controller::Arm_Operation::ROTATE:
		Serial.println("Command: Rotate");
		Serial.println("Value: " + String(this->getCommand()->value));
		Serial.println("");
		break;
	case Controller::Arm_Operation::GRAB:
		Serial.println("Command: Grab");
		Serial.println("Value: " + String(this->getCommand()->value));
		Serial.println("");
		break;
	case Controller::Arm_Operation::EXTEND:
		Serial.println("Command: Extend");
		Serial.println("Value: " + String(this->getCommand()->value));
		Serial.println("");
		break;
	case Controller::Arm_Operation::MICROSTEPS:
		//Microsteps should be parsed diffenrently
		break;
	case Controller::Arm_Operation::ERROR:
		Serial.println("Command: Error");
		Serial.println("Value: " + String(this->getCommand()->value));
		Serial.println("");
		break;
	}
}

//Execute Order 66
void Controller::executeCmd(){
	Controller::Arm_Operation cmdop = this->getCommand()->op;
	//TODO: Check efficiency of using a switch statement instead of an if/else chain
	//See Jump Tables and low-level intricacies produced by AVR
	switch (cmdop) {
	case Controller::Arm_Operation::ROTATE:
		rotateDriver->move(currentCmd->value);
		break;
	case Controller::Arm_Operation::GRAB:
		grabDriver->move(currentCmd->value);				//Activate
		break;
	case Controller::Arm_Operation::EXTEND:
		extendDriver->move(currentCmd->value);				//Rotate the extender motor by value
		break;
	case Controller::Arm_Operation::MICROSTEPS:
		//Empty for now
		break;
	case Controller::Arm_Operation::ERROR:
		Serial.println("Error, Unrecognized Command");
		break;

	}
}
