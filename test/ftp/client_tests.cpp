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
#include <filesystem>
#include <regex>
#include "ftp/client.hpp"
#include "ftp/ftp_exception.hpp"

using std::regex;
using std::string;

using ftp::ftp_exception;

class FtpClientTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        std::filesystem::create_directory(m_ftpServerDir);
        boost::filesystem::path pythonPath = boost::process::search_path("python3");

        /* Usage: python server.py port user password home_directory */
        m_ftpServerProcess = boost::process::child(pythonPath, "../ftp/server/server.py",
                                                   "2121", "user", "password", m_ftpServerDir);

        /* Wait for 2s to allow the server to start. */
        m_ftpServerProcess.wait_for(std::chrono::seconds(2));
    }

    static void TearDownTestSuite()
    {
        m_ftpServerProcess.terminate();
        std::filesystem::remove_all(m_ftpServerDir);
    }

    void TearDown() override
    {
        /* Clear ftp server directory after each test. */
        for (const auto & path : std::filesystem::directory_iterator(m_ftpServerDir))
        {
            std::filesystem::remove_all(path);
        }
    }

private:
    static const string m_ftpServerDir;
    static boost::process::child m_ftpServerProcess;
};

const string FtpClientTest::m_ftpServerDir = "test_server";
boost::process::child FtpClientTest::m_ftpServerProcess;

class test_ftp_observer : public ftp::client::event_observer
{
public:
    void on_reply(const string & reply) override
    {
        replies_.append(reply);
    }

    const string & get_replies() const
    {
        return replies_;
    }

private:
    string replies_;
};

TEST_F(FtpClientTest, OpenConnectionTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client(&ftp_observer);

    EXPECT_FALSE(client.is_open());

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.is_open());

    EXPECT_TRUE(client.close());
    EXPECT_FALSE(client.is_open());

    ASSERT_EQ(ftp_observer.get_replies(), "220 FTP server is ready.\r\n"
                                          "221 Goodbye.\r\n");
}

TEST_F(FtpClientTest, LoginTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client(&ftp_observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.close());

    ASSERT_EQ(ftp_observer.get_replies(), "220 FTP server is ready.\r\n"
                                          "331 Username ok, send password.\r\n"
                                          "230 Login successful.\r\n"
                                          "221 Goodbye.\r\n");
}

