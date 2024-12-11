/* 
Group 9: Express-O
By:
Aidan Bilon
Aviral Gupta
Quinlan Kane
Ted Kim 

V16.0


*/
#include "mindsensors-motormux.h"

//Function to better UI so the user knows they didn't press the right button
void pressEnter ()
{
	while (!getButtonPress(buttonEnter))
	{
	  while (getButtonPress(buttonUp) || getButtonPress(buttonLeft) || getButtonPress(buttonRight) || getButtonPress(buttonDown))
	  {
	    displayString (14, "     Please Press the Enter");
	    displayString (15, "             Button");
	    wait1Msec(2000);
	    displayString (14, "                           ");
	    displayString (15, "                           ");
	  }
	}

  while(getButtonPress(buttonAny))
    {}

  eraseDisplay();
}


//Keep track of the sensors and configures all of the motormux and sensors
void configureMotorMux ()
{
  //MotorExtender
  SensorType[S1] = sensorI2CCustom;
  MSMMUXinit();
  wait1Msec(50);

  SensorType[S2] = sensorEV3_Ultrasonic;
  wait1Msec(50);
  SensorType[S3] = sensorEV3_Touch;
	wait1Msec(50);
}

/*
MotorA is drinkX
MotorB is drinkY
MotorC is drinkZ
MotorD is rack and pinion
mmotor_S1_1 is spinning blade
mmotor_S1_2 is valve openeing
*/


//In case of overflow all motors deplete to 0
void emergencyStop(float drinkDistance)
{
	const float BASEHEIGHT = 26.5;
	const float PERCENT = 0.90;
	bool enterButton = false;

	while(enterButton == 0)
	{
	  if (SensorValue[S2] <= (BASEHEIGHT - ((BASEHEIGHT - drinkDistance)*PERCENT)))
	  {
	  	float failureDistance = SensorValue[S2];
	    motor[motorA] = motor[motorB] = motor[motorC] = motor[motorD] = 0;
	    MSMotorStop(mmotor_S1_1);
	    MSMMotor(mmotor_S1_2, 100);
	    wait1Msec(500);
	    MSMotorStop(mmotor_S1_2);

	  	eraseDisplay();

	 	  displayBigTextLine (4, "Overflow Error");
	  	displayBigTextLine (8, "Turning Off");
	  	displayBigTextLine (10, "All Motors");
	  	wait1Msec(2000);
	  	eraseDisplay();

	  	for (int counter = 0; counter < 10; counter++)
	  	{

	    	displayBigTextLine (4, "            ");
	    	wait1Msec(350);
	    	displayBigTextLine (4, "   !!     !!   ");
	    	wait1Msec(350);
	  	}
      displayBigTextLine (4, "Failure info:");
      displayBigTextLine (7, "%.1f", failureDistance);
	  	displayBigTextLine(10, "%.1f", BASEHEIGHT - ((BASEHEIGHT - drinkDistance)*PERCENT));
	  	displayBigTextLine(13, "%.1f", drinkDistance);
	  	wait1Msec(10000);
	  	eraseDisplay();
  		displayBigTextLine (1, "Press enter");
  		displayBigTextLine (3, "to shutdown");
	  	enterButton = true;
  	}
	if (getButtonPress(buttonEnter))
	{
		enterButton = true;
	}

  }
}

//Function to press and help store the values attached to the UI of the drink percent screen
int percentIncreaseAndDecrease (int percentDrink)
{
  while (!getButtonPress(buttonEnter))
  {
	  if (getButtonPress(buttonUp) && percentDrink < 100)
	  {
      while (getButtonPress(buttonAny))
        {}
		    percentDrink += 10;
	  }
	  if (getButtonPress(buttonDown) && percentDrink > 0)
	  {
      while (getButtonPress(buttonAny))
        {}
	      percentDrink -= 10;
	  }
		displayBigTextLine (5, "       ");
		displayBigTextLine (7, "%d", percentDrink);

	  while(!getButtonPress(buttonAny))
	  	{}
	}
  return percentDrink;
}

//To measure the cup height and volume of the cup
float measureCupHeight(int motorPower)
{
  nMotorEncoder[motorD] = 0;
  motor[motorD] = motorPower;
  while (!SensorValue[S3] == 1)
 		{}

  float distanceFromTop = (abs(nMotorEncoder[motorD]) * (3.188/4.0) * (4.0/10.0) * (8.0/360.0));   // (3.188cm/4studs) * (4studs/10 teeth on rack) * 8 teeth on gear/360 degrees = 0.07066666666 cm per degree
  
  motor[motorD] = -motorPower;
  
  while (abs(nMotorEncoder[motorD]) > 0)
  	{}
  motor[motorD] = 0;

  return distanceFromTop;
}

