#include "PreconditionRequireGuild.h"

bool Precondition::RequireGuild(discpp::Context ctx, std::string message) {
    if (ctx.channel.type != discpp::DM) {
        return true;
    }
    else {
        ctx.Send(message);
        return false;
    }
}