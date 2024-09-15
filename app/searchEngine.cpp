#include <iostream>
#include "crawler.hpp"
#include "configuration.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include"query.hpp"
#include"BFSCrawler.hpp"
#include"DFSCrawler.hpp"
#include "client.hpp"

void save_data(const std::unordered_map<std::string, std::unordered_map<std::string, unsigned int>>& words_index,
               const std::unordered_map<std::string, std::string>& page_titles) 
{
    nlohmann::json json_data;
    json_data["words_index"] = words_index;
    json_data["page_titles"] = page_titles;
    std::ofstream file("data.json");
    file << json_data.dump();
}

void handle_search_queries(Client& a_client, Query& a_query) 
{
    std::string search_term;
    std::cout << "Crawling is complete. You can now search for terms.\n";

    while (true) {
        std::cout << "Enter a search term (or 'exit' to quit): ";
        std::getline(std::cin, search_term);

        if (search_term == "exit") {
            break;
        }

        a_client.send_request(search_term);
        std::string response = a_client.recieve_response();

        std::cout << "Response:\n" << response << "\n";
    }

    std::cout << "Search phase ended. Exiting program.\n";
}

int main() 
{
    try {
        Configuration config("configuration.json");

        std::string port = "127.0.0.1"; 
        Client client(port, 1401);

        std::unique_ptr<Crawler> crawler;
        if (config.crawl_mode() == CRAWL_MODE::DFS) {
            crawler = std::make_unique<DFSCrawler>(config);
        } else {
            crawler = std::make_unique<BFSCrawler>(config);
        }

        std::thread crawl_thread([&crawler]() 
        {
            crawler->start_crawling();
            save_data(crawler->words_index(), crawler->page_titles());
        });
        
        Query query(crawler->words_index(), crawler->page_titles(), 10); 
        handle_search_queries(client, query);
        crawl_thread.join();
    
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

