# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <unordered_map>
# include <boost/graph/adjacency_list.hpp>
# include <boost/graph/topological_sort.hpp>
# include <queue>
# include <functional>
# include <algorithm>
using namespace std ;

struct VertexProperty {
    string name;
    string type;
};

string gModelName ;
string gNewToken ;
set<string> gInputs ;
set<string> gOutputs ;
int gandConstraint = 0;
int gorConstraint = 0;
int gnotConstraint = 0;
string gblifFile ;
unordered_map<string,int> gHeuristicCycleRecord ;
unordered_map<string,int> gASAPCycleRecord ;
unordered_map<string,int> gALAPCycleRecord ;
unordered_map<string,int> gSLACK ;
vector<unordered_map<string,vector<string>>> SchedulingResult  ;
vector<unordered_map<string,vector<string>>> ILPResult  ;

// Tree
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexProperty> DirectedGraph;
typedef boost::graph_traits<DirectedGraph>::vertex_descriptor vertex_descriptor_t;
DirectedGraph g;
typedef DirectedGraph::vertex_descriptor Vertex;
vertex_descriptor_t ghead, gtail ;
unordered_map<string,size_t> gGateVertex ;

// List Scheduling
unordered_map<string,vector<string>> gGateInbound ;
unordered_map<string,bool> gGateStatus ;
queue<Vertex> ANDreadyqueue ;
queue<Vertex> ORreadyqueue ;
queue<Vertex> NOTreadyqueue ;
queue<Vertex> ASAPreadyqueue ;

void readLabel(ifstream & file) ;
void analyzeLabel( string type, ifstream & file ) ;
void readUntilNotWhiteSpace( ifstream & file) ;
set<string> readGate(ifstream & file ) ;
vector<string> readNameGate(ifstream & file ) ;
string GetaToken(ifstream & file) ;
string ReadaToken( ifstream & file ) ;
string PeekaToken(ifstream & file) ;
bool IsNumber( char ch ) ;
string analyzeGate(ifstream & file, vector<string> gate) ;
void buildTree( string type, vector<string> gate ) ;
void PrintResult( vector<unordered_map<string,vector<string>>> Result) ;

void readFile() {
    ifstream inputFile( gblifFile ) ;
    if ( ! inputFile.is_open() )  {
        cerr << "File not opened !\n" ;
        return ;   
    }
    // else 
    //     cout << "File opened" << endl ;
    
    while( ! inputFile.eof()) {
        readLabel(inputFile) ;
    }
}

string ReadaToken( ifstream & file ) {
    readUntilNotWhiteSpace(file) ;
    string a_string ;
    char onechar = file.peek() ;
    while( file.peek() != '\n' && file.peek() != -1 ) {
        onechar = file.peek() ;
        if ( onechar == ' ' || onechar == '\n' || onechar == ',' || onechar == ';' || onechar == '\t' ){
            break;
        } // if
        
        a_string += file.get() ;
    } // while

    return a_string ;
}

string PeekaToken(ifstream & file) {
    string token ;
    if ( gNewToken.empty() ) {
      token = ReadaToken( file ) ;
      gNewToken = token ;

    } // if
    else
        token = gNewToken ;
    return token ;
}

string GetaToken(ifstream & file) {
    string token ;
    if ( gNewToken.empty() ) {
        return ReadaToken(file) ;
    }
    else {
        token = gNewToken ;
        gNewToken.clear() ;
    }
    return token ;
}

bool IsNumber( char ch ) { // 判斷一個字是不是數字
  if ( ( ch >= 48 && ch <= 57 ) ) return true ;
  else return false ;
} // IsNumber()

void readUntilNotWhiteSpace( ifstream & file) {
    while( file.peek() == ' ' || file.peek() == '\n' || file.peek() == '\\' ) {
        file.get() ;
    }
    return ;
}

void readLabel( ifstream & file ) {
    char onechar ;
    string firstString =  GetaToken(file) ;
    if ( firstString[0] == '.' )
        analyzeLabel( firstString, file) ;
} // readLabel

