// This is a program that takes an 8-bit (one byte per sample) 48k sample mono .wav file and creates a .h file.
// The .h file can then be used in Wii U programs to run with the Sndcore2 library to add sounds to Wii U games.
// ProcessWav.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXDATA 0x00800000u // This is currently set to 8Mbytes.


char fileData[MAXDATA];     // Data read from .wav file to be processed.
char fileHeader[46];        // The .wav header information at the start of the file.

int main(int argc, char** argv)
{
    FILE* inFile;
    FILE* outFile;

    char outFileName[100] = "";     // String to give out file a different name from the input.
    unsigned int len;               // string length for output file name.

    unsigned int dataSize;          // size of the data area of the file.

    int shiftData;                  // working integer to correctly handle offset in .wav data compared to WiiU.

    // If there is no command line parameter then exit the program as nothing to process.
    if (argc < 2)
    {
        printf("No file\n");
        exit(0);
    }
    // If wav is not in the string, it is not a wave file.
    // Note this could be improved to specifically check the extension.
    if (NULL == strstr(argv[1], "wav"))
    {
        printf("%s Not a wav file\n", argv[1]);
        exit(0);
    }

    // Open the file to get the data.
    if (fopen_s(&inFile, argv[1], "rb") != 0)
    {
        printf("%s file could not be opened\n", argv[1]);
        exit(0);
    }
    else
    {
        // Get the wave header information from the start of the file.
        // Note important to use fread not fgets as fgets will stop if it sees string terminator bytes (0).
        fread(fileHeader, 1, 44, inFile);
        // If the header starts with RIFF and has WAVE it is a .wav file
        if ((fileHeader[0] == 'R') && (fileHeader[1] == 'I') && (fileHeader[2] == 'F') && (fileHeader[3] == 'F') && (fileHeader[8] == 'W') && (fileHeader[9] == 'A') && (fileHeader[10] == 'V') && (fileHeader[11] == 'E'))
        {
            // Chunk size 16, audio format 1 byte per sample, 1 channel, sample rate 48000.
            if ((fileHeader[16] == 0x10u) && (fileHeader[20] == 0x01u) && (fileHeader[21] == 0x00u) && (fileHeader[22] == 0x01u) && (fileHeader[23] == 0x00u) && ((unsigned char)fileHeader[24] == 0x80u) && ((unsigned char)fileHeader[25] == 0xBBu))
            {
                // Extract the size of the data from the header file.
                dataSize = (((unsigned char)fileHeader[42] * 65536) + ((unsigned char)fileHeader[41] * 256) + (unsigned char)fileHeader[40]);
                if (dataSize < MAXDATA)
                {
                    // If OK get the data from the file into memory.
                    fread(fileData, 1, dataSize, inFile);

                    // Create the output headerfile name and then open the file.
                    strcat_s(outFileName, argv[1]);
                    strcat_s(outFileName, ".h");
                    if (fopen_s(&outFile, outFileName, "wt") != 0)
                    {
                        printf("out file could not be opened\n");
                        exit(0);
                    }
                    else
                    {
                        // Use the input filename to name the data array in the output header file.
                        len = (unsigned int)(strchr(outFileName, '.') - outFileName);
                        outFileName[len] = '\0';
                        fprintf(outFile, "unsigned char %sWave[%d] = {", outFileName, dataSize);

                        for (unsigned int x = 0; x < dataSize; x++)
                        {
                            // Start a new line periodically to have a readable format.
                            if (x % 32 == 0)
                            {
                                fprintf(outFile, "\n");
                            }
                            shiftData = fileData[x] - 0x80; // Move data down so that it is oscillating around 0 to match Wii U format.
                            fprintf(outFile, "0x%02X", (unsigned char)shiftData);
                            // The following logic is to ensure that a comma is not output for the end of the last line.
                            if (x < (dataSize - 1))
                            {
                                fprintf(outFile, ", ");
                            }
                        }
                        // The data array is closed with a semi-colon.
                        fprintf(outFile, " };\n");
                        // The files are closed as the array has been processed.
                        fclose(inFile);
                        fclose(outFile);
                    }
                }
                else
                {
                    printf("%s Data too big, more than 8Mbyte\n", argv[1]);
                    exit(0);
                }
            }
            else
            {
                printf("%s Not correct format of 16 byte chunks, byte per sample, mono and 48000 samples per second\n", argv[1]);
                exit(0);
            }
        }
        else
        {
            printf("%s Not a wav file\n", argv[1]);
            exit(0);
        }
    }
}

