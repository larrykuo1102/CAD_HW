# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <unordered_map>
# include <boost/graph/adjacency_list.hpp>
# include <queue>
# include <functional>
using namespace std ;

unordered_map<string,size_t> gGateVertex ;
unordered_map<string,vector<string>> gGateInbound ;
unordered_map<string,bool> gGateStatus ;
queue<string> readyqueue ;
string gModelName ;
string gNewToken ;
vector<string> gInputs ;
vector<string> gOutputs ;
int gandConstraint = 0;
int gorConstraint = 0;
int gnotConstraint = 0;
string gblifFile ;

void readLabel(ifstream & file) ;
void analyzeLabel( string type, ifstream & file ) ;
void readUntilNotWhiteSpace( ifstream & file) ;
vector<string> readGate(ifstream & file ) ;
string GetaToken(ifstream & file) ;
string ReadaToken( ifstream & file ) ;
string PeekaToken(ifstream & file) ;
bool IsNumber( char ch ) ;
string analyzeGate(ifstream & file, vector<string> gate) ;
void buildTree( string type, vector<string> gate ) ;

struct VertexProperty {
    string name;
    string type;
};

struct ReadyQueue {
    string name ;
    int delay ;
} ;

// Tree
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexProperty> DirectedGraph;
typedef boost::graph_traits<DirectedGraph>::vertex_descriptor vertex_descriptor_t;
DirectedGraph g;
typedef DirectedGraph::vertex_descriptor Vertex;
vertex_descriptor_t src, dst;
// 

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
    vertex_descriptor_t head, tail ;
    if ( ! gGateVertex["head"] && ! gGateVertex["tail"] ) {
        head = boost::add_vertex(g);
        tail = boost::add_vertex(g);
        g[head].name = "head" ;
        g[tail].name = "tail" ;
        gGateVertex["head"] = head ;
        gGateVertex["tail"] = tail ;
    } // if
    else {
        head = gGateVertex["head"] ;
        tail = gGateVertex["tail"] ;
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
            cout << i << " " ;
            DirectedGraph::vertex_descriptor temp_vertex = boost::add_vertex(g);
            gGateVertex[i] = temp_vertex ;
            g[temp_vertex].name = i ;
            g[temp_vertex].type = "NOP" ;
            boost::add_edge(head, temp_vertex, g);
        }

        cout << endl ;
    }
    else if ( type == ".outputs") {
        gOutputs = readGate(file) ;

        cout << "\toutputs: " ;
        for ( auto i : gOutputs ) {
            DirectedGraph::vertex_descriptor temp_vertex = boost::add_vertex(g);
            gGateVertex[i] = temp_vertex ;
            g[temp_vertex].name = i ;
            g[temp_vertex].type = "NOP" ;
            boost::add_edge(temp_vertex, tail, g);
        } // for
        // cout << endl ;
    }
    else if ( type == ".names") {
        cout << "\tnames: " ;
        vector<string> tempname = readGate(file) ;
        for ( auto i : tempname ) {
            cout << i << " " ;
        }
        cout << endl ;

        string type = analyzeGate(file,tempname) ;
        buildTree( type, tempname ) ;   // BuildTree
    }
    else if ( type == ".end") {
        string temp = GetaToken( file ) ;
        cout << temp << "\tend" << endl ;
    }
} // analyzeLabel

vector<string> readGate(ifstream & file ) {
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
    cout << temp_String << "\tFirst Gate" << endl ;
    if (temp_String.find('-') != string::npos )  {
        cout << "=====OR Gate=====" << endl ;
        type = "or" ;
        GetaToken(file) ;
        for ( int i = 2 ; i < gate.size() ; i ++ ) {
            GetaToken(file) ;
            GetaToken(file) ;
        }
    } // if
    else if ( temp_String.find('0') != string::npos) {
        cout << "=====NOT Gate=====" << endl ;
        type = "NOT" ;
        GetaToken(file) ;
    } // else if
    else {
        cout << "=====AND Gate=====" << endl ;
        GetaToken(file) ;
        type = "AND" ;
    }

    return type ;
}



void buildTree( string type, vector<string> gate ) {
    string dest = gate.back() ;
    if ( ! gGateVertex[dest] ) {
        DirectedGraph::vertex_descriptor v1 = boost::add_vertex(g);
        g[v1].name = dest ;
        g[v1].type = type ;
        gGateVertex[dest] = v1 ;
    }
    else if ( gGateVertex[dest] && g[gGateVertex[dest]].type.empty()) {
        g[gGateVertex[dest]].type = type ;
    }
    else {
        // 已經加過了
    } // else
    DirectedGraph::vertex_descriptor destVertex = gGateVertex[dest] ;
    gate.pop_back() ;
    for ( auto i : gate ) {
        if ( gGateVertex[i] ) {
            boost::add_edge(gGateVertex[i], destVertex, g ) ;

        } // if
        else {
            DirectedGraph::vertex_descriptor v1 = boost::add_vertex(g);
            gGateVertex[i] = v1 ;
            g[v1].name = i ;
            g[v1].type = type ;
            boost::add_edge(v1, destVertex, g) ;
        }
    }
} // buildTree

bool checkResourceValid( string gate ) {
    gGateStatus ;

    return false ;
} // checkResourceValid

int ListScheduling() {
    gandConstraint ;
    gorConstraint ;
    gnotConstraint ;
    gGateInbound ;
    // Change Status
        // Check GateResource Valid from gGateStatus
        // put ready Gate into readyqueue

    // Check how many resource this cycle can use

    // get resource from readyqueue ( 'select' priority )

    // run 1 cycle

    // cycle ++
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

int vertex_dfs() {
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
    /* code */
    return 0;
}
