module;

#include <spdlog/spdlog.h>
#include <boost/json.hpp>

export module DataMantainer;

import std.core;
import DBManager;
import DataBridge;
import DataDef;
import Util;

export {
    class DataMantainer {
    public:
        void Init(boost::json::object config, DBManager* man, DataBridge* bridge);
        void UpdateConifg(boost::json::object config);
    private:
        DBManager * _man;
        DataBridge* _bridge;

        boost::json::object _config;
        std::string _start_time="2000-01-01";
        std::vector<std::string> _codes;

        void _complete_data();
        void _udpate_codes();
    };
}

void DataMantainer::UpdateConifg(boost::json::object config)
{
    _config = config;
    _udpate_codes();

    if(_config.contains("start_time"))
    {
        _start_time = _config.at("start_time").as_string().c_str();
    }
    else
    {
        spdlog::warn("No start time in config");
    }
}

void DataMantainer::_udpate_codes()
{
    if(_config.contains("codes"))
    {
        spdlog::info("Updating codes");

        std::string codes_str = "";
        _codes.clear();
        auto codesArr = _config.at("codes").as_array();
        for (auto& code : codesArr)
        {
            std::string s = code.as_string().c_str();
            _codes.push_back(s);

            codes_str += std::format("{} ",s);
        }
        spdlog::info("Current codes: {}", codes_str);
    }
    else
    {
        spdlog::error("No codes in config");
    }
}

void DataMantainer::_complete_data()
{
    spdlog::info("Completing data");
    auto times = _man->GetLatestTime("cn");

    
    for (const auto& code: _codes)
    {
        DataQuery query;
        query.codes.emplace_back(code);
        if(times.contains(code))
        {
            query.start_time= times.at(code);
        }else
        {
            query.start_time = _start_time;
        }

        query.end_time= get_date_today();

        if(query.start_time == query.end_time)
        {
            continue;
        }

        auto data = _bridge->GetData(query);
        _man->UpdateData(data);
       /* if()
        query.codes.push_back(code);
        query.start_time = time;
        query.end_time = "2021-01-01";
        query.fields.push_back("open");
        query.fields.push_back("close");
        query.fields.push_back("high");
        query.fields.push_back("low");
        query.fields.push_back("volume");
        query.fields.push_back("money");
        query.fields.push_back("factor");
        auto records = _bridge->GetData(query);
        for (auto& record : records.data())
        {
                       auto& data = record.data;
            auto& fields = record.fields;
            auto& date = record.date;
            auto& code = record.code;
            auto open = std::get<double>(data[0]);
            auto close = std::get<double>(data[1]);
            auto high = std::get<double>(data[2]);
            auto low = std::get<double>(data[3]);
            auto volume = std::get<double>(data[4]);
            auto money = std::get<double>(data[5]);
            auto factor = std::get<double>(data[6]);
            auto avg = (open + close + high + low) / 4;
            auto adj_avg = avg * factor;
            auto adj_open = open * factor;
            auto adj_close = close * factor;
            auto adj_high = high * factor;
            auto adj_low = low * factor;
            auto adj_volume = volume / factor;
            auto adj_money = money / factor;
            auto adj_factor = 1 / factor;
            auto adj_data = std::vector<std::variant<double, std::string>>{
                           adj_open, adj_close, adj_high, adj_low, adj_volume, adj_money, adj_factor
                       };
            auto adj_fields = std::vector<std::string>{
                           "adj_open", "adj_close", "adj_high", "adj_low", "adj_volume", "adj_money", "adj_factor"
                       };
            DataElement adj_record;
            adj_record.code = code;
            adj_record.date = date;
            adj_record.fields = adj_fields;
            adj_record.data = adj_data;
            records.emplace_back(adj_record);
        }*/
    }
}

void DataMantainer::Init(boost::json::object config, DBManager* man, DataBridge* bridge)
{
    _man = man;
    _bridge= bridge;

   UpdateConifg(config);
    _complete_data();
}
