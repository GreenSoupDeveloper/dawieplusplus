#ifndef MIDIFUNCS_H
#define MIDIFUNCS_H

#include <iostream>
#include <cstdint>
#include <string>
#include <fstream>

// MIDI structures
struct MIDIHeader {
    char chunkType[4];  // "MThd"
    uint32_t chunkSize; // Should be 6 (big-endian)
    uint16_t format;    // 0, 1, or 2
    uint16_t numTracks; // Number of tracks
    uint16_t division;  // Ticks per quarter note
};

struct MIDIChunk {
    char chunkType[4];  // "MTrk"
    uint32_t chunkSize;
    uint8_t* data;
};

// Function declarations
void readMIDIHeader(std::ifstream& file, MIDIHeader& header);
void readMIDIChunk(std::ifstream& file, MIDIChunk& chunk);
std::string getMIDIMeta(const MIDIChunk& chunk, uint8_t metaType);
const char* MIDIPitchToNote(uint8_t pitch);

#endif // MIDIFUNCS_H