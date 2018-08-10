const int ROW = 6;
const int COLUMN = 7;
const int ROBOT_NUM = 1;
const int PLAYER_NUM = 2;
const float RADIUS_BOARD_WHEEL = 1.525;
const float RADIUS_DROP_WHEEL = 2.5;
const float RADIUS_SENSOR_WHEEL = 2.45 / 2;
const TLegoColors PLAYER_COLOUR = colorYellow;
const float DIST_TO_DROP = 14.75;
const float DIST_BETWEEN_COLUMNS = 3.5;
const float DIST_BETWEEN_ROWS = 2.65;
const float DIST_PLAYER = (COLUMN + 1.4) * DIST_BETWEEN_COLUMNS;
const int BOARD_MOTOR_POWER = 15;
const int SENSOR_MOTOR_POWER = 10;
const int DROP_MOTOR_POWER = 8;
int connect4Board[ROW][COLUMN];

void setupSensors();
void setupArray();
float distToRotation(float dist, float radius);
void addMove(int row, int column, int player);
void moveMotor(tMotor motor_num, int motorPower, float dist, float radius);
void moveBoardCalibrate(int motorPower);
TEV3Buttons getButton();
bool senseMove();
int decideMove();
void dropPiece(int column);
void checkPlay();
bool checkWin(int player);

void testBoard(int &numFailed, int &numTotal);
void testDrop(int &numFailed, int &numTotal);
void testSensorValue(int &numFailed, int &numTotal);
void testSensor(int &numFailed, int &numTotal);
void testCheckPlay(int &numFailed, int &numTotal);

task main()
{
	setupSensors();
	setupArray();
	int numTotal = 0, numFailed = 0;

	displayString(5, "Press enter to start tests");
	while (getButton() != buttonEnter)
	{}

	testBoard(numFailed, numTotal);
	displayString(10, "Press enter to continue");
	while (getButton() != buttonEnter)
	{}

	testDrop(numFailed, numTotal);
	displayString(10, "Press enter to continue");
	while (getButton() != buttonEnter)
	{}

	testSensorValue(numFailed, numTotal);
	displayString(10, "Press enter to continue");
	while (getButton() != buttonEnter)
	{}

	testSensor(numFailed, numTotal);
	displayString(10, "Press enter to continue");
	while (getButton() != buttonEnter)
	{}

	testCheckPlay(numFailed, numTotal);
	displayString(10, "Press enter to continue");
	while (getButton() != buttonEnter)
	{}

	eraseDisplay();
	displayString(3, "Total number of tests: %d", numTotal);
	displayString(4, "Number tests failed: %d", numFailed);
	displayString(5, "Press any button to exit");
	getButton();
}

void testBoard(int &numFailed, int &numTotal)
{
	TEV3Buttons button = buttonNone;

	for (int x = 1; x < 4; x++)
	{
		eraseDisplay();
		displayString(3, "TEST BOARD MOVEMENT");
		displayString(4, "This should move the board to ");
		displayString(5, "the %d column", x);
		moveMotor(motorC, -1 * BOARD_MOTOR_POWER, x * DIST_BETWEEN_COLUMNS, RADIUS_BOARD_WHEEL);
		wait1Msec(1000);
		moveBoardCalibrate(40);

		displayString(6, "Press enter if test passed");
		displayString(7, "Press down if test failed");

		button = getButton();
		if (button == buttonDown)
			numFailed ++;
		numTotal ++;
	}
}

void testDrop(int &numFailed, int &numTotal)
{
	TEV3Buttons button = buttonNone;

	eraseDisplay();
	displayString(3, "TEST DROP PIECE");
	displayString(4, "This should drop a piece");
	moveMotor(motorD, -1 * DROP_MOTOR_POWER, DIST_TO_DROP, RADIUS_DROP_WHEEL);
	moveMotor(motorD, DROP_MOTOR_POWER, DIST_TO_DROP, RADIUS_DROP_WHEEL);
	displayString(5, "Press enter if test passed");
	displayString(6, "Press down if test failed");

	button = getButton();

	if (button == buttonDown)
		numFailed ++;
	numTotal ++;

	eraseDisplay();
	for (int x = 1; x < COLUMN; x++)
	{
		displayString(4, "This should drop the piece");
		displayString(5, "in the %d column", x);
		dropPiece(x);
		displayString(5, "Press enter if test passed");
		displayString(6, "Press button if test failed");

		button = getButton();

		if (button == buttonDown)
			numFailed ++;
		numTotal ++;
		eraseDisplay();
	}
	//Reset array because dropPiece function will add move to array.
	for (int x = 0; x < ROW; x++)
		for (int y = 0; y < COLUMN; y++)
			connect4Board[x][y] = 0;
}

