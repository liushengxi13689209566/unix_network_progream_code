#include <iostream>
#include <algorithm>
#include <vector>
#include <sstream>
using namespace std;

class Solution
{
  public:
    vector<int> plusOne(vector<int> &digits)
    {
        auto End_iter = digits.rbegin();

        cout << "1.  *End_iter==" << *End_iter << endl;

        bool tag = false;
        (*End_iter) += 1;

        if (*End_iter >= 10)
        {
            for (auto i = End_iter ; i != digits.rend(); i++)
            {
                if (*i >= 10)
                {
                    (*i) %= 10;
                    if (i == digits.rend() - 1)
                    {
                        cout << "true::::::::::::" << endl;
                        tag = true;
                    }
                    else
                    {
                        *(i + 1) += 1;
                    }
                }
            }
            if (tag)
            {
                cout << "true::::::::::::" << endl;
                digits.insert(digits.begin(), 1); //在开头插入一个元素
            }
        }
        for (auto j : digits)
            cout << j << " ";
        cout << endl;
        return digits;
    }
};

int main()
{
    vector<int> vv{9};
    Solution ss;
    ss.plusOne(vv);

    return 0;
}