#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "client.hpp"

Client::Client(const std::string& a_server_ip, unsigned int a_server_port)
: m_server_ip{a_server_ip}
, server_m_port{a_server_port}
, m_url{"http://" + m_server_ip + ":" + std::to_string(server_m_port) + "/search?term="}
{
}

static size_t write_callback(void* a_contents, size_t a_size, size_t a_nmemb, std::string* a_response)
{
    size_t total_size = a_size * a_nmemb;
    a_response->append(static_cast<char*>(a_contents), total_size); 
    return total_size;
}

std::string Client::url_encode(const std::string& value)
{
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : value) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '-' ) {
            escaped << c;
        } else {
            escaped << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
        }
    }

    return escaped.str();
}

void Client::send_request(const std::string& a_term)
{
    
    CURL* curl;
    CURLcode res;

    {
        std::lock_guard<std::mutex> lock(m_response_mutex);
        m_response.clear();
    }
    curl = curl_easy_init();

    if (curl) {
        std::string encoded_term = url_encode(a_term);
        std::string full_url = m_url + encoded_term;

        curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str()); 
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_response); 

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "CURL error: " << curl_easy_strerror(res) << "\n";
        }

        curl_easy_cleanup(curl);
    }
    
}

std::string Client::recieve_response()
{
    std::lock_guard<std::mutex> lock(m_response_mutex);
    return m_response;
}
