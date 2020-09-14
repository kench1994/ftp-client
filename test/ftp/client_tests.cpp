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
using std::ifstream;
using std::streamsize;
using std::runtime_error;

using ftp::ftp_exception;

class FtpClientTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        std::filesystem::create_directory(m_downloadsDir);
        std::filesystem::create_directory(m_ftpServerDir);
        boost::filesystem::path pythonPath = boost::process::search_path("python3");

        /* Usage: python server.py port home_directory */
        m_ftpServerProcess = boost::process::child(pythonPath,
                                                   "../ftp/server/server.py", "2121",
                                                   m_ftpServerDir);

        /* Wait for 2s to allow the server to start. */
        m_ftpServerProcess.wait_for(std::chrono::seconds(2));
    }

    static void TearDownTestSuite()
    {
        m_ftpServerProcess.terminate();
        std::filesystem::remove_all(m_ftpServerDir);
        std::filesystem::remove_all(m_downloadsDir);
    }

    void TearDown() override
    {
        /* Clear ftp server directory after each test. */
        for (const auto & path : std::filesystem::directory_iterator(m_ftpServerDir))
        {
            std::filesystem::remove_all(path);
        }

        /* Clear downloads directory after each test. */
        for (const auto & path : std::filesystem::directory_iterator(m_downloadsDir))
        {
            std::filesystem::remove_all(path);
        }
    }

    static bool compareFiles(const string & path1, const string & path2)
    {
        ifstream f1(path1, ifstream::binary | ifstream::ate);
        if (!f1)
            throw runtime_error("Cannot open file: " + path1);

        ifstream f2(path2, ifstream::binary | ifstream::ate);
        if (!f2)
            throw runtime_error("Cannot open file: " + path2);

        if (f1.tellg() != f2.tellg())
            return false;

        f1.seekg(0);
        f2.seekg(0);
        while (!f1.eof())
        {
            const streamsize buffSize = 4096;
            char buff1[buffSize];
            char buff2[buffSize];

            f1.read(buff1, buffSize);
            if (f1.fail() && !f1.eof())
                throw runtime_error("Cannot read data from file: " + path1);

            f2.read(buff2, buffSize);
            if (f2.fail() && !f2.eof())
                throw runtime_error("Cannot read data from file: " + path2);

            if (memcmp(buff1, buff2, f1.gcount()) != 0)
                return false;
        }

        return true;
    }

    template<typename ...Strings>
    static string CRLF(Strings && ...strings)
    {
        return (... + (strings + string("\r\n")));
    }

    class TestFtpObserver : public ftp::client::event_observer
    {
    public:
        void on_reply(const string & reply) override
        {
            /* Replace unpredictable data. */
            string result = regex_replace(reply,
                                          regex(R"(229 Entering extended passive mode \(\|\|\|\d{1,5}\|\)\.)"),
                                                  "229 Entering extended passive mode (|||1234|).");

            m_replies.append(result);
        }

        const string & get_replies() const
        {
            return m_replies;
        }

    private:
        string m_replies;
    };
private:
    static const string m_downloadsDir;
    static const string m_ftpServerDir;
    static boost::process::child m_ftpServerProcess;
};

const string FtpClientTest::m_downloadsDir = "downloads";
const string FtpClientTest::m_ftpServerDir = "test_server";
boost::process::child FtpClientTest::m_ftpServerProcess;

TEST_F(FtpClientTest, OpenConnectionTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_FALSE(client.is_open());

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.is_open());

    EXPECT_TRUE(client.close());
    EXPECT_FALSE(client.is_open());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, ConnectionIsNotOpenTest)
{
    ftp::client client;
    bool catched = false;

    try
    {
        client.ls();
    }
    catch (const ftp_exception & ex)
    {
        catched = true;
        EXPECT_STREQ("Connection is not open.", ex.what());
    }

    EXPECT_TRUE(catched);
}

TEST_F(FtpClientTest, LoginTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "230 Login successful.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, LoginNonexistentUserTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_FALSE(client.login("nonexistent", "password"));
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "530 Authentication failed.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, LoginWrongPasswordTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_FALSE(client.login("user", "wrong password"));
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "530 Authentication failed.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, NoopTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.noop());
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "230 Login successful.",
                   "200 I successfully done nothin'.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, PwdTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.pwd());
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "230 Login successful.",
                   "257 \"/\" is the current directory.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, MkdirTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.mkdir("directory"));
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "230 Login successful.",
                   "257 \"/directory\" directory created.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, MkdirDirectoryAlreadyExistsTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.mkdir("directory"));
    EXPECT_FALSE(client.mkdir("directory"));
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "230 Login successful.",
                   "257 \"/directory\" directory created.",
                   "550 File exists.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, RmdirTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.mkdir("directory"));
    EXPECT_TRUE(client.rmdir("directory"));
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "230 Login successful.",
                   "257 \"/directory\" directory created.",
                   "250 Directory removed.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, RmdirNonexistentDirectoryTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_FALSE(client.rmdir("nonexistent"));
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "230 Login successful.",
                   "550 No such file or directory.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, RmTest)
{
    ftp::client client;

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.upload("../ftp/test_data/war_and_peace.txt", "war_and_peace.txt"));
    EXPECT_TRUE(client.rm("war_and_peace.txt"));
    EXPECT_TRUE(client.close());
}