//Animated brewing animation that dealt with moving steam while the drinks were mixing
void brewingAnimation()
{
  //Brewing Screen
  displayString (0, "          ((((");
  displayString (1, "         ((((");
  displayString (2, "          ))))");
  displayString (3, "       _ .---.");
  displayString (4, "      ( |`---'|");
  displayString (5, "       \\|     |");
  displayString (6, "       : .___,:");
  displayString (7, "        `-----'");
  displayString (9, " _  _  __   ___    __");
  displayString (10, "|_)|_)|_ | | | |\\|/__");
  displayString (11, "|_)| \|__|^|_|_| |\\_|");
  wait1Msec(300);
  displayString (0, "              ");
  displayString (1, "             ");
  displayString (2, "              ");
  displayString (0, "          ))))");
  displayString (1, "         ))))");
  displayString (2, "          ((((");
  wait1Msec(300);
}


void controlValve(int valveTime, bool openOrClose) // set openOrClose to 1 for opening and 0 for closing
{
  MSMMotor(mmotor_S1_2, 100-(200*openOrClose));
	wait1Msec(valveTime);
  MSMotorStop(mmotor_S1_2);
}

//Intorductory display function to keep the task main clean
void introDisplay()
{
	  displayString (0, "          ((   _____");
  displayString (1, "           ))  \\___/_");
  displayString (2, "          |~~| /~~~\\ \\");
  displayString (3, "         C|__| \\___/");
  displayString (4, "    __    _  _  __ __ __ _");
  displayString (5, "   |_ \\ /|_)|_)|_ (_ (_ / \\");
  displayString (6, "   |__/ \\|  | \\|____)__)\\_/");
  displayString (7, "        Press Start:");
  displayString (8, "------------------------------");
}

//Animated loading screen with adjustable time counts, only for UI and user experience
void loadingScreen(int loading)
{
	  for(int count = 0; count < loading; count++)
  {
    displayBigTextLine (5, "   Loading");
    wait1Msec(350);
    displayBigTextLine (5, "   Loading.");
    wait1Msec(350);
    displayBigTextLine (5, "   Loading..");
    wait1Msec(350);
    displayBigTextLine (5, "   Loading...");
    wait1Msec(350);
  }
  eraseDisplay();
}




