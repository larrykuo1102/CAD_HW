# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <unordered_map>
# include <boost/graph/adjacency_list.hpp>
# include <queue>
# include <functional>
using namespace std ;

struct VertexProperty {
    string name;
    string type;
};

struct ReadyQueue {
    string name ;
    int delay ;
} ;

string gModelName ;
string gNewToken ;
set<string> gInputs ;
set<string> gOutputs ;
int gandConstraint = 0;
int gorConstraint = 0;
int gnotConstraint = 0;
string gblifFile ;



// Tree
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexProperty> DirectedGraph;
typedef boost::graph_traits<DirectedGraph>::vertex_descriptor vertex_descriptor_t;
DirectedGraph g;
typedef DirectedGraph::vertex_descriptor Vertex;
vertex_descriptor_t ghead, gtail ;
unordered_map<string,size_t> gGateVertex ;
// 


// List Scheduling
unordered_map<string,vector<string>> gGateInbound ;
unordered_map<string,bool> gGateStatus ;
queue<Vertex> ANDreadyqueue ;
queue<Vertex> ORreadyqueue ;
queue<Vertex> NOTreadyqueue ;

void readLabel(ifstream & file) ;
void analyzeLabel( string type, ifstream & file ) ;
void readUntilNotWhiteSpace( ifstream & file) ;
set<string> readGate(ifstream & file ) ;
string GetaToken(ifstream & file) ;
string ReadaToken( ifstream & file ) ;
string PeekaToken(ifstream & file) ;
bool IsNumber( char ch ) ;
string analyzeGate(ifstream & file, vector<string> gate) ;
void buildTree( string type, vector<string> gate ) ;

