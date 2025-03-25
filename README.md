# Aether

## Overview

Aether is a UCI chess engine written in C. It uses a 16x16 mailbox array to represent the board to make off-the-board checks easier. In an 0x88 board (and variations such as 16x16), the difference between any two squares is unique with respect to distance and direction. This engine uses this fact to look up unit vectors and which pieces which can move between two squares from pre-calculated arrays, simplifying move generation. These ideas and others are described in more detail by H.G. Muller in [this](https://talkchess.com/viewtopic.php?t=81265) forum post.

Aether uses an alpha beta search and simple evaluation function to find the best move. It is a white-to-move engine.