task main ()
{
  configureMotorMux ();

  controlValve(2000, 0);

	introDisplay();

  pressEnter();

  //Loading screen
	loadingScreen(5);

  //Drink option screen
  displayBigTextLine (1, "Options:");
  displayBigTextLine (3, "- Drink x");
  displayBigTextLine (5, "- Drink y");
  displayBigTextLine (7, "- Drink z");
  displayString (9, "------------------------------");
  displayBigTextLine (11, "Press Enter");

  pressEnter();

  //Rack and pinion lever arm moving
  const int RACK_AND_PINION_POWER = 20;
  //Height of rack and pinion from base where cup is placed
  const int RACK_AND_PINION_HEIGHT = 26.5;
  //Assuming 6cm cup diameter and perfect cylinder to account for error
  const int CUP_RADIUS = 3;
  //In ml (distanceFromTop is in cm), V= pi * r^2 * h
  float distanceFromTop = measureCupHeight(RACK_AND_PINION_POWER);
  float cupVolume = PI*(pow(CUP_RADIUS,2))*(RACK_AND_PINION_HEIGHT - distanceFromTop)*0.8;

  wait1Msec(50);
  displayBigTextLine (2, "Cup volume is:");
  displayBigTextLine (4, "%f mL", cupVolume);
  wait1Msec(5000);


  displayBigTextLine (1, "Drink X");
  displayBigTextLine (3, "Percent:");

  int percentDrinkTempX = 0;
  int percentDrinkStoreX = 0;

  do
  {
  	percentDrinkStoreX = percentIncreaseAndDecrease (percentDrinkTempX);
  } while (percentDrinkStoreX < 0 || percentDrinkStoreX > 100);

  eraseDisplay();

  wait1Msec(500);

  //Drink Y percent
  displayBigTextLine (1, "Drink Y");
  displayBigTextLine (3, "Percent:");

  int percentDrinkTempY = 0;
  int percentDrinkStoreY = 0;
  int percentDrinkStoreZ = 0;

  while (percentDrinkStoreX + percentDrinkStoreY + percentDrinkStoreZ != 100)
  {

  	do{
  	percentDrinkStoreY = percentIncreaseAndDecrease (percentDrinkTempY);

  	} while(percentDrinkStoreY < 0 || percentDrinkStoreY > 100 || percentDrinkStoreY > 100 - percentDrinkStoreX);

	  percentDrinkStoreZ = 100 - (percentDrinkStoreY + percentDrinkStoreX);
	}

  eraseDisplay();

  const int COUNTS_PER_ML_PUMP1 = 49678/300; //Conversion factor as counts per ml. Values derived from calibration program
   const int COUNTS_PER_ML_PUMP2 = 44038/300;
  const int COUNTS_PER_ML_PUMP3 = 42726/300;

  const float Percent_To_Decimal = 0.01; //Converting percent to decimal percent
  float drinkXml = percentDrinkStoreX * Percent_To_Decimal * cupVolume;
  float drinkYml = percentDrinkStoreY * Percent_To_Decimal * cupVolume;
  float drinkZml = percentDrinkStoreZ * Percent_To_Decimal * cupVolume;

  nMotorEncoder[motorA] = 0;
	nMotorEncoder[motorB] = 0;
	nMotorEncoder[motorC] = 0;
	int countA = COUNTS_PER_ML_PUMP1 * drinkXml;
	int countB = COUNTS_PER_ML_PUMP2 * drinkYml;
	int countC = COUNTS_PER_ML_PUMP3 * drinkZml;
	int pwrA = 100;
	int pwrB = 100;
	int pwrC = 100;

	for(int i = 0; i < 3; i++){
		motor[motorA] = pwrA;
		motor[motorB] = pwrB;
		motor[motorC] = pwrC;

    brewingAnimation();
		//run until one of the motors are finished dispensing
		while (nMotorEncoder[motorA] <= countA && nMotorEncoder[motorB] <= countB && nMotorEncoder[motorC] <= countC)
		{}
		//check which motor is done dispensing and don't run it on the next cycle
		if(nMotorEncoder[motorA] >= countA){
			pwrA = 0;
			nMotorEncoder[motorA] = 0;
		}

		if(nMotorEncoder[motorB] >= countB){
			pwrB = 0 ;
			nMotorEncoder[motorB] = 0;
		}

		if(nMotorEncoder[motorC] >= countC){
			pwrC = 0 ;
			nMotorEncoder[motorC] = 0;
		}
	}

	motor[motorA] = motor[motorB] = motor[motorC] = 0;
	  //Turn on the spinning blade to start mixing the drink

  MSMMotor(mmotor_S1_1, -100);

  wait1Msec(5000);
  MSMotorStop(mmotor_S1_1);
	wait1Msec(250);
  controlValve(2000, 1);
  wait1Msec(250);
  MSMMotor(mmotor_S1_1, -50);


  //Closing screen
  eraseDisplay();
  displayString (0, " ___");
  displayString (1, "  | |_  _ __ |/     \\/ _  ");
  displayString (2, "  | | |(_|| ||\\     / (_)|_|");
  displayString (3, "______________________________");
  displayBigTextLine (4, "By:");
  displayBigTextLine (6, "Aidan Bilon");
  displayBigTextLine (8, "Aviral Gupta");
  displayBigTextLine (10, "Ted Kim");
  displayBigTextLine (12, "Quinlan Kane");
  wait1Msec(5000);

  eraseDisplay();
  displayBigTextLine (1, "Press enter");
  displayBigTextLine (3, "to shutdown");

	emergencyStop(distanceFromTop);

	while(!getButtonPress(buttonEnter))
	{}
  controlValve(2000, 0);

  eraseDisplay();

  //Stop Motor Extender
  MSMotorStop(mmotor_S1_1);
  MSMotorStop(mmotor_S1_2);

}

/*
Citations:
Front-end Coffee ASCII display: https://ascii.co.uk/art/coffee
Front-end Cup ASCII display: https://ascii.co.uk/art/cup
Front-end "BREWING" and "Thank you" ASCII word display (BigFig font):
https://ascii.co.uk/text
mindsensors-motormux.h function library: https://learn.uwaterloo.ca/d2l/le/content/870864/viewContent/4686513/View
common 1.h file: //https://learn.uwaterloo.ca/d2l/le/content/870864/viewContent/4686513/View
common-mmux.h file: //https://learn.uwaterloo.ca/d2l/le/content/870864/viewContent/4686513/View
mindsensors-motormux.h file: //https://learn.uwaterloo.ca/d2l/le/content/870864/viewContent/4686513/View
*/