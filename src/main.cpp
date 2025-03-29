#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <algorithm>

#ifdef _WIN32
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

#include "midifuncs.h"




int main(int argc, char *argv[]) {
    std::string testfile = "in.mid";
    if (argc > 1) {
        testfile = argv[1];
    }

    std::ifstream file(testfile, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open '" << testfile << "'\n";
        return 1;
    }

    MIDIHeader header;
    readMIDIHeader(file, header);

    std::cout << "Chunk Type: " << std::string(header.chunkType, 4) << "\n";
    std::cout << "Chunk Size: " << header.chunkSize << "\n";
    std::cout << "MIDI Format: " << header.format << "\n";
    std::cout << "Number of Tracks: " << header.numTracks << "\n";
    std::cout << "Time Division: " << header.division << "\n";

    MIDIChunk chunk;
    readMIDIChunk(file, chunk);

    std::cout << "Chunk Type: " << std::string(chunk.chunkType, 4) << "\n";
    std::cout << "Chunk Size: " << chunk.chunkSize << "\n";

    for (uint32_t i = 0; i < chunk.chunkSize; i++) {
        std::cout << "||| CHUNK #" << i << " ||| ";
        if (chunk.data[i] & 0x80) {
            if (chunk.data[i] == 0xFF) { // Meta event
                uint8_t type = chunk.data[i + 1];
                uint8_t length = chunk.data[i + 2];
                std::string metaData = getMIDIMeta(chunk, type);
                
                switch (type) {
                    case 0x03:
                        std::cout << "Channel name: " << metaData << "\n";
                        i += length + 2;
                        break;
                    case 0x2F:
                        std::cout << "End of track\n";
                        i += 2;
                        break;
                    case 0x51:
                        uint32_t tempo;
                        std::memcpy(&tempo, &chunk.data[i + 2], 3);
                        tempo = be32toh(tempo) & 0xFFFFFF;
                        std::cout << "Tempo: " << tempo << "\n";
                        i += 5;
                        break;
                    default:
                        std::cout << "---> META ERR: Unknown meta event 0x" << std::hex << (int)type << "\n";
                        i += 2;
                        break;
                }
            } else {
                uint8_t eventType = chunk.data[i] & 0xF0;
                uint8_t channel = chunk.data[i] & 0x0F;
                
                std::cout << "Event on Channel " << (int)channel << ": ";
                switch (eventType) {
                    case 0xB0:
                        std::cout << "Control Change: Controller " << (int)chunk.data[i + 1] << " = " << (int)chunk.data[i + 2] << "\n";
                        i += 2;
                        break;
                    case 0xC0:
                        std::cout << "Program Change on Channel " << (int)channel << ": Program " << (int)chunk.data[i + 1] << "\n";
                        i += 1;
                        break;
                    case 0x80:
                        std::cout << "Note off(Note: " << MIDIPitchToNote(chunk.data[i + 1]) << (chunk.data[i + 1] / 12 - 1) << ", Velocity: " << (int)chunk.data[i + 2] << ")\n";
                        i += 2;
                        break;
                    case 0x90:
                        std::cout << "Note on(Note: " << MIDIPitchToNote(chunk.data[i + 1]) << (chunk.data[i + 1] / 12 - 1) << ", Velocity: " << (int)chunk.data[i + 2] << ")\n";
                        i += 2;
                        break;
                    default:
                        std::cout << "---> ERR: Unknown event 0x" << std::hex << (int)chunk.data[i] << " <---\n";
                        i++;
                        break;
                }
            }
        } else {
            std::cout << "Data byte: 0x" << std::hex << (int)chunk.data[i] << " \n";
        }
    }
    std::cout << "\n";

    file.close();
    return 0;
}
