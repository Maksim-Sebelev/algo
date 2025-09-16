// скобочки

// #include <bits/stdc++.h>

#include <iostream>
#include <vector>

using namespace std;
using ull = unsigned long long int;



// void print_v(vector<char>& v)
// {
//     cout << "size = " << v.size() << endl;

//     for (char i: v)
//         cout << i << " ";

//     cout << endl << endl;
// }

bool update_stack(vector<char>& v, char s)
{
    // cout << "s = " << s << endl;

    if (v.size() == 0 && ((s == '(') || (s == '{') || (s == '[') || (s == '<')))
    {
        v.push_back(s);
        return true;
    }

    if (v.size() == 0)
    {
        return false;
    }

    char last_s = v.back();
    // cout << "last_s = " << last_s << endl;
    if (
        (last_s == '(' && s == ')') ||
        (last_s == '{' && s == '}') ||
        (last_s == '[' && s == ']') ||
        (last_s == '<' && s == '>')
    )
    {
        v.pop_back();
        return true;
    }

    else if (
        ((last_s == '(') && (s == ']' || s == '}' || s == '>')) ||
        ((last_s == '{') && (s == ')' || s == ']' || s == '>')) ||
        ((last_s == '[') && (s == ')' || s == '}' || s == '>')) ||
        ((last_s == '<') && (s == ')' || s == '}' || s == ']'))
    )

    {
        return false;
    }

    v.push_back(s);
    return true;
}

int main()
{
    vector<char> v(0);

    char s = 0;

    while (cin >> s)
    {
        bool res = update_stack(v, s);
        // print_v(v);
        if (!res)
        {
            cout << "NO";
            return 0;
        }
    }

    if (v.size() == 0)
    {
        cout << "YES";
        return 0;
    }

    cout << "NO";

    return 0;
}

