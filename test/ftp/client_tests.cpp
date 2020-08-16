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
#include <boost/process.hpp>
#include "ftp/client.hpp"

using std::string;

class FtpClientTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        boost::filesystem::path pythonPath = boost::process::search_path("python3");

        /* Usage: python server.py port user password home_directory */
        m_ftpServerProcess = boost::process::child(pythonPath, "../ftp/server/server.py",
                                                   "2121", "user", "password", ".");

        /* Wait for 2s to allow the server to start. */
        m_ftpServerProcess.wait_for(std::chrono::seconds(2));
    }

    static void TearDownTestSuite()
    {
        m_ftpServerProcess.terminate();
    }

private:
    static boost::process::child m_ftpServerProcess;
};

boost::process::child FtpClientTest::m_ftpServerProcess;

class test_ftp_observer : public ftp::client::event_observer
{
public:
    void on_reply(const string & reply) override
    {
        replies_.append(reply);
    }

    void on_error(const string & error) override
    {
        errors_.append(error);
    }

    const string & get_replies() const
    {
        return replies_;
    }

    const string & get_errors() const
    {
        return errors_;
    }

private:
    string replies_;
    string errors_;
};

TEST_F(FtpClientTest, OpenConnectionTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client;
    client.subscribe(&ftp_observer);

    EXPECT_FALSE(client.is_open());

    ftp::command_result result = client.open("localhost", 2121);
    EXPECT_EQ(result, ftp::command_result::ok);
    EXPECT_TRUE(client.is_open());

    result = client.close();
    EXPECT_EQ(result, ftp::command_result::ok);
    EXPECT_FALSE(client.is_open());

    EXPECT_EQ(ftp_observer.get_replies(), "220 FTP server is ready.\r\n"
                                          "221 Goodbye.\r\n");
    EXPECT_EQ(ftp_observer.get_errors(), "");
}

TEST_F(FtpClientTest, LoginTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client;
    client.subscribe(&ftp_observer);

    ftp::command_result result = client.open("localhost", 2121);
    EXPECT_EQ(result, ftp::command_result::ok);

    result = client.login("user", "password");
    EXPECT_EQ(result, ftp::command_result::ok);

    result = client.close();
    EXPECT_EQ(result, ftp::command_result::ok);

    EXPECT_EQ(ftp_observer.get_replies(), "220 FTP server is ready.\r\n"
                                          "331 Username ok, send password.\r\n"
                                          "230 Login successful.\r\n"
                                          "221 Goodbye.\r\n");
    EXPECT_EQ(ftp_observer.get_errors(), "");
}

TEST_F(FtpClientTest, NoopCommandTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client;
    client.subscribe(&ftp_observer);

    ftp::command_result result = client.open("localhost", 2121);
    EXPECT_EQ(result, ftp::command_result::ok);

    result = client.login("user", "password");
    EXPECT_EQ(result, ftp::command_result::ok);

    result = client.noop();
    EXPECT_EQ(result, ftp::command_result::ok);

    result = client.close();
    EXPECT_EQ(result, ftp::command_result::ok);

    EXPECT_EQ(ftp_observer.get_replies(), "220 FTP server is ready.\r\n"
                                          "331 Username ok, send password.\r\n"
                                          "230 Login successful.\r\n"
                                          "200 I successfully done nothin'.\r\n"
                                          "221 Goodbye.\r\n");
    EXPECT_EQ(ftp_observer.get_errors(), "");
}