void analyzeLabel( string type, ifstream & file ) {
    if ( ! gGateVertex["head"] && ! gGateVertex["tail"] ) {
        ghead = boost::add_vertex(g);
        gtail = boost::add_vertex(g);
        g[ghead].name = "head" ;
        g[gtail].name = "tail" ;
        g[ghead].type = "NOP" ;
        g[gtail].type = "NOP" ;
        gGateVertex["head"] = ghead ;
        gGateVertex["tail"] = gtail ;
    } // if
    else {
        ghead = gGateVertex["head"] ;
        gtail = gGateVertex["tail"] ;
    }
    if ( type == ".model") {
        gModelName = GetaToken( file ) ;
        // cout << "\tgModel Name " << gModelName << endl ;
    }
    else if ( type == ".inputs") {
        gInputs = readGate(file) ;
    }
    else if ( type == ".outputs") {
        gOutputs = readGate(file) ;
        // cout << "\toutputs: " ;
        gGateInbound["tail"] = vector<string>(gOutputs.begin(),gOutputs.end()) ;
        for ( auto i : gOutputs ) {
            DirectedGraph::vertex_descriptor temp_vertex = boost::add_vertex(g);
            gGateVertex[i] = temp_vertex ;
            g[temp_vertex].name = i ;
            boost::add_edge(temp_vertex, gtail, g);

            gGateStatus[i] = false ;
        } // for
        // cout << endl ;
    }
    else if ( type == ".names") {
        // cout << "\tnames: " ;
        vector<string> vectorName = readNameGate(file) ;
        // vector<string> vectorName ;
        for ( auto i : vectorName ) {
            // cout << i << " " ;
            // vectorName.push_back(i) ;
        }
        // cout << endl ;

        string type = analyzeGate(file,vectorName) ;
        buildTree( type, vectorName ) ;   // BuildTree
    }
    else if ( type == ".end") {
        string temp = GetaToken( file ) ;
        // cout << temp << "\tend" << endl ;
    }
} // analyzeLabel

set<string> readGate(ifstream & file ) {
    set<string> temp ;
    string temp_string ;
    temp_string = PeekaToken(file) ;
    while( ! temp_string.empty() && temp_string[0] != '.' && ! IsNumber(temp_string[0]) ) {
        temp_string = GetaToken( file ) ;
        temp.insert(temp_string) ;
        temp_string = PeekaToken(file) ;
    }
    return temp ;
} // readGate

vector<string> readNameGate(ifstream & file ) {
    vector<string> temp ;
    string temp_string ;
    temp_string = PeekaToken(file) ;
    while( ! temp_string.empty() && temp_string[0] != '.' && ! IsNumber(temp_string[0]) ) {
        temp_string = GetaToken( file ) ;
        temp.push_back(temp_string) ;
        temp_string = PeekaToken(file) ;
    }
    return temp ;
} // readGate

string analyzeGate(ifstream & file, vector<string> gate) {
    int gate_size = gate.size() ;
    string temp_String = GetaToken(file) ;
    string type ;
    if (temp_String.find('-') != string::npos )  {
        type = "OR" ;
        GetaToken(file) ;
        for ( int i = 2 ; i < gate.size() ; i ++ ) {
            GetaToken(file) ;
            GetaToken(file) ;
        }
    } // if
    else if ( temp_String.find('0') != string::npos) {
        type = "NOT" ;
        GetaToken(file) ;
    } // else if
    else {
        GetaToken(file) ;
        type = "AND" ;
    }

    return type ;
}

