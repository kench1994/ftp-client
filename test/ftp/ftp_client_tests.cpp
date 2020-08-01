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
#include "ftp/ftp_client.hpp"

using std::string;

using namespace ftp;

class FtpClientTest : public ::testing::Test
{
public:
    FtpClientTest()
    {
        m_pythonPath = boost::process::search_path("python3");
    }

protected:
    void SetUp() override
    {
        /* Usage: python ftp_server.py port user password home_directory */
        m_ftpServerProcess = boost::process::child(m_pythonPath,
                                                   "../ftp/ftp_server/ftp_server.py",
                                                   "2121", "user", "password", ".");

        /* Wait for 250ms to allow the server to start. */
        m_ftpServerProcess.wait_for(std::chrono::milliseconds(250));
    }

    void TearDown() override
    {
        m_ftpServerProcess.terminate();
    }

private:
    boost::filesystem::path m_pythonPath;
    boost::process::child m_ftpServerProcess;
};

class test_ftp_observer : public ftp_client::event_observer
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
    ftp_client client;
    client.subscribe(&ftp_observer);

    EXPECT_FALSE(client.is_open());

    command_result result = client.open("localhost", 2121);
    EXPECT_EQ(result, command_result::ok);
    EXPECT_TRUE(client.is_open());

    result = client.close();
    EXPECT_EQ(result, command_result::ok);
    EXPECT_FALSE(client.is_open());

    EXPECT_EQ(ftp_observer.get_replies(), "220 FTP server is ready.\r\n"
                                          "221 Goodbye.\r\n");
    EXPECT_EQ(ftp_observer.get_errors(), "");
}

TEST_F(FtpClientTest, LoginTest)
{
    test_ftp_observer ftp_observer;
    ftp_client client;
    client.subscribe(&ftp_observer);

    command_result result = client.open("localhost", 2121);
    EXPECT_EQ(result, command_result::ok);

    result = client.login("user", "password");
    EXPECT_EQ(result, command_result::ok);

    result = client.close();
    EXPECT_EQ(result, command_result::ok);

    EXPECT_EQ(ftp_observer.get_replies(), "220 FTP server is ready.\r\n"
                                          "331 Username ok, send password.\r\n"
                                          "230 Login successful.\r\n"
                                          "221 Goodbye.\r\n");
    EXPECT_EQ(ftp_observer.get_errors(), "");
}