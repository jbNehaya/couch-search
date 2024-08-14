#include<fstream> //ifstream
#include<stdexcept>
#include<sstream>  //isstringstream
#include <iostream>
#include <nlohmann/json.hpp>
#include "configuration.hpp"

using json = nlohmann::json;
Configuration::Configuration(std::string const& a_file_path)
{
    load_configuration_from_file(a_file_path);
}

void Configuration::load_configuration_from_file(std::string const& a_file_path)
{
    std::ifstream file(a_file_path);
    if(!file.is_open()){
        throw std::runtime_error("Unable to open this file: " + a_file_path);
    }

   json config;
   file >> config;

   try {
        using vector = std::vector<std::string>;

        m_start_URLs = config.at("start_URLs").get<vector>();
        m_max_pages = config.value("max_pages", 0);
        m_max_depth = config.value("max_depth", 0);

        std::string mode = config.value("crawl_mode", "BFS");
        if(mode == "DFS"){
        m_crawl_mode = CRAWL_MODE::DFS;
        }else{
        m_crawl_mode = CRAWL_MODE::BFS;
        }
    } catch (json::exception const& e){
        std::cerr << "error parsing configuration file: "<< e.what() << "\n";
    }
}

std::vector<std::string> const& Configuration::start_URLs() const
{
    return m_start_URLs;
}

unsigned int Configuration::max_pages() const
{
    return m_max_pages;
}

unsigned int Configuration::max_depth() const
{
    return m_max_depth;
}

CRAWL_MODE Configuration::crawl_mode() const
{
    return m_crawl_mode;
}
