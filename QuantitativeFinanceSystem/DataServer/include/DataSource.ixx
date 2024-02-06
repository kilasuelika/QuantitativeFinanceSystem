#pragma once
export module DataSource;

import DataDef;

export{
    class DataSource {
    public:
        virtual DataRecords GetData(const DataQuery& query) = 0;
        virtual ~DataSource() = default;
    private:

    };

}