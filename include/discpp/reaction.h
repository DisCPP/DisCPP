#ifndef DISCPP_REACTION_H
#define DISCPP_REACTION_H

#include "discord_object.h"
#include "emoji.h"



namespace discpp {

	class Reaction : public DiscordObject {
	public:
		Reaction() = default;
		Reaction(rapidjson::Document& json);
		Reaction(const int& count, const bool& from_bot, const discpp::Emoji& emoji) : count(count), from_bot(from_bot), emoji(emoji) { }

		int count;
		bool from_bot;
		discpp::Emoji emoji;
	};
}

#endif