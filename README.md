# Console Tetris

A simple, yet satisfyingly playable Tetris clone built for the Windows console using C++.  
Features include:
- Piece rotation and movement
- Automatic line-clearing with visual animations
- Dynamic scoring system
- Clean and responsive ASCII-style rendering

---

## 🎯 Based on the Javidx9 Tutorial  
This implementation is lovingly adapted from the **"Tetris – Programming from Scratch (Quick and Simple C++)"** tutorial by **Javidx9**, originally demonstrated in his [YouTube video](https://www.youtube.com/watch?v=8OK8_tHeCIA&list=PLrOv9FMX8xJE8NgepZR1etrsU63fDDGxO).  

His step-by-step guide provided the core mechanics and structure, which I expanded with:
- Flashing animated line clears
- Scoring system
- Big ASCII “GAME OVER” message
- Minor optimizations for smoother gameplay

---

## 🎮 Gameplay Instructions

| Key                  | Action                         |
|----------------------|---------------------------------|
| **← Left Arrow**     | Move piece left                 |
| **→ Right Arrow**    | Move piece right                |
| **↓ Down Arrow**     | Drop piece faster               |
| **R**                | Rotate current piece (once per press) |

**Goal:** Clear lines to score points. The game ends when pieces stack to the top of the field.

---

## ✨ Features

- **Frame-based Rendering** – Smooth console updates.
- **Line-Clear Animation** – Flashing rows before removal for visual feedback.
- **Score Tracking** – Points awarded based on number of lines cleared at once.
- **Big ASCII “GAME OVER”** – Dramatic end screen when the game finishes.
- **Simple, Portable Code** – No external dependencies; pure C++ and WinAPI.

---

## 🛠️ Building & Running

1. Clone or download this repository.
2. Open in **Microsoft Visual Studio** or another Windows-compatible C++ IDE.
3. Compile and run — no extra libraries required.
4. Enjoy the retro console Tetris experience!

---

## 📝 License & Credits

- **Original Inspiration**: [Javidx9’s YouTube tutorial](https://www.youtube.com/watch?v=8OK8_tHeCIA&list=PLrOv9FMX8xJE8NgepZR1etrsU63fDDGxO)
- **Enhancements**: Added animations, scoring system, ASCII art, and improved input handling.
- **License**: Open source — feel free to explore, tweak, or extend.


