#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <boost/json.hpp>

#include <boost\date_time\gregorian\gregorian.hpp>

export module Util;

import std.core;

export{

    std::string get_date_today()
    {
       auto gdt1 = boost::gregorian::day_clock::local_day();
       return boost::gregorian::to_iso_string(gdt1);
    }
    std::string normalize_date_str(std::string str)
    {
        str.erase(std::remove(str.begin(), str.end(), '-'), str.end());
        return str;
    }
    boost::json::value load_json(const std::string& filename) {
        std::ifstream ifs(filename);
        if (!ifs.is_open()) {
            spdlog::error("Could not load json file: {}", filename);
            throw std::runtime_error("Could not load json file: " + filename);
        }

        std::string jsonStr((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        boost::json::value jv = boost::json::parse(jsonStr);
        spdlog::info("json file loaded: {}", filename);
        return jv;
    }
    void init_logger() {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/mylogfile.txt", true);

        spdlog::logger logger("multi_sink", { console_sink, file_sink });
        spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger));

        spdlog::info("Logger init finished.");
    }
}