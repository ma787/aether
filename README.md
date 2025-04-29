# Aether

## Overview

Aether is a chess engine written in C which supports the UCI protocol. It also supports a non-standard display command "d", which prints the board and other information associated with the current position.

## Features
- 16x16 mailbox board representation
- Vector attack lookup tables
- Legal move generation (except en passant and king moves)
- Alpha-Beta search with iterative deepening
- Quiescence search
- MVV-LVA move ordering
- Killer move and alpha improvement heuristics
- Zobrist hashing and PV move hash table
- Repetition detection (though not aware of threefold repetition)
- Simple material and square-based evaluation

For more information on the move generation features, see [this](https://talkchess.com/viewtopic.php?t=81265) forum post by H.G. Muller.