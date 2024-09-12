#ifndef BFSCRAWLER_HPP_
#define BFSCRAWLER_HPP_

#include <queue>
#include <thread>
#include<mutex>
#include<condition_variable>
#include "crawler.hpp"
#include "configuration.hpp"
#include "pageParser.hpp"
#include "downloader.hpp"

class BFSCrawler : public Crawler {
public:

    BFSCrawler(Configuration const& a_config);
    ~BFSCrawler() override = default;

    void start_crawling() override;
    std::unordered_map<std::string, std::unordered_map<std::string, unsigned int>> words_index() const override;
    std::unordered_map<std::string, std::string> page_titles() const override;

private:

    void crawl_page(std::string const& a_url, unsigned int a_depth) override;
    bool should_skip_page(std::string const& a_url, unsigned int a_depth) const override;
    std::string extract_domain(std::string const& a_url) const override;
    bool is_valid_link(std::string const& a_URL) const override;
    void print_information() const override;
    void bfs_crawl(std::vector<std::string> const& a_startUrls);
    void worker_thread();

private:

    unsigned int m_page_count;
    unsigned int m_ignored_links_count;
    Configuration const& m_config;
    Downloader m_downloader;
    PageParser m_parser;
    std::unordered_set<std::string> m_visited_pages;
    std::unordered_map<std::string, std::set<std::string>> m_page_linkes;
    std::unordered_map<std::string, std::unordered_map<std::string, unsigned int>> m_words_index;
    std::unordered_map<std::string, std::string> m_page_titles;
    
    std::mutex m_mutex;
    std::condition_variable m_cond_var;
    std::queue<std::pair<std::string, unsigned int>> m_toVisit;
    bool m_done = false;
    std::vector<std::thread> m_threads;
    
};

#endif // BFSCRAWLER_HPP_
