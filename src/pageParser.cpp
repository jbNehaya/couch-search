#include<gumbo.h>
#include<sstream>
#include<cctype>
#include<algorithm>
#include<iostream>
#include "pageParser.hpp"

void PageParser::parsing(std::string const& a_content)
{
    GumboOutput* output = gumbo_parse(a_content.c_str());

    extract_title(output->root);
    extract_words(output->root);
    extract_links(output->root);

    gumbo_destroy_output(&kGumboDefaultOptions, output);
}

PageParser::unordered_map const& PageParser::word_index() const
{
    return m_word_index;
}

std::set<std::string> const& PageParser::page_links() const
{
    return m_page_links;
}

std::string const& PageParser::title() const
{
    return m_title;
}

void PageParser::extract_title(GumboNode* a_node)
{
    if (a_node == nullptr || a_node->type != GUMBO_NODE_ELEMENT) {
        return;
    }

    GumboNode* head_node = find_child_with_tag(a_node, GUMBO_TAG_HEAD);
    if (!head_node) {
        return; 
    }

    GumboNode* title_node = find_child_with_tag(head_node, GUMBO_TAG_TITLE);
    if (title_node) {
        m_title = extract_text_from_node(title_node);
    }
}

std::string clean_word(const std::string& word) 
{
    std::string clean;
    std::copy_if(word.begin(), word.end(), std::back_inserter(clean), [](char c) {
        return std::isalnum(c) || std::isspace(c); 
    });
    return clean;
}

void PageParser::extract_words(GumboNode* a_node) 
{
    if (a_node == nullptr) {
        return;
    }

    switch (a_node->type) {
        case GUMBO_NODE_TEXT:
            process_gumbo_text_node(a_node);
            break;
        case GUMBO_NODE_ELEMENT:
            process_gumbo_node_element(a_node);
            break;
        default:
            break;
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

GumboNode* PageParser::find_child_with_tag(GumboNode* a_node, GumboTag a_tag) 
{
    if (a_node == nullptr || a_node->type != GUMBO_NODE_ELEMENT) {
        return nullptr;
    }

    GumboElement* element = &a_node->v.element;
    for (size_t i = 0; i < element->children.length; ++i) {
        GumboNode* child = static_cast<GumboNode*>(element->children.data[i]);

        if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == a_tag) {
            return child; 
        }
    }

    return nullptr;
}

std::string PageParser::extract_text_from_node(GumboNode* a_node) 
{
    if (a_node == nullptr || a_node->type != GUMBO_NODE_ELEMENT) {
        return "";
    }

    if (a_node->v.element.children.length > 0) {
        GumboNode* text_node = static_cast<GumboNode*>(a_node->v.element.children.data[0]);

        if (text_node->type == GUMBO_NODE_TEXT) {
            return std::string(text_node->v.text.text);
        }
    }

    return "";  
}

void PageParser::process_gumbo_text_node(GumboNode* a_node) 
{
    std::string text = a_node->v.text.text;
    std::string word;
    std::istringstream stream(text);
    while(stream >> word) {
        std::string cleaned_word = clean_word(word);
        std::transform(cleaned_word.begin(), cleaned_word.end(), cleaned_word.begin(), ::tolower);
        if (!cleaned_word.empty()) {
            ++m_word_index[cleaned_word];
        }
    }
}

void PageParser::process_gumbo_node_element(GumboNode* a_node)
{
    GumboElement* element = &a_node->v.element;

    if (element->tag == GUMBO_TAG_P || element->tag == GUMBO_TAG_H1 || element->tag == GUMBO_TAG_H2 ||
        element->tag == GUMBO_TAG_H3 || element->tag == GUMBO_TAG_H4 || element->tag == GUMBO_TAG_TITLE) {

        for (size_t i = 0; i < element->children.length; ++i) {
            extract_words(static_cast<GumboNode*>(element->children.data[i]));
        }
    } else if (element->tag != GUMBO_TAG_SCRIPT && element->tag != GUMBO_TAG_STYLE) {
        for (size_t i = 0; i < element->children.length; ++i) {
            extract_words(static_cast<GumboNode*>(element->children.data[i]));
        }
    }
}