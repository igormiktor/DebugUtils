#include <iostream>
#include <fstream>
#include <vector>

#include "DebugUtils.hpp"





int main( int, char** )
{
    std::cout << "Hello from DUTest!" << std::endl;
    std::cout << "C++ version is " << __cplusplus << std::endl;
    std::cout << "DebugUtils is " << ( DEBUGUTILS_ON ? "ON" : "OFF" ) << std::endl;

    DebugUtils::DebugFileOn debugFileEnabled( "Test_Log" );  
    // Above line could be replace by macro: logDebugToFile( "Test_Log" ) 

    debugV( "This is a debug message" );

    std::string ex1 = "Test string";
    std::map<int, std::string> ex2 = { {1, "one"}, {2, "two"}, {3, "three"}, {4, "four"} };
    std::pair<int, double> ex3 = { 18, 2.71828};
    std::vector<std::vector<int>> ex4 = 
    {
        { 11, 12, 13 },
        { 21, 22, 23 },
        { 31, 32, 33 }
    };
   
    debugV( ex1 );
    debugV( ex2 );
    debugV( ex3 );
    debugV( ex4 );
    debugV( ex1, ex3 );

    std::vector<int> v;
    for ( auto i = 1; i <= 3; i++ )
    {
        v.push_back( i * i );
        debugV( i, v );
    }
    debugV( v );

    debugM( "Test/demo is complete" );
}
