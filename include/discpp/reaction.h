#ifndef DISCPP_REACTION_H
#define DISCPP_REACTION_H

#include "discord_object.h"
#include "emoji.h"

#include <nlohmann/json.hpp>

namespace discpp {

	class Reaction : public DiscordObject {
	public:
		Reaction() = default;
		Reaction(rapidjson::Document& json);
		Reaction(int count, bool from_bot, discpp::Emoji emoji) : count(count), from_bot(from_bot), emoji(emoji) { }

		int count;
		bool from_bot;
		discpp::Emoji emoji;
	};
}

#endif