#include <gtest/gtest.h>
#include <discpp/message.h>
#include <nlohmann/json.hpp>

TEST(Message, MessageCreation) {
    nlohmann::json j =
    {
        {"d", {
            {"attachments", {
                {"filename", "1d1de67151d4abbf85b9ea41f0e697fd89579b36v2_00.jpg"},
                {"height", 494},
                {"id", "704421796908236880"},
                {"proxy_url", "https://media.discordapp.net/attachments/699405108982579303/704421796908236880/1d1de67151d4abbf85b9ea41f0e697fd89579b36v2_00.jpg"},
                {"size", 31744},
                {"url", "https://cdn.discordapp.com/attachments/699405108982579303/704421796908236880/1d1de67151d4abbf85b9ea41f0e697fd89579b36v2_00.jpg"},
                {"width", 512}
            }},
            {"author", {
                {"avatar", "a069ed93aa0c686c385a033189ea2851"},
                {"discriminator", "1744"},
                {"id", "150312037426135041"},
                {"public_flags", 640},
                {"username", "Intexisty≡ƒöÑ"}
            }},
            //{"channel_id", "699405108982579303"},
            {"content", ""},
            {"edited_timestamp", {}},
            {"embeds", {}},
            {"flags", 0},
            //{"guild_id", "699405108982579300"},
            {"id", "704421796874682489"},
            {"member", {
                {"deaf", false},
                {"hoisted_role", "699405292366069851"},
                {"joined_at", "2020-04-13T23:46:21.031000+00:00"},
                {"mute", false},
                {"roles", {
                    "699405292366069851",
                    "699405478492635217"
                }},
            }},
            {"mention_everyone", false},
            {"mention_roles", {}},
            {"mentions", {}},
            {"pinned", false},
            {"timestamp", "2020-04-27T20:00:43.899000+00:00"},
            {"tts", false},
            {"type", 0}}
        },
        {"op", 0},
        {"s", 5},
        {"t", "MESSAGE_CREATE"}
    };
    nlohmann::json data = j["d"];
    discpp::Message message = discpp::Message(data);

    // TODO
    EXPECT_EQ(message.author.avatar, "a069ed93aa0c686c385a033189ea2851");
}