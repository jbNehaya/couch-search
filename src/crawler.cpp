#include <regex>
#include"crawler.hpp"

Crawler::Crawler(Configuration const& a_config)
: m_page_count{0}
, m_ignored_links_count{0}
, m_config{a_config}
{
}

void Crawler::start_crawling() 
{
    const auto& start_Urls = m_config.start_URLs();

    crawl_using_specific_mode(start_Urls);
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_done = true;
    }
    m_cond_var.notify_all();
    for (auto& thread : m_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    print_information();
}

std::unordered_map<std::string, std::unordered_map<std::string, unsigned int>> Crawler::words_index() const 
{
    return m_words_index;
}

std::unordered_map<std::string, std::string> Crawler::page_titles() const 
{
    return m_page_titles;
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
    
    for (const auto& start_url : start_Urls) {
        std::string start_url_domain = extract_domain(start_url);
        if(current_url_domain == start_url_domain) {
            return true;
        }
    }
    return false;
}

void Crawler::print_information() const
{
    std::cout << "Crawling finished\n";
    std::cout << "Counts of words indexed: " << m_words_index.size() << "\n";
    std::cout << "Counts of pages crawled: " << m_page_count << "\n";
    std::cout << "Counts of ignored links: " << m_ignored_links_count << "\n";
}
