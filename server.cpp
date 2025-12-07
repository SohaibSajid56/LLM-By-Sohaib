// server.cpp
#include <cstdlib>
#include <string>
#include <iostream>

#include "crow_all.h"
#include "memory_engine.h"

#include <curl/curl.h>

std::string GLOBAL_API_KEY = ""; // FIXED

// Helper for libcurl: write callback
static size_t writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    auto *str = static_cast<std::string *>(userdata);
    str->append(ptr, size * nmemb);
    return size * nmemb;
}

// JSON escape helper
static std::string jsonEscape(const std::string &s)
{
    std::string out;
    out.reserve(s.size() + 10);
    for (char c : s)
    {
        if (c == '\\' || c == '"')
            out.push_back('\\'), out.push_back(c);
        else if (c == '\n')
            out += "\\n";
        else if (c == '\r')
            out += "\\r";
        else if (c == '\t')
            out += "\\t";
        else
            out.push_back(c);
    }
    return out;
}

// Call Groq LLaMA-3 and return result
std::string callGroqAI(const std::string &prompt)
{
    if (GLOBAL_API_KEY.empty())
        return "[ERROR] GROQ_API_KEY not set in environment (thread)";

    std::string apiKey = GLOBAL_API_KEY;

    CURL *curl = curl_easy_init();
    if (!curl)
        return "[ERROR] Failed to initialize CURL.";

    std::string responseString;

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.groq.com/openai/v1/chat/completions");

    std::string authHeader = "Authorization: Bearer " + apiKey;
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, authHeader.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    std::string data =
        "{"
        "\"model\": \"llama-3.1-8b-instant\","
        "\"messages\": [{ \"role\": \"user\", \"content\": \"" +
        jsonEscape(prompt) + "\" }]"
                             "}";

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::string err = "[ERROR] curl_easy_perform failed: ";
        err += curl_easy_strerror(res);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return err;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    auto json = crow::json::load(responseString);
    if (!json)
        return "[ERROR] Failed to parse Groq JSON response.";

    if (json.has("error"))
        return json["error"]["message"].s();

    try
    {
        auto &choices = json["choices"];
        return choices[0]["message"]["content"].s();
    }
    catch (...)
    {
        return "[ERROR] Unexpected Groq JSON format.";
    }
}

// =======================================================
//              CORS MIDDLEWARE (REQUIRED)
// =======================================================
struct CORS
{
    struct context
    {
    };

    void before_handle(crow::request &, crow::response &res, context &)
    {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
    }

    void after_handle(crow::request &, crow::response &res, context &)
    {
        res.add_header("Access-Control-Allow-Origin", "*");
    }
};

int main()
{
    crow::App<CORS> app;
    MemoryEngine engine;

    const char *key = std::getenv("GROQ_API_KEY");
    if (key)
        GLOBAL_API_KEY = key;
    else
        std::cout << "WARNING: GROQ_API_KEY missing" << std::endl;

    std::cout << "DEBUG ENV KEY = " << GLOBAL_API_KEY << std::endl;

    // =======================================================
    //      REQUIRED OPTIONS ROUTE FOR BROWSER
    // =======================================================
    CROW_ROUTE(app, "/prompt").methods(crow::HTTPMethod::OPTIONS)([](const crow::request &, crow::response &res)
                                                                  {
        res.code = 204; // No Content
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
        res.end(); });

    // POST /prompt route
    CROW_ROUTE(app, "/prompt").methods(crow::HTTPMethod::POST)([&engine](const crow::request &req)
                                                               {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("prompt"))
            return crow::response(400, "Missing 'prompt'");

        std::string prompt = body["prompt"].s();
        std::string storedResponse;

        crow::json::wvalue respJson;

        if (engine.get(prompt, storedResponse))
        {
            respJson["response"] = storedResponse;
            respJson["from_cache"] = true;
            return crow::response(200, respJson);
        }

        std::string aiAnswer = callGroqAI(prompt);
        engine.put(prompt, aiAnswer);

        respJson["response"] = aiAnswer;
        respJson["from_cache"] = false;
        return crow::response(200, respJson); });

    // Health endpoint
    CROW_ROUTE(app, "/health")([]
                               {
        crow::json::wvalue resp;
        resp["status"] = "ok";
        return crow::response(200, resp); });

    const char *portEnv = std::getenv("PORT");
    uint16_t port = portEnv ? static_cast<uint16_t>(std::stoi(portEnv)) : 8080;

    std::cout << "Starting server on port " << port << std::endl;
    app.port(port).multithreaded().run();
}
