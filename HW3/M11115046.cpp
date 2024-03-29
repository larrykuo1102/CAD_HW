# include <iostream>
# include <fstream>
#include <sstream>
# include <vector>
# include <string>
# include <unordered_map>
# include <set>
# include <unordered_set>
# include <algorithm>

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
int gNewGateNum = 0 ;
vector<ProductTerm> gAllGateProductTerms ;
bool gisStopPaddingSubstitution = false ;
bool gisStopAllCubeSubstitution = false ;

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
    while( ! temp_string.empty() && temp_string[0] != '.' && ! IsNumber(temp_string[0]) && temp_string[0] != '-' ) {
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
    if ( Xi[0] == 'Y' || Xi[0] == 'W' )
        return result ;
    for ( auto cube : cubes ) {
        if ( cube.find( Xi ) != cube.end() ) { // 有找到Xi[0]的就加入 result
            result.push_back( set<string>(cube.begin(),cube.end())) ;
        } // if
    } // for
    
    return result ;
}

set<string> CUBES_MAX(  vector<set<string>> cubes , string Xi, int index, unordered_map<string,int> & allliteral, bool & have_before ) {
    int size = cubes.size() ;
    int count_cube = 0 ;
    set<string> result ;
    result.insert(Xi) ;
    int max = 100000000, mintermindex = 0 ;
    for ( int i = 0 ; i < cubes.size() ; i ++ ) {
        if ( cubes[i].size() < max ) {
            max = cubes[i].size() ;
            mintermindex = i ;
        }
    }

    for ( auto i : cubes[mintermindex]) {
        if ( i != Xi && i[0] != 'Y' && i[0] != 'W' ) {
            for ( auto cube : cubes ) {
                if ( cube.find( i ) != cube.end() ) {
                    count_cube ++ ;
                } // if
                else {
                    break ;
                }
            } // for

            if ( count_cube == size ) {
                result.insert( i ) ;
                if ( allliteral[i] > allliteral[Xi]  ) {
                    have_before = true ;
                    return result ;
                }
            }

            count_cube = 0 ;
        } // if
    }

    return result ;
} // CUBES_MAX

