# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <unordered_map>
# include <set>
# include <unordered_set>
using namespace std ;


struct ProductTerm {
    string output ;
    vector<set<string>> Cubes ;
} ;

string gModelName ;
string gNewToken ;
string gblifFile ;
vector<string> gInputs ;
vector<string> gOutputs ;

vector<ProductTerm> gAllGateProductTerms ;

void readUntilNotWhiteSpace( ifstream & file) ;
string GetaToken(ifstream & file) ;
string ReadaToken( ifstream & file ) ;
string PeekaToken(ifstream & file) ;
bool IsNumber( char ch ) ;
void readLabel(ifstream & file) ;
vector<string> readGate(ifstream & file ) ;
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

        tempProductTerm.Cubes.push_back( set<string>( minterm.begin(), minterm.end() ) ) ;
        minterm.clear() ;
        GetaToken(file) ; // read output value
        temp_String = PeekaToken(file) ;
    } // while
    
    gAllGateProductTerms.push_back( tempProductTerm ) ;
}


vector<set<string>> CUBES( vector<set<string>> cubes, string Xi ) {
    vector<set<string>> result ;
    if ( Xi[0] == 'Y' )
        return result ;
    for ( auto cube : cubes ) {
        if ( cube.find( Xi ) != cube.end() ) { // 有找到Xi[0]的就加入 result
            result.push_back( set<string>(cube.begin(),cube.end())) ;
        } // if
    } // for
    
    return result ;
}

set<string> CUBES_MAX(  vector<set<string>> cubes , string Xi ) {
    int size = cubes.size() ;
    int count_cube = 0 ;
    set<string> result ;
    result.insert(Xi) ;
    for ( auto i : cubes[0]) {
        if ( i != Xi ) {
            for ( auto cube : cubes ) {
                if ( cube.find( i ) != cube.end() ) {
                    count_cube ++ ;
                } // if
                else {
                    break ;
                }
            } // for

            if ( count_cube == size )
                result.insert( i ) ;

            count_cube = 0 ;
        } // if
    }

    return result ;
} // CUBES_MAX

vector<set<string>> cubesDivideC( vector<set<string>> cubes , set<string> Xi ) {
    for ( auto i : Xi ) {
        for ( int j = 0 ; j < cubes.size() ; j ++ ) {
            if ( cubes[j].find(i) == cubes[j].end() ) {
                cubes.erase( cubes.begin() + j ) ;
                j -- ;
            } // if
        } // for
    } // for

    for ( int i = 0 ; i < cubes.size() ; i ++ ) {
        for ( auto j : Xi )
            cubes[i].erase(j) ;
    }
    return cubes ;
} // cubesDivideC

bool ExistBeforeLiteral( vector<string> allliteral, int index, set<string> Xi ) {
    for ( int i = 0 ; i < index ; i ++ ) {
        if ( Xi.find(allliteral[i]) != Xi.end() ) {
            return false ;
        } // if
    } // for

    return true ;
} // ExistBeforeLiteral

void Kernel( vector<set<string>> cubes, vector<string> allliteral,  int index, vector<vector<set<string>>> & K,  vector<set<string>> & coK ) {
    int  allliteral_size = allliteral.size() ;
    vector<set<string>> newkernel ;
    set<string> C ;
    for ( int i = index ; i < allliteral_size ; i ++ ) {
        if ( CUBES( cubes, allliteral[i] ).size() >= 2 ) {
            C = CUBES_MAX( cubes, allliteral[i]) ;

            if ( ExistBeforeLiteral( allliteral, i, C ) ) {
                newkernel = cubesDivideC(cubes, C ) ;
                Kernel( newkernel, allliteral, i+1, K, coK) ;
            }
        }
    } // for

    K.push_back( cubes ) ;
    coK.push_back( C ) ;
} // Kernel

vector<string> GetAllliteral( vector<set<string>> cubes ) {
    set<string> all_set ;
    for ( auto cube : cubes ) {
        for ( auto i : cube ) {
            all_set.insert( i ) ;
        } // for    
    } // for

    return vector<string>( all_set.begin(), all_set.end() ) ;
} //

void Divide( vector<string> cube, vector<set<string>> dividend ) {

} // 

void Subtition( ) {

} // Subtition

int PickHighestYK( vector<vector<set<string>>> Kernel ) {
    int max_index = 0 ;
    int max = -1 ;

    // for ( auto eachKernelCubes : Kernel) {
    for ( int i = 0 ; i < Kernel.size() ; i ++ ) {
        set<string> tempset ;
        for ( auto cube : Kernel[i] ) {
            for (auto rit = cube.rbegin(); rit != cube.rend(); ++rit) {
                string tempstring = *rit ;
                if ( tempstring[0] == 'Y' ) {
                    tempset.insert(tempstring) ;
                    break ;
                } 
            }
        } // for
        if( int(tempset.size()) > max ) {
            max_index = i ;
            max = tempset.size() ;
        }
    }

    cout << "Maxindex " << max_index << " " << max << endl ;
    return max_index ;
} // 