void testSensorValue(int &numFailed, int &numTotal)
{
	moveMotor(motorC, -1 * BOARD_MOTOR_POWER, DIST_PLAYER, RADIUS_BOARD_WHEEL);
	eraseDisplay();
	displayString(3, "TEST SENSOR VALUE");
	displayString(4, "Put a yellow piece in");
	displayString(5, "each column");
	displayString(6, "Then press enter button");
	while (getButton() != buttonEnter)
	{}
	eraseDisplay();
	moveBoardCalibrate(40);

	for (int x = 0; x < COLUMN; x++)
	{
		moveMotor(motorC, -1 * BOARD_MOTOR_POWER, x * DIST_BETWEEN_COLUMNS, RADIUS_BOARD_WHEEL);
		wait1Msec(1000);
		if (SensorValue[S1] != PLAYER_COLOUR)
			numFailed++;
		numTotal++;
		wait1Msec(1000);
		if (x != 0)
		{
			moveBoardCalibrate(40);
		}
		wait1Msec(100);
	}

	moveMotor(motorC, -1 * BOARD_MOTOR_POWER, DIST_PLAYER, RADIUS_BOARD_WHEEL);
	displayString(4, "Fill up the 0 column");
	displayString(5, "with yellow pieces");
	displayString(6, "Then press enter button");
	while (getButton() != buttonEnter)
	{}
	moveBoardCalibrate(40);
	eraseDisplay();

	for (int x = 0; x < ROW; x++)
	{
		moveMotor(motorB, -1 * SENSOR_MOTOR_POWER, x * DIST_BETWEEN_ROWS, RADIUS_SENSOR_WHEEL);
		wait1Msec(1000);
		if (SensorValue[S1] != PLAYER_COLOUR)
			numFailed++;
		numTotal++;
		wait1Msec(100);
		moveMotor(motorB, SENSOR_MOTOR_POWER, x * DIST_BETWEEN_ROWS, RADIUS_SENSOR_WHEEL);
	}
	wait1Msec(1000);
	moveMotor(motorC, -1 * BOARD_MOTOR_POWER, DIST_PLAYER, RADIUS_BOARD_WHEEL);
	eraseDisplay();
	displayString(4, "Sensor Value tests completed");
	displayString(5, "Empty connect 4 board");
	displayString(6, "Then press enter");
	while (getButton() != buttonEnter)
	{}

	moveBoardCalibrate(40);
}

void testSensor(int &numFailed, int &numTotal)
{
	bool move = false;
	displayString(3, "TEST SENSOR");

	for (int x = 0; x < 3; x++)
	{
		eraseDisplay();
		moveMotor(motorC, -1 * BOARD_MOTOR_POWER, DIST_PLAYER, RADIUS_BOARD_WHEEL);
		displayString(4, "Place a yellow piece in");
		displayString(5, "column %d", x);
		displayString(6, "then press enter");
		displayString(7, "Column numbers start at 0...");
		while (getButton() != buttonEnter)
		{}
		moveBoardCalibrate(40);

		move = senseMove();

		if (!move)
		{
			numFailed ++;
			displayString(8, "Test failed, not sensed");
		}
		else if (connect4Board[0][x] != PLAYER_NUM)
		{
			numFailed++;
			displayString(8, "Test failed, sensed wrong spot");
		}
		else
			displayString(8, "Test passed");

		numTotal ++;

		displayString(9, "Press any button to continue.");
		getButton();
	}
	//Reset array because senseMove function will add the move to the array.
	for (int x = 0; x < ROW; x++)
		for (int y = 0; y < COLUMN; y++)
			connect4Board[x][y] = 0;

	moveMotor(motorC, -1 * BOARD_MOTOR_POWER, DIST_PLAYER, RADIUS_BOARD_WHEEL);
	eraseDisplay();
	displayString(4, "Empty connect 4 board then");
	displayString(5, "press the enter button");
	while (getButton() != buttonEnter)
	{}
	moveBoardCalibrate(40);
}

