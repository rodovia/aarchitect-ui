#pragma once

#include <string>
#include <vector>

class CommandLineParser
{
public:
    CommandLineParser(int argc, const char** argv)
    {
        for (int i = 1; i < argc; i++)
        {
            this->tokens.push_back(std::string(argv[i]));
        }
    }

    const std::string& GetCommandOption(const std::string& option) const noexcept;
    bool CommandExists(const std::string& option) const noexcept;

private:
    std::vector<std::string> tokens;
};

class ExtraArgumentsParser
{
public:
    ExtraArgumentsParser(int argc, const char** argv)
    {
        for (int i = 1; i < argc; i++)
        {
            this->tokens.push_back(std::string(argv[i]));
        }
    }

    const std::string& GetCommandOption(const std::string& option) const noexcept;
    const std::string& GetCommandOptionNoValue(const std::string& option) const noexcept;
private:
    const std::string ParseAlso(const std::string& raw) const noexcept;
    std::vector<std::string> tokens;
};
