#ifndef BFSCRAWLER_HPP_
#define BFSCRAWLER_HPP_

#include <queue>
#include "crawler.hpp"

class BFSCrawler : public Crawler {
public:

    BFSCrawler(Configuration const& a_config);
    ~BFSCrawler() override = default;

private:

    void crawl_using_specific_mode(std::vector<std::string> const& a_startUrls) override;
    void crawl_page(std::string const& a_url, unsigned int a_depth) override;
    void worker_thread() override;

private:

    std::queue<std::pair<std::string, unsigned int>> m_toVisit;
    
};
#endif // BFSCRAWLER_HPP_


