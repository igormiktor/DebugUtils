#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "DebugUtils.hpp"





int main( int, char** )
{
    std::cout << "Hello from DUTest!" << std::endl;
    std::cout << "C++ version is " << __cplusplus << std::endl;
    std::cout << "DebugUtils is " << ( DEBUGUTILS_ON ? "ON" : "OFF" ) << std::endl;

    // Comment out the next line to send debug output to std::cerr
    DebugUtils::DebugFileOn debugFileEnabled( "Test_Log" );  
    // Above line could be replace by macro: logDebugToFile( "Test_Log" ) 

    std::string ex1 = "Test string";
    std::map<int, std::string> ex2 = { {1, "one"}, {2, "two"}, {3, "three"}, {4, "four"} };
    std::pair<int, double> ex3 = { 18, 2.71828};
    std::vector<std::vector<int>> ex4 = 
    {
        { 11, 12, 13 },
        { 21, 22, 23 },
        { 31, 32, 33 }
    };

    // Make sure compiler in no-debug case doesn't strip everything out
    std::cout << "ex1: " << ex1 << std::endl;
    std::cout << "ex2[3]: " << ex2[3] << std::endl;
    std::cout << "ex3.second: " << ex3.second << std::endl;
    std::cout << "ex4[1][1]: " << ex4[1][1] << std::endl;

    debugV( "This is a debug message" );
   
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

    // Conditional debugging stuff (change to false to turn off the following debug statements)
    auto includeThisDebug{ true };

    debugCondM( includeThisDebug, "This is a conditional debug message");
    debugCondV( includeThisDebug, ex1, ex2 );
    // End conditional debugging examples

    // Finished
    debugM( "Test/demo is complete" );

    // Make sure compiler doesn't strip out loop above when optimizing
    std::cout << "v[1]: " << v[1] << std::endl;
}
