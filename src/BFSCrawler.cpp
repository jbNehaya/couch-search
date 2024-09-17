#include <regex>
#include<queue>
#include"BFSCrawler.hpp"

BFSCrawler::BFSCrawler(Configuration const& a_config)
: Crawler(a_config)
{
}

void BFSCrawler::crawl_using_specific_mode(std::vector<std::string> const& a_startUrls) 
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& url : a_startUrls) {
            m_toVisit.push({url, 0});
        }
    }
    
    unsigned int num_threads = std::thread::hardware_concurrency();
    for (unsigned int i = 0; i < num_threads; ++i) {
        m_threads.emplace_back(&BFSCrawler::worker_thread, this);
    }
}

void BFSCrawler::worker_thread()
{
    while (true) {
        std::pair<std::string, unsigned int> url_depth;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cond_var.wait(lock, [this]() { return !m_toVisit.empty() || m_done; });
            if (m_done && m_toVisit.empty()) {
                return;
            }
            url_depth = m_toVisit.front();
            m_toVisit.pop();
        }
        
        auto [url, depth] = url_depth;

        if (should_skip_page(url, depth)) {
            continue;
        }

        crawl_page(url, depth);

        std::vector<std::string> links;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            links = std::vector<std::string>(m_page_linkes[url].begin(), m_page_linkes[url].end());
        }

        for (const auto& link : links) {
            if (is_valid_link(link)) {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_toVisit.push({link, depth + 1});
            } else {
                ++m_ignored_links_count;
            }
        }
    }
}

void BFSCrawler::crawl_page(std::string const& a_url, unsigned int a_depth)
{
    auto content = m_downloader.download_url_content(a_url);
    m_parser.parsing(content);
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_page_linkes[a_url] =  m_parser.page_links();
        for (const auto& [word, count] : m_parser.word_index()) {
            m_words_index[word][a_url] += count;
        }
        m_page_titles[a_url] = m_parser.title();
        m_visited_pages.insert(a_url);
        ++m_page_count;
    }
}