TEST_F(FtpClientTest, RmNonexistentFileTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_FALSE(client.rm("nonexistent"));
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "230 Login successful.",
                   "550 No such file or directory.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, CdTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.mkdir("directory"));
    EXPECT_TRUE(client.cd("directory"));
    EXPECT_TRUE(client.cd(".."));
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "230 Login successful.",
                   "257 \"/directory\" directory created.",
                   "250 \"/directory\" is the current directory.",
                   "250 \"/\" is the current directory.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, CdNonexistentDirectoryTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_FALSE(client.cd("nonexistent"));
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "230 Login successful.",
                   "550 No such file or directory.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, LsTest)
{
    ftp::client client;

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.mkdir("directory"));
    EXPECT_TRUE(client.ls());
    EXPECT_TRUE(client.ls("directory"));
    EXPECT_TRUE(client.close());
}

TEST_F(FtpClientTest, LsNonexistentDirectoryTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_FALSE(client.ls("nonexistent"));
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "230 Login successful.",
                   "229 Entering extended passive mode (|||1234|).",
                   "550 No such file or directory.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, BinaryTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.binary());
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "230 Login successful.",
                   "200 Type set to: Binary.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, UploadTest)
{
    ftp::client client;

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.binary());
    EXPECT_TRUE(client.upload("../ftp/test_data/war_and_peace.txt", "war_and_peace.txt"));
    EXPECT_TRUE(client.mkdir("directory"));
    EXPECT_TRUE(client.upload("../ftp/test_data/war_and_peace.txt", "directory/war_and_peace.txt"));
    EXPECT_TRUE(client.close());

    EXPECT_TRUE(compareFiles("../ftp/test_data/war_and_peace.txt", "test_server/war_and_peace.txt"));
    EXPECT_TRUE(compareFiles("../ftp/test_data/war_and_peace.txt", "test_server/directory/war_and_peace.txt"));
}

TEST_F(FtpClientTest, UploadOnNonexistentPathTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.binary());
    EXPECT_FALSE(client.upload("../ftp/test_data/war_and_peace.txt", "nonexistent/war_and_peace.txt"));
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "230 Login successful.",
                   "200 Type set to: Binary.",
                   "229 Entering extended passive mode (|||1234|).",
                   "550 No such file or directory.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, UploadNonexistentFileTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));

    bool catched = false;

    try
    {
        client.upload("nonexistent", "nonexistent");
    }
    catch (const ftp_exception & ex)
    {
        catched = true;
        EXPECT_STREQ("Cannot open file 'nonexistent'.", ex.what());
    }

    EXPECT_TRUE(catched);
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "230 Login successful.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, DownloadTest)
{
    ftp::client client;

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.binary());
    EXPECT_TRUE(client.upload("../ftp/test_data/war_and_peace.txt", "war_and_peace.txt"));
    EXPECT_TRUE(client.download("war_and_peace.txt", "downloads/war_and_peace.txt"));
    EXPECT_TRUE(client.close());

    EXPECT_TRUE(compareFiles("../ftp/test_data/war_and_peace.txt", "downloads/war_and_peace.txt"));
}

TEST_F(FtpClientTest, DownloadNonexistentFileTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.binary());
    EXPECT_FALSE(client.download("nonexistent", "downloads/nonexistent"));
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "230 Login successful.",
                   "200 Type set to: Binary.",
                   "229 Entering extended passive mode (|||1234|).",
                   "550 No such file or directory.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, DownloadFileAlreadyExistsTest)
{
    ftp::client client;

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.binary());
    EXPECT_TRUE(client.upload("../ftp/test_data/war_and_peace.txt", "war_and_peace.txt"));
    EXPECT_TRUE(client.download("war_and_peace.txt", "downloads/war_and_peace.txt"));

    bool catched = false;

    try
    {
        client.download("war_and_peace.txt", "downloads/war_and_peace.txt");
    }
    catch (const ftp_exception & ex)
    {
        catched = true;
        EXPECT_STREQ("The file 'downloads/war_and_peace.txt' already exists.", ex.what());
    }

    EXPECT_TRUE(catched);
    EXPECT_TRUE(client.close());
}

TEST_F(FtpClientTest, StatTest)
{
    ftp::client client;

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.stat());
    EXPECT_TRUE(client.close());
}

TEST_F(FtpClientTest, StatDirectoryTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_TRUE(client.stat("."));
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "230 Login successful.",
                   "213-Status of \"/\":",
                   "213 End of status.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, StatNonexistentFileTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.login("user", "password"));
    EXPECT_FALSE(client.stat("nonexistent"));
    EXPECT_TRUE(client.close());

    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "331 Username ok, send password.",
                   "230 Login successful.",
                   "550 No such file or directory.",
                   "221 Goodbye."),
              observer.get_replies());
}

TEST_F(FtpClientTest, SystemTest)
{
    TestFtpObserver observer;
    ftp::client client(&observer);

    EXPECT_TRUE(client.open("localhost", 2121));
    EXPECT_TRUE(client.system());
    EXPECT_TRUE(client.close());

    /* pyftpdlib responds '215 UNIX Type: L8' regardless of the system type. */
    EXPECT_EQ(CRLF("220 FTP server is ready.",
                   "215 UNIX Type: L8",
                   "221 Goodbye."),
              observer.get_replies());
}
