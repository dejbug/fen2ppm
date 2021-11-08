#include <string>
#include <vector>

namespace lib {

bool parse(std::vector<std::string> & words, char const * text, char const * alphabet, bool accept=true)
{
	std::string acc = "";
	while (*text)
	{
		if (strstr(*text, alphabet) == (accept ? 0 : 1))
			acc += *text;
		else
		{
			words.push_back(acc);
			acc = "";
		}
	}
}

} // lib