void buildTree( string type, vector<string> gate ) {
    string dest = gate.back() ;
    if ( ! gGateVertex[dest] ) { // 如果沒有destination的node
        DirectedGraph::vertex_descriptor v1 = boost::add_vertex(g);
        g[v1].name = dest ;
        g[v1].type = type ;
        gGateVertex[dest] = v1 ;
    }
    else if ( gGateVertex[dest] && g[gGateVertex[dest]].type.empty()) { // 如果已經有destination的node 但是沒有type
        g[gGateVertex[dest]].type = type ;
    }
    else {
        // 已經加過了
    } // else
    DirectedGraph::vertex_descriptor destVertex = gGateVertex[dest] ;
    gate.pop_back() ;
    gGateInbound[dest] = gate ;
    for ( auto i : gate ) {
        gGateStatus[i] = false ;
        if ( gGateVertex[i] ) { // 如果已經有這個節點
            boost::add_edge(gGateVertex[i], destVertex, g ) ;
        } // if
        else { // 如果沒有這個節點
            if ( gInputs.find(i) == gInputs.end() ) {
                DirectedGraph::vertex_descriptor v1 = boost::add_vertex(g);
                gGateVertex[i] = v1 ;
                g[v1].name = i ;
                g[v1].type = type ;
                boost::add_edge(v1, destVertex, g) ;
            } // if
            else {
                bool edgeExists;
                DirectedGraph::edge_descriptor  e;
                boost::tie(e, edgeExists) = boost::edge(ghead, destVertex, g) ;
                if (!edgeExists) { // 不要有重複的邊
                    e = boost::add_edge(ghead, destVertex, g).first ;
                } // if

                gGateStatus[i] = true ;
            }
        }
    } // for
} // buildTree

bool AddReadyQueue( Vertex resource, string algo ) {
    if ( algo == "ASAP") {
        ASAPreadyqueue.push(resource) ;
    }
    else {
        if (g[resource].type == "AND")
            ANDreadyqueue.push(resource) ;
        else if (g[resource].type == "OR")
            ORreadyqueue.push(resource) ;
        else 
            NOTreadyqueue.push(resource) ;
    }
    if ( g[resource].name == "tail") 
        return true ;
    return false ;
} // AddReadyQueue

void PickResource( vector<Vertex> & nextGate, string algo, int cycle ) {
    unordered_map<string,vector<string>> tempmap ;
    tempmap["AND"] = vector<string>() ;
    tempmap["OR"] = vector<string>() ;
    tempmap["NOT"] = vector<string>() ;
    if ( algo == "ASAP" ) {
        while( ! ASAPreadyqueue.empty()) {
            nextGate.push_back(ASAPreadyqueue.front()) ;
            gASAPCycleRecord[g[ASAPreadyqueue.front()].name] = cycle ;
            ASAPreadyqueue.pop() ;
        } // while
    } // if
    else {
        for ( int i = 0 ; i < gandConstraint ; i ++ ) {
            if ( ANDreadyqueue.size() == 0 ) 
                break ;
            nextGate.push_back(ANDreadyqueue.front()) ;
            gHeuristicCycleRecord[g[ANDreadyqueue.front()].name] = cycle ;
            tempmap["AND"].push_back(g[ANDreadyqueue.front()].name) ;
            ANDreadyqueue.pop() ;
        } // for
        for ( int i = 0 ; i < gorConstraint ; i ++ ) {
            if ( ORreadyqueue.size() == 0 ) 
                break ;
            nextGate.push_back(ORreadyqueue.front()) ;
            gHeuristicCycleRecord[g[ORreadyqueue.front()].name] = cycle ;
            tempmap["OR"].push_back(g[ORreadyqueue.front()].name) ;
            ORreadyqueue.pop() ;
        } // for
        for ( int i = 0 ; i < gnotConstraint ; i ++ ) {
            if ( NOTreadyqueue.size() == 0 ) 
                break ;
            nextGate.push_back(NOTreadyqueue.front()) ;
            gHeuristicCycleRecord[g[NOTreadyqueue.front()].name] = cycle ;
            tempmap["NOT"].push_back(g[NOTreadyqueue.front()].name) ;
            NOTreadyqueue.pop() ;
        } // for
        
        SchedulingResult.push_back(tempmap) ;
    }
} // PickResource

