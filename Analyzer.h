using namespace std;
#define int long long
template<typename tem>
ostream &operator<<(ostream &out, vector<tem> &a) {
    for (auto &x: a) out << x << ' ';
    return out;
}
// Analyzer ////////////////////////////////////////////////////////////////////////

map<string, pair<int, vector<int>>> SymbolTable;
map<string , int> vars;  /// acts as stack memory during running

void Analyzer(TreeNode *node) {


    if (node->node_kind == ID_NODE || node->node_kind == READ_NODE || node->node_kind == ASSIGN_NODE) {

        if(SymbolTable.find(node->id) == SymbolTable.end() ){
            if(node->child[0] != NULL)  /// take the first occurrence for the variable
                SymbolTable[node->id].first = node->child[0]->num;

            SymbolTable[node->id].second.push_back(node->line_num);
        }else{
            SymbolTable[node->id].second.push_back(node->line_num);
        }
    }
    /// traverse the tree and collect all variables
    for (auto &i: node->child)
        if (i)
            Analyzer(i);
    if (node->sibling)
        Analyzer(node->sibling);

    /// type checking

    ///(1) IF exception   if 1+1 then (wrong statement)
    if (node->node_kind == IF_NODE) {
        if (node->child[0]->expression_data_type != BOOLEAN) {
            cout << ("ERROR If test must be BOOLEAN\n");
        }
    }
    ///(2) Wrong comparators $ < #  or Read < 1
    if (node->node_kind == OPER_NODE)
        if (node->child[0]->expression_data_type != INTEGER
            || node->child[1]->expression_data_type != INTEGER)
            cout << ("ERROR Operator applied to non-integers") << endl;

    ///(3) until 5 (wrong);
    if (node->node_kind == REPEAT_NODE)
        if (node->child[1]->expression_data_type != BOOLEAN)
            cout << ("ERROR Repeat test must be BOOLEAN") << endl;

    ///(4) u can print ID or Nums only
    if (node->node_kind == WRITE_NODE)
        if (node->child[0]->expression_data_type != INTEGER)
            cout << ("ERROR Write works only for INTEGER") << endl;

    ///(5) Assign number or Id to variable is only valid otherwise is wrong  ex: fact := #
    if (node->node_kind == ASSIGN_NODE)
        if (node->child[0]->expression_data_type != INTEGER)
            cout << ("ERROR Assign works only for INTEGER") << endl;

}


void PrintSymbolTable() {
    for (auto &it: SymbolTable) {
        cout << "[var=" << it.first << "]" << "[Mem=" << it.second.first << "]";
        for (auto &v: it.second.second) {
            cout << "[Line=" << v << "]";
        }
        cout << '\n';
    }
}

int CheckType(TreeNode *node) {
    if (node->node_kind == NUM_NODE) {
        return node->num;
    }
    if (node->node_kind == ID_NODE) {
        return vars[node->id];
    }
    int left = CheckType(node->child[0]);
    int right = CheckType(node->child[1]);


    if (node->oper == EQUAL) return left == right;
    if (node->oper == LESS_THAN) return left < right;
    if (node->oper == PLUS) return left + right;
    if (node->oper == MINUS) return left - right;
    if (node->oper == TIMES) return left * right;

    if (node->oper == DIVIDE) {
        if (right != 0)
            return left / right;
        else {
            throw runtime_error("Division by zero");
        }
    }

    if (node->oper == POWER) return pow(left, right);

    throw 0;
}

void ExecuteFlow(TreeNode *node) {
    //check for assigning
    if (node->node_kind == ASSIGN_NODE) {
        int assign_var = CheckType(node->child[0]);
        vars[ node->id] = assign_var;

    }
    // check for read node
    if (node->node_kind == READ_NODE) {
        cout << "Enter " << node->id << " :";
        int value; cin >>value;
        vars[node->id] = value;
    }
    if (node->node_kind == WRITE_NODE) {
        int write_var =  CheckType(node->child[0]);
        cout  << "Answer :" << write_var ;
    }
    // check for if node
    if (node->node_kind == IF_NODE) {
        int condtion = CheckType(node->child[0]);
        if (condtion)
            ExecuteFlow(node->child[1]);
        else if (node->child[2])
            ExecuteFlow(node->child[2]);
    }
    if (node->node_kind == REPEAT_NODE) {
        while (true) {
            ExecuteFlow(node->child[0]);
            if (CheckType(node->child[1])) {
                break;
            }
        }
    }
    if (node->sibling)ExecuteFlow(node->sibling);
}

void Run(TreeNode *node) {
    for(auto i:SymbolTable){
        pair<string ,int > pp = {i.first   ,i.second.first};
        vars.insert(pp);
    }
    ExecuteFlow(node);
}