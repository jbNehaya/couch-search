#include"query.hpp"

Query::Query(const std::unordered_map<std::string, std::unordered_map<std::string, unsigned int>>& a_words_index,
             const std::unordered_map<std::string, std::string>& a_page_titles,
             unsigned int a_max_results)
: m_words_index(a_words_index)
, m_page_titles(a_page_titles)
, m_max_results(a_max_results)
{
}

std::vector<std::pair<std::string, std::string>> Query::search(std::string const& a_term)
{
    auto results_for_term = get_results_for_term(a_term);
    auto sorted_results = sort_results_descending(results_for_term);
    trim_results(sorted_results);
    return map_to_titles(sorted_results);
}

std::vector<std::pair<std::string, unsigned int>> Query::get_results_for_term(std::string const& a_term) const
{
    std::vector<std::pair<std::string, unsigned int>> results;

    auto it = m_words_index.find(a_term);
    if (it != m_words_index.end()) {
        for (const auto& link_with_count : it->second) {
            results.emplace_back(link_with_count.first, link_with_count.second);
        }
    }

    return results;
}

std::vector<std::pair<std::string, unsigned int>> Query::sort_results_descending(std::vector<std::pair<std::string, unsigned int>>& a_results) const
{
    std::sort(a_results.begin(), a_results.end(), [](const auto& a, const auto& b) {
        return a.second > b.second; 
    });

    return a_results;
}

void Query::trim_results(std::vector<std::pair<std::string, unsigned int>>& a_results) const
{
    if (m_max_results > 0 && a_results.size() > m_max_results) {
        a_results.resize(m_max_results);
    }
}

std::vector<std::pair<std::string, std::string>> Query::map_to_titles(const std::vector<std::pair<std::string, unsigned int>>& a_results) const
{
    std::vector<std::pair<std::string, std::string>> titled_results;

    for (const auto& result : a_results) {
        auto page_title_it = m_page_titles.find(result.first);
        if (page_title_it != m_page_titles.end()) {
            titled_results.emplace_back(result.first, page_title_it->second);
        } else {
            titled_results.emplace_back(result.first, "Unknown Title");
        }
    }

    return titled_results;
}