int ListScheduling() {
    bool finish = false ;
    int cycle = 0 ;
    vector<bool> visited(boost::num_vertices(g), false);
    unordered_map<Vertex,int> waitingqueue;
    vector<Vertex> nextGate ;
    nextGate.push_back(ghead) ;
    while( ! finish ) {
        // cout << "Cycle : " << cycle << endl ;
        if ( ! nextGate.empty() ) { 
            for ( auto i : nextGate ) {
                DirectedGraph::adjacency_iterator ai, ai_end;
                // cout << "  ListScheduling test, Vertex : " << g[i].name << endl ;
                for (boost::tie(ai, ai_end) = boost::adjacent_vertices(i, g); ai != ai_end; ++ai) {
                    // cout <<"  | " << g[*ai].name << "'s turn" << endl ;
                    if ( ! waitingqueue[*ai] ) {
                        waitingqueue[*ai] = 0;
                    } // if


                    if ( cycle == 0 ) {  // initial when cycle is 0
                        for ( auto inbound : gGateInbound[g[*ai].name]) {
                            // cout << inbound << " test " << gGateStatus[inbound] << endl ;
                            if ( gGateStatus[inbound] )
                                waitingqueue[*ai] ++ ;
                        } // for
                    } // if
                    else 
                        waitingqueue[*ai] ++ ;

                    if ( waitingqueue[*ai] >= gGateInbound[g[*ai].name].size() ) {
                        finish = AddReadyQueue( *ai, "List" ) ;
                        // cout << "       " << g[*ai].name << " is ready" << endl ;
                    } // if
                    else {
                        // cout << "    | " << g[*ai].name << " not enough inbound :" << waitingqueue[*ai] << endl ;
                        
                    }
                } // for
            }
            nextGate.clear() ;
        } // if
        else { // no new gate
            // cout << "no new gate" << endl ;
        } // else 

        if ( ! finish )
            PickResource( nextGate,"List", cycle ) ;
        
        // run 1 cycle
        // if ( ! nextGate.empty()) {
        //     // cout << "=== " ;
        //     for ( auto test : nextGate ) {
        //         cout << g[test].name << " " ;
        //     }
        //     // cout << "=== " << endl ;
        // }
        // else {
        //     // cout << "=== empty === " << endl ;
        // }
        cycle ++ ;
    } // while

    // cout << "Perform " << cycle-1 << " cycles" << endl ;
    return cycle ;
} // ListScheduling

void CalculateSLAP() {
    for ( auto i : gASAPCycleRecord ) {
        gSLACK[i.first] = gALAPCycleRecord[i.first] - i.second ;
    } // for
} // CalculateSLAP


