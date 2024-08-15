#ifndef CRAWLER_HPP_
#define CRAWLER_HPP_

#include<string>
#include<unordered_set>
#include<unordered_map>
#include<set>

#include"configuration.hpp"
#include"downloader.hpp"
#include "pageParser.hpp"

class Crawler {
public:

    Crawler(Configuration const& a_config);

    void start_crawling();
    bool is_valid_link(std::string const& a_URL) const;
    void save_in_DB();
    std::unordered_map<std::string, std::unordered_map<std::string, unsigned int> > words_index() ;
    std::unordered_map<std::string, std::string> page_titles() ;
    
private:

    void bfs_crawl(std::vector<std::string> const& a_startUrls); 
    //void dfs_crawl(std::vector<std::string> const& a_startUrls); 
    void crawl_page(std::string const& a_url, unsigned int a_depth);
    bool should_skip_page(std::string const& a_url, unsigned int a_depth) const; 
    std::string extract_domain(std::string const& a_url) const;
    void print_information() const;
    


private:

    using unordered_map_page_with_count = std::unordered_map<std::string, unsigned int>;

    unsigned int m_page_count;
    unsigned int m_ignored_links_count;

    const Configuration& m_config;
    Downloader m_downloader;
    PageParser m_parser;

    std::unordered_set<std::string> m_visited_pages;
    std::unordered_map<std::string, std::set<std::string>> m_page_linkes;
    std::unordered_map<std::string, unordered_map_page_with_count > m_words_index;
    
    std::unordered_map<std::string, std::string> m_page_titles;

};

#endif // CRAWLER_HPP_

