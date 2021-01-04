#ifndef REQUIREGUILD_PRECON_REQUIRE_GUILD_H
#define REQUIREGUILD_PRECON_REQUIRE_GUILD_H

#include <discpp/context.h>
#include <string>

namespace Precondition {
    bool RequireGuild(discpp::Context ctx, std::string message = "This command requires you to be in a guild to run");
}

#endif