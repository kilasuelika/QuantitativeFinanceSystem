module;
#include <spdlog/spdlog.h>
#include <boost/python.hpp>
#include <boost/json.hpp>

export module DataBridge;

import std.core;
import DataDef;
import DataSource;
import AKShareWrapper;
export {
    class DataBridge {
    public:
        void Init(const boost::json::value& config);
        DataRecords GetData(const DataQuery& query);

    private:
        std::vector<std::unique_ptr<DataSource>> _sources;
    };
}

DataRecords DataBridge::GetData(const DataQuery& query)
{
    spdlog::info("Processing query: {}", query.to_string());
    return _sources[0]->GetData(query);
}

void DataBridge::Init(const boost::json::value& config) {
    spdlog::info("Init AKShareWrapper");
    //
    auto py_home_name = "PYTHONHOME";
    auto py_home=config.at(py_home_name).as_string().c_str();
    spdlog::info("Setting PYTHONHOME: {}", py_home);
    SetEnvironmentVariableA(py_home_name, py_home);

    auto ak = std::make_unique<AKShareWrapper>();
    ak->Init();
    _sources.emplace_back(std::move(ak));
}
