#pragma once

#include "base/base.hpp"

namespace my
{

class Http
{

  private:
  protected:
    std::vector<std::string>           line;
    std::map<std::string, std::string> fields;
    std::string                        body;

    bool parse(const s8 *data)
    {
        return false;
    }
};

class HttpRequest : public Http
{
};

class HttpResponse : public Http
{
};
} // namespace my
