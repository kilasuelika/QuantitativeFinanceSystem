module;

#include <boost/date_time/gregorian/greg_date.hpp>
//#include <boost/flyweight.hpp>

export module DataDef;

import std.core;

import Util;

export{
    struct DataQuery {
        std::vector<std::string> codes, fields;
        std::string start_time, end_time;

        std::string to_string() const;
    };

    /*struct DataElement {
        boost::flyweights::flyweight<std::string> code, field;
        boost::flyweights::flyweight<boost::gregorian::date> date;
        double data;
    };*/
    struct DataElement {
        std::string code;
        std::string time;
        std::unordered_map<std::string, std::variant<double, std::string>> data;

        const auto& operator[](const std::string& field)const
        {
            return data.at(field);
        }
    };
   
    class DataRecords {

    public:

        template<typename T>
        void emplace_back(T&& t)
        {
            _data.emplace_back(std::forward<T>(t));
        }

        const auto& data()const
        {
                       return _data;
        }
    private:
        std::vector<DataElement> _data;
    };

}

std::string DataQuery::to_string() const
{
    if (codes.size() > 0)
    {
        return std::format("{}, {}~{}", codes[0], normalize_date_str( start_time), normalize_date_str( end_time));
    }else
    {
        return "";
    }
}
