# Project 4 — Fluffy the Robot Pet

## Description
A virtual pet simulation managing core stats (food, fun, energy), bitwise mood flags, time progression, and recovery mechanics.
name : samah ahmed mahmoud ahmed
email : sammahmedzz50@gmail.com
## How to Build and Run
1. Open terminal in the project directory.
2. Compile with warnings enabled:
   ```bash
   gcc -Wall -Wextra main.c -o fluffy
Run the executable:

On Windows: .\fluffy.exe

On Linux/Mac: ./fluffy

Explain Why
Why use Macros (such as SET_BIT and CLR_BIT) for the mood state?
To conserve memory by packing multiple Boolean states (asleep, hungry, sad, sick) into a single byte (uint8_t), utilizing efficient bitwise operators for precise flag manipulation.
