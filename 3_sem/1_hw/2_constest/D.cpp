#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

using ull = unsigned long long int;

std::vector<std::string> gray_code(ull n)
{
    if (n == 0) return {};
    if (n == 1) return {"0", "1"};
    
    std::vector<std::string> prev = gray_code(n - 1);
    std::vector<std::string> result;

    for (const std::string& s: prev)
        result.push_back("0" + s);
    
    for (auto it = prev.rbegin(); it != prev.rend(); it++)
        result.push_back("1" + *it);

    return result;
}

int main()
{
    ull n = 0;
    std::cin >> n;
    
    std::vector<std::string> codes = gray_code(n);
    
    for (std::string& i_code: codes)
        std::cout << i_code << std::endl;

    return EXIT_SUCCESS;
}
