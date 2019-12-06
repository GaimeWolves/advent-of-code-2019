#include "compile.hpp"
#include <sstream>

#include "util.hpp"

std::map<std::string, std::pair<int, int>> Instructions = {
    {"add", {1, 3}},
    {"mul", {2, 3}},
    {"in", {3, 1}},
    {"out", {4, 1}},
    {"jnz", {5, 2}},
    {"jz", {6, 2}},
    {"slt", {7, 3}},
    {"se", {8, 3}},
    {"hlt", {99, 0}},
};

std::string compileFile(std::ifstream *file)
{
    int memSize = 0;
    std::vector<Instruction> code = std::vector<Instruction>();
    std::vector<int> intCode = std::vector<int>();

    std::string line;
    int lineNr = 0;
    while(std::getline(*file, line))
    {
        lineNr++;
        std::string instr = "";
        std::vector<Parameter> params = std::vector<Parameter>();

        bool comment = false;

        int start = -1;
        for (int i = 0; i <= line.size(); i++)
        {
            if (line[i] == ';') //Comment
                comment = true;

            if (instr == "")
            {
                if (line[i] != ' ' && line[i] != '\t' && start == -1) //Normal char, not space or tab.
                    start = i;
                else if (start != -1 && instr == "")
                {
                    if (line[i] == ' ' || line[i] == '\t' || line[i] == '\000' || comment)
                    {
                        instr = line.substr(start, i - start);

                        if (Instructions.find(instr) == Instructions.end())
                        {
                            std::cerr << "Invalid instruction in line " << lineNr << " at " << (i + 1) << ": " << instr << std::endl;
                            file->close();
                            delete file;
                            file = nullptr;
                            exit(-1);
                        }

                        start = i;
                    }
                }
            }
            else
            {
                std::string param = "";

                if (line[i] == '\000' || comment)
                {
                    param = line.substr(start, i - start);
                    param = trim(param);
                    if (param.size() == 0)
                        break;
                }
                if (line[i] == ',')
                {
                    param = line.substr(start, i - start);
                    start = i + 1;
                    param = trim(param);

                    if (param.size() == 0)
                    {
                        std::cerr << "Stray comma in line " << lineNr << " at " << (i + 1) << std::endl;
                        file->close();
                        delete file;
                        file = nullptr;
                        exit(-1);
                    }
                }

                if (param != "")
                {
                    int mode = 0;
                    if (param[0] == '$')
                    {
                        mode = 1;
                        param = param.substr(1);
                    }

                    int parameter;

                    try
                    {
                        std::size_t s;
                        parameter = std::stoi(param, &s, 0);
                        if (s != param.size())
                        {
                            std::cerr << "Invalid number in line " << lineNr << " at " << (i + 1) << std::endl;
                            file->close();
                            delete file;
                            file = nullptr;
                            exit(-1);
                        }
                    }
                    catch(...)
                    {
                        std::cerr << "Invalid number in line " << lineNr << " at " << (i + 1) << std::endl;
                        file->close();
                        delete file;
                        file = nullptr;
                        exit(-1);
                    }

                    if (mode == 0 && memSize < parameter)
                        memSize = parameter;

                    params.push_back({mode, parameter});
                }
            }

            if (comment)
                break;
        }

        if (instr == "")
            continue;

        auto opCodeInfo = Instructions[instr];
        if (opCodeInfo.second != params.size())
        {
            std::cerr << "Invalid parameter count in line " << lineNr << std::endl;
            file->close();
            delete file;
            file = nullptr;
            exit(-1);
        }

        code.push_back({opCodeInfo.first, params});
    }

    for (auto&& instr : code)
    {
        auto vec = instr.toIntCode();
        intCode.insert(std::end(intCode), std::begin(vec), std::end(vec));
    }

    if (intCode.size() < memSize)
        intCode.resize(memSize);

    std::string out = "";

    for (auto&& i : intCode)
        out += "," + std::to_string(i);

    return out.substr(1);
}