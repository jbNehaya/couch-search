#include <iostream>
#include "crawler.hpp"
#include "configuration.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include"query.hpp"
#include"BFSCrawler.hpp"
#include"DFSCrawler.hpp"

void save_data(const std::unordered_map<std::string, std::unordered_map<std::string, unsigned int>>& words_index,
               const std::unordered_map<std::string, std::string>& page_titles) 
{
    nlohmann::json json_data;
    json_data["words_index"] = words_index;
    json_data["page_titles"] = page_titles;
    std::ofstream file("data.json");
    file << json_data.dump();
}

int main() 
{
    try {
        Configuration config("configuration.json");

        std::unique_ptr<Crawler> crawler;
        if(config.crawl_mode() == CRAWL_MODE::BFS) {
            crawler = std::make_unique<BFSCrawler>(config);
        }/* else if (config.crawl_mode() == CRAWL_MODE::DFS) {
            crawler = std::make_unique<DFSCrawler>(config);
        } */else {
            crawler = std::make_unique<BFSCrawler>(config);
        }

        crawler->start_crawling();
        save_data(crawler->words_index(), crawler->page_titles());
        
        Query query(crawler->words_index(), crawler->page_titles(), 10); 


        std::string search_term;
        std::cout << "Crawling is complete. You can now search for terms.\n";

        while (true) {
            std::cout << "Enter a search term (or 'exit' to quit): ";
            std::getline(std::cin, search_term);

            if (search_term == "exit") {
                break;
            }

            auto results = query.search(search_term);
            std::cout << "\nSearch results for term '" << search_term << "':\n";
            for (const auto& [url, title] : results) {
                std::cout << "Title: " << title << "\n";
                std::cout << "URL: " << url << "\n";
                std::cout << "-----------------------\n";
            }

            if (results.empty()) {
                std::cout << "No results found for the term '" << search_term << "'.\n";
            }
            std::cout << "\n";
        }

        std::cout << "Search phase ended. Exiting program.\n";
    
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
