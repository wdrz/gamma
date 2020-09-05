# Gamma - a console game in C
## Build
Compile code by running

```
mkdir release
cd release
cmake ..
make
```

## Run & Description
After compiling and running the program type
```
I 10 12 2 3
```
to start a new game in an interactive mode (10x12 board, 2 players, maximum 3 components). Players go one by one claiming empty fields 
(when a player cannot claim a field, he misses his turn. He can also pass). A component is a set of fields claimed by one player that 
form a connected area. Each player's teritory at every moment of the game can consist of maksimum 3 components in total (number decided 
before each game). The game ends when no player can make a move. A player who claimed more fields wins. Moreover, each player once for a
game can make a "golden move" which means that he can claim field that has been already claimed by other player (but the number of components 
of any player's teritory still cannot exceed 3).

One can also run the game in bath mode
```
B 10 12 2 3
```
and command the game engine by running specific commands.

## Details
Autor: Witold Drzewakowski
Spring 2020, as a final project for IPP course at the Faculty of Mathematics, Informatics and Mechanics of the University of Warsaw
