# SimplePuzzleGame

Simple game about combining Pieces in +4 clusters
Puyo like game, played alone no AI to compete with

***Game made using C++ and SDL to render and play sound***
Everything except the font and background music made by me.
Including sound effects and sprites :^)

The gameplay instructions are always on screen, but just in case:

(arrow keys) Left/Right: move Piece | Up: Hard Drop | Down: Slow Drop
			 Z: Rotate | P: Pause | M: Mute Music





# Code:
## main
	-initiates SDl, game and renderer
	-contains the main loop of game logic -> render game
		game.run()  ->  renderer.display(); 
	-framerate calculation

## Piece & PlayerPiece
	- all info and code about the pieces and the player controlled piece
	- mostly the same but player one has added code for having another piece and more animations

## RenderWindow
	-framerate synced to Vsync, I have a 144hz screen, 
		sometimes game runs at 144, sometimes at 60. couldn't figure out why, runs well either way
	- display() is called every frame
	- has huge functions because of switch cases and drawing things to the window
	- drawing specific sprites and text to screen uses many lines but logic is simple

## Game
	-where most of the code is
	-run() is called every frame,
		check for keypresses and updates internal gamelogic and positions
	-3 Big functions:
		-updatePlayerInput():	updates game depending on input

		-updateGame():			updates internal logic and positions, changes states, score etc
								lots of code checking for states and movement possibilities

		-updateRealPositions(): micro manages animations on screen positions
								lot's of code calculating time and space moved

About the animations and pixel art:
	I chose a 16 bit style, meaning sprites are 16x 16 pixels
	Pixel art requires positions to be pixel perfect, so a lot of logic was harder (for me at least) because of that.
	Because of this animation could be tweened either, it has to be by pixel or intergers divisible by 16.
	 A lot of the code of the animations is written along how many frames it takes something.
	 The code is not bound by framerate. If I say 6 frames, that means 6/60 = 0.1 seconds
	 It is easier for me to think in frames in a second (60 theoretical max), than time passed
	 this is because in animation things are animated on 1s, 2s or 4s e.g not time frames.

Thank you and I hope you enjoy my little game.