void ILP_Formulation( int lamb ) {
    ofstream outputFile( gblifFile + ".lp");
    string goal, subjectTo, bounds, general, sequencing, resourceCstring ;
    CalculateSLAP() ;
    vector<pair<string, int>> SLACKvector(gSLACK.begin(), gSLACK.end());

    vector<unordered_map<string,vector<string>>> resourceContraints(lamb,unordered_map<string,vector<string>>(
        {{"AND",vector<string>()},{"OR",vector<string>()},{"NOT",vector<string>()}})) ;
    sort(SLACKvector.begin(), SLACKvector.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
        return a.second < b.second;
    });
    int temp = 0 ;
    for (auto i : SLACKvector) {
        if ( temp != i.second ) {
            // cout << endl << i.first << " " << i.second << " " ;
            temp = i.second ;
        }
        // else 
        //     cout << i.first << " " << i.second << " " ;
    }
    // cout << "\n===============================================\n" ;
    goal += "Minimize\n" ; 
    for ( int i = 1 ; i <= gSLACK["tail"]+1 ; i ++ ) {
        if ( i != 1 )
            goal += " +" ;
        goal += " " + to_string(i+gASAPCycleRecord["tail"]) + " tail," + to_string(i+gASAPCycleRecord["tail"]) ;
    }// for

    subjectTo += "Subject To\n" ;
    // Unique start times
    for ( auto i : SLACKvector ) {
        if ( i.first == "head")
            continue ;
        if ( gSLACK[i.first] == 0 ) {
            string startgate = i.first + "," + to_string(gASAPCycleRecord[i.first] ) ; 
            subjectTo += startgate  + " = 1\n" ;
            resourceContraints[gASAPCycleRecord[i.first]-1][g[gGateVertex[i.first]].type].push_back( startgate ) ;
        } // if
        else { // slack > 0
            for ( int j = 1 ; j <= gSLACK[i.first]+1 ; j ++ ) {
                string startgate = i.first + "," + to_string( gASAPCycleRecord[i.first] + j ) ;
                resourceContraints[gASAPCycleRecord[i.first]+j-1][g[gGateVertex[i.first]].type].push_back( startgate ) ;
                subjectTo += " " +  startgate ;
                if ( j + 1 <= gSLACK[i.first] + 1 )
                    subjectTo += " +" ;
            } // for

            subjectTo += " = 1\n" ;
        } // else
    } // for

    // Sequencing
    for ( auto i : gGateInbound ) {
        if ( gSLACK[i.first] == 0 )
            continue;
        for ( auto j : i.second ) {
            if ( ! gGateStatus[j] ) { 
                if ( gSLACK[j] == 0 )
                    continue;
                if ( gSLACK[j] > 0 ) {
                    sequencing += " " ;
                    for ( int k = 0 ; k <= gSLACK[i.first] ; k ++ ) {
                        sequencing += to_string(k+gASAPCycleRecord[i.first]+1) + " " + i.first + "," + to_string(k+gASAPCycleRecord[i.first]+1) ;
                        if ( k + 1 <= gSLACK[i.first] ) 
                            sequencing += " + " ;
                        else 
                            sequencing += " - " ;
                    } // for
                    for ( int k = 0 ; k <= gSLACK[j] ; k ++ ) {
                        sequencing += to_string(k+gASAPCycleRecord[j]+1) + " " + j + "," + to_string(k+gASAPCycleRecord[j]+1) ;
                        if ( k + 1 <= gSLACK[j] ) 
                            sequencing += " - " ;
                        else
                            sequencing += " >= 1\n" ;
                    } // for
                } // if
            } // if
            else {
                continue;
            } // else
        } // 
    } // for
    subjectTo += sequencing ;
    // Resource constraints
    for ( auto i : resourceContraints ) {
        for ( auto j : i ) {
            bool valid = false ;
            if ( j.first == "AND" && gandConstraint < j.second.size() ) 
                valid = true ;
            if ( j.first == "OR" && gorConstraint < j.second.size() ) 
                valid = true ;
            if ( j.first == "NOT" && gnotConstraint < j.second.size() ) 
                valid = true ;
            
            if ( j.first != "NOP" && valid ) {
                resourceCstring += " " ;
                for ( auto k : j.second ) {
                    resourceCstring += k  ;
                    if ( k != j.second.back())
                        resourceCstring += " + " ;
                }  // for
            } // if

            if ( j.first == "AND" && valid ) 
                resourceCstring += " <= " + to_string(gandConstraint) ;
            if ( j.first == "OR" && valid ) 
                resourceCstring += " <= " + to_string(gorConstraint) ;
            if ( j.first == "NOT" && valid ) 
                resourceCstring += " <= " + to_string(gnotConstraint) ;
            if (valid)
                resourceCstring += "\n" ;
        } // for

    } // for
    subjectTo += resourceCstring ;
    general += "Binary\n" ;
    for ( auto i : SLACKvector ) {
        if ( i.first == "head")
            continue ;
        else {
            for ( int j = 0 ; j <= gSLACK[i.first] ; j ++ ) 
                general += " " + i.first + "," + to_string(j+gASAPCycleRecord[i.first]+1) + "\n" ;
        }
    } // for

    outputFile << goal << endl << subjectTo  << general ;
    outputFile.close() ;
} // ILP_Formulation

void ALAP(int maxTimeStep) {
    std::vector<Vertex> topological_order;
    boost::topological_sort(g, std::back_inserter(topological_order));   
    std::vector<int> alap(boost::num_vertices(g), maxTimeStep);
    for (auto it = topological_order.begin(); it != topological_order.end(); ++it) {
        Vertex v = *it;
        if (boost::out_degree(v, g) == 0 ) {
            alap[v] = std::numeric_limits<int>::max();
            gALAPCycleRecord[g[v].name] = maxTimeStep ;
        } else {
            int min_successor_alap = maxTimeStep;
            DirectedGraph::out_edge_iterator ei, ei_end;
            for (boost::tie(ei, ei_end) = boost::out_edges(v, g); ei != ei_end; ++ei) {
                Vertex successor = boost::target(*ei, g);
                min_successor_alap = std::min(min_successor_alap, alap[successor]);
            }
            if ( g[v].name != "head")
                gALAPCycleRecord[g[v].name] = min_successor_alap - 1 ;
            alap[v] = min_successor_alap - 1; // 減少gate執行的時間
        }
    }
}