void readFile() {
    ifstream inputFile( "./aoi_benchmark/"+ gblifFile ) ;
    if ( ! inputFile.is_open() )  {
        cerr << "File not opened !\n" ;
        return ;   
    }
    else 
        cout << "File opened" << endl ;
    
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
    while( file.peek() == ' ' || file.peek() == '\n' ) {
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
    // vertex_descriptor_t head, tail ;
    if ( ! gGateVertex["head"] && ! gGateVertex["tail"] ) {
        ghead = boost::add_vertex(g);
        gtail = boost::add_vertex(g);
        g[ghead].name = "head" ;
        g[gtail].name = "tail" ;
        gGateVertex["head"] = ghead ;
        gGateVertex["tail"] = gtail ;
    } // if
    else {
        ghead = gGateVertex["head"] ;
        gtail = gGateVertex["tail"] ;
    }
    cout << "analyzeLabel " << type << endl ;
    if ( type == ".model") {
        gModelName = GetaToken( file ) ;
        cout << "\tgModel Name " << gModelName << endl ;
    }
    else if ( type == ".inputs") {
        cout << "\tinputs: " ;
        gInputs = readGate(file) ;

        for ( auto i : gInputs ) {
            // DirectedGraph::vertex_descriptor temp_vertex = boost::add_vertex(g);
            // gGateVertex[i] = temp_vertex ;
            // g[temp_vertex].name = i ;
            // g[temp_vertex].type = "NOP" ;
            // boost::add_edge(ghead, temp_vertex, g);
            
            // gGateStatus[i] = true ;
        }

        cout << endl ;
    }
    else if ( type == ".outputs") {
        gOutputs = readGate(file) ;
        cout << "\toutputs: " ;
        gGateInbound["tail"] = vector<string>(gOutputs.begin(),gOutputs.end()) ;
        for ( auto i : gOutputs ) {
            // DirectedGraph::vertex_descriptor temp_vertex = boost::add_vertex(g);
            // gGateVertex[i] = temp_vertex ;
            // g[temp_vertex].name = i ;
            // g[temp_vertex].type = "NOP" ;
            // boost::add_edge(temp_vertex, gtail, g);

            gGateStatus[i] = false ;
        } // for
        // cout << endl ;
    }
    else if ( type == ".names") {
        cout << "\tnames: " ;
        set<string> tempname = readGate(file) ;
        vector<string> vectorName ;
        for ( auto i : tempname ) {
            cout << i << " " ;
            vectorName.push_back(i) ;
        }
        cout << endl ;

        string type = analyzeGate(file,vectorName) ;
        buildTree( type, vectorName ) ;   // BuildTree
    }
    else if ( type == ".end") {
        string temp = GetaToken( file ) ;
        cout << temp << "\tend" << endl ;
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

string analyzeGate(ifstream & file, vector<string> gate) {
    int gate_size = gate.size() ;
    string temp_String = GetaToken(file) ;
    string type ;
    // cout << temp_String << "\tFirst Gate" << endl ;
    if (temp_String.find('-') != string::npos )  {
        // cout << "=====OR Gate=====" << endl ;
        type = "or" ;
        GetaToken(file) ;
        for ( int i = 2 ; i < gate.size() ; i ++ ) {
            GetaToken(file) ;
            GetaToken(file) ;
        }
    } // if
    else if ( temp_String.find('0') != string::npos) {
        // cout << "=====NOT Gate=====" << endl ;
        type = "NOT" ;
        GetaToken(file) ;
    } // else if
    else {
        // cout << "=====AND Gate=====" << endl ;
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

void AddReadyQueue( Vertex resource ) {
    if ( g[resource].type == "AND") {
        ANDreadyqueue.push(resource) ;
    } // if
    else if (g[resource].type == "OR") {
        ORreadyqueue.push(resource) ;
    }
    else {
        NOTreadyqueue.push(resource) ;
    }

    if ( g[resource].name == "tail")
        cout << "Finish " << endl ;
} // AddReadyQueue

void PickResource( vector<Vertex> & nextGate ) {
    for ( int i = 0 ; i < gandConstraint ; i ++ ) {
        if ( ANDreadyqueue.size() == 0 ) 
            break ;
        nextGate.push_back(ANDreadyqueue.back()) ;
        ANDreadyqueue.pop() ;
    } // for
    for ( int i = 0 ; i < gorConstraint ; i ++ ) {
        if ( ORreadyqueue.size() == 0 ) 
            break ;
        nextGate.push_back(ORreadyqueue.back()) ;
        ORreadyqueue.pop() ;
    } // for
    for ( int i = 0 ; i < gnotConstraint ; i ++ ) {
        if ( NOTreadyqueue.size() == 0 ) 
            break ;
        nextGate.push_back(NOTreadyqueue.back()) ;
        NOTreadyqueue.pop() ;
    } // for


} // PickResource

int ListScheduling() {
    gandConstraint ;
    gorConstraint ;
    gnotConstraint ;
    
    bool finish = false ;
    int cycle = 0 ;
    vector<bool> visited(boost::num_vertices(g), false);
    unordered_map<Vertex,int> waitingqueue;
    vector<Vertex> nextGate ;
    nextGate.push_back(ghead) ;
    while( ! finish && cycle < 5 ) {
        cout << "Cycle : " << cycle << endl ;
        if ( ! nextGate.empty() ) { 
            for ( auto i : nextGate ) {
                DirectedGraph::adjacency_iterator ai, ai_end;
                cout << "  ListScheduling test, Vertex : " << g[i].name << endl ;
                for (boost::tie(ai, ai_end) = boost::adjacent_vertices(i, g); ai != ai_end; ++ai) {
                    cout <<"  | " << g[*ai].name << "'s turn" << endl ;
                    if ( ! waitingqueue[*ai] ) {
                        waitingqueue[*ai] = 0;
                    } // if


                    if ( cycle == 0 ) { 
                        for ( auto inbound : gGateInbound[g[*ai].name]) {
                            // cout << inbound << " test " << gGateStatus[inbound] << endl ;
                            if ( gGateStatus[inbound] )
                                waitingqueue[*ai] ++ ;
                        } // for
                    } // if
                    else 
                        waitingqueue[*ai] ++ ;

                    if ( waitingqueue[*ai] == gGateInbound[g[*ai].name].size() ) {
                        AddReadyQueue( *ai ) ;
                        cout << "       " << g[*ai].name << " is ready" << endl ;
                    } // if
                    else {
                        cout << "    | " << g[*ai].name << " not enough inbound :" << waitingqueue[*ai] << endl ;
                    }
                } // for
            }
            nextGate.clear() ;
        } // if
        else { // no new gate

        } // else 

        // Change Status
            // Check GateResource Valid from gGateStatus
            // put ready Gate into readyqueue

        // Check how many resource this cycle can use
        // get resource from readyqueue ( 'select' priority )
        PickResource( nextGate) ;

        // run 1 cycle
        if ( ! nextGate.empty()) {
            cout << "=== " ;
            for ( auto test : nextGate ) {
                cout << g[test].name << " " ;
            }
            cout << "=== " << endl ;
        }
        else {
            cout << "=== empty === " << endl ;
        }
        cycle ++ ;
    } // while

    return cycle ;
} // ListScheduling


int ILPSolver() {

} // ILPSolver

void ILP_Formulation() {
    // Unique start times

    // Sequencing

    // Resource constraints

    // 
} // ILP_Formulation

int ALAP() {

} // ALAP

int ASAP() {

} // ASAP

void vertex_dfs() {
    cout << "Vertex_dfs : " << endl ;
    vector<bool> visited(boost::num_vertices(g), false);

    function<void(Vertex)> dfs = [&](Vertex v) {
        visited[v] = true;
        cout << "Visiting vertex " << v << " " << g[v].name << " " << g[v].type << std::endl;
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
    cout << "=================================================" << endl ;
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

int main(int argc, char const *argv[])
{   
    cout << "Start" << endl ;
    string option = ManageOption(argc, argv) ;
    readFile() ;
    if ( option == "-h") {

    } // if
    else if ( option == "-e") {

    }
    vertex_dfs() ;

    ListScheduling() ;
    /* code */
    return 0;
}