void testCheckPlay(int &numFailed, int &numTotal)
{
	eraseDisplay();
	displayString(3, "TEST CHECK PLAY");
	displayString(4, "Do not play any piece");
	displayString(5, "Press enter when ready");
	while(getButton() != buttonEnter)
	{}

	bool move = senseMove();

	if (!move)
		displayString(5, "Move not sensed, test passed");
	else
	{
		displayString(5, "Move sensed, test failed");
		numFailed++;
	}
	numTotal++;

	moveMotor(motorC, -1 * BOARD_MOTOR_POWER, DIST_PLAYER, RADIUS_BOARD_WHEEL);
	eraseDisplay();
	displayString(4, "Play any piece anywhere");
	displayString(5, "Press enter when ready");
	while(getButton() != buttonEnter)
	{}

	moveBoardCalibrate(40);
	move = senseMove();

	if (move)
		displayString(5, "Move sensed, test passed");
	else
	{
		displayString(5, "Move not sensed, test failed");
		numFailed++;
	}
	numTotal++;
}

//Written by: Emma Davis
void setupSensors()
{
	SensorType[S1] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S1] = modeEV3Color_Color;
	wait1Msec(50);

	SensorType[S2] = sensorEV3_Touch;
}

//Written by: Emma Davis
//Declare all values in the array as 0, need to do it this way because of robotC
void setupArray()
{
	for (int x = 0; x < ROW; x++)
	{
		for (int y = 0; y < COLUMN; y++)
			connect4Board[x][y] = 0;
	}
}

//Written by: Emma Davis
float distToRotation(float dist, float radius)
{
	return dist * 180 / (PI * radius);
}

//Written by: Emma Davis
void addMove(int row, int column, int player)
{
	//Set value in array to a 1 or 2 for either the robot or player's move.
	connect4Board[row][column] = player;
}

//Written by: Emma Davis
//Move a given motor a given distance using the given radius of the wheel.
void moveMotor(tMotor motor_num, int motorPower, float dist, float radius)
{
	nMotorEncoder[motor_num] = 0;
	motor[motor_num] = motorPower;
	
	while (abs(nMotorEncoder[motor_num]) < distToRotation(dist, radius))
	{}
	motor[motor_num] = 0;
}

//Written by: Emma Davis
//Move the board until it hits the touch sensor to go back to the initial
//position of the board.
void moveBoardCalibrate(int motorPower)
{
	motor[motorC] = motorPower;

	while (SensorValue[S2] == 0)
	{}
	motor[motorC] = 0;
}

//Written by: Emma Davis
//Returns the button pressed
TEV3Buttons getButton()
{
	TEV3Buttons button = buttonNone;
	while (!getButtonPress(buttonAny))
	{}

	if (getButtonPress(buttonUp))
		button = buttonUp;
	else if (getButtonPress(buttonEnter))
		button = buttonEnter;
	else if (getButtonPress(buttonDown))
		button = buttonDown;
	else if (getButtonPress(buttonRight))
		button = buttonRight;
	else
		button = buttonLeft;

	while(getButtonPress(buttonAny))
	{}

	return button;
}

//Written by: Emma Davis
bool senseMove()
{
	//Assume player has only played one piece (player does not cheat).
	//Only check spots that are possible to be played in based on array.
	bool movePlayed = false;
	for (int x = 0; x < COLUMN; x++)
	{
		for (int y = 0; y < ROW; y++)
		{
			//If there is an empty spot that could be occupied by the new piece
			if (connect4Board[y][x] == 0)
			{
				//Check if piece was played in the empty spot by moving the
				//sensor and the board the required distance.
				moveMotor(motorC, -1 * BOARD_MOTOR_POWER, x * DIST_BETWEEN_COLUMNS, RADIUS_BOARD_WHEEL);
				moveMotor(motorB, -1 * SENSOR_MOTOR_POWER, y * DIST_BETWEEN_ROWS, RADIUS_SENSOR_WHEEL);
				
				//Wait one second for sensor to read colour correctly.
				wait1Msec(1000);
				bool exitLoop = false;
				//If the move is sensed add the move to the array and exit loops
				if (SensorValue[S1] == PLAYER_COLOUR)
				{
					addMove(y, x, PLAYER_NUM);

					exitLoop = true;
					movePlayed = true;
				}

				//Only calibrate the board if it is not already at the initial
				//position then move the sensor back to the initial position
				if (x != 0)
					moveBoardCalibrate(40);
				moveMotor(motorB, SENSOR_MOTOR_POWER, y * DIST_BETWEEN_ROWS, RADIUS_SENSOR_WHEEL);
				
				//Exit the loops
				if (exitLoop)
					x = COLUMN + 1;
				y = ROW + 1;
			}
		}
	}
	return movePlayed;
}

