module;
#include <boost/python.hpp>
#include <boost\date_time\gregorian\gregorian.hpp>
#include <StrMap.h>
#include <spdlog/spdlog.h>

export module AKShareWrapper;

import DataDef;
import DataSource;
import std.core;

namespace bp = boost::python;
using namespace boost::gregorian;
using namespace std;

export {
    class AKShareWrapper : public DataSource {
    public:
        void Init();
        DataRecords GetData(const DataQuery& query) override;
        const vector<date>& get_trade_dates();

    private:
        vector<date> _get_trade_dates();

        bp::object _akshare, _main;
        bp::object _tarde_date_fun;
        bp::object _stock_zh_a_hist;
        bp::object _double;

        vector<date> _trade_dates;
    };
}

const vector<date>& AKShareWrapper::get_trade_dates() {
    return _trade_dates;
}
vector<date> AKShareWrapper::_get_trade_dates() {
    auto df = _tarde_date_fun();

    //遍历
    bp::object shape = df.attr("shape");
    int rows = bp::extract<int>(shape[0]);
    int cols = bp::extract<int>(shape[1]);

    vector<date> dates;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            bp::object value = df.attr("iloc")[bp::make_tuple(i, j)];
            date dt(bp::extract<int>(value.attr("year"))(),
                bp::extract<int>(value.attr("month"))(),
                bp::extract<int>(value.attr("day"))());
            dates.emplace_back(std::move(dt));
        }
    }

    return dates;
}

DataRecords AKShareWrapper::GetData(const DataQuery& query) {
    DataRecords res;

    std::unordered_set<string> codes(query.codes.begin(), query.codes.end());

    while (!codes.empty())
    {
        for (auto it=codes.begin();it!=codes.end();++it)
        {
            try
            {
                bp::dict kwargs;
                kwargs["symbol"] = *it;
                kwargs["period"] = "daily",
                kwargs["start_date"] = query.start_time,
                kwargs["end_date"] = query.end_time,
                kwargs["adjust"] = "qfq";

                auto df = _stock_zh_a_hist(*boost::python::tuple() , **kwargs);

                 // 
                int rows = bp::extract<int>(df.attr("shape")[0]);
                spdlog::info("Getting {} rows data for code {}", rows, *it);

                for(int i=0;i<rows;++i)
                {
                    auto row = df.attr("iloc")[i];
                    DataElement ele;
                    ele.code= *it;
                    ele.time= bp::extract<string>( bp::str(row.attr("iloc")[0]))();
                    static constexpr std::array<int, 7> idx_map{ 1, 2, 3, 4, 5, 6, 10 };
                    for (int i=0;i<PRICE_FIRLDS.size();++i)
                    {
                        const auto& k = PRICE_FIRLDS[i];
                        auto j=idx_map[i];

                        bp::object py_ele = row.attr("iloc")[j];
                        ele.data[k] = bp::extract<double>(_double(py_ele))();
                        /*if (i != 4)
                        {
                            ele.data[k] = bp::extract<double>(_double(row.attr("iloc")[j]))();
                        }else
                        {
                            ele.data[k] =double(bp::extract<int>(row.attr("iloc")[j])());
                        }*/
                    }
                    res.emplace_back(ele);
                }

                // 获取成功后删除
                it = codes.erase(it);
            }
            catch (...)
            {
                PyErr_Print();
                ++it;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return res;
}

void AKShareWrapper::Init() {
    Py_Initialize();

    _main = bp::import("builtins");
    _double = _main.attr("float");

    _akshare = bp::import("akshare");

    _tarde_date_fun = _akshare.attr("tool_trade_date_hist_sina");
    _stock_zh_a_hist=_akshare.attr("stock_zh_a_hist");


    _trade_dates = get_trade_dates();
}
