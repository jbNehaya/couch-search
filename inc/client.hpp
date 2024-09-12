#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <curl/curl.h>
#include <mutex>
#include <thread>

class Client {
public:

    Client(const std::string& a_server_ip, unsigned int a_server_port);
    
    void send_request(const std::string& a_term);
    std::string recieve_response();
    void wait_for_completion();

private:

    std::string url_encode(const std::string& value);
    
private:

    std::string m_server_ip;
    unsigned int server_m_port;
    std::string m_url;
    std::string m_response; 
    std::mutex m_response_mutex;
};

#endif // CLIENT_HPP

