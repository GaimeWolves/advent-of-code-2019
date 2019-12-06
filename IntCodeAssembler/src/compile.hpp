#pragma once

#include <vector>
#include <map>
#include <fstream>
#include <string>

struct Parameter
{
    int mode;
    int value;
};

struct Instruction
{
    int opcode;
    std::vector<Parameter> params;

    std::vector<int> toIntCode()
    {
        std::vector<int> ret = std::vector<int>(1);
        int code = opcode;
        int power = 100;
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