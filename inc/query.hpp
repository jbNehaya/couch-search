#ifndef QUERY_HPP_
#define QUERY_HPP_

#include<unordered_map>
#include<algorithm>
#include<string>
#include<vector>
#include <utility> 
#include <thread>
#include <mutex>
#include <condition_variable>

class Query {
public:
    
    using PageTitleMap = std::unordered_map<std::string, std::string>;
    using MapPageWithCount = std::unordered_map<std::string, unsigned int>;
    using WordsIndexMap = std::unordered_map<std::string, MapPageWithCount>;
    using SearchResult = std::vector<std::pair<std::string,  std::string>>;

    Query(WordsIndexMap const& a_words_index,PageTitleMap const& a_page_titles,unsigned int a_max_results = 0);
    SearchResult search(std::string const& a_term);

private:

    using ResultPair = std::pair<std::string, unsigned int>;
    using ResultVector = std::vector<ResultPair>;
    using ExcludeTerms_Neg = std::vector<std::string>;
    using IncludeTerms_Pos = std::vector<std::string>;
    using PairPositiveNeg =  std::pair<IncludeTerms_Pos,ExcludeTerms_Neg>;

    ResultVector get_results_for_term(std::string const& a_term) const;
    
    ResultVector sort_results_descending(ResultVector& a_results) const;
    void trim_results(ResultVector& a_results) const;
    std::vector<std::pair<std::string, std::string>> map_to_titles(const ResultVector& a_results) const;

    ResultVector intersect_results(ResultVector const& a_first_results, ResultVector const& a_second_results) const;
    PairPositiveNeg parse_term_to_pos_neg(std::string const& a_term);
    ResultVector process_inclusion_terms(IncludeTerms_Pos const& a_include_terms);
    ResultVector process_exclusion_terms(ResultVector a_results_from_process_include,ExcludeTerms_Neg const& a_exclude_terms);
    ResultVector filter_exclusions(ResultVector a_results_from_process_include,std::unordered_set<std::string> a_exclusions);

    void process_inclusion_terms_worker(size_t a_start, size_t a_end, const IncludeTerms_Pos& a_include_terms, ResultVector& a_results, std::mutex& a_res_mutex) const;
    void process_exclusion_terms_worker(size_t a_start, size_t a_end, const ExcludeTerms_Neg& a_exclude_terms, std::unordered_set<std::string>& a_exclusions, std::mutex& a_exclusions_mutex) const; 


private:

    WordsIndexMap m_words_index;
    PageTitleMap m_page_titles;
    unsigned int m_max_results;

    mutable std::mutex m_mutex;
    mutable std::condition_variable m_cv;
    bool m_done = false;
    std::vector<std::thread> m_threads;
};
#endif //QUERY_HPP_
