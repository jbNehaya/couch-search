#include <iostream>
#include "crawler.hpp"
#include "configuration.hpp"
#include"query.hpp"

int main() 
{
    try {
        Configuration config("configuration.json");
        Crawler crawler(config);

        crawler.start_crawling();
        Query query(crawler.words_index(), crawler.page_titles(), 10); 

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
