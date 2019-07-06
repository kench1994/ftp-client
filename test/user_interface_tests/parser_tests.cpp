#include <gtest/gtest.h>
#include <string>
#include "parser.hpp"

using namespace std::literals::string_literals;
using std::pair;
using std::vector;
using std::string;

TEST(ParserTest, ParseCommandTest)
{
    EXPECT_EQ(parse_command(""), pair(""s, vector<string>{}));

    EXPECT_EQ(parse_command("pwd"), pair("pwd"s, vector<string>{}));

    EXPECT_EQ(parse_command("open ftp.cisco.com 21"),
              pair("open"s, vector{"ftp.cisco.com"s, "21"s}));

    EXPECT_EQ(parse_command("get remote_file local_file"),
              pair("get"s, vector{"remote_file"s, "local_file"s}));

    EXPECT_EQ(parse_command("get \"   filename.txt   \""),
              pair("get"s, vector{"   filename.txt   "s}));

    EXPECT_EQ(parse_command("get \"/public/dir 1/file_name.txt\" \"tmp/dir 2/file.txt\""),
              pair("get"s, vector{"/public/dir 1/file_name.txt"s, "tmp/dir 2/file.txt"s}));

    EXPECT_EQ(parse_command("get \"/ public / dir 1 /  file_name  \" \"tmp/dir 2/file\""),
              pair("get"s, vector{"/ public / dir 1 /  file_name  "s, "tmp/dir 2/file"s}));
}
