#ifndef NETWORK__HPP
#define NETWORK__HPP

namespace Network {
  // NOTE: may move to seperate file
  namespace Query {
    namespace {
      enum Http_Request {
        GET, POST
      };
      size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
      }

      std::string sendRequest(enum Http_Request hr, const char* url, const char* request) {
        if (url == nullptr) {
          Log::print_error(Log::NETWORK, "Invalid input");
          return "";
        }
        if (hr == POST && request == nullptr) {
          Log::print_error(Log::NETWORK, "Invalid request input");
          return "";
        }

        CURL* curl;
        std::string read_buffer = "";

        curl = curl_easy_init();
        if (!curl) {
          Log::print(Log::NETWORK, "Was unable to initalize curl");
          return "";
        }

        CURLcode res;


        curl_easy_setopt(curl, CURLOPT_URL, url);
        if (hr == POST)
          curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, nullptr);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
          Log::print(Log::NETWORK, "Function curl_easy_perform() failed: ", curl_easy_strerror(res));
          read_buffer = "";
        }

        curl_easy_cleanup(curl);
        return read_buffer;
      }
    }

    // NOTE: with other arguments, defaults to a GET request.
    std::string query(std::string url) {
      return sendRequest(GET, url.c_str(), nullptr);
    }

    std::string query(std::string url, std::string request) {
      return sendRequest(POST, url.c_str(), request.c_str());
    }
  }
  /* }}} */
}

#endif
// vim: tabstop=2 shiftwidth=2 softtabstop=2 expandtab
