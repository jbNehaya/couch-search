#ifndef CONFIGURATION_HPP_
#define CONFIGURATION_HPP_

#include <string>
#include <vector>

enum class CRAWL_MODE {
    BFS,
    DFS
};

class Configuration {
public:

    Configuration(std::string const& a_file_path) ;
    ~Configuration() = default;

    void load_configuration_from_file(std::string const& a_file_path);
    
    std::vector<std::string> const& start_URLs() const;
    CRAWL_MODE crawl_mode() const;
    unsigned int max_depth() const;
    unsigned int max_pages() const;

private:

    std::vector<std::string> m_start_URLs;
    CRAWL_MODE m_crawl_mode;
    unsigned int m_max_depth;
    unsigned int m_max_pages;
};

#endif //CONFIGURATION_HPP_

