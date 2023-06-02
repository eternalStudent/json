int32 utf8_decode(byte* str, ssize cap, uint32* codepoint){
	static byte length[] = {
		1, 1, 1, 1, // 000xx
		1, 1, 1, 1,
		1, 1, 1, 1,
		1, 1, 1, 1,
		0, 0, 0, 0, // 100xx
		0, 0, 0, 0,
		2, 2, 2, 2, // 110xx
		3, 3,       // 1110x
		4,          // 11110
		0,          // 11111
	};
	static byte first_byte_mask[] = { 0, 0x7F, 0x1F, 0x0F, 0x07 };
	static byte final_shift[] = { 0, 18, 12, 6, 0 };
	
	if (cap <= 0) return 0;

	int32 size = 1;
		
	byte b = str[0];
	byte l = length[b >> 3];
	if (0 < l && l <= cap){
		uint32 cp = (b & first_byte_mask[l]) << 18;
		switch (l){
			case 4: cp |= ((str[3] & 0x3F) << 0);
			case 3: cp |= ((str[2] & 0x3F) << 6);
			case 2: cp |= ((str[1] & 0x3F) << 12);
			default: break;
		}
		cp >>= final_shift[l];
		
		*codepoint = cp;
		size = l;
	}
		
	return size;
}