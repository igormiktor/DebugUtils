/*
    DebuggingUtils,hpp   
    
    Debugging code using policy types that drive template specialization and function overloading 
    that generate debugging code when requested and no code not requested.  
    
    Debug code generation is selected by #define DEBUGUTILS_ON to be non-zero at compile time.  

    There is no preprocessor selected code generation.  All the selection of what code is generated 
    (or not generated) is governed by template specialization (helped along by type inference by the 
    compiler) and function overloading.  Certain template specializations and function overloads 
    generate debugging code; other template specializations and function overloads generate no code.

    Varidic template functions allow as many variables to be output as desired.  Template recursion
    enables the debug printing of complex nested data types and data structures (e.g., vectors of pairs, 
    lists of vectors, maps of strings, queues of tuples).

    A small number of preprocessor macros are provided to provide a way to conveniently capture __FILE__ and 
    __LINE__ at the point of calling the debugging functions.  If someone knows a way to do this without using 
    a macro function, let me know!  Note that none of these macros are conditional.  Another macro simply hides
    a simple but rote object instantiation and is absolutely not necessary.

    This code was an experiment to see how far modern C++20 tools can replace the preprocessor to conditionally 
    generate code.  Turns out I was able to entirely replace it.

    This code is adapted from code by Anshul Johri.  They did not provide any license or copyright info.
    You can access their version at https://github.com/Anshul-Johri-1/Debug-Template

    This version of the code is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/






#ifndef DebugUtils_hpp
#define DebugUitls_hpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <type_traits>
#include <string>
#include <iomanip>
#include <ctime>
#include <bits/stdc++.h>



#ifndef DEBUGUTILS_ON
    #define DEBUGUTILS_ON 0
#endif





namespace DebugUtils
{

    // Utility structure used to type switch on DEBUGUTILS_ON 
    template<bool B> 
    struct TypeSelect {};

    template<>
    struct TypeSelect<true>
    {
        using type = typename std::true_type;
    };

    template<>
    struct TypeSelect<false>
    {
        using type = typename std::false_type;
    };



    // DebugUtilsPolicy is a type used to manage template instantiation and specialization,
    // as well as overload resolution.
    
    // When DebugUtilsPolicy == std::true_type, code for debugging is compiled.
    // When DebugUtilsPolicy == std::false_type, it is not.

    using DebugUtilsPolicy = typename TypeSelect<DEBUGUTILS_ON>::type;



    // This generic type is an intentionally trivial class
    template <typename T>
    class DebugFileOnBase
    {
        public:
            constexpr DebugFileOnBase( T, const char* ) {}
    };

    // Specialization applies when debug mode is on (DebugUtilsPolicy == std::true_type)
    // It is the only template instantiation that does anything
    template <>
    class DebugFileOnBase<std::true_type>
    {
        public:
            DebugFileOnBase( std::true_type, const char* filename ) 
                : mErrorLogFile{}, mOriginalCerrBuff{ nullptr }
            {
                auto t = std::time( nullptr );
                auto tm = *std::localtime( &t );
                std::ostringstream oss;
                oss << std::put_time( &tm, "%Y%m%d_%H%M%S" );
                
                std::string fn{ filename };
                fn += '_' + oss.str() + ".log";
                mErrorLogFile.open( fn );
                if ( mErrorLogFile )
                {
                    mOriginalCerrBuff = std::cerr.rdbuf();          // Save cerr buffer to restore in destructor
                    std::cerr.rdbuf( mErrorLogFile.rdbuf() );
                    std::cerr << "Error logging redirected to file " << fn << std::endl;
                }
                else
                {
                    std::cerr << "Unable to open debug logging file " << fn << std::endl;
                }
            }

            ~DebugFileOnBase()
            {
                std::cerr << "Closing the debug logging file" << std::endl;
                if ( mOriginalCerrBuff )
                {
                    std::cerr.rdbuf( mOriginalCerrBuff );       // Restore cerr
                }
                // ofstream destructor closes the file
            }


        private:
            std::ofstream       mErrorLogFile;
            std::streambuf*     mOriginalCerrBuff;
    };


    class DebugFileOn : public DebugFileOnBase<DebugUtilsPolicy>
    {
        public:
            DebugFileOn( const char* filename ) : DebugFileOnBase( DebugUtilsPolicy{}, filename ) {}
    };



    // Following code uses template recursion on variadic function templates using
    // the function print(...).

    // Helper concept/requirement
    template <typename T>
    concept is_iterable = requires( T &&x ) { begin(x); } &&
                            !std::is_same_v<std::remove_cvref_t<T>, std::string>;


    // The functions below handle single arguments, which provide the "base cases" for template recursion (the single argument case)
    
    // There handle specific types of single arguments base cases
    inline void print( const char* x )  { std::cerr << x; }

    inline void print( char x )  { std::cerr << "\'" << x << "\'"; }

    inline void print( bool x )  { std::cerr << (x ? "T" : "F"); }

    inline void print( std::string x )  { std::cerr << "\"" << x << "\""; }

    inline void print( std::vector<bool>& v )
    { 
        // Overloaded this because stl optimizes vector<bool> by using
        // _Bit_reference instead of bool to conserve space.
        int f{ 0 };
        std::cerr << '{';
        for ( auto &&i : v )
        {
            std::cerr << (f++ ? "," : "") << ( i ? "T" : "F" );
        }
        std::cerr << "}";
    }

    // This template handles all other single argument cases
    template <typename T>
    void print( T&& x )
    {
        if constexpr ( is_iterable<T> )                                 // Various iterables...
        {
            if ( size(x) && is_iterable<decltype( *(begin(x)) )> )      // Iterable inside Iterable
            {
                int f{ 0 };
                std::cerr << "\n~~~~~\n";
                int w = std::max( 0, (int) std::log10( size(x) - 1 ) ) + 2;
                for ( auto&& i : x )
                {
                    std::cerr << std::setw(w) << std::left << f++, print(i), std::cerr << "\n";
                }
                std::cerr << "~~~~~\n";
            }
            else                                                        // A plain, normal Iterable
            {
                int f{ 0 };
                std::cerr << "{";
                for ( auto&& i : x )
                    std::cerr << ( f++ ? "," : "" ), print( i );
                std::cerr << "}";
            }
        }
        else if constexpr ( requires { x.pop(); } )                     // Stacks, Priority Queues, Queues 
        {
            auto temp{ x };
            int f{ 0 };
            std::cerr << "{";
            if constexpr ( requires { x.top(); } )
                while ( !temp.empty() )
                    std::cerr << ( f++ ? "," : "" ), print( temp.top() ), temp.pop();
            else
                while ( !temp.empty() )
                    std::cerr << ( f++ ? "," : "" ), print( temp.front() ), temp.pop();
            std::cerr << "}";
        }
        else if constexpr ( requires { x.first; x.second; } )           // Pair 
        {
            std::cerr << '(', print( x.first ), std::cerr << ',', print( x.second ), std::cerr << ')';
        }
        else if constexpr ( requires { get<0>(x); } )                   // Tuple 
        {
            int f{ 0 };
            std::cerr << '(', apply( [&f](auto... args) { (( std::cerr << (f++ ? "," : ""), print(args) ), ...); }, x );
            std::cerr << ')';
        }
        else
            std::cerr << x;                                             // Anything else
    }


    // This is the variadic function that drives the recursion
    template <typename T, typename... V>
    void printer( const char* names, T&& head, V&&... tail )
    {
        int i{ 0 };
        for ( int bracket = 0; names[i] != '\0' and ( names[i] != ',' or bracket > 0 ); i++ )
        {
            if ( names[i] == '(' or names[i] == '<' or names[i] == '{' )
                bracket++;
            else if ( names[i] == ')' or names[i] == '>' or names[i] == '}' )
                bracket--;
        }
        std::cerr.write( names, i ) << " = ";
        print( head );
        if constexpr ( sizeof...(tail) )
            std::cerr << " ||", printer( names + i + 1, tail... );
        else
            std::cerr << " ]\n";
    }


    // This a version of the variadic function for plain arrays 
    // Pass using macros as debugArr( array1Ptr, N1, array2Ptr, N2, array3Ptr, N3 )
    template <typename T, typename... V>
    void printerArr( const char* names, T arr[], size_t n, V... tail )
    {
        size_t i = 0;
        for ( ; names[i] and names[i] != ','; i++ )
            std::cerr << names[i];
        for ( i++; names[i] and names[i] != ','; i++ )
            ;
        std::cerr << " = {";
        for ( size_t ind = 0; ind < n; ind++ )
            std::cerr << ( ind ? "," : "" ), print( arr[ind] );
        std::cerr << "}";
        if constexpr ( sizeof...( tail ) )
            std::cerr << " ||", printerArr( names + i + 1, tail... );
        else
            std::cerr << " ]\n";
    }


    // Following set-up the functions actually called by user code.  They are overloaded on the first parameter.
    // When the first parameter is of type convertible to std::true_type, actual debug code is generated.
    // When the first parameter is convertible to std::false_type, empty functions (compiler eliminated) are generated.


    //*** debugPrinterV() variants

    // Debugging version overload
    template <typename T, typename... V>
    void debugPrinter( std::true_type, const char* filename, int lineNbr, const char* names, T&& head, V&&... tail )
    {
        std::cerr << std::filesystem::path{ filename }.filename().string() << "(" << lineNbr << ") [ ", printer( names, head, tail... );
    }

    // Non-debugging version overload, an empty function
    template <typename T, typename... V>
    constexpr void debugPrinter( std::false_type, const char* filename, int lineNbr, const char* names, T&& head, V&&... tail ) {}
 
    // Function overload actually called in user code that triggers selection of debug/non-debug versions
    template <typename T, typename... V>
    void debugPrinter( const char* filename, int lineNbr, const char* names, T&& head, V&&... tail )
    {
        debugPrinter( DebugUtilsPolicy{}, filename, lineNbr, names, head, tail... );
    }


    //*** debugPrinterArr() variants

    // Debugging version overload
    template <typename T, typename... V>
    void debugPrinterArr( std::true_type, const char* filename, int lineNbr, const char* names, T arr[], size_t n, V... tail )
    {
        std::cerr << std::filesystem::path{ filename }.filename().string() << "(" << lineNbr << ") [ ", printerArr( names, arr, n, tail... );
    }

   // Non-debugging version overload
    template <typename T, typename... V>
    constexpr void debugPrinterArr( std::false_type, const char* filename, int lineNbr, const char* names, T arr[], size_t n, V... tail ) {}

    // Function overload actually called in user code that triggers selection of debug/non-debug versions
    template <typename T, typename... V>
    void debugPrinterArr( const char* filename, int lineNbr, const char* names, T arr[], size_t n, V... tail )
    { 
        debugPrinterArr( DebugUtilsPolicy{}, filename, lineNbr, names, arr, n, tail... );
    }


    //*** debugMsg() variants ***

    // Debugging version overload
    template<typename T>
    void debugMsg( std::true_type, const char* filename, int lineNbr, T output )
    {
        std::cerr << std::filesystem::path{ filename }.filename().string() << "(" << lineNbr << "): " << output << std::endl;
    }

    // Non-debuging version overload
    template<typename T>
    constexpr void debugMsg( std::false_type, const char*,  int , T ) {}

    // Function overload actually called in user code that triggers selection of debug/non-debug versions
    template <typename T>
    void debugMsg( const char* filename, int lineNbr, T msg )
    {
        debugMsg( DebugUtilsPolicy{}, filename, lineNbr, msg );
    }

}       // namespace DebugUtils


// Convenience macros to facilitate use of __FILE__ and __LINE__ and catenation of variable names
#define debugV(...)         DebugUtils::debugPrinter( __FILE__,  __LINE__, #__VA_ARGS__, __VA_ARGS__ )
#define debugArr(...)       DebugUtils::debugPrinterArr( __FILE__,  __LINE__, #__VA_ARGS__, __VA_ARGS__ )
#define debugM( msg )       DebugUtils::debugMsg( __FILE__,  __LINE__, msg )

#define logDebugToFile( filename )      DebugUtils::DebugFileOn debugEnabled( filename )

#endif  // DebugUtils_h
