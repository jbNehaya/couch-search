#include<sstream>
#include<unordered_set>
#include"query.hpp"

Query::Query(const std::unordered_map<std::string, std::unordered_map<std::string, unsigned int>>& a_words_index,
             const std::unordered_map<std::string, std::string>& a_page_titles,
             unsigned int a_max_results)
: m_words_index(a_words_index)
, m_page_titles(a_page_titles)
, m_max_results(a_max_results)
{
}

Query::SearchResult Query::search(std::string const& a_term)
{

    auto [include_terms, exclude_terms] = parse_term_to_pos_neg(a_term);
    auto results = process_inclusion_terms(include_terms);
    results = process_exclusion_terms(results, exclude_terms);
    auto sorted_results = sort_results_descending(results);
    trim_results(sorted_results);
    return map_to_titles(sorted_results);
}

Query::ResultVector Query::get_results_for_term(std::string const& a_term) const
{
    Query::ResultVector results;

    auto it = m_words_index.find(a_term);
    if (it != m_words_index.end()) {
        for (const auto& link_with_count : it->second) {
            results.emplace_back(link_with_count.first, link_with_count.second);
        }
    }

    return results;
}

Query::ResultVector Query::sort_results_descending(Query::ResultVector& a_results) const
{
    std::sort(a_results.begin(), a_results.end(), 
             [](const auto& a, const auto& b) {
                return a.second > b.second; 
              });

    return a_results;
}

void Query::trim_results(Query::ResultVector& a_results) const
{
    if (m_max_results > 0 && a_results.size() > m_max_results) {
        a_results.resize(m_max_results);
    }
}

std::vector<std::pair<std::string, std::string>> Query::map_to_titles(const Query::ResultVector& a_results) const
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

Query::PairPositiveNeg Query::parse_term_to_pos_neg(std::string const& a_term)
{
    std::istringstream stream_to_split(a_term);
    Query::ExcludeTerms_Neg excludeTerms;
    Query::IncludeTerms_Pos includeTerms;
    std::string word_from_strem;

    while(stream_to_split >> word_from_strem){
        if(word_from_strem[0] == '-'){
            excludeTerms.push_back(word_from_strem.substr(1)); // beacuse should be without - to search 
        }
        else{
            includeTerms.push_back(word_from_strem);
        }
    }
    Query::PairPositiveNeg result{includeTerms,excludeTerms};
    return result;
}

Query::ResultVector Query::process_inclusion_terms(Query::IncludeTerms_Pos const& a_include_terms)
{
    if(a_include_terms.empty()){
        return {};
    }
    
    auto results = get_results_for_term(a_include_terms[0]);
    size_t num_threads = std::thread::hardware_concurrency();
    size_t size = a_include_terms.size();
    size_t step = size/num_threads;
    m_threads.clear();

    for(size_t i=0; i < num_threads; ++i){
        size_t start = i*step + 1;
        size_t end =start + step;
        if(i == num_threads -1){// beacuse if this last thread should include all remaining terms
            end = size;
        }
        m_threads.emplace_back(&Query::process_inclusion_terms_worker, this, start, end, std::ref(a_include_terms), std::ref(results));
    }

    for (auto& thread : m_threads) {
        thread.join();
    }
    
    return results;
}

void Query::process_inclusion_terms_worker(size_t a_start, size_t a_end, const IncludeTerms_Pos& a_include_terms, ResultVector& a_results) const 
{
        auto term_results = get_results_for_term(a_include_terms[a_start]);
        for(size_t i = a_start ; i< a_end; ++i){
            auto next_term_res = get_results_for_term(a_include_terms[i]);
            term_results = intersect_results(term_results, next_term_res);
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        a_results = intersect_results(a_results, term_results);

}

Query::ResultVector Query::process_exclusion_terms(Query::ResultVector a_results_from_process_include, Query::ExcludeTerms_Neg const& a_exclude_terms)
{
    if(a_results_from_process_include.empty()){
        return {};
    }

    std::unordered_set<std::string> exclusions;
    size_t num_threads = std::thread::hardware_concurrency();
    size_t size = a_exclude_terms.size();
    size_t step = size/num_threads;
    m_threads.clear();
    
    for(size_t i=0; i < num_threads; ++i){
        size_t start = i*step + 1;
        size_t end =start + step;
        if(i == num_threads -1){
            end = size;
        }
        m_threads.emplace_back(&Query::process_exclusion_terms_worker, this, start, end, std::ref(a_exclude_terms), std::ref(exclusions));
    }

    for (auto& thread : m_threads) {
        thread.join();
    }

    return filter_exclusions(a_results_from_process_include, exclusions);
}

void Query::process_exclusion_terms_worker(size_t a_start, size_t a_end, const ExcludeTerms_Neg& a_exclude_terms, std::unordered_set<std::string>& a_exclusions) const 
{
    for (size_t i = a_start; i < a_end; ++i) {
        auto exc_res = get_results_for_term(a_exclude_terms[i]);
        std::unordered_set<std::string> local_exclusions;
        for (const auto& page : exc_res) {
            local_exclusions.insert(page.first);
        }
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            a_exclusions.insert(local_exclusions.begin(), local_exclusions.end());    
        }
        
    }
}

Query::ResultVector Query::filter_exclusions(Query::ResultVector a_results_from_process_include, std::unordered_set<std::string> a_exclusions)
{
    
    Query::ResultVector filtered_res;
    for(const auto& res : a_results_from_process_include){
        if(a_exclusions.find(res.first) == a_exclusions.end()){
            filtered_res.emplace_back(res);
        }
    }
    return filtered_res;
}