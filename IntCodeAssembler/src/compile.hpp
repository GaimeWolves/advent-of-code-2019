#pragma once

#include <vector>
#include <map>
#include <fstream>
#include <string>

struct Parameter
{
    int64_t mode;
    int64_t value;
};

struct Instruction
{
    int64_t opcode;
    std::vector<Parameter> params;

    std::vector<int64_t> toIntCode()
    {
        std::vector<int64_t> ret = std::vector<int64_t>(1);
        int64_t code = opcode;
        int64_t power = 100;
        for (auto&& param : params)
        {
            code += power * param.mode;
            power *= 10;
            ret.push_back(param.value);
        }
        ret[0] = code;
        return ret;
    }
};

std::string compileFile(std::ifstream *file);