#include "compile.hpp"
#include <sstream>
#include <cctype>

#include "util.hpp"
#include "err.hpp"

std::map<std::string, int> labels;

static std::map<std::string, std::pair<int, int>> Instructions = {
    {"add", {1, 3}},
    {"mul", {2, 3}},
    {"in", {3, 1}},
    {"out", {4, 1}},
    {"jnz", {5, 2}},
    {"jz", {6, 2}},
    {"slt", {7, 3}},
    {"se", {8, 3}},
    {"crb", {9, 1}},
    {"hlt", {99, 0}}
};

static void findLabels(std::ifstream* file)
{
    std::string line;
    int64_t lineNr = 0;
    int64_t currentAddress = 0;
    while(std::getline(*file, line))
    {
        lineNr++;
        std::string part;
        std::istringstream iss(line);

        bool instr = true;
        while(std::getline(iss, part, ' '))
        {
            part = trim(part);

            if (part[0] == ';')
                break;

            if (part[part.size() - 1] == ':') //Is a label
            {
                part = part.substr(0, part.size() - 1);
                labels[part] = currentAddress;
            }
            else if (instr)
            {
                if (Instructions.find(part) == Instructions.end())
                    throwError({ SYNTAX_ERROR_INVALID_INSTRUCTION, line, lineNr, line.find(part), part.size() });

                auto opCodeInfo = Instructions[part];
                currentAddress += opCodeInfo.second + 1;
                continue;
            }

            instr = false;
        }
    }
}

static Error parseParameter(std::vector<Parameter>& params, std::string line)
{
    int64_t mode;
    switch (line[0])
    {
    case '$':
        mode = 1;
        break;
    case '%':
        mode = 2;
        break;
    default:
        if (!std::isdigit(line[0]))
            return { SYNTAX_ERROR_INVALID_PARAMETER_PREFIX, "", 0, 0, line.size() };
        mode = 0;
        break;
    }

    if (!std::isdigit(line[0]))
        line = line.substr(1);

    int64_t parameter;

    try
    {
        std::size_t s;
        parameter = std::stoll(line, &s, 0);

        if (s != line.size())
            return { SYNTAX_ERROR_INVALID_NUMBER, "", 0, 0, line.size() };
    }
    catch(...)
    {
        return { SYNTAX_ERROR_INVALID_NUMBER, "", 0, 0, line.size() };
    }

    params.push_back({ mode, parameter });

    return { NO_ERROR, "", 0, 0, 0 };
}

static Error parseLine(std::vector<Instruction>& code, std::string line)
{
    std::string part;
    std::istringstream iss(line);

    std::string instr = "";

    while(std::getline(iss, part, ' '))
    {
        part = trim(part);
        if (part == "")
            return { SYNTAX_ERROR_STRAY_COMMA, line, 0, line.find(part), part.size() };

        if (part[0] == ';')
            return { NO_ERROR, "", 0, 0, 0 };

        if (Instructions.find(part) != Instructions.end())
        {
            instr = part;
            break;
        }
        else
            return { SYNTAX_ERROR_INVALID_INSTRUCTION, line, 0, line.find(part), part.size() };
    }

    if (instr != "")
    {
        std::vector<Parameter> params{};

        while (std::getline(iss, part, ','))
        {
            part = trim(part);
            if (part == "")
                return { SYNTAX_ERROR_STRAY_COMMA, line, 0, 0, line.size() };

            if (part[0] == ';')
                break;

            auto c = part.find(';');
            part = part.substr(0, c);

            Error err = parseParameter(params, part);
            if (err.code)
            {
                err.line = line;
                err.index = line.find(part);
                return err;
            }

            if (c != std::string::npos)
                break;
        }

        auto opCodeInfo = Instructions[instr];
        if (opCodeInfo.second != params.size())
            return { SYNTAX_ERROR_WRONG_PARAMETER_COUNT, line, 0, 0, line.size() };
        code.push_back({ opCodeInfo.first, params });
    }

    return { NO_ERROR, "", 0, 0, 0 };
}

std::string compileFile(std::ifstream *file)
{
    std::vector<Instruction> code = std::vector<Instruction>();
    std::vector<int64_t> intCode = std::vector<int64_t>();

    std::string line;
    int64_t lineNr = 0;
    while(std::getline(*file, line))
    {
        lineNr++;
        Error err = parseLine(code, line);
        if (err.code)
        {
            err.lineNr = lineNr;
            throwError(err);
        }
    }

    for (auto&& instr : code)
    {
        auto vec = instr.toIntCode();
        intCode.insert(std::end(intCode), std::begin(vec), std::end(vec));
    }

    std::string out = "";

    for (auto&& i : intCode)
        out += "," + std::to_string(i);

    return out.substr(1);
}