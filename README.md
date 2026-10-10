# Markov Chain Next Word Generator
**Markov Chains** describe sequences of possible events in which the probability of each next event depends solely on the current one. Each event changes the current state to another randomly, based on a `transition matrix` which specifies the discrete distributions defining the probability of each event firing.

A good blog by Andrew Healey, which gives a deeper overlook of Markov chains can be found [here](https://healeycodes.com/generating-text-with-markov-chains).

This repository materializes a Markov chain for guessing the next word in a text. Given a specific input file, the program parses it into phrases, and each phrase gets assigned a unique class. Each class then gets defined with possible next words for it, those that create a new valid phrase already indexed by the program. These classes get written into a transition matrix, which gets fed into the chain and ran for `i` iterations, producing a text.

## Usage

Get a text file, and put it's contents into `text.txt`.

Run the text sampler python script to format the text correctly:

`python ./sample_text.py`

Then, build the C program:

```
cmake -DCMAKE_BUILD_TYPE=Release -B build
cd build
make

cp ../probabilities.mrkv ./
./markov_chains
```

## Config

You can specify keyword length, input and output file in the head of the `sample_text.py` script.

You can specify iteration amount in the bottom of the `main.c` file.
