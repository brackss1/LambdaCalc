#include <iostream>
#include <string>
#include <tuple>

using S = std::string;
using PS = std::pair<S, S>;
using PI = std::pair<S, int>;
using PC = std::pair<char, S>;

#define rag(con) std::begin(con), std::end(con)

S brcut(const S &s)
{
    if (s.size() >= 2 && s[0] == '(' && s[s.size() - 1] == ')')
        return s.substr(1, s.size() - 2);

    return s;
} //両端の丸括弧を除去して中身を取り出す

PI extract_first(const S &s)
{

    bool f = 0;
    int i = 0;

    for (; i < s.size(); ++i)
    {
        if (s[i] != ' ')
        {
            if (s[i] == '(')
                f = 1;
            break;
        }
    }

    std::string t;
    int k = s.size();

    if (!f)
    {
        for (int j = i; j < s.size(); ++j)
        {
            if (s[j] == ' ')
            {
                k = j;
                break;
            }
            t += s[j];
        }
    }
    else
    {
        t = "(";
        int cnt = 0;
        for (int j = i + 1; j < s.size(); ++j)
        {
            t += s[j];
            if (s[j] == '(')
                ++cnt;
            else if (s[j] == ')')
            {
                if (cnt)
                    --cnt;
                else
                {
                    k = j + 1;
                    break;
                }
            }
        }
    }

    return PI(t, k);
} //blankcutの補助関数。
/*" XXX ..."からXXXを取り出す。
XXXは空白を含まない連続した文字列または丸括弧で括られた文字列。*/

PS blankcut(const S &s)
{
    PI pi = extract_first(s);
    PI pii("", -1);
    if (pi.second < s.size())
        pii = extract_first(s.substr(pi.second));

    return PS(pi.first, pii.first);
} //" XXX YYY ..."からXXXとYYYをペアにして取り出す。

bool IsApply(const S &s)
{
    if (s.size() < 8)
        return 0;
    return s.substr(2, 6) == "lambda";
} /*関数適用かどうかを返す。
ただし、関数部分が関数部分適用によるものであった場合もfalseが返される。
trueが返されるのは関数部分が直接関数として書かれたもののみ。*/

bool IsFunc(const S &s)
{

    if (s.size() < 7)
        return 0;
    return s.substr(1, 6) == "lambda";
} //関数かどうかを返す。

PC extFunc(const S &s)
{

    PS res = blankcut(brcut(s).substr(6));

    return PC(res.first[1], res.second);
} //関数から引数と返り値を取り出す。

S _replace(char x, S t, const S &s)
{

    S u;

    for (auto &&c : s)
    {
        if (c == x)
            u += t;
        else
            u += c;
    }

    return u;
} //replaceの補助関数。文字列を愚直に置換する。置換を受けるTermがλ抽象でも関数適用でもない時の置換に使う。

S replace(char x, S t, const S &s)
{

    if (s == "")
        return "";

    if (IsFunc(s))
    {
        char arg;
        S ret;
        std::tie(arg, ret) = extFunc(s);
        if (arg == x)
            return s;
        return "(lambda (" + std::string(1, arg) + ") " + replace(x, t, ret) + ')';
    }

    PS ps = blankcut(brcut(s));

    if (ps.second == "")
        return _replace(x, t, ps.first);

    return '(' + replace(x, t, ps.first) + ' ' + replace(x, t, ps.second) + ')';
} //置換を実行。

S process(const S &s)
{
    if (IsFunc(s))
    {
        //std::cerr << "YESF" << std::endl;
        char arg;
        S ret;
        std::tie(arg, ret) = extFunc(s);
        return "(lambda (" + std::string(1, arg) + ") " + process(ret) + ')';
    }

    if (IsApply(s))
    {
        PS ps = blankcut(brcut(s));
        char arg;
        S ret;
        std::tie(arg, ret) = extFunc(ps.first);
        return replace(arg, ps.second, ret);
    }

    PS ps = blankcut(brcut(s));

    if (ps.second == "")
        return ps.first;

    return '(' + process(ps.first) + ' ' + ps.second + ')';
} //β簡約を複数、有限ステップ実行する。無限簡約を回避するため。

int main()
{

    while (1)
    {
        S ss;

        std::getline(std::cin, ss);

        while (1)
        {
            S t = process(ss);

            if (ss == t) //これ以上簡約しても変わらないなら、それがβ正規形だ。
                break;
            ss = t;
        }

        std::cout << ss << std::endl;
    }

    return 0;
}