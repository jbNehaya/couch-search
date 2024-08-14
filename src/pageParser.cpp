#include<gumbo.h>
#include<sstream>
#include<cctype>
#include<algorithm>
#include "pageParser.hpp"

void PageParser::parsing(std::string const& a_content)
{
    GumboOutput* output = gumbo_parse(a_content.c_str());

    extract_title(output->root);
    extract_words(output->root);
    extract_links(output->root);

    gumbo_destroy_output(&kGumboDefaultOptions, output);
}

PageParser::unordered_map PageParser::word_index() const
{
    return m_word_index;
}

std::set<std::string> PageParser::page_links() const
{
    return m_page_links;
}

std::string const& PageParser::title() const
{
    return m_title;
}


void PageParser::extract_title(GumboNode* a_node)
{
    if(a_node->type == GUMBO_NODE_ELEMENT){
        GumboElement* element = &a_node->v.element;
        if(element->tag == GUMBO_TAG_TITLE){
            GumboNode* text_node = static_cast<GumboNode*>(element->children.data[0]);
            m_title = text_node->v.text.text;
        }
    }
}

void PageParser::extract_words(GumboNode* a_node)
{
    if(a_node->type == GUMBO_NODE_TEXT){
        std::string text = a_node->v.text.text;
        std::string word;
        std::istringstream stream(text);
        while(stream >> word) {
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);
            ++m_word_index[word];
        }
    }
    if(a_node->type == GUMBO_NODE_ELEMENT){
        GumboElement* element = &a_node->v.element;
        for(size_t i =0; i< element->children.length; ++i){
            extract_words(static_cast<GumboNode*>(element->children.data[i]));
        }
    }
      
}

void PageParser::extract_links(GumboNode* a_node)
{
    if(a_node->type == GUMBO_NODE_ELEMENT){
        GumboElement* element = &a_node->v.element;
        if(element->tag == GUMBO_TAG_A){
            GumboAttribute* href = gumbo_get_attribute(&element->attributes, "href");
            if(href) {
                m_page_links.insert(href->value);
            }
        }
        for(size_t i =0; i< element->children.length; ++i){
            extract_links(static_cast<GumboNode*>(element->children.data[i]));
        }
    }   
}