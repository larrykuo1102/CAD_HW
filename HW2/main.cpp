# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <unordered_map>
using namespace std ;

// vector<string>
unordered_map<string,string> gNameGate ;
string gModelName ;
string gNewToken ;
vector<string> gInputs ;
vector<string> gOutputs ;

void readLabel(ifstream & file) ;
void analyzeLabel( string type, ifstream & file ) ;
void readUntilNotWhiteSpace( ifstream & file) ;
vector<string> readGate(ifstream & file ) ;
string GetaToken(ifstream & file) ;
string ReadaToken( ifstream & file ) ;
string PeekaToken(ifstream & file) ;
bool IsNumber( char ch ) ;
void analyzeGate(ifstream & file, vector<string> gate) ;

void readFile() {
    ifstream inputFile( "./aoi_benchmark/aoi_sample01.blif") ;
    if ( ! inputFile.is_open() )  {
        cerr << "File not opened !" ;
        return ;   
    }
    else 
        cout << "File opened" << endl ;
    
    while( ! inputFile.eof()) {
        readLabel(inputFile) ;
    }
}

string ReadaToken( ifstream & file ) {
    // cout << "ReadaToken" << endl ;
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

    // cout << a_string << " end ReadaToken" << endl ;
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

void readLabel( ifstream & file ) {
    // cout << "readLabel" << endl ;
    char onechar ;
    string firstString =  GetaToken(file) ;
    if ( firstString[0] == '.' )
        analyzeLabel( firstString, file) ;
} // readLabel

void analyzeLabel( string type, ifstream & file ) {
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
        }

        cout << endl ;
    }
    else if ( type == ".outputs") {
        gOutputs = readGate(file) ;
        cout << "\toutputs: " ;
        for ( auto i : gOutputs ) {
            cout << i << " " ;
        }
        cout << endl ;
    }
    else if ( type == ".names") {
        cout << "\tnames: " ;
        vector<string> tempname = readGate(file) ;
        for ( auto i : tempname ) {
            cout << i << " " ;
        }
        cout << endl ;

        analyzeGate(file,tempname) ;

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

void analyzeGate(ifstream & file, vector<string> gate) {
    int gate_size = gate.size() ;
    string temp_String = GetaToken(file) ;
    cout << temp_String << "\tFirst Gate" << endl ;
    if (temp_String.find('-') != string::npos )  {
        cout << "=====OR Gate=====" << endl ;
        GetaToken(file) ;
        for ( int i = 2 ; i < gate.size() ; i ++ ) {
            GetaToken(file) ;
            GetaToken(file) ;
        }
    } // if
    else if ( temp_String.find('0') != string::npos) {
        cout << "=====NOT Gate=====" << endl ;
        GetaToken(file) ;
    } // else if
    else {
        cout << "=====AND Gate=====" << endl ;
        GetaToken(file) ;
    }

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


int main(int argc, char const *argv[])
{   
    cout << "Start" << endl ;
    readFile() ;
    /* code */

    cout << "End" << endl ;
    return 0;
}
