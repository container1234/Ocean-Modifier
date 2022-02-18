#include "config.hpp"
#include <tesla.hpp>
#include <fstream>

namespace ocean
{
    // nlohmann::json Setting::read()
    // {
    //     std::fstream fs("config.json", std::fstream::in);
    //     if (fs.fail())
    //         return init();
    //     nlohmann::json json = nlohmann::json::parse(fs, nullptr, false);
    //     if (!json.is_discarded())
    //         return json;
    //     return init();
    // }

    // void Setting::update(std::function<void(nlohmann::json &)> const &updater)
    // {
    //     // nlohmann::json json = read();
    //     // updater(json);
    //     // std::fstream fs(FILE_PATH, std::fstream::out);
    //     // fs << json.dump(2);
    // }

    // nlohmann::json Setting::init()
    // {
    //     std::fstream fs(FILE_PATH, std::fstream::out | std::fstream::trunc);
    //     nlohmann::json json = nlohmann::json::object();
    //     fs << json;
    //     return json;
    // }

    void to_json(nlohmann::json &j, const Config &config)
    {
    }

    void from_json(const nlohmann::json &j, Config &config)
    {
        j.at("seed").get_to(config.seed);
        j.at("tide").get_to(config.tide);
        j.at("event").get_to(config.event);
    }
}