void runMultipleCubeExtraction() {
    vector<vector<vector<set<string>>>> eachProducttermsKernel ;
    for ( int i = 0 ; i < gAllGateProductTerms.size() ; i ++ ) {
    // for ( int i = 2 ; i < 3 ; i ++ ) {
        vector<vector<set<string>>> tempKernel ;
        vector<set<string>> tempCoKernel ;
        vector<string> tempLiteral = GetAllliteral(gAllGateProductTerms[i].Cubes) ;
        Kernel( gAllGateProductTerms[i].Cubes, tempLiteral, 0, tempKernel, tempCoKernel ) ;
        eachProducttermsKernel.push_back(tempKernel) ;
    } // for

    vector<set<string>> auxCube ;

    for ( int i = 0 ; i < eachProducttermsKernel.size() ; i ++ ) { // 不同gate
        string NewTag = "Y" + to_string(i) ;
        // tempset.insert(NewTag) ;
        
        for ( auto eachKernelCubes : eachProducttermsKernel[i] ) { // 有很多kernel
            set<string> tempset ;
            tempset.insert(NewTag) ;
            for ( auto KernelCube : eachKernelCubes ) { // 每一個kernel裡面的productterm
                string tempstring = "X" ;
                for ( auto term : KernelCube ) // productterm
                    tempstring += term ;
                
                tempset.insert( tempstring ) ;
            } // for
            
            auxCube.push_back( tempset ) ;  
        } // for
    } // for    

    // for ( auto cube : auxCube ) {
    //     for ( auto i : cube ) {
    //         cout << i ;
    //     } // for

    //     if ( cube != auxCube.back() )
    //         cout << " + " << endl  ;
    // } // for
    // cout << endl ;

    vector<vector<set<string>>> tempKernel ;
    vector<set<string>> tempCoKernel ;
    vector<string> tempLiteral = GetAllliteral(auxCube) ;
    Kernel( auxCube, tempLiteral, 0, tempKernel, tempCoKernel ) ;

    // for ( auto eachKernelCubes : tempKernel) {
    //         cout << "{" ;

    //         for ( auto cube : eachKernelCubes ) {
    //             for ( auto i : cube ) {
    //                 cout << i ;
    //             } // for

    //             if ( cube.size() != 0 && cube != eachKernelCubes.back() )
    //                 cout << " + "  ;
    //         } // for

    //         cout << "}" << endl ;
    // }


    // cout << "{" ;

    // for ( auto cube : tempCoKernel ) {
    //     for ( auto i : cube ) {
    //         cout << i << " " ;
    //     } // for

    //     if ( cube.size() != 0  )
    //         cout <<  " + \n"  ;
    // } // for

    // cout << "}" << endl ;
    int destinateCok = PickHighestYK(tempKernel) ; 
    for ( auto i : tempCoKernel[destinateCok] ) { // 這就是CoKernel 
        i.erase(i.begin()) ;
        cout << i ;
    } 
    cout << endl ;

    // 把CoKernel 裡面的X 刪掉並且轉成 vector<set<string>> 的形式
    // substition
} // runMultipleCubeExtraction

void ManageOption(int argc, char const *argv[]) {  
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << "BLIF_FILE" << std::endl;
        return ;
    } // if

    gblifFile = argv[1];
}

void PrintSumofProduct() {

    for ( auto AllMinterms : gAllGateProductTerms ) {
        cout  << AllMinterms.output << " = " ;
        for ( auto cube : AllMinterms.Cubes ) {
            for ( auto each : cube ) {
                cout << each ;
            }

            if ( cube != AllMinterms.Cubes.back())
                cout << " + " ;
        }

        cout <<  endl ;
    }

}

int main(int argc, char const *argv[])
{
    ManageOption(argc, argv) ;
    readFile() ;
    PrintSumofProduct() ;
    for ( int i = 0 ; i < 10 ; i ++ ) {
        runMultipleCubeExtraction() ;
    }

    return 0;
}

/*
multiple cube extraction{
    先把每一個sumofproduct 去做kernel演算法 並且拿到kernel K() CoK()也要留

    將每一個Kernel的值變成一個變數再做成一個productterm 並乘上自己的label

    把新做的productterm全部or在一起 並且做一次kernel演算法得到 K() CoK()

}

f = vector<vector<string>> 
kernels ( f, j, coKernel ) {
    k = vector<vector<vector<string>>>
    for i to n {
        if CUBES(f,Xi).size() >= 2 {
            C = cubes_max( f, xi )

            temp = kernels( f/C, i + 1) ;
            coKernel.push_back( C ) ;
            k.push_back( temp ) ;
        }
    }

}

CUBES( vector<set<string>>  f, xi ) {

    result = for each in f,  each contain xi 


    return vector<set<string>>  ;
}

cubes_max( f / Xi) {
    result = for each in f,  each contain xi 
}

substition( gAllGateProductTerms,  kernels ) {
    for kernel in kernels :
        for each in gAllGateProductTerms :
            if kernel in each :
                取代
                new 新的gate
}

division () {

}

*/