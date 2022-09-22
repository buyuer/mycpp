#pragma once

#include "mycpp/base/base.hpp"

namespace mycpp
{

class Http
{

  private:
  protected:
    std::vector<std::string>           line;
    std::map<std::string, std::string> fields;
    std::string                        body;

    bool parse(const uint8_t *data)
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
} // namespace mycpp
