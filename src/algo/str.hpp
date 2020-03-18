#pragma once

namespace my {

    template<class T>
    void split(const basic_string <T> &str1, const basic_string <T> &str2, vector <basic_string<T>> &str3) {
        size_t pos1 = 0;
        size_t pos2 = 0;
        while (true) {
            pos2 = str1.find(str2, pos1);
            if (pos2 != string::npos) {
                str3.push_back(str1.substr(pos1, pos2 - pos1));
                pos1 = pos2 + str2.length();
            } else {
                if (pos1 > 0) {
                    str3.push_back(str1.substr(pos1, str1.length()));
                }
                break;
            }
        }
    }


}