# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <unordered_map>
using namespace std ;

// vector<string>
unordered_map<string,string> gNameGate ;
string gModelName ;

void readGate(ifstream & file) ;
void analyzeGate( string type, ifstream & file ) ;
void readUntilNotWhiteSpace( ifstream & file) ;

void readFile() {
    ifstream inputFile( "./aoi_benchmark/aoi_sample01.blif") ;
    if ( ! inputFile.is_open() )  {
        cerr << "File not opened !" ;
        return ;   
    }
    else 
        cout << "File opened" << endl ;
    
    while( ! inputFile.eof()) {
        readGate(inputFile) ;
    }
}

string readString( ifstream & file ) {
    cout << "readString" << endl ;
    readUntilNotWhiteSpace(file) ;
    // string line ;
    // getline(file,line) ;
    // cout << line  << endl ;
    string a_string ;
    char onechar = file.peek() ;
    while( file.peek() != '\n' && file.peek() != -1 ) {
        onechar = file.peek() ;
        if ( onechar == ' ' || onechar == '\n' || onechar == ',' || onechar == ';' || onechar == '\t' ){
            break;
        } // if
        
        a_string += file.get() ;
    } // while

    cout << a_string << " end readString" << endl ;
    return a_string ;
}

void readGate( ifstream & file ) {
    // cout << "readGate" << endl ;
    char onechar ;
    string firstString ;
    while( file.peek() != '\n') {
        onechar = file.get() ;
        if ( onechar == '.' ) {
            firstString = onechar + readString(file) ;
            analyzeGate( firstString, file) ;
        } // if
        else {

        } //
    } // while
    return ;
} // readGate

void analyzeGate( string type, ifstream & file ) {
    cout << "analyzeGate " << type << endl ;
    if ( type == ".model") {
        gModelName = readString( file ) ;
        cout << "gModel Name " << gModelName << endl ;
    }
    else if ( type == ".inputs") {
        string temp = readString( file ) ;
        cout << temp << " inputs" << endl ;
        
    }
    else if ( type == ".outputs") {
        string temp = readString( file ) ;
        cout << temp << " outputs" << endl ;
    }
    else if ( type == ".names") {
        string temp = readString( file ) ;
        cout << temp << " names" << endl ;
    }
    else if ( type == ".end") {
        string temp = readString( file ) ;
        cout << temp << " end" << endl ;
    }
} // analyzeGate

void readUntilNotWhiteSpace( ifstream & file) {
    while( file.peek() == ' ' ) {
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
