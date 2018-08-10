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

//Written by: Stefanson Nelson
task main()
{
	setupSensors();
	setupArray();

	displayString (5,"You are going down kid >:D");
	displayString (6,"Press any button to start");

	getButton();
	moveBoardCalibrate(40);

	clearTimer(T1);
	bool winningMoveRobot = false, winningMovePlayer = false;
	int columnNum = 0;

	while (!winningMoveRobot && !winningMovePlayer)
	{
		eraseDisplay();
		displayString (5,"Robot's Turn");

		//Call function to decide what move to play in, function will return -1
		//if all columns are full. If all columns are full exit loop.
		columnNum = decideMove();
		if (columnNum == -1)
			break;

		dropPiece(columnNum);
		//Check if robot has won
		winningMoveRobot = checkWin(ROBOT_NUM);

		//If the robot has not won, let the player play their move.
		if (!winningMoveRobot)
		{
			//Move the board away from the colour sensor and the dropping
			//mechanism so they have room to play.
			moveMotor(motorC, -1 * BOARD_MOTOR_POWER, DIST_PLAYER, RADIUS_BOARD_WHEEL);
			eraseDisplay();
			displayString(5, "Player, your turn!");
			displayString(6, "Press enter after playing");

			//Check to make sure the player has played
			checkPlay();
			//Check if the player has won
			winningMovePlayer = checkWin(PLAYER_NUM);
		}
	}

	eraseDisplay();
	//Move the board away from the colour sensor and the dropping mechanism
	//so they can emtpy the board because the game has finished.
	moveMotor(motorC, -1 * BOARD_MOTOR_POWER, DIST_PLAYER, RADIUS_BOARD_WHEEL);
	int game_time=0;
	game_time = time1(T1);

	//If the loop wa exited because the board was full.
	if (columnNum == -1)
		displayString(5, "Tie Game!");
	else if (winningMovePlayer)
		displayString (5,"Player Wins!");
	else
		displayString (5,"Robot Wins!");

	//Display the gametime in minutes
	displayString(7,"Gametime: %f min", game_time/1000.0/60);
	displayString(8, "Press any button to exit");

	getButton();
	eraseDisplay();
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

