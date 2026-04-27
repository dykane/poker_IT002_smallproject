# Texas Hold’em Poker Simulation (OOP Project)

## Overview

This project is an object-oriented simulation of the Texas Hold’em Poker game, implemented in C++.
It was developed as part of the Object-Oriented Programming (OOP) course at the University of Information Technology – VNU HCMC.

The system models a complete poker gameplay pipeline, including:

* Deck creation and shuffling
* Player state and actions
* Betting rounds
* Hand evaluation
* AI-driven decision making

The main objective is to simulate multiple games and analyze the effectiveness of different strategies over time.

---

## Game Rules (Texas Hold’em)

Each player:

* Receives 2 private cards (hole cards)
* Combines them with 5 community cards to form the best possible 5-card hand

### Game flow:

* Pre-Flop (each player receives 2 cards)
* Flop (3 community cards revealed)
* Turn (4th community card)
* River (5th community card)
* Showdown (determine winner)

At showdown, the player with the strongest hand wins the pot.
If multiple players have the same hand rank, the pot is split evenly.

---

## AI Strategies

### 1. brainrot (Always Call)

* Always calls regardless of hand strength
* Extremely simple baseline strategy
* Useful for comparison

### 2. choiantoan (Safe Play)

* Uses a fixed threshold (2000 chips)
* If required call exceeds threshold → fold
* Otherwise → call
* Conservative and risk-controlled

### 3. choiconao (Smart Play)

* Evaluates hand strength dynamically
* Uses:

  * Card ranking
  * Suit matching (flush potential)
  * Consecutive cards (straight potential)
  * Probabilistic decisions

Behavior depends on number of visible cards:

* 2 cards (preflop logic)
* 5 cards (flop)
* 6 cards (turn)
* 7 cards (river)

---

## Project Structure

```
Poker/
│── poker.h
│── Card.cpp
│── Deck.cpp
│── Player.cpp
│── Pot.cpp
│── Tactics.cpp
│── BettingRound.cpp
│── Game.cpp
│── Main.cpp
```

---

## Core Components

### Card & Deck

* Standard 52-card deck (4 suits, 13 ranks)
* Shuffle implemented using `mt19937`

### Player

**Attributes:**

* `chip`
* `bet_amount`
* `active`
* `is_all_in`

**Actions:**

* `call()`
* `raise()`
* `fold()`
* `all_in()`

### HandEvaluator

Evaluates poker hands:

* Pair
* Two Pair
* Three of a Kind
* Straight
* Flush
* Full House
* Four of a Kind

### Betting System

* Iterative betting rounds
* Players can call, raise, or fold
* Loop continues until all bets are equal or players fold

### Game Engine

* Simulates multiple rounds
* Tracks winners
* Compares strategies statistically

---

## How It Works

1. Initialize players (each starts with 5000 chips)
2. Shuffle deck and deal cards
3. Execute betting rounds:

   * Pre-Flop → Flop → Turn → River
4. Evaluate hands and determine winner(s)
5. Repeat for `n` rounds
6. Output win statistics

### Example output:

```
Player 5 (Smart AI) wins: X rounds
Player 4 (Safe AI) wins: Y rounds
Player 3 (Safe AI) wins: Z rounds
Player 2 (Always Call) wins: A rounds
Player 1 (Always Call) wins: B rounds
```

---

## How to Run

### Requirements

* C++ compiler (g++, clang++)
* C++11 or later

### Compile

```
g++ *.cpp -o poker
```

### Run

```
./poker
```

### Input

```
Enter number of games: 1000
```

---

## Simulation Insights

The simulation supports large-scale runs (100 → 100000 games) to analyze:

* Strategy performance
* Risk vs reward trade-offs
* Differences between simple and complex AI

**Key observation:**
The "always call" strategy performs surprisingly well in simplified environments due to the absence of psychological factors such as bluffing.

---

## Learning Objectives

This project demonstrates:

* Object-Oriented Programming design (encapsulation, modularity)
* Game simulation architecture
* AI decision-making logic
* Probability-based reasoning
* Performance analysis through repeated simulation

---

## Future Improvements

* Full hand ranking system (Royal Flush, Straight Flush, etc.)
* Real betting mechanics (dealer rotation, blinds system)
* Graphical user interface or web-based version
* Advanced AI (Monte Carlo simulation, reinforcement learning)
* Multiplayer support

---

## Authors

* Student: Hoang Duy Khanh - 
* Course: Object-Oriented Programming (IT002)
* Instructor: Vũ Tuấn Hải
