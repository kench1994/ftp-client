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
    EXPECT_EQ(pair(command::open, vector{"ftp.cisco.com"s, "21"s}),
              parse_command("open ftp.cisco.com 21"));

    EXPECT_EQ(pair(command::user, vector<string>{"anonymous"s}),
              parse_command("user anonymous"));

    EXPECT_EQ(pair(command::cd, vector<string>{"../home/dir/"s}),
              parse_command("cd ../home/dir/"));

    EXPECT_EQ(pair(command::ls, vector<string>{"."s}),
              parse_command("ls ."));

    EXPECT_EQ(pair(command::put, vector{"local_file"s, "remote_file"s}),
              parse_command("put local_file remote_file"));
    
    EXPECT_EQ(pair(command::get, vector{"remote_file"s, "local_file"s}),
              parse_command("get remote_file local_file"));

    EXPECT_EQ(pair(command::get, vector{"   filename.txt   "s}),
              parse_command("get \"   filename.txt   \""));

    EXPECT_EQ(pair(command::get, vector{"/public/dir 1/file_name.txt"s, "tmp/dir 2/file.txt"s}),
              parse_command("get \"/public/dir 1/file_name.txt\" \"tmp/dir 2/file.txt\""));

    EXPECT_EQ(pair(command::get, vector{"/ public / dir 1 /  file_name  "s, "tmp/dir 2/file"s}),
              parse_command("get \"/ public / dir 1 /  file_name  \" \"tmp/dir 2/file\""));

    EXPECT_EQ(pair(command::pwd, vector<string>{}),
              parse_command("pwd"));

    EXPECT_EQ(pair(command::mkdir, vector<string>{"dir"s}),
              parse_command("mkdir dir"));

    EXPECT_EQ(pair(command::rmdir, vector<string>{"dir"s}),
              parse_command("rmdir dir"));

    EXPECT_EQ(pair(command::del, vector<string>{"file"s}),
              parse_command("del file"));

    EXPECT_EQ(pair(command::stat, vector<string>{}),
              parse_command("stat"));

    EXPECT_EQ(pair(command::syst, vector<string>{}),
              parse_command("syst"));

    EXPECT_EQ(pair(command::binary, vector<string>{}),
              parse_command("binary"));

    EXPECT_EQ(pair(command::size, vector<string>{"filename"s}),
              parse_command("size filename"));

    EXPECT_EQ(pair(command::noop, vector<string>{}),
              parse_command("noop"));

    EXPECT_EQ(pair(command::close, vector<string>{}),
              parse_command("close"));

    EXPECT_EQ(pair(command::help, vector<string>{}),
              parse_command("help"));

    EXPECT_EQ(pair(command::exit, vector<string>{}),
              parse_command("exit"));
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
        ASSERT_STREQ("Invalid command.", ex.what());
    }
    catch (...)
    {
        FAIL() << "Got unexpected exception.";
    }

    ASSERT_TRUE(catched);
}

TEST(ParserTest, ParseCaseInsensitiveCommandTest)
{
    EXPECT_EQ(pair(command::stat, vector<string>{}), parse_command("STAT"));

    EXPECT_EQ(pair(command::syst, vector<string>{}), parse_command("SysT"));

    EXPECT_EQ(pair(command::binary, vector<string>{}), parse_command("BiNaRy"));
}
