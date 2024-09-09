#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <curl/curl.h>

class Client {
public:

    Client(const std::string& a_server_ip, unsigned int a_server_port);
    
    void send_request(const std::string& a_term);
    std::string recieve_response();

private:

    std::string url_encode(const std::string& value);
    size_t write_callback(void* a_contents, size_t a_size, size_t a_nmemb, std::string* a_response);
    
private:

    std::string m_server_ip;
    unsigned int server_m_port;
    std::string m_url;
    std::string m_response; 
};
#endif // CLIENT_HPP

