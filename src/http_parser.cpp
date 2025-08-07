#include "http_parser.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cctype>

HTTP_Parser::HTTP_Parser() {}

HTTP_Request HTTP_Parser::parse_request(int bytes_received, char buffer[]) {
    HTTP_Request req{};

    std::string raw_request(buffer, bytes_received);

    // Separate headers and body
    size_t header_end = raw_request.find("\r\n\r\n");
    if (header_end == std::string::npos) {
        //std::cerr << "[parser] Invalid request: missing \\r\\n\\r\\n\n";
        return req;
    }

    std::string header_part = raw_request.substr(0, header_end + 2);  // keep \r\n
    std::string body_part = raw_request.substr(header_end + 4);       // after \r\n\r\n

    std::istringstream header_stream(header_part);
    this->parse_header(header_stream, req);

    this->parse_body(body_part, req);

    return req;
}

std::string HTTP_Parser::generate_response(HTTP_Response& response, std::string code_text) {
    std::ostringstream res;
    res << "HTTP/1.1 " << response.status_code << " "
        << code_text << "\r\n"
        << "Content-Length: " << response.body.size() << "\r\n"
        << "Content-Type: application/json\r\n"
        << "Connection: close\r\n"
        << "\r\n"
        << response.body;

    return res.str();
}

std::string HTTP_Parser::default_404() {
    std::string body = "404 Not Found";
    std::ostringstream res;
    res << "HTTP/1.1 404 Not Found\r\n"
        << "Content-Length: " << body.size() << "\r\n"
        << "Content-Type: text/plain\r\n"
        << "Connection: close\r\n"
        << "\r\n"
        << body;
    return res.str();
}

std::string HTTP_Parser::lower_case(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return s;
}

void HTTP_Parser::parse_header(std::istringstream& request_stream, HTTP_Request& req) {
    std::string line;
    std::string method;
    std::string path;

    if (std::getline(request_stream, line)) {
        std::istringstream line_stream(line);
        line_stream >> method >> path;
    }

    req.method = method;

    // Parse query string
    size_t separator = path.find('?');
    if (separator != std::string::npos) {
        std::string query = path.substr(separator + 1);
        std::stringstream ss(query);
        std::string kv;
        while (std::getline(ss, kv, '&')) {
            size_t eq = kv.find('=');
            if (eq != std::string::npos) {
                std::string key = kv.substr(0, eq);
                std::string val = kv.substr(eq + 1);
                req.queries[key] = val;
            }
        }
        path = path.substr(0, separator);
    }

    req.path = path;

    // Headers
    while (std::getline(request_stream, line) && line != "\r") {
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string val = line.substr(colon + 1);

            // Trim
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            val.erase(0, val.find_first_not_of(" \t\r\n"));

            req.headers[key] = val;
        }
    }
}

void HTTP_Parser::parse_body(std::string& body, HTTP_Request& req) {
    auto it = req.headers.find("Content-Type");
    if (it != req.headers.end() && it->second.find("application/json") != std::string::npos) {
        parse_json(req, body);
    }
}


void HTTP_Parser::parse_json(HTTP_Request& req, std::string& json_string) {
    json_string.erase(
        std::remove_if(json_string.begin(), json_string.end(), ::isspace),
        json_string.end()
    );

    if (json_string.front() != '{' || json_string.back() != '}') return;

    json_string = json_string.substr(1, json_string.size() - 2);

    std::stringstream ss(json_string);
    std::string pair;

    while (std::getline(ss, pair, ',')) {
        size_t colon = pair.find(':');
        if (colon == std::string::npos) continue;

        std::string key = pair.substr(0, colon);
        std::string value = pair.substr(colon + 1);

        // Remove quotes from key
        if (!key.empty() && key.front() == '"')
            key = key.substr(1, key.size() - 2);

        // Value parsing
        if (!value.empty() && value.front() == '"') {
            value = value.substr(1, value.size() - 2);
            req.body[key] = value;
        } else {
            std::string lower = lower_case(value);
            if (lower == "true") {
                req.body[key] = true;
            } else if (lower == "false") {
                req.body[key] = false;
            } else {
                try {
                    if (value.find('.') != std::string::npos) {
                        req.body[key] = std::stod(value);
                    } else {
                        req.body[key] = std::stoi(value);
                    }
                } catch (...) {
                    req.body[key] = value;
                }
            }
        }
    }
}
