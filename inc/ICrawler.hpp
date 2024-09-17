#ifndef ICRAWLER_HPP_
#define ICRAWLER_HPP_

class ICrawler{
public:

    virtual ~ICrawler() = default;
    virtual void start_crawling() = 0;

};
#endif //ICRAWLER_HPP_


