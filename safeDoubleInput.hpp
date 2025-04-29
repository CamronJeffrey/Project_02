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

// Safely handles and validates double input from user
double safeDoubleInput(const string& prompt);
