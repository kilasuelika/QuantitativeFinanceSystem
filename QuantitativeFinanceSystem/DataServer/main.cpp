#include <boost/program_options.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include "include/DataServer.h"

namespace po = boost::program_options;
int main(int argc, char** argv) {
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("config", po::value< std::string >()->default_value("config.json"), ".json config file");

    po::variables_map vm; //容器
    po::store(po::parse_command_line(argc, argv, desc), vm);  //执行解析
    po::notify(vm);  //一定要加上这一句

    std::string config = vm["config"].as<std::string>();

    DataServer server;
    server.Run(config);
}