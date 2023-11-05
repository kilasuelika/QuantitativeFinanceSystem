#pragma once
#include <boost/python.hpp>

class AKShareWrapper {
public:
    void Init();
private:
    boost::python::object _akshare;
};