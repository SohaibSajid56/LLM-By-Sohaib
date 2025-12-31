#include <fstream>
#include <string>
#include <vector>
using namespace std;

int main()
{
    ofstream file("memory.txt");

    vector<string> prompts = {
        "hello", "hi", "hey", "good morning", "good evening",
        "who are you", "what is ai", "what is dsa", "what is btree",
        "what is hashtable", "what is stack", "what is queue",
        "what is linked list", "what is graph", "what is recursion"};

    for (int i = 0; i < 10000; i++)
    {
        string key = prompts[i % prompts.size()] + " " + to_string(i);
        string value = "This is a cached response for prompt " + to_string(i);
        file << key << "|" << value << "\n";
    }

    file.close();
    return 0;
}
