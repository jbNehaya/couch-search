#ifndef DOWNLOADER_HPP_
#define DOWNLOADER_HPP_

#include <string>
#include<curlpp/Easy.hpp>

class Downloader {
public:

    Downloader() = default;
    ~Downloader() = default;

    std::string download_url_content(std::string const& a_URL);

private:

    curlpp::Easy m_request;

};


#endif //DOWNLOADER_HPP_

