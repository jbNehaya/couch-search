#include<sstream>
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
    std::istringstream stream_to_split(a_term);
    std::vector<std::string> terms;
    std::string word_from_strem;

    while(stream_to_split >> word_from_strem){
        terms.push_back(word_from_strem);
    }

    if(terms.empty()){
        return {};
    }

    auto results = get_results_for_term(terms[0]);
    for(size_t i = 1; i<terms.size();++i){
        auto term_results = get_results_for_term(terms[i]);
        results = intersect_results(results, term_results);
    }

    auto sorted_results = sort_results_descending(results);
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
    std::sort(a_results.begin(), a_results.end(), 
             [](const auto& a, const auto& b) {
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

Query::ResultVector Query::intersect_results(Query::ResultVector const& a_first_results, Query::ResultVector const& a_second_results) const
{
    std::unordered_map<std::string, unsigned int> result_map;
    for (const auto& [page, count] : a_first_results){
        result_map[page] = count;
    }
    
    Query::ResultVector intersection;
    for(const auto& [page, count] : a_second_results){
        if(result_map.find(page) != result_map.end()){
            intersection.emplace_back(page, result_map[page]+ count);
        }
    }
    return intersection;
}