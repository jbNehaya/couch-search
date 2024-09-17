#ifndef CRAWLER_HPP_
#define CRAWLER_HPP_

#include<string>
#include<unordered_set>
#include<unordered_map>
#include<set>
#include <thread>
#include<mutex>
#include<condition_variable>
#include"configuration.hpp"
#include"downloader.hpp"
#include "pageParser.hpp"
#include "ICrawler.hpp"

class Crawler : public ICrawler {
public:

    Crawler(Configuration const& a_config);
    virtual ~Crawler() = default;

    virtual void start_crawling() override;
    std::unordered_map<std::string, std::unordered_map<std::string, unsigned int>> words_index() const;
    std::unordered_map<std::string, std::string> page_titles() const;

protected:

    virtual void crawl_using_specific_mode(std::vector<std::string> const& a_startUrls) = 0;
    virtual void crawl_page(std::string const& a_url, unsigned int a_depth) = 0;
    virtual void worker_thread() = 0;
    bool should_skip_page(std::string const& a_url, unsigned int a_depth) const;
    std::string extract_domain(std::string const& a_url) const;
    bool is_valid_link(std::string const& a_URL) const;
    void print_information() const;

protected:
    
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
    bool m_done = false;
    std::vector<std::thread> m_threads;
    
};

/*
class ICrawler {
public:
    virtual void start() = 0;
};


template <typename SearchPolicy>
class Crawler {
public:
    void start() override {
        SerachPolicy container;
        
    }
};
*/
#endif // CRAWLER_HPP_


