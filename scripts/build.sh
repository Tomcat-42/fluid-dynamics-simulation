#!/usr/bin/bash
TARGET=fluid

#! Loads the correct environment for the target
module load gcc/11.1

# Build the simulation executable
make

# Copy the executable to the user dir
cp -av "$TARGET" "$SCRATCH"
