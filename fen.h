bool fen_unpack(char buf[64+1], char const * fen)
{
	// ASSUME: fen is zero-terminated.
	// ASSUME: fen has no leading whitespace.

	size_t len = 0;
	for (size_t k=0; k<64+8 && *fen; ++k, ++fen)
	{
		if(strchr(" \t\r\n", *fen))
			break;
		else if (*fen == '/')
		{
			if (len % 8 != 0) return false;
			continue;
		}
		else if (isdigit(*fen))
		{
			int const d = (*fen - '0');
			if (d < 0 || d > 9) return false;
			for (int i=0; i<d && len<64; ++i)
				buf[len++] = '-';
		}
		else
		{
			// buf[len++] = *fen;
			if (len >= 64) return false;
			buf[len++] = *fen;
		}
	}

	buf[64] = '\0';
	return true;
}

char fen_translate(char c, char const map[32+1])
{
	switch (c)
	{
		case 'p': return map[0];
		case 'P': return map[1];
		case 'n': return map[2];
		case 'N': return map[3];
		case 'b': return map[4];
		case 'B': return map[5];
		case 'r': return map[6];
		case 'R': return map[7];
		case 'q': return map[8];
		case 'Q': return map[9];
		case 'k': return map[10];
		case 'K': return map[11];
		default: return ' ';
	}
}

bool fen_translate(char buf[64+1], char const map[32+1])
{
	for (size_t i=0; i<64; ++i)
	{
		buf[i] = fen_translate(buf[i], map);
	}
	return true;
}
