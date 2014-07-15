#ifndef __INDEXBUILDER_HPP__
#define __INDEXBUILDER_HPP__

#include <string>

class IndexBuilder {
    public:
        IndexBuilder(std::string src) : src(src) {}
    private:
        std::string src;
};

#endif
