#pragma once

namespace my {

    class Http {

    private:

        std::vector<std::string> line;
        std::map<std::string, std::string> fields;
        std::string body;

    protected:

        bool parse(const s8 *data) {
            return false;
        }
    };

    class HttpRequest : public Http {

    };


    class HttpResponse : public Http {

    };
}
