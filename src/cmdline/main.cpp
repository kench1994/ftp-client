/*
 * MIT License
 *
 * Copyright (c) 2019 Denis Kovalchuk
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

#include "ftp/client.hpp"
#include <iostream>
#include <regex>

using std::regex;
using std::string;
using std::ifstream;
using std::streamsize;
using std::runtime_error;

class FtpObserver : public ftp::client::event_observer
{
    public:
        void on_reply(const string & reply) override
        {
            /* Replace unpredictable data. */
            string result = regex_replace(reply,
                                            regex(R"(229 Entering extended passive mode /(/|/|/|/d{1,5}/|/)/.)"),
                                                    "229 Entering extended passive mode (|||1234|).");
            std::cout << result << std::endl;
            m_replies.append(result);
        }

        const string & get_replies() const
        {
            return m_replies;
        }

    private:
        string m_replies;
};

int main(int argc, char *argv[])
{
    FtpObserver observer;
    ftp::client client(&observer);

    client.open("localhost", 20182);
    client.login("server12345", "server12345");
    client.upload("/home/rhel/Downloads/gcc-8.5.0.tar.gz", "war_and_peace.txt");
    getchar();
    return EXIT_SUCCESS;
}
