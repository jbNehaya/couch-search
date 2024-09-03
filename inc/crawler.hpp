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
    virtual ~Crawler() = default;

    virtual void start_crawling() = 0;
    virtual std::unordered_map<std::string, std::unordered_map<std::string, unsigned int>> words_index() const = 0;
    virtual std::unordered_map<std::string, std::string> page_titles() const = 0;

protected:
    virtual void crawl_page(std::string const& a_url, unsigned int a_depth) = 0;
    virtual bool should_skip_page(std::string const& a_url, unsigned int a_depth) const = 0;
    virtual std::string extract_domain(std::string const& a_url) const = 0;
    virtual bool is_valid_link(std::string const& a_URL) const = 0;
    virtual void print_information() const = 0;
};

#endif // CRAWLER_HPP_

