# DinoRun Game (RTOS-Based)

NOTE: A LOT OF THE CODE I DELETED BECAUSE ITS A CLASS ASSIGNMENT. I SHOW A LITTLE, BUT I DONT WANT TO GET IN TROUBLE WITH UF.

## Overview
A simple, yet fully functional game implemented on an embedded RTOS platform. DinoRun showcases real-time task scheduling, input handling, and display control on a microcontroller-based system.

## Features
- RTOS-based task scheduling for smooth gameplay
- On-screen player character with obstacle detection
- Button input handling for jump and game control
- LCD display output showing game state in real time
- Modular design for easy expansion or adaptation to other games

## Tech Stack / Components
- **Microcontroller:** TIVA C LaunchPad  
- **Display:** LCD module for game visualization  
- **Input:** Buttons for player controls  
- **Software:** Custom RTOS threads for game loop, input handling, and display updates  

## Development Notes
- Implemented multiple threads: game logic, input reading, and display refresh
- Used periodic and aperiodic tasks to ensure consistent game timing and responsive controls
- Balanced RTOS priorities to avoid lag in user input or frame updates
- Learned how real-time scheduling interacts with input/output on constrained hardware

## Key Learnings
- Real-time systems require careful task partitioning and timing analysis
- Thread prioritization is critical for responsive interactive applications
- Embedded game design teaches hardware-software integration and system-level debugging
