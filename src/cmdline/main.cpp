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
#include <boost/shared_array.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>

class AsioThreadPool
{
public:
	explicit AsioThreadPool(size_t size)
		: m_to_work(m_io_service)
		, m_io_strand(m_io_service)
	{
		for (size_t i = 0; i < size; ++i) {
			m_trdWorkerGroup.create_thread(
				boost::bind(&boost::asio::io_service::run, &m_io_service));
		}
	}
	~AsioThreadPool()
	{
		m_io_service.stop();
		m_trdWorkerGroup.join_all();
	}
	// Add new work item to the pool.
	template<class T>
	void push_back(T t) {
		m_io_strand.post(t);
	}
private:
	boost::thread_group m_trdWorkerGroup;
	boost::asio::io_service m_io_service;
	boost::asio::io_service::strand m_io_strand;
	boost::asio::io_service::work m_to_work;
};

class FtpObserver : public ftp::client::event_observer
{
    public:
        void on_reply(const std::string & reply) override
        {
            /* Replace unpredictable data. */
            //std::string result = regex_replace(reply,\
                                            regex(R"(229 Entering extended passive mode /(/|/|/|/d{1,5}/|/)/.)"),\
                                                    "229 Entering extended passive mode (|||1234|).");
            //m_replies.append(result);
        }

        const std::string & get_replies() const
        {
            return m_replies;
        }

    private:
        std::string m_replies;
};

typedef struct tagFtpSession{
    tagFtpSession(){
        uDoneCnt = 0;
    }
    unsigned int uDoneCnt;
    std::unique_ptr<FtpObserver> spObserver;
    std::unique_ptr<ftp::client> spFtpClient;
    std::unique_ptr<ftp::detail::data_connection> spDataConn;
}FtpSession;

constexpr unsigned int TERM_CNT = 500;
constexpr unsigned int FILE_BLOCK = 400;///1600;

void parallel_work(uint64_t ullTaskCnt, const std::function<void(uint64_t, uint64_t)>& fnWork) {
	//硬件并发能力
	auto const uRealTrdCnt = std::thread::hardware_concurrency();
	//每个线程分配任务数
	uint64_t const ulTaskCntPerTrd = ullTaskCnt / uRealTrdCnt;


	uint64_t ullBeginPos = 0, ullEndpos = 0;
	std::vector<std::thread> vTrdWorkers(uRealTrdCnt - 1);
	//填充到线程
	for (unsigned int uTdxIdx = 0; uTdxIdx < (uRealTrdCnt - 1); uTdxIdx++) {
		ullEndpos = ullBeginPos + ulTaskCntPerTrd;
		vTrdWorkers[uTdxIdx] = std::thread([ullBeginPos, ullEndpos, fnWork]() {
			fnWork(ullBeginPos, ullEndpos);
		});
		ullBeginPos = ullEndpos;
	}

	fnWork(ullEndpos, ullTaskCnt);
	//等待子线程完成  
	std::for_each(vTrdWorkers.begin(), vTrdWorkers.end(), std::mem_fn(&std::thread::join));
}


int main(int argc, char *argv[])
{
    //100M的文件 100 * 1024KB
	//每次发送64K
	//需要分1600次发送
    std::array<boost::shared_array<char>, FILE_BLOCK> fileBlocks;
	for (unsigned int i = 0; i < FILE_BLOCK; i++)
	{
		boost::shared_array<char> spBuffer(new char[65536]);
		memset(spBuffer.get(), i + 1, 65536);
		fileBlocks[i] = std::move(spBuffer);
	}
	printf("dts prepare done!\n");

    std::unordered_map<unsigned int, std::unique_ptr<FtpSession>> hashmapFtps;
    for (unsigned int i = 0; i < TERM_CNT; i++)
	{
        auto spFtpSession = std::make_unique<FtpSession>();
        spFtpSession->spObserver = std::make_unique<FtpObserver>();
        spFtpSession->spFtpClient = std::make_unique<ftp::client>(spFtpSession->spObserver.get());
		hashmapFtps.emplace(i, std::move(spFtpSession));
    }

    {
        //todo:抽象成类
        //区间begin，end 区间任务 lambda
        auto session_prepare_work = [&hashmapFtps](uint64_t begin, uint64_t end) {
            for (auto i = begin; i < end; i++) {
                auto *pFtpSession = hashmapFtps.at(i).get();
                auto *pFtpClient = pFtpSession->spFtpClient.get();

                if(pFtpClient->open("fe80::1205:14e1:f17a:8b8a", 20182) && pFtpClient->login("server12345", "server12345"))
                    continue;
                std::cerr << "failed!" << std::endl;
            }
        };
        parallel_work(TERM_CNT, session_prepare_work);

        std::cout << "session prepare done" << std::endl;
    }

    {
        auto prepare_upload = [&hashmapFtps](uint64_t begin, uint64_t end) {
            for (auto i = begin; i < end; i++) {
                auto *pFtpSession = hashmapFtps.at(i).get();
                auto *pFtpClient = pFtpSession->spFtpClient.get();
                
                auto spDataConn = std::move(pFtpClient->prepare_upload(std::to_string(i).append(".txt")));
                if(spDataConn){
                    pFtpSession->spDataConn = std::move(spDataConn);
                    continue;
                }
                std::cerr << "failed!" << std::endl;
            }
        };
        parallel_work(TERM_CNT, prepare_upload);
    }

    AsioThreadPool trd_pool(12);
	for (unsigned int i = 0; i < TERM_CNT;)
	{
        auto *pFtpSession = hashmapFtps.at(i).get();

		trd_pool.push_back([pFtpSession, &fileBlocks, i]() {

			if (FILE_BLOCK == pFtpSession->uDoneCnt)
				return;
            if(!pFtpSession->spDataConn)
            {
                ++pFtpSession->uDoneCnt;
                return;
            }
            auto *pDataConn = pFtpSession->spDataConn.get();
            auto *pFtpClient = pFtpSession->spFtpClient.get();

            pDataConn->send(fileBlocks[pFtpSession->uDoneCnt].get(), 65536);
            printf("客户端 %0d 发送数据分片 %d \n", i + 1, ++pFtpSession->uDoneCnt);

            if (FILE_BLOCK == pFtpSession->uDoneCnt)
            {
                if(pFtpClient->upload_cache(pDataConn, "\r\n", strlen("\r\n")))
                {
                    printf("客户端 %0d 发送完成\n", i + 1);

                    pFtpClient->close();
                    return;
                }
                std::cerr << "failed!" << std::endl;
                return;
            }
		});

		if (TERM_CNT == i + 1)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			if (FILE_BLOCK == pFtpSession->uDoneCnt)
				break;
			i = 0;
			continue;
		}
		i++;
		
		//
	}


    getchar();
    return EXIT_SUCCESS;
}
