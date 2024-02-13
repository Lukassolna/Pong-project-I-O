# Pong Game Project
Pong Game Development on the ChipKIT Uno32 Board
## Objective and Requirements

This project aims to develop the classic pong game on the Uno32 ChipKIT. The game allows two players to use paddles to hit a ball back and forth, aiming to score points by getting the ball past the opponent's paddle. Key requirements include:

- Display on the built-in OLED graphical display.
- Ball movement in X and Y directions.
- Paddle movement in Y direction.
- Natural bouncing of the ball on paddles and sides.
- Detection and display of player wins.

### Bonus Features 

- Score tracking.
- Goal-scoring effects.
- Computer opponent mode.

## Solution Approach

The game will be implemented on the ChipKIT Uno32 board, with gameplay shown on the built-in screen. Players will control paddles using push buttons. The development will utilize MCB32tools, with programming in C to handle ball dynamics and game logic.

## Verification Strategy

The game will undergo various tests to ensure functionality, including:

- Verification of correct ball bounce behavior.
- Testing paddle movement accuracy.
- Ensuring the goal-scoring mechanism operates as expected.
- External testing to identify issues not caught initially.
- Extensive testing to confirm all requirements are met.
- Examination for corner cases and rare scenarios.
