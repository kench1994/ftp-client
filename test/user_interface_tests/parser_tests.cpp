#include <gtest/gtest.h>
#include <string>
#include "parser.hpp"
#include "command.hpp"
#include "local_exception.hpp"

using namespace std::literals::string_literals;
using std::pair;
using std::vector;
using std::string;

TEST(ParserTest, ParseCommandTest)
{
    EXPECT_EQ(parse_command("open ftp.cisco.com 21"),
              pair(command::open, vector{"ftp.cisco.com"s, "21"s}));

    EXPECT_EQ(parse_command("user anonymous"), pair(command::user, vector<string>{"anonymous"s}));

    EXPECT_EQ(parse_command("cd ../home/dir/"), pair(command::cd, vector<string>{"../home/dir/"s}));

    EXPECT_EQ(parse_command("ls ."), pair(command::ls, vector<string>{"."s}));

    EXPECT_EQ(parse_command("get remote_file local_file"),
              pair(command::get, vector{"remote_file"s, "local_file"s}));

    EXPECT_EQ(parse_command("get \"   filename.txt   \""),
              pair(command::get, vector{"   filename.txt   "s}));

    EXPECT_EQ(parse_command("get \"/public/dir 1/file_name.txt\" \"tmp/dir 2/file.txt\""),
              pair(command::get, vector{"/public/dir 1/file_name.txt"s, "tmp/dir 2/file.txt"s}));

    EXPECT_EQ(parse_command("get \"/ public / dir 1 /  file_name  \" \"tmp/dir 2/file\""),
              pair(command::get, vector{"/ public / dir 1 /  file_name  "s, "tmp/dir 2/file"s}));

    EXPECT_EQ(parse_command("pwd"), pair(command::pwd, vector<string>{}));

    EXPECT_EQ(parse_command("mkdir dir"), pair(command::mkdir, vector<string>{"dir"s}));

    EXPECT_EQ(parse_command("stat"), pair(command::stat, vector<string>{}));

    EXPECT_EQ(parse_command("syst"), pair(command::syst, vector<string>{}));

    EXPECT_EQ(parse_command("binary"), pair(command::binary, vector<string>{}));

    EXPECT_EQ(parse_command("size filename"), pair(command::size, vector<string>{"filename"s}));

    EXPECT_EQ(parse_command("noop"), pair(command::noop, vector<string>{}));

    EXPECT_EQ(parse_command("close"), pair(command::close, vector<string>{}));

    EXPECT_EQ(parse_command("help"), pair(command::help, vector<string>{}));

    EXPECT_EQ(parse_command("exit"), pair(command::exit, vector<string>{}));
}

TEST(ParserTest, ParseInvalidCommandTest)
{
    bool gotException = false;

    try
    {
        parse_command("some invalid command");
    }
    catch (const local_exception & ex)
    {
        gotException = true;
        ASSERT_STREQ(ex.what(), "Invalid command.");
    }
    catch (...)
    {
        FAIL() << "Got unexpected exception.";
    }

    ASSERT_EQ(gotException, true);
}

TEST(ParserTest, ParseCaseInsensitiveCommandTest)
{
    EXPECT_EQ(parse_command("STAT"), pair(command::stat, vector<string>{}));

    EXPECT_EQ(parse_command("SysT"), pair(command::syst, vector<string>{}));

    EXPECT_EQ(parse_command("BiNaRy"), pair(command::binary, vector<string>{}));
}
