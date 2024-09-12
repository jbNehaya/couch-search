#include <iostream>
#include <sstream>
#include<curlpp/cURLpp.hpp>
#include<curlpp/Options.hpp>
#include "downloader.hpp"

std::string Downloader::download_url_content(std::string const& a_URL)
{
    std::ostringstream response;
    try{
        m_request.setOpt(curlpp::options::Url(a_URL));
        m_request.setOpt(curlpp::options::WriteStream(&response));
        m_request.perform();

    } catch(curlpp::RuntimeError &e){
        std::cerr << "Runtime error: " << e.what() << "\n";
    } catch(curlpp::LogicError &e){
        std::cerr << "LogicError error: " << e.what() << "\n";
    }
    return response.str();
}