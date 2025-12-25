// server.cpp
#include <cstdlib>
#include <string>
#include <iostream>

#include "crow_all.h"
#include "memory_engine.h"

#include <curl/curl.h>

string GLOBAL_API_KEY = "";

static size_t writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    auto *str = static_cast<string *>(userdata);
    str->append(ptr, size * nmemb);
    return size * nmemb;
}

static string jsonEscape(const string &s)
{
    string out;
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

string callGroqAI(const string &prompt)
{
    if (GLOBAL_API_KEY.empty())
        return "[ERROR] GROQ_API_KEY not set in environment";

    CURL *curl = curl_easy_init();
    if (!curl)
        return "[ERROR] Failed to initialize CURL.";

    string responseString;

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.groq.com/openai/v1/chat/completions");

    struct curl_slist *headers = nullptr;
    string authHeader = "Authorization: Bearer " + GLOBAL_API_KEY;

    headers = curl_slist_append(headers, authHeader.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    string data =
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
        string err = "[ERROR] CURL Error: ";
        err += curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        return err;
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    auto json = crow::json::load(responseString);
    if (!json)
        return "[ERROR] Invalid Groq JSON.";

    if (json.has("error"))
        return json["error"]["message"].s();

    try
    {
        return json["choices"][0]["message"]["content"].s();
    }
    catch (...)
    {
        return "[ERROR] Unexpected Groq JSON format.";
    }
}

struct CORS
{
    struct context
    {
    };

    void before_handle(const crow::request &req,
                       crow::response &res,
                       context &ctx,
                       crow::detail::context<CORS> &parent_ctx)
    {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
    }

    void after_handle(const crow::request &req,
                      crow::response &res,
                      context &ctx,
                      crow::detail::context<CORS> &parent_ctx)
    {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
    }
};

int main()
{
    crow::App<CORS> app;
    MemoryEngine engine;

    const char *key = getenv("GROQ_API_KEY");
    if (key)
        GLOBAL_API_KEY = key;

    cout << "Using API KEY: " << GLOBAL_API_KEY << endl;

    CROW_ROUTE(app, "/hashtable")([&engine]
                                  {
        crow::json::wvalue res;
        vector<crow::json::wvalue> items;

        auto entries = engine.getHashTableSnapshot();

        for (auto& [k, v] : entries)
        {
            crow::json::wvalue obj;
            obj["key"] = k;
            obj["value"] = v;
            items.push_back(obj);
        }

        res["entries"] = std::move(items);
        return crow::response(200, res); });

    CROW_ROUTE(app, "/prompt").methods(crow::HTTPMethod::OPTIONS)([](const crow::request &, crow::response &res)
                                                                  {
        res.code = 204;
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
        res.end(); });

    CROW_ROUTE(app, "/prompt").methods(crow::HTTPMethod::POST)([&](const crow::request &req)
                                                               {
            auto body = crow::json::load(req.body);
            if (!body || !body.has("prompt"))
                return crow::response(400, "Missing 'prompt'");

             string prompt = body["prompt"].s();
             string normalized = normalizePrompt(prompt);
             string stored;
             string source;

             crow::json::wvalue resp;

             if (engine.get(normalized, stored, source))
             {
                 resp["response"] = stored;
                 resp["source"] = source;
                 return crow::response(200, resp);
             }

             string answer = callGroqAI(prompt);
            engine.put(prompt, answer);

            resp["response"] = answer;
            resp["source"] = "groq";  
            return crow::response(200, resp); });

    CROW_ROUTE(app, "/health")([]
                               {
        crow::json::wvalue x;
        x["status"] = "ok";
        return crow::response(200, x); });

    uint16_t port = 8080;
    if (const char *p = getenv("PORT"))
        port = stoi(p);

    cout << "Running on port " << port << endl;
    app.port(port).multithreaded().run();
}
