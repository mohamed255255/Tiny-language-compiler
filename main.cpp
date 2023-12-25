#include <bits/stdc++.h>
#include"Scanner.h"
#include"parser.h"
#include"Analyzer.h"

using namespace std;
signed main() {
    auto *CI  = new CompilerInfo("input.txt", "output.txt", "debug.txt");

    TreeNode *root = Parse(CI);

    PrintTree(root , 0);

    cout << "---------------------------------------------------------\n";

    Analyzer(root);

    PrintSymbolTable();

    Run(root);

    DestroyTree(root);

    return 0;
}


