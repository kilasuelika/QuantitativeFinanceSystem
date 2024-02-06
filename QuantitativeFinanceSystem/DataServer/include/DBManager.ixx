module;
#include <spdlog/spdlog.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <StrMap.h>

export module DBManager;

import std.core;
import DataDef;

export {
    class DBManager {
    public:
        DBManager();
        void Init();
        std::unordered_map<std::string, std::string> GetLatestTime(std::string_view sv);
        void UpdateData(const DataRecords& data);
    private:
        void _create_table();

        void _bind_variant(SQLite::Statement& query, int idx, const std::variant<double, std::string>& data);
        SQLite::Database db;
    };
}

void DBManager::_bind_variant(SQLite::Statement& query, int idx, const std::variant<double, std::string>& data)
{
    if(data.index()==0)
    {
        query.bind(idx, std::get<double>(data));
    }
    else
    {
        query.bind(idx, std::get<std::string>(data));
    }
}
void DBManager::UpdateData(const DataRecords& data)
{
    spdlog::info("Updating data");

    
    try
    {
        SQLite::Transaction transaction(db);

        SQLite::Statement query(db, std::format("INSERT OR REPLACE INTO cn VALUES(?,?,?,?,?,?,?,?,?)"));

        for (const auto& ele : data.data())
        {
            query.bind(1, ele.time);
            query.bind(2, ele.code);
            _bind_variant(query, 3, ele[OPEN_COL]);
            _bind_variant(query, 4, ele[HIGH_COL]);
            _bind_variant(query, 5, ele[LOW_COL]);
            _bind_variant(query, 6, ele[CLOSE_COL]);
            _bind_variant(query, 7, ele[VOLUME_COL]);
            _bind_variant(query, 8, ele[AMOUNT_COL]);
            _bind_variant(query, 9, ele[TURNOVER_COL]);
            query.exec();

            query.reset();
        }
        transaction.commit();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error while updating data: " << e.what() << std::endl;
    }
}

std::unordered_map<std::string, std::string> DBManager::GetLatestTime(std::string_view sv)
{
    std::unordered_map<std::string, std::string> res;

        try
        {
            SQLite::Statement query(db, std::format( "SELECT MAX(time) AS latest_time, code FROM {} GROUP BY {}",sv , CODE_COL));

            while (query.executeStep())
            {
                std::string latestTime = query.getColumn(0).getString();
                std::string code = query.getColumn(1).getString();
                res[code]=latestTime;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error while getting latest times: " << e.what() << std::endl;
        }

        return res;
}

DBManager::DBManager()
    : db("data/data.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)
{
    

}

void DBManager::_create_table()
{
    spdlog::info("Creating cn table");
    const char* sql = "CREATE TABLE cn ("
        TIME_COL " TEXT, "
        CODE_COL " TEXT, "
    OPEN_COL " DOUBLE, "
    HIGH_COL " DOUBLE, "
    LOW_COL " DOUBLE, "
    CLOSE_COL " DOUBLE, "
    VOLUME_COL " DOUBLE, "
    AMOUNT_COL " DOUBLE, "
    TURNOVER_COL " DOUBLE, "
    "UNIQUE(" TIME_COL " , " CODE_COL ")"
    ");";
    db.exec(sql);
}

void DBManager::Init()
{
    if(!db.tableExists("cn"))
    {
        _create_table();
    }
}
