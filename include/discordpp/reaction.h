#ifndef DISCORDPP_REACTION_H
#define DISCORDPP_REACTION_H

#include "discord_object.h"
#include "emoji.h"

#include <nlohmann/json.hpp>

namespace discord {

	class Reaction : public DiscordObject {
	public:
		Reaction() = default;
		Reaction(nlohmann::json json);
		Reaction(int count, bool from_bot, discord::Emoji emoji) : count(count), from_bot(from_bot), emoji(emoji) { }

		int count;
		bool from_bot;
		discord::Emoji emoji;
	};
}

#endif