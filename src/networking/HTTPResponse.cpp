#include "HTTPResponse.h"
#include <iostream>

HTTPResponse::HTTPResponse(const std::string &rawResponse) {
    int state = 0;
    unsigned int cursor = 0;
    unsigned int start = 0;
    std::string propertyKey;
    for (cursor = 0; cursor < rawResponse.length(); cursor++) {
        if (state % 2 == 0 && rawResponse[cursor] == ' ') {
            if (state == 0) {
                version = rawResponse.substr(0, cursor - start);
            }
            else {
                propertyKey = rawResponse.substr(start, cursor - start - 1);
            }
            start = cursor + 1;
            state++;
        }
        else if (state % 2 == 1 && rawResponse[cursor] == '\r') {
            if (state == 1) {
                status = rawResponse.substr(start, cursor - start);
            }
            else {
                properties.insert(std::pair<std::string, std::string>(propertyKey, rawResponse.substr(start, cursor - start)));
            }
            start = cursor + 2; // start after new line
            state++;
        }
        else if (state % 2 == 0 && rawResponse[cursor] == '\r') {
            cursor += 2; // skip over the rest of the new lines
            break;
        }
    }
    body = rawResponse.substr(cursor, rawResponse.length() - cursor - 1);
    //std::cout << "Body is [" << body << "]" << std::endl;
    statusCode = stoi(status);
    std::cout << "HTTPResponse::HTTPResponse - Status is " << status << std::endl;
}
