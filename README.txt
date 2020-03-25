	This is an open source application copyrighted by Kyle McGrath in 2020 directed towards an academic audience that is not libre. Do not use in commercial products. This application dynamically links to open source Qt libraries in accordance with GPLv2. If you redistribute this software, you must redistribute with this notice. You may not reuse the source code or application without permission from the author.

	Under MusicalGraphGame menu item, you can find “Preferences”. This opens a dialog to change difficulty. Under Game Options menu item, you can find “New Game”. If you change the difficulty you must start a new game.

	If you make changes to the source code, then you should run “Clean” and recompile the entire project.

Tutorial for compilation: https://www.youtube.com/watch?v=lF9lt30Djgo

	The goal of the game is to have CRT value reach your TGT value. The current node of the graph is highlighted in red and bounces slightly. The player may move to any node to which this node is connected. When a node is selected, the current value (CRT in the upper left hand corner) is added to the nodes value modulo the modulus (given by MOD in the lower left hand corner). Once the player achieves the goal of having CRT equal TGT, the progress bar seen on the right moves up one. Once the progress bar reaches 10, the level is won.

	This game demonstrates a new and difficult problem in mathematics. Access preferences in the many called "MusicalMathGame > Preferences". If you change the difficulty to "Medium" the tutorial mode turns off and in "Hard" mode, the modulus increases. For a large modulus, the game is extremely difficult. 

	The slider to change problemIndex adds the ability to change the "level" or progress through the game without winning at the lower levels. As the level increases the order of the graph increases. Once problemIndex is around 35, the player should notice that the game switches between one of three problem types with and emphasis on more difficult problems.

