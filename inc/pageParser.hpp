#ifndef PAGEPARSER_HPP_
#define PAGEPARSER_HPP_

#include<string>
#include<unordered_map>
#include <set>
#include <gumbo.h>

class PageParser {
public:

    using unordered_map = std::unordered_map<std::string, unsigned int>;
    
    PageParser() = default;
    ~PageParser() = default;

    void parsing(std::string const& a_content);
    unordered_map word_index() const;
    std::set<std::string> page_links() const;
    std::string const& title() const;

private:

    void extract_links(GumboNode* a_node);

    void extract_words(GumboNode* a_node);
    void process_gumbo_text_node(GumboNode* a_node);
    void process_gumbo_node_element(GumboNode* a_node);


    void extract_title(GumboNode* a_node) ;
    GumboNode* find_child_with_tag(GumboNode* a_node, GumboTag a_tag);
    std::string extract_text_from_node(GumboNode* a_node);

private:

    std::string m_title;
    std::unordered_map<std::string, unsigned int> m_word_index;
    std::set<std::string> m_page_links;

};

#endif //PAGEPARSER_HPP_
