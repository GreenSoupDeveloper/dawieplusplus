#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define be16toh(x) _byteswap_ushort(x)
#define be32toh(x) _byteswap_ulong(x)
#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define be16toh(x) OSSwapBigToHostInt16(x)
#define be32toh(x) OSSwapBigToHostInt32(x)
#else
#include <endian.h>
#endif

// MIDI structures
struct MIDIHeader {
    char chunkType[4];  // "MThd"
    uint32_t chunkSize; // Should be 6 (big-endian)
    uint16_t format;    // 0 = single track, 1 = multi-track, 2 = multiple independent patterns
    uint16_t numTracks; // Number of tracks
    uint16_t division;  // Time division (ticks per quarter note)
};

struct MIDIChunk {
    char chunkType[4]; // "MTrk"
    uint32_t chunkSize;
    uint8_t *data;
};

void readMIDIHeader(std::ifstream& file, MIDIHeader& header) {
    file.read(header.chunkType, 4);
    file.read(reinterpret_cast<char*>(&header.chunkSize), 4);
    file.read(reinterpret_cast<char*>(&header.format), 2);
    file.read(reinterpret_cast<char*>(&header.numTracks), 2);
    file.read(reinterpret_cast<char*>(&header.division), 2);

    // Convert from big-endian
    header.chunkSize = _byteswap_ulong(header.chunkSize);
    header.format = _byteswap_ushort(header.format);
    header.numTracks = _byteswap_ushort(header.numTracks);
    header.division = _byteswap_ushort(header.division);
}

void readMIDIChunk(std::ifstream& file, MIDIChunk& chunk) {
    file.read(chunk.chunkType, 4);
    file.read(reinterpret_cast<char*>(&chunk.chunkSize), 4);

    chunk.chunkSize = be32toh(chunk.chunkSize);

    chunk.data = new uint8_t[chunk.chunkSize];
    file.read(reinterpret_cast<char*>(chunk.data), chunk.chunkSize);
}

std::string getMIDIMeta(const MIDIChunk& chunk, uint8_t metaType) {
    switch (metaType) {
    case 0x03: // channel
    {
        uint8_t chanlength = chunk.data[3];
        std::string chanbuffer(reinterpret_cast<const char*>(chunk.data + 4), chanlength);
        return chanbuffer;
    }
    case 0x02: // copyright text
    {
        uint8_t copyrightlength = chunk.data[15];
        std::string copyrightbuffer(reinterpret_cast<const char*>(chunk.data + 16), copyrightlength);
        return copyrightbuffer;
    }
    case 0x01: // text
    {
        uint8_t txtlength = chunk.data[46];
        std::string txtbuffer(reinterpret_cast<const char*>(chunk.data + 47), txtlength);
        return txtbuffer;
    }
    default:
        std::cerr << "invalid id thinger thing\n";
        return "error";
    }
}

const char* MIDIPitchToNote(uint8_t pitch) {
    static const char* notes[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    return notes[pitch % 12];
}