#include <discpp/utils.h>
#include <gtest/gtest.h>
#include <vector>
#include <string>

TEST(Utils, Endpoint) {
	EXPECT_EQ("https://discordapp.com/api/v6/", discpp::Endpoint(""));
}
TEST(Utils, StartsWithPrefix) {
	EXPECT_EQ(true, discpp::StartsWith("~test", "~"));
}
TEST(Utils, StartsWithNoPrefix) {
	EXPECT_EQ(false, discpp::StartsWith("test", "~"));
}
TEST(Utils, SplitString) {
	std::vector<std::string> expectedResult = { "test", "test" };
	EXPECT_EQ(expectedResult, discpp::SplitString("test test", " "));
}
TEST(Utils, CombineStringVector) {
	std::vector<std::string> stringVector = { "test", "test" };
	EXPECT_EQ("test test", discpp::CombineStringVector(stringVector, " ", 0));
}
TEST(Utils, Base64Encode) {
	EXPECT_EQ("dGVzdA==", discpp::Base64Encode("test"));
}
TEST(Utils, EscapeString) {
	EXPECT_EQ("\\\\ \\\" \\a \\b \\f \\r \\t", discpp::EscapeString("\\ \" \a \b \f \r \t"));
}
TEST(Utils, TimeFromSnowflake) {
	EXPECT_EQ(1455907582, discpp::TimeFromSnowflake("150312037426135041"));
}
TEST(Utils, FormatTimeFromSnowflake) {
	EXPECT_EQ("2016-02-19 @ 18:46:22 GMT", discpp::FormatTimeFromSnowflake("150312037426135041"));
}