//Written by: Emma Davis
int decideMove()
{
	int move = 0, numFull = 0;
	bool columnFull[COLUMN];
	
	//Checks and stores the columns that are full.
	for (int x = 0; x < COLUMN; x++)
	{
		if (connect4Board[ROW-1][x] == 0)
			columnFull[x] = false;
		else
		{
			columnFull[x] = true;
			numFull ++;
		}
	}

	//If all of the columns are full the function will return -1.
	if (numFull == COLUMN)
		return -1;

	//Randomly generates the number of a column to play in.
	//If the column that is chosen is full, another number will be generated.
	move = random(COLUMN-1);
	while (columnFull[move])
		move = random(COLUMN-1);

	return move;
}

//Written by Edward Wang
void dropPiece(int column){
	//Move the board to the right spot for the piece to be dropped
	moveMotor(motorC, -1 * BOARD_MOTOR_POWER, column * DIST_BETWEEN_COLUMNS, RADIUS_BOARD_WHEEL);

	//Turn gear forward and backwards same amount to drop the piece.
	moveMotor(motorD, -1 * DROP_MOTOR_POWER, DIST_TO_DROP, RADIUS_DROP_WHEEL);
	moveMotor(motorD, DROP_MOTOR_POWER, DIST_TO_DROP, RADIUS_DROP_WHEEL);

	//Figure out the row that piece was played in and add the move to the array.
	int row = 0;
	for (int x = 0; x < ROW; x++)
	{
		if (connect4Board[x][column] == 0)
		{
			row = x;
			break;
		}
	}
	addMove(row, column, ROBOT_NUM);

	//Move the board back to the initial poition if it is not already there.
	if (column != 0)
		moveBoardCalibrate(40);
}

//Written by: Alex Stenberg
void checkPlay()
{
	//Wait for player to press the enter button then move the board back to
	//the inital position
	while(getButton() != buttonEnter)
	{}
	moveBoardCalibrate(40);

	bool played = senseMove();
	//If the players move was not sensed ask the player to play again
	while (!played)
	{
		eraseDisplay();
		if (!played)
		{
			//Move the board away from the colour sensor and the dropping
			//mechanism so player can play a piece
			moveMotor(motorC, -1 * BOARD_MOTOR_POWER, column * DIST_BETWEEN_COLUMNS, RADIUS_BOARD_WHEEL);
			displayString(4, "Your move was not detected");
			displayString(5, "Please play, then press");
			displaystring(6, "the enter button");
		}

		while(getButton() != buttonEnter)
		{}
		played = senseMove();
	}
	eraseDisplay();
}

//Written by: Alex Stenberg
//Returns true if given player has won, returns false if player has not won.
bool checkWin(int player)
{
	for (int i = 0; i<COLUMN; i++)
	{
		for (int j = 0; j < ROW; j++)
		{
			if (j<3)
			{
				//For vertical case
				if (connect4Board[j][i] == player && connect4Board[j + 1][i] == player && connect4Board[j + 2][i] == player && connect4Board[j + 3][i] == player)
					return true;
				if (i<4)
				{
					//For diagonal with positive slope case
					if (connect4Board[j][i]==player && connect4Board[j+1][i+1] == player && connect4Board[j+2][i+2] == player && connect4Board[j+3][i+3] == player)
						return true;
				}
			}

			if (i < 4)
			{
				//For horizontal case
				if (connect4Board[j][i] == player && connect4Board[j][i+1] == player && connect4Board[j][i+2] == player && connect4Board[j][i+3] == player)
					return true;
				if (j >= 3)
				{
					//Needed because robotC compiler will not accecpt index's
					//with numbers being subtracted
					int indexj1 = j-1, indexj2 = j-2, indexj3 = j-3;
					//For diagonal with negative slope case
					if (connect4Board[j][i] == player && connect4Board[indexj1][i+1] == player && connect4Board[indexj2][i+2] == player && connect4Board[indexj3][i+3] == player)
						return true;
				}
			}
		}
	}
	return false;
}