vector<set<string>> cubesDivideC( vector<set<string>> cubes , set<string> Xi ) {
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

void Kernel( vector<set<string>> cubes, set<string> tempCo,  vector<string> & allliteral,  int index, vector<vector<set<string>>> & K,  vector<set<string>> & coK, unordered_map<string,int> & allliteralMap ) {
    int  allliteral_size = allliteral.size() ;
    vector<set<string>> newkernel ;
    set<string> C ;
    for ( int i = index ; i < allliteral_size ; i ++ ) {
        vector<set<string>> tempCube = CUBES( cubes, allliteral[i] ) ;
        if ( tempCube.size() >= 2 ) {
            bool havebefore = false ;
            C = CUBES_MAX( tempCube, allliteral[i], i, allliteralMap, havebefore) ;
            if ( ! havebefore ) {
                newkernel = cubesDivideC(tempCube, C ) ;
                C.insert(tempCo.begin(), tempCo.end()) ;
                Kernel( newkernel, C, allliteral, i+1, K, coK, allliteralMap) ;
            }
        }
    } // for

    K.push_back( cubes ) ;
    coK.push_back( set<string>( tempCo.begin(), tempCo.end() ) ) ;
} // Kernel

vector<string> GetAllliteral( vector<set<string>> cubes ) {
    set<string> all_set ;
    for ( auto cube : cubes ) {
        for ( auto i : cube ) {
            all_set.insert( i ) ;
        } // for    
    } // for

    return vector<string>( all_set.begin(), all_set.end() ) ;
} // GetAllliteral

vector<set<string>> IntersactionCube( vector<set<string>> A, vector<set<string>> B ){
    vector<set<string>> result ;
    for ( int i = 0 ; i < A.size() ; i ++ ) {
        for ( int j = 0 ; j < B.size() ; j ++ ) {
            if ( A[i] == B[j]) {
                result.push_back(B[j]) ;
                break ;
            }
        } // for
    } // for
    return result ;
} // Intersaction

vector<set<string>> MultipleCube( vector<set<string>> A, vector<set<string>> B ) {
    vector<set<string>> result ;

    for ( auto a : A ) {
        for ( auto b : B ) {
            set<string> temp ( a.begin(), a.end() ) ;
            temp.insert( b.begin(), b.end() ) ;
            result.push_back( temp ) ;
        } // for
    } // for

    return result ;
} // MultipleCube

vector<set<string>> MinusCube(  vector<set<string>> A, vector<set<string>> B  ) {
    for ( auto b : B ) {
        for ( int i = 0 ; i < A.size() ; i ++ ) {
            if ( A[i] == b ) {
                A.erase( A.begin() + i ) ;
                break ;
            }
        } // for
    } // for

    return A ;
} // MinusCube

pair<vector<set<string>>, vector<set<string>>> AlgebraicDivision( vector<set<string>> cubes, vector<set<string>> dividend ) {
    vector<set<string>> Q, R;
    for ( auto divide : dividend ) {
        vector<set<string> >interPart ;
        for ( auto cube : cubes ) {
            if (includes(cube.begin(), cube.end(), divide.begin(), divide.end())) {
                interPart.push_back( set<string>( cube.begin(), cube.end() ) ) ;
            }
        } // for

        vector<set<string>> Di = cubesDivideC( interPart, divide ) ;

        if ( interPart.empty() )
            return make_pair(Q, cubes); 

        if ( divide == dividend.front() ) {
            Q = Di ;
        } // if
        else {
            Q = IntersactionCube(Q, Di) ; // Q = Q ^ D
        } // else
    } // for
    

    // Calculate R = A - Q x B
    R = MinusCube( cubes, MultipleCube(Q,dividend)) ;

    return make_pair( Q,R) ;
} // AlgebraicDivision

bool Filtering( vector<set<string>> cubes, vector<set<string>> dividend  ) {
    if ( dividend.size() > cubes.size() )
        return false ;
    else {
        vector<string> temp1 = GetAllliteral(cubes) ;
        vector<string> temp2 = GetAllliteral(dividend) ;
        if ( temp2.size() > temp1.size() )
            return false ;
        set<string> tempset(temp1.begin(),temp1.end() ) ;
        for ( auto div : temp2 ) {
            if ( tempset.find(div) == tempset.end() ) 
                return false ;
        } // for
    } // else

    return true ;
} // Filtering

void Substitution( vector<set<string>> CoK ) {
    // for ( auto test : CoK ) {
    //     for ( auto a : test ) {
    //         cout << a <<  " " ;
    //     } // for
    //     cout << " + " ;
    // } // for
    // cout << endl ;
    int countsubstitution = 0 ;
    string newSubstitutionName ;
    int gALLTermsSize = gAllGateProductTerms.size() ;
    for ( int i = 0 ; i < gALLTermsSize ; i ++ ) {
        if ( Filtering( gAllGateProductTerms[i].Cubes, CoK ) ) {
            auto result = AlgebraicDivision( gAllGateProductTerms[i].Cubes, CoK ) ;
            // for ( auto a : result.first ) {
            //     for ( auto b : a  )
            //         cout << "test " << b.size() << " " ;
            //     cout << " + " ;
            // }
            if ( result.first.size() > 0 ) {
                if ( countsubstitution == 0 ) {
                    ProductTerm newproductterm ;
                    newSubstitutionName = "NEW" + to_string(gNewGateNum) ;
                    newproductterm.output = newSubstitutionName ;
                    newproductterm.Cubes = CoK ;
                    gAllGateProductTerms.push_back(newproductterm) ;
                    gNewGateNum ++ ;
                }
                
                vector<set<string>> newCube ;
                for ( auto each : result.first ) {
                    each.insert(newSubstitutionName) ;
                    newCube.push_back(set<string>(each.begin(), each.end())) ;
                } // for
                for ( auto each : result.second ) {
                    newCube.push_back( set<string>(each.begin(), each.end())) ;
                } // for

                gAllGateProductTerms[i].Cubes = newCube ;
                countsubstitution ++ ;
            } // if
            else {
                
            }
        } // if
    } // for
} // Substitution

int CountCommas(string str) {
    int count = 0;
    for (char ch : str) 
        if (ch == ',') 
            count++;

    return count;
}

int PickHighestYK( vector<vector<set<string>>> Kernel, vector<set<string>> CoK ) {
    int max_index = -1 ;
    int max = -1 ;

    for ( int i = 0 ; i < CoK.size() ; i ++ ) {
        int eachCoKSize = CoK[i].size() ;
        CoK[i].erase("") ;
        int temp_count = 0 ;
        if ( eachCoKSize >= 1 ) {
            for ( auto a : CoK[i] )
                temp_count += CountCommas(a) ;

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

            // cout << "index: " << i << " CoK literal: " <<  temp_count << " MatchKernel : " << tempset.size() << endl ;
            temp_count = temp_count * ( tempset.size() -1 ) ;

        } // if

        if( temp_count > 0 && temp_count > max ) {
            max_index = i ;
            max = temp_count;
        }
    } // for
    // cout << "Maxindex " << max_index << " " << max << endl ;
    // cout << "Kernel: " << endl ;
    // for ( auto product : Kernel[max_index] ) {
    //     for ( auto a : product ) {
    //         cout << a ;
    //     }
    //     cout << " + " ;
    // } 
    // cout << endl ;
    // cout << "CoK: " << endl ;
    // for ( auto a : CoK[max_index]) {
    //     cout << a << " " ;
    // }
    // cout << endl ;
    return max_index ;
} // 

void PersonalSubstitution( vector<set<string>> CoK, vector<vector<set<string>>> Kernel, int index) {
    int maxKernelIndex = -1, max = -10, maxcount = -1 ;
    for ( int i = 0 ; i < Kernel.size() ; i ++ ) {
        int count = ((int(Kernel[i].size())-1) * CoK[i].size()) -1 ; // (k-1) * |Cok|-2
        if ( count  > max ) {
            max = count ;
            maxKernelIndex = i ;
            maxcount = int(Kernel[i].size()) ;
        }   // if
        else if ( count == max ) {
            if ( int(Kernel[i].size()) > maxcount ) {
                max = count ;
                maxKernelIndex = i ;
                maxcount = int(Kernel[i].size()) ;
            }
        }
    } // for    
    if ( max < 0 )
        return ;

    string newSubstitutionName ;
    auto result = AlgebraicDivision( gAllGateProductTerms[index].Cubes, vector<set<string>>({CoK[maxKernelIndex]}) ) ;
    if ( result.first.size() > 0 ) {
        ProductTerm newproductterm ;
        newSubstitutionName = "NEW" + to_string(gNewGateNum) ;
        newproductterm.output = newSubstitutionName ;
        newproductterm.Cubes = result.first ;
        gAllGateProductTerms.push_back(newproductterm) ;
        gNewGateNum ++ ;

        vector<set<string>> newCube ;
        newCube.push_back( set<string>(CoK[maxKernelIndex].begin(), CoK[maxKernelIndex].end() ) ) ;
        newCube[0].insert(newSubstitutionName) ;
        for ( auto each : result.second ) {
            newCube.push_back( set<string>(each.begin(), each.end())) ;
        } // for

        gAllGateProductTerms[index].Cubes = newCube ;
    } // if

    
} // 

void runMultipleCubeExtraction() {
    vector<vector<vector<set<string>>>> eachProducttermsKernel ;
    vector<vector<set<string>>> eachProducttermsCoK ;
    for ( int i = 0 ; i < gAllGateProductTerms.size() ; i ++ ) {
        set<string> pertempCoK ;
        vector<vector<set<string>>> tempKernel ;
        vector<set<string>> tempCoKernel ;
        vector<string> tempLiteral = GetAllliteral(gAllGateProductTerms[i].Cubes) ;
        unordered_map<string,int> tempLiteralMap ;
        for ( int i = 0 ; i < tempLiteral.size() ; i ++ )
            tempLiteralMap.insert(pair<string,int>({tempLiteral[i],i})) ;
        // cout << "Run Kernel" << endl ;
        Kernel( gAllGateProductTerms[i].Cubes, pertempCoK,  tempLiteral, 0, tempKernel, tempCoKernel, tempLiteralMap ) ;
        // for ( int i = 0 ; i < tempKernel.size() ; i ++) {
        //     cout << "{" ;

        //     for ( auto cube : tempKernel[i] ) {
        //         for ( auto i : cube ) {
        //             cout << i ;
        //         } // for

        //         if ( cube.size() != 0 && cube != tempKernel[i].back() )
        //             cout << " + "  ;
        //     } // for

        //     cout << "}" ;

        //     for ( auto co : tempCoKernel[i])
        //         cout << co << " " ;
        //     cout << endl ;
        // }
        eachProducttermsKernel.push_back(tempKernel) ;
        eachProducttermsCoK.push_back(tempCoKernel) ;
    } // for

    vector<set<string>> auxCube ;
    // cout << "run Aux cube" << endl ;
    unordered_map<string,int> auxCubeCalculatecount ;
    for ( int i = 0 ; i < eachProducttermsKernel.size() ; i ++ ) { // 不同gate
        for ( int j = 0 ; j < eachProducttermsKernel[i].size() ; j ++ ) { // 有很多kernel
            set<string> tempset ;
            for ( auto KernelCube : eachProducttermsKernel[i][j] ) { // 每一個kernel裡面的productterm
                string tempstring = "X" ;
                vector<string> sortedvec(KernelCube.begin(), KernelCube.end() );
                sort( sortedvec.begin(), sortedvec.end() ) ;
                for ( auto term : sortedvec ) // productterm
                    tempstring += "," + term ;
                
                auxCubeCalculatecount[tempstring] ++ ;
            } // for
        } // for
    } // for    

    for ( int i = 0 ; i < eachProducttermsKernel.size() ; i ++ ) { // 不同gate
        string NewTag = "Y" + to_string(i) ;
        // tempset.insert(NewTag) ;
        
        for ( int j = 0 ; j < eachProducttermsKernel[i].size() ; j ++ ) { // 有很多kernel
            set<string> tempset ;
            tempset.insert(NewTag) ;
            int templiteralcount = 0 ;
            for ( auto KernelCube : eachProducttermsKernel[i][j] ) { // 每一個kernel裡面的productterm
                string tempstring = "X" ;
                vector<string> sortedvec(KernelCube.begin(), KernelCube.end() );
                sort( sortedvec.begin(), sortedvec.end() ) ;
                for ( auto term : sortedvec ) // productterm
                    tempstring += "," + term ;
                
                if ( auxCubeCalculatecount[tempstring] > 1 ) {
                    tempset.insert( tempstring ) ;
                    templiteralcount += sortedvec.size() ;
                }
            } // for

            if ( tempset.size() > 1 && templiteralcount > 1) 
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
    set<string> pertempCoK ;
    vector<vector<set<string>>> tempKernel ;
    vector<set<string>> tempCoKernel ;
    vector<string> tempLiteral = GetAllliteral(auxCube) ;
    // cout << tempLiteral.size() << endl ; ;
    unordered_map<string,int> tempLiteralMap ;
    for ( int i = 0 ; i < tempLiteral.size() ; i ++ )
        tempLiteralMap.insert(pair<string,int>({tempLiteral[i],i})) ;
    // cout << "Run aux padding kernel" << endl ;
    Kernel( auxCube, pertempCoK, tempLiteral, 0, tempKernel, tempCoKernel, tempLiteralMap ) ;
    // cout << "Kernel:\n" ;
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


    // cout << "CoK: " << tempCoKernel.size() << " \n{" ;

    // for ( auto cube : tempCoKernel ) {
    //     for ( auto i : cube ) {
    //         cout << i << " " ;
    //     } // for

    //     if ( cube.size() != 0  )
    //         cout <<  " + \n"  ;
    // } // for

    // cout << "}" << endl ;
    // cout << "Run PickKernel" << endl ;
    int destinateCok = PickHighestYK(tempKernel, tempCoKernel) ; 
    if ( destinateCok < 0 )
        gisStopPaddingSubstitution = true ;
    if ( gisStopPaddingSubstitution )
        return ;
    vector<set<string>> cofactor ;
    for ( auto i : tempCoKernel[destinateCok] ) { // 這就是CoKernel 
        istringstream iss(i);
        string token;
        set<string> tokens;

        while (std::getline(iss, token, ',')) { // 
            if ( token != "X" )    
                tokens.insert(token);
        }
        cofactor.push_back(tokens) ;
    } 
    
    // cout << cofactor.size() << " " << cofactor[0].size() << endl ;
    if ( cofactor.size() == 1 && cofactor[0].size() == 1 )
        gisStopPaddingSubstitution = true ;
    if ( gisStopPaddingSubstitution )
        return ;
    
    Substitution(cofactor) ;
} // runMultipleCubeExtraction

void runPersonalCubeExtraction() {
    vector<vector<vector<set<string>>>> eachProducttermsKernel ;
    vector<vector<set<string>>> eachProducttermsCoK ;
    int allgateindex = 0 ;
    while ( allgateindex < gAllGateProductTerms.size() ) {
        // cout << "PersonalExtraction Round: " << allgateindex << endl ;
        set<string> pertempCoK ;
        vector<vector<set<string>>> tempKernel ;
        vector<set<string>> tempCoKernel ;
        vector<string> tempLiteral = GetAllliteral(gAllGateProductTerms[allgateindex].Cubes) ;
        unordered_map<string,int> tempLiteralMap ;
        for ( int i = 0 ; i < tempLiteral.size() ; i ++ )
            tempLiteralMap.insert(pair<string,int>({tempLiteral[i],i})) ;
        Kernel( gAllGateProductTerms[allgateindex].Cubes, pertempCoK,  tempLiteral, 0, tempKernel, tempCoKernel, tempLiteralMap ) ;
        // for ( int i = 0 ; i < tempKernel.size() ; i ++) {
        //     cout << "{" ;

        //     for ( auto cube : tempKernel[i] ) {
        //         for ( auto i : cube ) {
        //             cout << i ;
        //         } // for

        //         if ( cube.size() != 0 && cube != tempKernel[i].back() )
        //             cout << " + "  ;
        //     } // for

        //     cout << "}" ;

        //     for ( auto co : tempCoKernel[i])
        //         cout << co << " " ;
        //     cout << endl ;
        // }
        // cout << "PersonalSubstitution" << endl ;
        PersonalSubstitution(tempCoKernel,tempKernel,allgateindex) ;
        
        allgateindex ++ ;
    } // for
} // runPersonalCubeExtraction

void runAllCubeExtraction() {
    vector<set<string>> auxCube ;

    for ( int i = 0 ; i < gAllGateProductTerms.size() ; i ++ ) { // 很多gate
        string newTag = "W" + to_string(i) ;
        for ( int j = 0 ; j < gAllGateProductTerms[i].Cubes.size() ; j ++ ) { // cube
            set<string> newSet(gAllGateProductTerms[i].Cubes[j].begin(), gAllGateProductTerms[i].Cubes[j].end()) ;
            if ( newSet.size() > 1 ) {
                newSet.insert(newTag) ;
                auxCube.push_back( newSet ) ;
            }
        } // for
    } // for

    set<string> pertempCoK ;
    vector<vector<set<string>>> tempKernel ;
    vector<set<string>> tempCoKernel ;
    vector<string> tempLiteral = GetAllliteral(auxCube) ;
    unordered_map<string,int> tempLiteralMap ;
    for ( int i = 0 ; i < tempLiteral.size() ; i ++ )
        tempLiteralMap.insert(pair<string,int>({tempLiteral[i],i})) ;
    Kernel( auxCube, pertempCoK,  tempLiteral, 0, tempKernel, tempCoKernel, tempLiteralMap ) ;


    int maxKernelIndex = -1, max = -10, maxcount = -1 ;
    for ( int i = 0 ; i < tempKernel.size() ; i ++ ) {
        if (int(tempCoKernel[i].size()) == 1 )
            continue ;
        int count = ((int(tempKernel[i].size())-1) * (tempCoKernel[i].size()-1)) -1 ; // (k-1) * |Cok|-2
        if ( count  > max ) {
            max = count ;
            maxKernelIndex = i ;
            maxcount = int(tempKernel[i].size()) ;
        }   // if
        else if ( count == max ) {
            if ( int(tempKernel[i].size()) > maxcount ) {
                max = count ;
                maxKernelIndex = i ;
                maxcount = int(tempKernel[i].size()) ;
            }
        }
    } // for    

    if ( max < 0 ) {
        gisStopAllCubeSubstitution = true ;
        return ;
    }

    Substitution( vector<set<string>> ({tempCoKernel[maxKernelIndex]}) ) ;

}

void ManageOption(int argc, char const *argv[]) {  
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << std::endl;
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

int calculateLiteral() {
    int result = 0 ;
    for ( auto each : gAllGateProductTerms ) 
        for ( auto cubes : each.Cubes ) 
            result += int(cubes.size()) ;

    return result ;
} // calculateLiteral

void outputBLIF() {
    ofstream outputfile("out.blif");
    outputfile << ".model " << gModelName << endl ;
    outputfile << ".inputs " ;
    for ( auto input : gInputs ) 
        outputfile << input << " " ;
    outputfile << endl << ".outputs " ;
    for ( auto output : gOutputs )
        outputfile << output << " " ;
    outputfile << endl ;
    for ( auto each : gAllGateProductTerms ) {
        outputfile << ".names " ;
        set<string> everySymbol ;
        for ( auto cube : each.Cubes ) {
            for ( auto a : cube ) {
                if ( a [0] == '~' )
                    everySymbol.insert(a.substr(1,a.size()-1)) ;
                else  
                    everySymbol.insert(a) ;
            } // for
        } // for

        vector<string> everySymbolvec(everySymbol.begin(), everySymbol.end() ) ;
        for ( auto symbol : everySymbolvec )
            outputfile << symbol << " " ;
        outputfile << each.output ;
        outputfile << endl ;

        for ( auto cube : each.Cubes) {
            string gatestring ;
            for ( auto symbol : everySymbolvec ) {
                if ( cube.find(symbol) != cube.end() ) {
                    gatestring += "1" ;
                } // if
                else if (cube.find("~"+symbol) != cube.end()) {
                    gatestring += "0" ;
                } // else if
                else {
                    gatestring += "-" ;
                }
            }
            gatestring += " 1\n" ;
            outputfile << gatestring ;
        }

    } // for
    outputfile << ".end" << endl ;

} // outputBLIF

int main(int argc, char const *argv[])
{
    ManageOption(argc, argv) ;
    readFile() ;
    if (gAllGateProductTerms.size() == 0)
        return 0 ;
    cout << "Performing..." << endl ;
    int originalLiteralCount = calculateLiteral() ;
    // PrintSumofProduct() ;

    runPersonalCubeExtraction() ;
    // PrintSumofProduct() ;
    while( ! gisStopAllCubeSubstitution ) {
        // cout << "runAllCubeExtraction: " << endl ;
        runAllCubeExtraction() ;
    }

    int count = 0 ;
    while( ! gisStopPaddingSubstitution ) {
        // cout << "runMultipleCubeExtraction: " << count << endl ;
        runMultipleCubeExtraction() ;
        // PrintSumofProduct() ;
        count ++ ;
    }
    int newLiteralCount = calculateLiteral() ;
    cout << "originalLiterCount: " << originalLiteralCount << endl ;
    cout << "newLiteralCount: " << newLiteralCount << endl ;
    outputBLIF() ;
    return 0;
}
