   char *buffer = malloc(length + 1);
                    memcpy(buffer, chunk.data + i + 3, length);
                    buffer[length] = '\0';