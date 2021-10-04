int32 utf8_decode(void* buf, int64 cap, uint32* codepoint) {
    static const char lengths[] = { // number of bytes based on 
        1, 1, 1, 1,                 // the 5 most significant bits
        1, 1, 1, 1,                 // of the first byte.
        1, 1, 1, 1, 
        1, 1, 1, 1,
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        2, 2, 2, 2, 
        3, 3, 
        4, 
        0
    };

    static const uint32 masks[]  = {0x00, 0x7f, 0x1f, 0x0f, 0x07};
    static const int32 shiftc[] = {0, 18, 12, 6, 0};

    byte* str = (byte*)buf;
    if (cap <= 0) return 0;
    byte b = str[0];
    int32 length = lengths[b >> 3];
    if (0 < length && length <= cap) {
        *codepoint = (b & masks[length]) << 18;
        switch (length) {
            case 3: *codepoint |= ((str[3] & 0x3f) << 0);
            case 2: *codepoint |= ((str[2] & 0x3f) << 6);
            case 1: *codepoint |= ((str[1] & 0x3f) << 12);
            case 0: *codepoint >>= shiftc[length];
        }
    }
    return length;
}