TEST_F(FtpClientTest, LoginNonexistentUserTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client(&ftp_observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_FALSE(client.login("nonexistent", "password"));
    EXPECT_TRUE(client.close());

    ASSERT_EQ(ftp_observer.get_replies(), "220 FTP server is ready.\r\n"
                                          "331 Username ok, send password.\r\n"
                                          "530 Authentication failed.\r\n"
                                          "221 Goodbye.\r\n");
}

TEST_F(FtpClientTest, LoginWrongPasswordTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client(&ftp_observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_FALSE(client.login("user", "wrong password"));
    EXPECT_TRUE(client.close());

    ASSERT_EQ(ftp_observer.get_replies(), "220 FTP server is ready.\r\n"
                                          "331 Username ok, send password.\r\n"
                                          "530 Authentication failed.\r\n"
                                          "221 Goodbye.\r\n");
}

TEST_F(FtpClientTest, NoopTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client(&ftp_observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.noop());
    EXPECT_TRUE(client.close());

    ASSERT_EQ(ftp_observer.get_replies(), "220 FTP server is ready.\r\n"
                                          "331 Username ok, send password.\r\n"
                                          "230 Login successful.\r\n"
                                          "200 I successfully done nothin'.\r\n"
                                          "221 Goodbye.\r\n");
}

TEST_F(FtpClientTest, PwdTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client(&ftp_observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.pwd());
    EXPECT_TRUE(client.close());

    ASSERT_EQ(ftp_observer.get_replies(), "220 FTP server is ready.\r\n"
                                          "331 Username ok, send password.\r\n"
                                          "230 Login successful.\r\n"
                                          "257 \"/\" is the current directory.\r\n"
                                          "221 Goodbye.\r\n");
}

TEST_F(FtpClientTest, MkdirTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client(&ftp_observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.mkdir("directory"));
    EXPECT_TRUE(client.close());

    ASSERT_EQ(ftp_observer.get_replies(), "220 FTP server is ready.\r\n"
                                          "331 Username ok, send password.\r\n"
                                          "230 Login successful.\r\n"
                                          "257 \"/directory\" directory created.\r\n"
                                          "221 Goodbye.\r\n");
}

TEST_F(FtpClientTest, MkdirDirectoryAlreadyExistsTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client(&ftp_observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.mkdir("directory"));
    EXPECT_FALSE(client.mkdir("directory"));
    EXPECT_TRUE(client.close());

    ASSERT_EQ(ftp_observer.get_replies(), "220 FTP server is ready.\r\n"
                                          "331 Username ok, send password.\r\n"
                                          "230 Login successful.\r\n"
                                          "257 \"/directory\" directory created.\r\n"
                                          "550 File exists.\r\n"
                                          "221 Goodbye.\r\n");
}

TEST_F(FtpClientTest, RmdirTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client(&ftp_observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.mkdir("directory"));
    EXPECT_TRUE(client.ls());
    EXPECT_TRUE(client.rmdir("directory"));
    EXPECT_TRUE(client.ls());
    EXPECT_TRUE(client.close());

    /* Replace unpredictable data. */
    string replies = ftp_observer.get_replies();

    replies = regex_replace(replies,
                            regex(R"(229 Entering extended passive mode \(\|\|\|\d{1,5}\|\)\.)"),
                                    "229 Entering extended passive mode (|||1234|).");

    replies = regex_replace(replies,
                            regex(R"(drwxr.*directory)"),
                                    "drwxr-xr-x 2 user staff 64 Aug 23 11:45 directory");

    ASSERT_EQ(replies, "220 FTP server is ready.\r\n"
                       "331 Username ok, send password.\r\n"
                       "230 Login successful.\r\n"
                       "257 \"/directory\" directory created.\r\n"
                       "229 Entering extended passive mode (|||1234|).\r\n"
                       "125 Data connection already open. Transfer starting.\r\n"
                       "drwxr-xr-x 2 user staff 64 Aug 23 11:45 directory\r\n"
                       "226 Transfer complete.\r\n"
                       "250 Directory removed.\r\n"
                       "229 Entering extended passive mode (|||1234|).\r\n"
                       "125 Data connection already open. Transfer starting.\r\n"
                       "226 Transfer complete.\r\n"
                       "221 Goodbye.\r\n");
}

TEST_F(FtpClientTest, RmdirNonexistentDirectoryTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client(&ftp_observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_FALSE(client.rmdir("nonexistent"));
    EXPECT_TRUE(client.close());

    ASSERT_EQ(ftp_observer.get_replies(), "220 FTP server is ready.\r\n"
                                          "331 Username ok, send password.\r\n"
                                          "230 Login successful.\r\n"
                                          "550 No such file or directory.\r\n"
                                          "221 Goodbye.\r\n");
}

TEST_F(FtpClientTest, CdTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client(&ftp_observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.mkdir("directory"));
    EXPECT_TRUE(client.cd("directory"));
    EXPECT_TRUE(client.cd(".."));
    EXPECT_TRUE(client.close());

    ASSERT_EQ(ftp_observer.get_replies(), "220 FTP server is ready.\r\n"
                                          "331 Username ok, send password.\r\n"
                                          "230 Login successful.\r\n"
                                          "257 \"/directory\" directory created.\r\n"
                                          "250 \"/directory\" is the current directory.\r\n"
                                          "250 \"/\" is the current directory.\r\n"
                                          "221 Goodbye.\r\n");
}

TEST_F(FtpClientTest, CdNonexistentDirectoryTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client(&ftp_observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_FALSE(client.cd("nonexistent"));
    EXPECT_TRUE(client.close());

    ASSERT_EQ(ftp_observer.get_replies(), "220 FTP server is ready.\r\n"
                                          "331 Username ok, send password.\r\n"
                                          "230 Login successful.\r\n"
                                          "550 No such file or directory.\r\n"
                                          "221 Goodbye.\r\n");
}

TEST_F(FtpClientTest, LsTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client(&ftp_observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.mkdir("directory"));
    EXPECT_TRUE(client.ls());
    EXPECT_TRUE(client.ls("directory"));
    EXPECT_TRUE(client.close());

    /* Replace unpredictable data. */
    string replies = ftp_observer.get_replies();

    replies = regex_replace(replies,
                            regex(R"(229 Entering extended passive mode \(\|\|\|\d{1,5}\|\)\.)"),
                                    "229 Entering extended passive mode (|||1234|).");

    replies = regex_replace(replies,
                            regex(R"(drwxr.*directory)"),
                                    "drwxr-xr-x 2 user staff 64 Aug 23 11:45 directory");

    ASSERT_EQ(replies, "220 FTP server is ready.\r\n"
                       "331 Username ok, send password.\r\n"
                       "230 Login successful.\r\n"
                       "257 \"/directory\" directory created.\r\n"
                       "229 Entering extended passive mode (|||1234|).\r\n"
                       "125 Data connection already open. Transfer starting.\r\n"
                       "drwxr-xr-x 2 user staff 64 Aug 23 11:45 directory\r\n"
                       "226 Transfer complete.\r\n"
                       "229 Entering extended passive mode (|||1234|).\r\n"
                       "125 Data connection already open. Transfer starting.\r\n"
                       "226 Transfer complete.\r\n"
                       "221 Goodbye.\r\n");
}

TEST_F(FtpClientTest, LsNonexistentDirectoryTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client(&ftp_observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_FALSE(client.ls("nonexistent"));
    EXPECT_TRUE(client.close());

    /* Replace unpredictable data. */
    string replies = ftp_observer.get_replies();

    replies = regex_replace(replies,
                            regex(R"(229 Entering extended passive mode \(\|\|\|\d{1,5}\|\)\.)"),
                            "229 Entering extended passive mode (|||1234|).");

    ASSERT_EQ(replies, "220 FTP server is ready.\r\n"
                       "331 Username ok, send password.\r\n"
                       "230 Login successful.\r\n"
                       "229 Entering extended passive mode (|||1234|).\r\n"
                       "550 No such file or directory.\r\n"
                       "221 Goodbye.\r\n");
}

TEST_F(FtpClientTest, BinaryTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client(&ftp_observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.binary());
    EXPECT_TRUE(client.close());

    ASSERT_EQ(ftp_observer.get_replies(), "220 FTP server is ready.\r\n"
                                          "331 Username ok, send password.\r\n"
                                          "230 Login successful.\r\n"
                                          "200 Type set to: Binary.\r\n"
                                          "221 Goodbye.\r\n");
}

TEST_F(FtpClientTest, UploadTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client(&ftp_observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.ls());
    EXPECT_TRUE(client.binary());
    EXPECT_TRUE(client.upload("../ftp/test_data/war_and_peace.txt", "war_and_peace.txt"));
    EXPECT_TRUE(client.ls());
    EXPECT_TRUE(client.close());

    /* Replace unpredictable data. */
    string replies = ftp_observer.get_replies();

    replies = regex_replace(replies,
                            regex(R"(229 Entering extended passive mode \(\|\|\|\d{1,5}\|\)\.)"),
                            "229 Entering extended passive mode (|||1234|).");

    replies = regex_replace(replies,
                            regex(R"(-rw-r--r--.* 3359584 .*war_and_peace.txt)"),
                            "-rw-r--r-- 1 user staff 3359584 Aug 23 11:45 war_and_peace.txt");

    ASSERT_EQ(replies, "220 FTP server is ready.\r\n"
                       "331 Username ok, send password.\r\n"
                       "230 Login successful.\r\n"
                       "229 Entering extended passive mode (|||1234|).\r\n"
                       "125 Data connection already open. Transfer starting.\r\n"
                       "226 Transfer complete.\r\n"
                       "200 Type set to: Binary.\r\n"
                       "229 Entering extended passive mode (|||1234|).\r\n"
                       "125 Data connection already open. Transfer starting.\r\n"
                       "226 Transfer complete.\r\n"
                       "229 Entering extended passive mode (|||1234|).\r\n"
                       "125 Data connection already open. Transfer starting.\r\n"
                       "-rw-r--r-- 1 user staff 3359584 Aug 23 11:45 war_and_peace.txt\r\n"
                       "226 Transfer complete.\r\n"
                       "221 Goodbye.\r\n");
}

TEST_F(FtpClientTest, UploadNonexistentFileTest)
{
    test_ftp_observer ftp_observer;
    ftp::client client(&ftp_observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.ls());

    bool catched = false;

    try
    {
        client.upload("nonexistent", "nonexistent");
    }
    catch (const ftp_exception & ex)
    {
        catched = true;
        EXPECT_STREQ(ex.what(), "Cannot open file nonexistent.");
    }

    EXPECT_TRUE(catched);
    EXPECT_TRUE(client.ls());
    EXPECT_TRUE(client.close());

    /* Replace unpredictable data. */
    string replies = ftp_observer.get_replies();

    replies = regex_replace(replies,
                            regex(R"(229 Entering extended passive mode \(\|\|\|\d{1,5}\|\)\.)"),
                            "229 Entering extended passive mode (|||1234|).");

    ASSERT_EQ(replies, "220 FTP server is ready.\r\n"
                       "331 Username ok, send password.\r\n"
                       "230 Login successful.\r\n"
                       "229 Entering extended passive mode (|||1234|).\r\n"
                       "125 Data connection already open. Transfer starting.\r\n"
                       "226 Transfer complete.\r\n"
                       "229 Entering extended passive mode (|||1234|).\r\n"
                       "125 Data connection already open. Transfer starting.\r\n"
                       "226 Transfer complete.\r\n"
                       "221 Goodbye.\r\n");
}
