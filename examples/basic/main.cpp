#include <bot.h>

#include <iostream>
#include <fstream>

int main(int argc, const char* argv[]) {
	std::ifstream token_file("D:\\Code\\cpp\\discord\\DexunBot\\token.txt", std::ios::out);
	std::string token;
	std::getline(token_file, token);

	discord::Bot bot{ token, "!" };

	return bot.Run();
}