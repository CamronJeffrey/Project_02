#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <limits>
#include <cctype>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

    Bank_Management(string acc, int id) : account(acc), IdPassword(id), balance(0.0) {}
