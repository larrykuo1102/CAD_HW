# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <unordered_map>
# include <set>
using namespace std ;


struct ProductTerm {
    string output ;
    vector<vector<string>> AllMinterms ;
} ;

string gModelName ;
string gNewToken ;
string gblifFile ;
set<string> gInputs ;
set<string> gOutputs ;

vector<ProductTerm> gAllGateProductTerms ;

void readUntilNotWhiteSpace( ifstream & file) ;
string GetaToken(ifstream & file) ;
string ReadaToken( ifstream & file ) ;
string PeekaToken(ifstream & file) ;
bool IsNumber( char ch ) ;
void readLabel(ifstream & file) ;
set<string> readGate(ifstream & file ) ;
void analyzeLabel( string type, ifstream & file ) ;
vector<string> readNameGate(ifstream & file ) ;
void analyzeGate(ifstream & file, vector<string> gate) ;

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

    if ( type == ".model") {
        gModelName = GetaToken( file ) ;
    }
    else if ( type == ".inputs") {
        gInputs = readGate(file) ;
    }
    else if ( type == ".outputs") {
        gOutputs = readGate(file) ;

    }
    else if ( type == ".names") {

        vector<string> vectorName = readNameGate(file) ;
        for ( auto i : vectorName ) {
        }
        // cout << endl ;
        cout << ".names" << endl ;
        analyzeGate(file,vectorName) ;
    }
    else if ( type == ".end") {
        string temp = GetaToken( file ) ;
        // cout << temp << "\tend" << endl ;
    }
} // analyzeLabel

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
} // readNameGate

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


void analyzeGate(ifstream & file, vector<string> gate) {
    int gate_size = gate.size() ;
    string temp_String = PeekaToken(file) ;
    ProductTerm tempProductTerm ;
    vector<string> minterm ;
    tempProductTerm.output = gate.back() ;
    while(  ! temp_String.empty() && temp_String[0] != '.' ) {
        temp_String = GetaToken( file ) ;
        int string_size = temp_String.size() ;
        for ( int i = 0 ; i < string_size ; i ++ ) {
            if ( temp_String[i] == '1' ) {
                minterm.push_back( gate[i] ) ;
            }
            else if ( temp_String[i] == '0' ) {
                minterm.push_back( "~" + gate[i] ) ;
            }
            else if ( temp_String[i] == '-' ) {

            }
        }

        tempProductTerm.AllMinterms.push_back( vector<string>( minterm.begin(), minterm.end() ) ) ;
        minterm.clear() ;
        GetaToken(file) ; // read output value
        temp_String = PeekaToken(file) ;
    } // while
    
    gAllGateProductTerms.push_back( tempProductTerm ) ;
}


void Cube() {

} //

void Kernel() {

} // Kernel

void Divide() {

} // 

void ManageOption(int argc, char const *argv[]) {  
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << "BLIF_FILE" << std::endl;
        return ;
    } // if

    gblifFile = argv[1];
}

void PrintSumofProduct() {

    for ( auto gAllMinterms : gAllGateProductTerms ) {
        for ( auto Minterms : gAllMinterms.AllMinterms ) {
            for ( auto each : Minterms ) {
                cout << each ;
            }

            if ( Minterms != gAllMinterms.AllMinterms.back())
                cout << " + " ;
        }

        cout << " = " << gAllMinterms.output << endl ;
    }

}

int main(int argc, char const *argv[])
{
    ManageOption(argc, argv) ;
    readFile() ;
    PrintSumofProduct() ;
    return 0;
}
