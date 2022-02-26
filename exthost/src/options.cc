#include "options.hxx"
#include <algorithm>
#include <iterator>

#define ALSO_STRING "-also"

const std::string& 
CommandLineParser::GetCommandOption(const std::string& option) const noexcept
{
    std::vector<std::string>::const_iterator iter;
    iter = std::find(this->tokens.begin(), this->tokens.end(), option);
    if (iter != this->tokens.end() && ++iter != this->tokens.end())
    {
        return *iter;
    }

    static const std::string mpt("");
    return mpt;
}

bool 
CommandLineParser::CommandExists(const std::string& option) const noexcept
{
    return std::find(
        this->tokens.begin(), this->tokens.end(), option
    ) != this->tokens.end();
}

const std::string& 
ExtraArgumentsParser::GetCommandOption(const std::string& option) const noexcept
{
    std::vector<std::string>::const_iterator iter;
    iter = std::find(this->tokens.begin(), this->tokens.end(), option);

    // Do the iterator tail count in std::distance? Need to test it.
    if (iter != this->tokens.end() && 
                !(std::distance(iter, this->tokens.end()) < 3))
    {
        std::vector<std::string>::const_iterator alsoBase;
        alsoBase = ++(++iter);
        const std::string& alsoParsed = this->ParseAlso(*alsoBase);
        return alsoParsed;
    }

    return "";
}

/* same as ExtraArgumentsParser::GetCommandOptionNoValue but advancing only one time
    in alsoBase decl */
const std::string&
ExtraArgumentsParser::GetCommandOptionNoValue(const std::string& option) const noexcept
{
    std::vector<std::string>::const_iterator iter;
    iter = std::find(this->tokens.begin(), this->tokens.end(), option);

    if (iter != this->tokens.end() && 
                !(std::distance(iter, this->tokens.end()) < 3))
    {
        std::vector<std::string>::const_iterator alsoBase;
        alsoBase = ++iter;
        const std::string& alsoParsed = this->ParseAlso(*alsoBase);
        return alsoParsed;
    }

    return "";
}

const std::string 
ExtraArgumentsParser::ParseAlso(const std::string& raw) const noexcept
{
    if (raw.rfind("-also", 0) == std::string::npos)
    {
        return raw;
    }
    
    std::string parsed;
    std::copy(raw.begin(), raw.end(), parsed.begin());
    parsed.erase(parsed.begin(), parsed.begin() + 
        std::string(ALSO_STRING).size());

    return parsed;
}
