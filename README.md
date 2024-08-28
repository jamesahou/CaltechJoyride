# Game Design Document

## Section 0: Summary
This section should tell the reader what to expect in the future sections.  In particular, it should contain at least:
- a working title for your game
    - JPL Joyride
- a list of your team members (and their roles if decided)
    - Pranit Gunjal, Michael Girma, James Hou
- a "concept statement" of your game ("your game in a tweet")
    - At JPL, an alien egg hatches and escapes. The Nefarious Dr. Douglass wants to domesticate the alien and run inhumane experiments on it. The main character is an alien whose goal is to escape. The alien manages to get a jetpack while running away. Dr. Douglass sends rockets after the alien and the goal of the game is to survive as long as possible. There are also different power ups that enable the alien to survive.


## Section 1: Gameplay
This section should address simple questions about how your game works:
- How does your game progress?
    - The game begins when the user clicks on the homescreen or presses a specific key. Then the user, an alien with a rocket jetpack, is vaulted into the screen (the lab breakout),  after which the alien traverses throughout the JPL lab, in an infinite game.
    - As the user flies throughout the lab, they are faced with a barrage of obstacles, including electric fences, rockets and even laser arrays. If the user collides with any of these obstacles, they are either damaged or destroyed and upon destruction, the game ends and resets to the initial state.
    - The alien can move up by activating its jetpack but is also subject to gravitational pull, so once the jetpack is off, the alien falls back down. 
    - The alien also can break power-up blocks which give it extra abilities such as 2x speed, teleportation, gravity inversion and time slowdown.

- What are the win and loss conditions? 
    - You don’t win (infinite) but you could lose if you hit something. We also plan to implement a high score/leaderboard to incentivize gameplay.
- Are there levels?
    - It would be infinite, but it gets progressively harder as you go on depending on the score the player has. For example, by adding more obstacles, making the obstacles move faster, and move randomly so they would be unpredictable.
- Are there points?
    - There will be points and it would be calculated on how far the player has gone or how long they lasted. 

This section should also address:
- **controls** (keyboard? mouse? tongue?)
    - the keyboard is used by the user to turn the jetpack on and off and also take power-up actions.
- **physics** (how does your game incorporate the physics engine?)
    - Gravity will be adjusted depending on the abilities of the alien. The jetpack will also be using the force creators we implemented. Collision logic will also be used to detect interactions between the user and the obstacles/enemies.
- **game flow** (what does the game look like from start to end for the player?)
    - The player only moves up and down but the moving objects give it the illusion of a moving screen.
- **graphics** (will you draw polygons? use sprites? make your own vector graphics?)
    - Sprites will be developed that represent the various characters in the game along with a background that will hopefully move as the game progresses.

## Section 2: Feature Set
This section should reduce your game to a set of individual features (remember iterative development?).  Your team should
assign these features to individual group members and assign each one a priority from one to four (1 = game cannot work without this, 4 = cool but not necessary).

- Build “stationary” obstacles: - 1
    - Generate graphics for electric fences/physical obstacles
    - Program obstacle specific handlers
- Generate Graphics and Controls - 1
    - This feature is responsible for rendering crucial game elements such as the user character, jetpack, stationary background.
    - Handles user movement + physics + jetpack animation
- Health bar / Score Counter - 1
    - Tracks and renders health bar/heart count
    - Tracks and renders the score of each run and saves at the end of the run.
- Build moving/accelerating/dynamic (time dependent) enemy/obstacles - 2
    - Random rockets flying across the screen (accelerating)
    - Timed laser arrays that instantaneously span the screen
    - Rotating electric fences
- Ability Power Ups - 2
    - Force field - creates a handler that destroys repels anything that collides with a larger circle around the user
    - Slow obstacles/enemies - dial down speed (potentially stretching graphics) of incoming obstacles or objects
- Attribute Powerups - 2
    - Program spawning of powerup blocks
    - Refill health - Refills health state of user and health bar on the screen
    - x2 score counter - 2x multiplier to user score over a certain time period
    - Damage muffler - reduce damage of obstacles per certain game conditions.
- Music + Sound Effects - 3
    - Sound effects for whenever collision happens or jetpack is activated
    - A simple free to use theme music that plays as the user is playing the game
- AI aiming enemy - 3
    - Gets the enemy to “hone” in on the player as the game progresses. The easy level will have the enemy just move across the screen, the medium level will have the enemy randomly move up and down while moving forward. The hard level will have the enemy hone on the player with some slight randomness to ensure that the game is playable.
- Welcome/Register menu- 3
    - Creates username and password for the player. Redirects the player into the Skin Store/Economy and starts/quits the game. And the option for the player to choose to play without score (ability to respawn).
- Respawn / checkpoint - 4
    - Respawns from the last checkpoint so the user can keep playing infinitely.
- Leaderboard/Score Storing - 4
    - Stores the 5 highest scores during a player's time in the game. (no respawn)
- Skin Store / Economy - 4
    - Creates skin store and skin objects purchasable with accumulated game points
    - Tracks which skins the user has over multiple sessions (file database) and loads the correct one upon game start

## Section 3: Timeline
This section should assign every feature in the previous section to a particular group member and a particular week they will implement it.

Michael
1. By 5/31, Build “stationary” obstacles: - 1
2. By 5/31, Build moving/accelerating/dynamic (time dependent) enemy/obstacles - 2
3. By 6/7, Welcome/Register menu- 3
4. By 6/7, Leaderboard/Score Storing - 4

Pranit
1. By 5/31, Health Bar - 1
2. By 5/31, Attribute Powerups - 2
3. By 6/7, AI aiming enemy - 3
4. By 6/7, Respawn / checkpoint - 4

James
1. By 5/31, Generate Graphics and Controls - 1
2. By 5/31 Ability Power Ups - 2
3. By 6/7 Music + Sound Effects - 3
4. By 6/7 Skin Store / Economy - 4

## Section 4: Disaster Recovery
This section should describe how each member plans to get back on track if they fall behind.  Please take this section seriously.

Michael: Make sure to make use of office hours. Discuss with the others and Prof. Blank on how to get back on track. On top of that, make sure to use the deadlines set and take them seriously. My goal is to be ahead of the deadlines and not procrastinate. My deadlines are there in order to make me accountable for and if for some reason, I cannot get it done on time, I will be able to catch up and finish it on time. 

Pranit
- I would first notify my teammates and ask them for input on how to progress with the project
- Go to Office Hours and discuss my issues with Professor Blank and the TA’s (i.e if I’m having a bug or if I need some help starting this piece)
- If I don’t know what to do at all, I will try to confer with my teammates and see if they have some input on what to do. 
- The most obvious step in the plan is to put more effort and emphasis into completing the project and getting it done

James: If I fall behind my plan is to first notify my groupmates on my schedule and lay out a recovery plan in the next week (splitting the tasks into even smaller deliverable parts). I will also visit office hours frequently to catch up/work efficiently and avoid any stumps. If the delay is especially disastrous, I will reach out to our lab TAs and Professor Blank to figure out the course of action.
