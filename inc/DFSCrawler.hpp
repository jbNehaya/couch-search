#ifndef DFSCRAWLER_HPP_
#define DFSCRAWLER_HPP_

#include <stack>
#include "crawler.hpp"

class DFSCrawler : public Crawler {
public:

    DFSCrawler(Configuration const& a_config);
    ~DFSCrawler() override = default;

private:

    void crawl_using_specific_mode(std::vector<std::string> const& a_startUrls) override;
    void crawl_page(std::string const& a_url, unsigned int a_depth) override;
    void worker_thread() override;

private:

    std::stack<std::pair<std::string, unsigned int>> m_toVisit;

};
#endif // DFSCRAWLER_HPP_


