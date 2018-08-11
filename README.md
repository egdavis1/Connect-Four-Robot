# Connect-Four-Robot

This code plays a game of connect four against a user using sensors and motors from a Lego EV3 Robot as well as robotC code.
This was a group project done by Emma Davis, Stefanson Nelson, Alexandar Stenberg, and Edward Wang.

The board was put on wheels powered by a motor so the board could move to the correct position to line up with the dropping mechanism and the colour sensor.  The dropping mechanism used a gear to move 3D printed parts to let one Connect Four piece into a slot.  Then the motor rotates the gear the opposite way to block another piece from going into the slot while letting the piece in the slot drop into the connect four board guided by laser cut parts.  A third motor was used to move the colour sensor up and down to sense different rows of the Connect Four board.  A touch sensor was used to calibrate the board after it moved to ensure there was minimal error in the motorencoder for the duration of the game.

The code includes test code for testing all of functions of the robot to ensure the robot is working correctly before a game of Connect Four can be played.
