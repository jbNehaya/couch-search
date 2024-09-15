#include"DFSCrawler.hpp"
#include <regex>

DFSCrawler::DFSCrawler(Configuration const& a_config)
: m_page_count{0}
, m_ignored_links_count{0}
, m_config{a_config}
{
}

void DFSCrawler::start_crawling() 
{
    const auto& start_Urls = m_config.start_URLs();

    dfs_crawl(start_Urls);
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

std::unordered_map<std::string, std::unordered_map<std::string, unsigned int>> DFSCrawler::words_index() const 
{
    return m_words_index;
}

std::unordered_map<std::string, std::string> DFSCrawler::page_titles() const 
{
    return m_page_titles;
}

void DFSCrawler::dfs_crawl(std::vector<std::string> const& a_startUrls) 
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& url : a_startUrls) {
            m_toVisit.push({url, 0});
        }
    }
    
    unsigned int num_threads = std::thread::hardware_concurrency();
    for (unsigned int i = 0; i < num_threads; ++i) {
        m_threads.emplace_back(&DFSCrawler::worker_thread, this);
    }
}

void DFSCrawler::worker_thread()
{
    while (true) {
        std::pair<std::string, unsigned int> url_depth;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cond_var.wait(lock, [this]() { return !m_toVisit.empty() || m_done; });
            if (m_done && m_toVisit.empty()) {
                return;
            }
            url_depth = m_toVisit.top();
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

void DFSCrawler::crawl_page(std::string const& a_url, unsigned int a_depth)
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

bool DFSCrawler::should_skip_page(std::string const& a_url, unsigned int a_depth) const 
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

std::string DFSCrawler::extract_domain(std::string const& a_url) const
{
    std::regex domain_regex(R"(https?://([^/]+))");
    std::smatch match;
    if (std::regex_search(a_url, match, domain_regex)) {
        return match[1].str();
    }
    return "";
}

bool DFSCrawler::is_valid_link(std::string const& a_URL) const
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

void DFSCrawler::print_information() const
{
    std::cout << "Crawling finished\n";
    std::cout << "Counts of words indexed: " << m_words_index.size() << "\n";
    std::cout << "Counts of pages crawled: " << m_page_count << "\n";
    std::cout << "Counts of ignored links: " << m_ignored_links_count << "\n";
}
