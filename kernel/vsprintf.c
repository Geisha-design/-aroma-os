int vsprintf(char* buf, const char* fmt, int num) {
	char* str;

	for (str = buf; *fmt; ++fmt) {
		if (*fmt != '%') {
			*str++ = *fmt;
			continue;
		}

		int start_not_zero = 0;

		//*str++ = '0';
		//*str++ = 'x';

		for (int i = 7; i >= 0; i--) {
			int cur_index_num = ((num >> (i * 4)) & 0x0000000f);
			char c;
			if (cur_index_num != 0) {
				start_not_zero = 1;
			}
			if (cur_index_num < 10) {
				c = (char)(cur_index_num)+0x30;
			} else {
				c = (char)(cur_index_num)+0x57;
			}

			if (start_not_zero) {
				*str++ = c;
			}
		}
	}

	*str = '\0';

	return str - buf;
}
