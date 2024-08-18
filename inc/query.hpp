#ifndef QUERY_HPP_
#define QUERY_HPP_

#include<unordered_map>
#include<algorithm>
#include<string>
#include<vector>
#include <utility> 


class Query {
public:
    
    using unordered_map_page_with_count = std::unordered_map<std::string, unsigned int>;
    Query(const std::unordered_map<std::string, unordered_map_page_with_count>& a_words_index,
          const std::unordered_map<std::string, std::string>& a_page_titles,
          unsigned int a_max_results = 0);

    std::vector<std::pair<std::string,  std::string>> search(std::string const& a_term);

private:

    std::vector<std::pair<std::string, unsigned int>> get_results_for_term(std::string const& a_term) const;
    std::vector<std::pair<std::string, unsigned int>> sort_results_descending(std::vector<std::pair<std::string, unsigned int>>& a_results) const;
    void trim_results(std::vector<std::pair<std::string, unsigned int>>& a_results) const;
    std::vector<std::pair<std::string, std::string>> map_to_titles(const std::vector<std::pair<std::string, unsigned int>>& a_results) const;

    using ResultPair = std::pair<std::string, unsigned int>;
    using ResultVector = std::vector<ResultPair>;
    ResultVector intersect_results(ResultVector const& a_first_results, ResultVector const& a_second_results) const;

private:

    std::unordered_map<std::string, unordered_map_page_with_count > m_words_index;
    std::unordered_map<std::string, std::string> m_page_titles;
    unsigned int m_max_results;
};
#endif //QUERY_HPP_