int ASAP() {
    bool finish = false ;
    int cycle = 0 ;
    vector<bool> visited(boost::num_vertices(g), false);
    unordered_map<Vertex,int> waitingqueue;
    vector<Vertex> nextGate ;
    nextGate.push_back(ghead) ;
    while( ! finish ) {
        if ( ! nextGate.empty() ) { 
            for ( auto i : nextGate ) {
                DirectedGraph::adjacency_iterator ai, ai_end;
                for (boost::tie(ai, ai_end) = boost::adjacent_vertices(i, g); ai != ai_end; ++ai) {
                    if ( ! waitingqueue[*ai] ) {
                        waitingqueue[*ai] = 0;
                    } // if

                    if ( cycle == 0 ) {  // initial when cycle is 0
                        for ( auto inbound : gGateInbound[g[*ai].name]) {
                            if ( gGateStatus[inbound] )
                                waitingqueue[*ai] ++ ;
                        } // for
                    } // if
                    else 
                        waitingqueue[*ai] ++ ;

                    if ( waitingqueue[*ai] == gGateInbound[g[*ai].name].size() ) 
                        finish = AddReadyQueue( *ai, "ASAP" ) ;
                } // for
            }
            nextGate.clear() ;
        } // if

        if ( ! finish )
            PickResource( nextGate, "ASAP", cycle) ;
        else if ( finish ) {
            gASAPCycleRecord[g[ASAPreadyqueue.front()].name] = cycle ;
        }
        cycle ++ ;
    } // while

    return cycle ;
} // ASAP


void ILPSolver( int heuristicCycle ) {
    ILP_Formulation( heuristicCycle ) ;
    string filename = gblifFile + ".lp" ;
    string command = "gurobi_cl ResultFile=./ILPsolver.sol " + filename ;
    system(command.c_str());
    ifstream inputFile( "./ILPsolver.sol" ) ;
    vector<pair<string,int>> data ;
    string templine, gate, choice ;
    unordered_map<string,vector<string>> tempmap ;
    tempmap["AND"] = vector<string>() ;
    tempmap["OR"] = vector<string>() ;
    tempmap["NOT"] = vector<string>() ;
    getline( inputFile, templine) ;
    while( ! inputFile.eof()) {
        inputFile >> gate >> choice ;
        if ( choice == "1" ) {
            pair<std::string, int> resultpair;
            int pos = gate.find(',') ;
            resultpair.first = gate.substr(0,pos) ;
            resultpair.second = stoi(gate.substr(pos+1 ,gate.size()-pos)) ;
            data.push_back( resultpair ) ;
            choice = "0" ;
        }
    } // while
    
    sort(data.begin(), data.end(), [](const pair<std::string, int>& a, const pair<std::string, int>& b) {
        return a.second < b.second;
    });
    int current = 1 ;
    for ( auto i : data) {
        if ( i.second == current ) {
            tempmap[g[gGateVertex[i.first]].type].push_back(i.first) ;
        } // if
        else {
            ILPResult.push_back(tempmap) ;
            tempmap["AND"] = vector<string>() ;
            tempmap["OR"] = vector<string>() ;
            tempmap["NOT"] = vector<string>() ;
            tempmap[g[gGateVertex[i.first]].type].push_back(i.first) ;
            current ++ ;
        }
    } // for

    
} // ILPSolver

void vertex_dfs() {
    // cout << "Vertex_dfs : " << endl ;
    vector<bool> visited(boost::num_vertices(g), false);

    function<void(Vertex)> dfs = [&](Vertex v) {
        visited[v] = true;
        // cout << "Visiting vertex " << v << " " << g[v].name << " " << g[v].type << std::endl;
        DirectedGraph::adjacency_iterator vi, vi_end;
        for (boost::tie(vi, vi_end) = boost::adjacent_vertices(v, g); vi != vi_end; ++vi) {
            Vertex neighbor = *vi;
            if (!visited[neighbor]) {
                dfs(neighbor);
            }
        }
    };

    for (Vertex v : boost::make_iterator_range(boost::vertices(g))) {
        if (!visited[v]) {
            dfs(v);
        }
    }
    // cout << "=================================================" << endl ;
}

