#include"queue"
#include <regex>
#include "crawler.hpp"

Crawler::Crawler(Configuration const& a_config)
: m_page_count{0}
, m_ignored_links_count{0}
, m_config{a_config}
{
}

void Crawler::start_crawling() 
{
    const auto& start_Urls = m_config.start_URLs();

    if (m_config.crawl_mode() == CRAWL_MODE::BFS) {
        bfs_crawl(start_Urls);
    } /*else if (m_config.crawl_mode() == CRAWL_MODE::DFS) {
        dfs_crawl(start_Urls);
    }*/

    print_information();
}

std::unordered_map<std::string, std::unordered_map<std::string, unsigned int>> Crawler::words_index() 
{
    return m_words_index;
}

std::unordered_map<std::string, std::string> Crawler::page_titles() 
{
    return m_page_titles;
}

void Crawler::bfs_crawl(std::vector<std::string> const& a_startUrls) 
{
    std::queue<std::pair<std::string, unsigned int>> toVisit;

    for (const auto& url : a_startUrls) {
        toVisit.push({url, 0});
    }

    while (!toVisit.empty() && m_page_count < m_config.max_pages()) {
        auto [url, depth] = toVisit.front();
        toVisit.pop();

        if (should_skip_page(url, depth)) {
            continue;
        }

        crawl_page(url, depth);
        
       for (const auto& link : m_page_linkes[url]) {
            if (is_valid_link(link)) {
                toVisit.push({link, depth + 1});
            } else {
                ++m_ignored_links_count;
            }
        }
    }
}

void Crawler::crawl_page(std::string const& a_url, unsigned int a_depth)
{
    auto content = m_downloader.download_url_content(a_url);
    m_parser.parsing(content);
    
    m_page_linkes[a_url] =  m_parser.page_links();
    for (const auto& [word, count] : m_parser.word_index()) {
        m_words_index[word][a_url] += count;
    }
    m_page_titles[a_url] = m_parser.title();
    m_visited_pages.insert(a_url);
    ++m_page_count;
}

bool Crawler::should_skip_page(std::string const& a_url, unsigned int a_depth) const 
{
    if (m_visited_pages.find(a_url) != m_visited_pages.end()) {
        return true;
    }

    if (a_depth > m_config.max_depth()) {
        return true;
    }

    if (m_config.max_pages() > 0 && m_page_count >= m_config.max_pages()) {
        return true;
    }

    return false;
}

std::string Crawler::extract_domain(std::string const& a_url) const
{
    std::regex domain_regex(R"(https?://([^/]+))");
    std::smatch match;
    if (std::regex_search(a_url, match, domain_regex)) {
        return match[1].str();
    }
    return "";
}

bool Crawler::is_valid_link(std::string const& a_URL) const
{
    std::string current_url_domain = extract_domain(a_URL);
    const auto& start_Urls = m_config.start_URLs();
    
    size_t size = start_Urls.size();
    for(size_t i=0; i< size; ++i){
        std::string start_url_domain = extract_domain(start_Urls[i]);
        if(current_url_domain == start_url_domain){
            return true;
        }
    }
    return false;
}

void Crawler::print_information() const
{
    std::cout << "Crawling finished \n";
    std::cout << "Counts of words indexed: " << m_words_index.size() << "\n";
    std::cout << "Counts of pages crawled: " << m_page_count << "\n";
    std::cout << "Counts of ignored links: " << m_ignored_links_count << "\n";

}

