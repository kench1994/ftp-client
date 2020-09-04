/*
 * MIT License
 *
 * Copyright (c) 2020 Denis Kovalchuk
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <gtest/gtest.h>
#include <string>
#include "cmdline/command_parser.hpp"
#include "cmdline/command.hpp"
#include "cmdline/cmdline_exception.hpp"

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

    EXPECT_EQ(parse_command("put local_file remote_file"),
              pair(command::put, vector{"local_file"s, "remote_file"s}));
    
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

    EXPECT_EQ(parse_command("rmdir dir"), pair(command::rmdir, vector<string>{"dir"s}));

    EXPECT_EQ(parse_command("del file"), pair(command::del, vector<string>{"file"s}));

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
    bool catched = false;

    try
    {
        parse_command("some invalid command");
    }
    catch (const cmdline_exception & ex)
    {
        catched = true;
        ASSERT_STREQ(ex.what(), "Invalid command.");
    }
    catch (...)
    {
        FAIL() << "Got unexpected exception.";
    }

    ASSERT_TRUE(catched);
}

TEST(ParserTest, ParseCaseInsensitiveCommandTest)
{
    EXPECT_EQ(parse_command("STAT"), pair(command::stat, vector<string>{}));

    EXPECT_EQ(parse_command("SysT"), pair(command::syst, vector<string>{}));

    EXPECT_EQ(parse_command("BiNaRy"), pair(command::binary, vector<string>{}));
}