string ManageOption(int argc, char const *argv[]) {  
    if (argc != 6) {
        cerr << "Usage: " << argv[0] << " -e/-h BLIF_FILE AND_CONSTRAINT OR_CONSTRAINT NOT_CONSTRAINT" << std::endl;
        return "failed";
    } // if

    for (int i = 1; i < argc; ++i) {
        if ( i == 1 && i + 1 < argc) {
            gblifFile = argv[i + 1];
            i++;
        } else if (i == 3) {
            gandConstraint = stoi(argv[i]);
        } else if (i == 4) {
            gorConstraint = stoi(argv[i]);
        } else if (i == 5) {
            gnotConstraint = stoi(argv[i]);
        }
    }

    return argv[1] ;
}

void PrintResult( vector<unordered_map<string,vector<string>>> Result) { 
    for ( int i = 0 ; i < Result.size() ; i ++  ) {
        cout << i+1 << ": {" ;

        for (auto j : Result[i]["AND"]) {
            cout << j ;
            if ( j != Result[i]["AND"].back() )
                cout << " " ;
        } // for
        cout << "} {" ;
        for (auto j : Result[i]["OR"]) {
            cout << j ;
            if ( j != Result[i]["OR"].back() )
                cout << " " ;
        } // for
        cout << "} {" ;
        for (auto j : Result[i]["NOT"]) {
            cout << j ;
            if ( j != Result[i]["NOT"].back() )
                cout << " " ;
        } // for
        cout << "}" << endl ;
    }
    cout << "LATENCY: " << Result.size() << endl ;
}


int main(int argc, char const *argv[])
{   
    // cout << "Start" << endl ;
    string option = ManageOption(argc, argv) ;
    readFile() ;
    if ( option == "-h") {
        ListScheduling() ;
        // cout << "==============Heuristic Result==============" << endl ;
        cout << "Heuristic Scheduling Result" << endl ;
        PrintResult(SchedulingResult) ;
    } // if
    else if ( option == "-e") {
        ListScheduling() ;
        // cout << "Finish Heuristic" << endl ;
        ASAP() ;
        ALAP(SchedulingResult.size()) ;
        ILPSolver(SchedulingResult.size()+1) ;
        cout << "ILP-based Scheduling Result" << endl ;
        PrintResult(ILPResult) ;
    }
    // vertex_dfs() ;

    cout << "END" << endl ;
    // vector<pair<string, int>> heurvector(gHeuristicCycleRecord.begin(), gHeuristicCycleRecord.end());
    // sort(heurvector.begin(), heurvector.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
    //     return a.second < b.second;
    // });
    // int temp = 0 ;
    // for (auto i : heurvector) {
    //     if ( temp != i.second ) {
    //         cout << endl << i.first << " " << i.second << " " ;
    //         temp = i.second ;
    //     }
    //     else 
    //         cout << i.first << " " << i.second << " " ;
    // }
    // cout << "\n====================ASAP====================" << endl ;
    
    // vector<pair<string, int>> ASAPvector(gASAPCycleRecord.begin(), gASAPCycleRecord.end());
    // sort(ASAPvector.begin(), ASAPvector.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
    //     return a.second < b.second;
    // });
    // temp = 0 ;
    // for (auto i : ASAPvector) {
    //     if ( temp != i.second ) {
    //         cout << endl << i.first << " " << i.second << " " ;
    //         temp = i.second ;
    //     }
    //     else 
    //         cout << i.first << " " << i.second << " " ;
    // }
    // cout << "\n====================ALAP====================" << endl ;

    
    // vector<pair<string, int>> ALAPvector(gALAPCycleRecord.begin(), gALAPCycleRecord.end());
    // sort(ALAPvector.begin(), ALAPvector.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
    //     return a.second < b.second;
    // });
    // temp = 0 ;
    // for (auto i : ALAPvector) {
    //     if ( temp != i.second ) {
    //         cout << endl << i.first << " " << i.second << " " ;
    //         temp = i.second ;
    //     }
    //     else 
    //         cout << i.first << " " << i.second << " " ;
    // }

    // cout << "\n===============ILP Formulation==============" << endl ;
    
    return 0;
}
