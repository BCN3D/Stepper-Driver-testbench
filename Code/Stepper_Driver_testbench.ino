/*-------------------------------------------------
Stepper Driver test bench

BCN3D Technologies - Fundacio CIM
Marc Cobler - July 2015

This sketch is used in the Stepper driver test bench
It consist in 2 capacitive touch buttons that activates
different routines. 

The purpose is to callibrate the stepper drivers and test
them out with a turning motor.

---------------------------------------------------*/
#include <CapacitiveSensor.h>
#include <AccelStepper.h>
//--------CONSTANTS------------------
#define SENSORPOWER 5
#define SENSORMOTOR 2
#define SENDPIN 4

#define STEP 8
#define DIR 9
#define EN 10

#define POWER 7

#define THRESHOLD 3800	//Threshold for the capacitive Sensor pins
#define MAXSPEED 1000

//--------VARIABLES-----------------
unsigned long csSum1, csSum2;
bool powerState;

CapacitiveSensor motorTest = CapacitiveSensor(SENDPIN, SENSORMOTOR);	//
CapacitiveSensor powerOn = CapacitiveSensor(SENDPIN, SENSORPOWER);		//

AccelStepper stepper(AccelStepper::DRIVER, STEP, DIR);
void setup()
{
	powerState = false;		//24V turned off at beginning
	pinMode(STEP, OUTPUT);
	pinMode(DIR, OUTPUT);
	pinMode(EN, OUTPUT);
	
	Serial.begin(115200);
	Serial.println("Starting the Stepper Driver test bench...");

	stepper.setMaxSpeed(MAXSPEED);
	stepper.setEnablePin(EN);
	stepper.disableOutputs();
	stepper.moveTo(500);
}

void loop()
{
	CSMotor();
	CSPower();

}
//----------USER FUNCTIONS-----------------
void CSMotor() {
	long cs = motorTest.capacitiveSensor(80); //a: Sensor resolution is set to 80
	if (cs > 100) { //b: Arbitrary number
		csSum1 += cs;
		Serial.println(cs);
		if (csSum1 >= THRESHOLD) //c: This value is the threshold, a High value means it takes longer to trigger
		{
			moveMotor();
			Serial.print("Trigger Motor Test Button: ");
			Serial.println(csSum1);
			if (csSum1 > 0) { csSum1 = 0; } //Reset
			motorTest.reset_CS_AutoCal(); //Stops readings
		}
		} else {
		csSum1 = 0; //Timeout caused by bad readings
	}
	
}
void CSPower() {
	long cs = powerOn.capacitiveSensor(80); //a: Sensor resolution is set to 80
	if (cs > 100) { //b: Arbitrary number
		csSum2 += cs;
		Serial.println(cs);
		if (csSum2 >= THRESHOLD) //c: This value is the threshold, a High value means it takes longer to trigger
		{
			conmute24V();
			Serial.print("Trigger Power Button: ");
			Serial.println(csSum2);
			if (csSum2 > 0) { csSum2 = 0; } //Reset
			powerOn.reset_CS_AutoCal(); //Stops readings
		}
		} else {
		csSum2 = 0; //Timeout caused by bad readings
	}
	
}
void moveMotor() {
	if (stepper.distanceToGo() == 0)
		stepper.moveTo(-stepper.currentPosition());
		
	stepper.run();
}

void conmute24V() {
	if (powerState)
	{
		Serial.println("24V ON");
		digitalWrite(POWER, HIGH);
	} else {
		Serial.println("24V OFF");
		digitalWrite(POWER, LOW);
	}
	
	powerState = !powerState; 
}