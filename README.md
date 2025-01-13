# ProcessWav
Process a 48000 sample per second 8-bit mono .wav file to a C header file for use on the Wii U.

This tool has been developed to work with the Wii U Sndcore2 API. The tool creates an array of 8-bit signed integers in the form needed by Sndcore2, to play sounds on the Wii U. This header file can then be used with some code in the Wii U, to play the sound.

This tool was quickly developed to support some Wii U Homebrew simple games development. It should operate correctly for Uncompompressed mono .wav files of 48000 samples per second 8-bit data. I used NCH Software WavePad to save sound files in this format, so that they could be processed.
