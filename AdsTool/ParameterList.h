// SPDX-License-Identifier: MIT
/**
    Copyright (C) 2021 Beckhoff Automation GmbH & Co. KG
    Author: Patrick Bruenn <p.bruenn@beckhoff.com>
 */

#pragma once

#include "AdsDef.h"
#include <functional>
#include <sstream>
#include <map>

namespace bhf
{
struct ParameterOption {
    static constexpr bool asHex = true;
    const std::string key;
    std::string value;
    bool isFlag;
    bool wasSet;
    ParameterOption(const std::string k,
                    bool              f = false,
                    const std::string v = "")
        : key(k), value(v), isFlag(f), wasSet(false)
    {}
};

template<typename T>
T StringTo(const std::string& v)
{
    T value = 0;
    if (v.size()) {
        const auto asHex = (v.npos != v.rfind("0x", 0));
        std::stringstream converter;
        converter << (asHex ? std::hex : std::dec) << v;
        converter >> value;
    }
    return value;
}

template<> std::string StringTo<>(const std::string& v);
template<> bool StringTo<bool>(const std::string& v);
template<> uint8_t StringTo<uint8_t>(const std::string& v);

struct ParameterList {
    using MapType = std::map<std::string, ParameterOption>;
    MapType map;
    ParameterList(std::initializer_list<ParameterOption> list)
    {
        for (auto p: list) {
            map.emplace(p.key, p);
        }
    }

    int Parse(int argc, const char* argv[]);

    template<typename T>
    T Get(const std::string& key) const
    {
        auto it = map.find(key);
        if (it == map.end()) {
            throw std::runtime_error("invalid parameter " + key);
        }
        return StringTo<T>(it->second.value);
    }
};

struct Commandline {
    Commandline(std::function<int(const char*)> usage, int argc, const char* argv[]);
    template<typename T>
    T Pop(const char* errorMessage = nullptr)
    {
        if (argc) {
            --argc;
            return StringTo<T>(*(argv++));
        }
        if (errorMessage) {
            usage(errorMessage);
        }
        return T {};
    }
    ParameterList& Parse(ParameterList& params);
private:
    std::function<int(const char*)> usage;
    int argc;
    const char** argv;
};
template<>
const char* Commandline::Pop(const char* errorMessage);
}
