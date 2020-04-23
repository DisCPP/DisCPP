#include "PreconditionRequireGuild.h"

bool Precondition::RequireGuild(discord::Context ctx, std::string message) {
    if (ctx.channel.type != discord::DM) {
        return true;
    }
    else {
        ctx.Send(message);
        return false;
    }
}