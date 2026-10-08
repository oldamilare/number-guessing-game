# Number Guessing Game

A command line number guessing game written in C++. It only uses the standard library.

Project page: https://roadmap.sh/projects/number-guessing-game

The computer picks a number between 1 and 100 and you try to guess it before you run out of chances. After every wrong guess it tells you whether the number is greater or less.

## Build and run

You need a C++17 compiler (`clang++` or `g++`). On a Mac, running `xcode-select --install` gives you one.

```bash
clang++ -std=c++17 -Wall -o guess guess.cpp
./guess
```

## Playing

```
Welcome to the Number Guessing Game!
I'm thinking of a number between 1 and 100.
...
Please select the difficulty level:
1. Easy (10 chances)
2. Medium (5 chances)
3. Hard (3 chances)

Enter your choice: 2

Great! You have selected the Medium difficulty level.
You have 5 chances to guess the correct number.
Let's start the game!

Enter your guess: 50
Incorrect! The number is less than 50.
Enter your guess: 25
Incorrect! The number is greater than 25.
Enter your guess: 35
Incorrect! The number is less than 35.
Enter your guess: 30
Congratulations! You guessed the correct number in 4 attempts.
It took you 21.3 seconds.
```

You can pick a level with its number or its name (`1` or `easy`).

## Extras

- **Play again**: after each round it asks if you want another go.
- **Timer**: shows how long a winning round took.
- **Hints**: type `hint` instead of a guess. You get up to three per round (odd or even, a range of 20 numbers that contains the answer, then whether it's a multiple of 5). Hints don't use up a chance.
- **High scores**: the fewest attempts for each difficulty is saved in `highscores.txt` next to where you run the game. Rounds where you used hints don't count towards it.

## Input handling

Anything that isn't a whole number from 1 to 100 is rejected without using up a chance. Closing the input (Ctrl+D) ends the game cleanly.
