// ***************************************************************************
// bamtools_getopt.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 26 May 2010
// ---------------------------------------------------------------------------
// Provides a configurable commandline parser used by the BamTools subtools
// ***************************************************************************

#ifndef BAMTOOLS_GETOPT_H
#define BAMTOOLS_GETOPT_H

// C includes
#include <cassert>
#include <cstdlib>

// C++ includes
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace BamTools { 

class GetOpt {
  
    // ctors & dtor
    public:
      
        // ctor: takes the 'standard' command line args (optional offset)
        GetOpt(int argc, char* argv[], int offset = 0);
        
        // d-tor
        ~GetOpt(void);
  
    // set rules for bare word arguments
    public:
        // add an optional 'bare word' argument (eg 'help')  
        // 'name' is not used on the command line, but for reporting
        void addOptionalArgument(const std::string& name, std::string* value); 
        
        // add a required 'bare word' argument (eg input data file)
        // 'name' is not used on the command line, but for reporting
        void addRequiredArgument(const std::string& name, std::string* value);
      
    // set rules for key=>value options  
    public:
        // add standard option with arguments ( -Wall, -O2, --type=foo )
        void addOption(const char shortName, const std::string& longName, std::string* value);
        
        // add an option whose argument is optional (eg --log may default to dumping to stderr, unless a file is specified )
        // must provide a default string 
        void addOptionalOption(const char shortName, const std::string& longName, std::string* value, const std::string& defaultValue);
        void addOptionalOption(const std::string& longName, std::string* value, const std::string& defaultValue);
        
        // add a repeatable option (like compiler includes -I/path/ -I/path2/ etc)
        // only supporting one type of name (short/long) for this option for now
        void addRepeatableOption(const char shortName, std::vector<std::string>* values);        // single char version 
        void addRepeatableOption(const std::string& longName, std::vector<std::string>* values); // long name version   
        
        // add an option that takes a variable number of arguments ( --files f1 f2 f3 f4... )
        void addVariableLengthOption(const std::string& longName, std::vector<std::string>* values);
        
    // set rules for on/off switch    
    public:
        // on/off switch ( --verbose --searchOnly ) only long names supported for now
        void addSwitch(const std::string& longName, bool* ok);
        
    // parse and query methods
    public:
      
        // get application name
        const std::string& applicationName(void) const;
      
        // query if particular 'bare-word' argument is set
        bool isSet(const std::string& name) const;
      
        // runs parser (does validation and assign values to arguments) 
        // returns success/fail
        bool parse(void);
        
        void print(void);
        
    // define Option-related types & enums
    private:
        enum OptionType { OptUnknown = 0
                        , OptEnd
                        , OptSwitch
                        , OptArg1
                        , OptOptional
                        , OptRepeat
                        , OptVariable
                        };
                        
        // define Option 
        struct Option {
          
            // ctor
            Option(OptionType t = OptUnknown, const char shortName = 0, const std::string& longName = "")
                : Type(t)
                , ShortName(shortName)
                , LongName(longName)
                , BoolValue(0)
            { }
            
            // data members
            OptionType Type;
            char ShortName;
            std::string LongName;
            union { 
                bool* BoolValue; 
                std::string* StringValue; 
                std::vector<std::string>* ListValue;
            };
            std::string Default;
        };
    
    // internal methods
    private:
        void init(int argc, char* argv[], int offset);
        void saveOption(const Option& opt); // const & ?? he doesnt use it - why?
        void setSwitch(const Option& opt);
        
    // data members
    private:
        std::vector<Option>        m_options;
        std::map<std::string, int> m_setOptions;
        std::vector<std::string>   m_args;
        std::string                m_appname;
        
        int m_numberRequiredArguments;
        int m_numberOptionalArguments;
        Option m_requiredArgument;
        Option m_optionalArgument;
        
        int m_currentArgument;
};

inline
GetOpt::GetOpt(int argc, char* argv[], int offset) 
{
    init(argc, argv, offset);
}

inline
GetOpt::~GetOpt(void) { }      

// add an optional 'bare word' argument (eg 'help')
// 'name' is not used on the command line, but for reporting
inline
void GetOpt::addOptionalArgument(const std::string& name, std::string* value) {
  
    Option opt( OptUnknown, 0, name );
    opt.StringValue = value;
    m_optionalArgument = opt;
    ++m_numberOptionalArguments;
    *value = std::string();
}

// add a required 'bare word' argument (eg input data file)
// 'name' is not used on the command line, but for reporting
inline
void GetOpt::addRequiredArgument(const std::string& name, std::string* value) {
    
    Option opt( OptUnknown, 0, name );
    opt.StringValue = value;
    m_requiredArgument = opt;
    ++m_numberRequiredArguments;
    *value = std::string();
}

// add standard option with arguments ( -Wall, -O2, --type=foo )
inline
void GetOpt::addOption(const char shortName, const std::string& longName, std::string* value) {
    
    Option opt( OptArg1, shortName, longName );
    opt.StringValue = value;
    saveOption(opt);
    *value = std::string();
}

// add an option whose argument is optional (eg --log may default to dumping to stderr, unless a file is specified )
// must provide a default string 
// short & long name version
inline
void GetOpt::addOptionalOption(const char shortName, const std::string& longName, std::string* value, const std::string& defaultValue) {
  
    Option opt( OptOptional, shortName, longName );
    opt.StringValue = value;
    opt.Default = defaultValue;
    saveOption(opt);
    *value = std::string();
}

// long name only version
inline
void GetOpt::addOptionalOption(const std::string& longName, std::string* value, const std::string& defaultValue) {
    addOptionalOption(0, longName, value, defaultValue);
}

// add a repeatable option (like compiler includes -I/path/ -I/path2/ etc)
// only supporting one type of name (short/long) for this option for now
// short name only version
inline
void GetOpt::addRepeatableOption(const char shortName, std::vector<std::string>* values) {
  
    Option opt( OptRepeat, shortName, std::string() );
    opt.ListValue = values;
    saveOption(opt);
    *values = std::vector<std::string>();
}

// long name only version
inline
void GetOpt::addRepeatableOption(const std::string& longName, std::vector<std::string>* values) {
  
    Option opt( OptRepeat, 0, longName );
    opt.ListValue = values;
    saveOption(opt);
    *values = std::vector<std::string>();
}

// add an option that takes a variable number of arguments ( --files f1 f2 f3 f4... )
inline
void GetOpt::addVariableLengthOption(const std::string& longName, std::vector<std::string>* values) {
  
    Option opt( OptVariable, 0, longName );
    opt.ListValue = values;
    saveOption(opt);
    *values = std::vector<std::string>();
}

// on/off switch ( --verbose --searchOnly ) only long names supported for now
inline
void GetOpt::addSwitch(const std::string& longName, bool* ok) {

    Option opt( OptSwitch, 0, longName );
    opt.BoolValue = ok;
    saveOption(opt);
    *ok = false;
}

inline
const std::string& GetOpt::applicationName(void) const {
    return m_appname;
}

inline
void GetOpt::init(int argc, char* argv[], int offset) {
  
    m_numberRequiredArguments = 0;
    m_numberOptionalArguments = 0;
    m_currentArgument         = 1;
    
    if ( argc > 0 ) {
      
        // store app name
        std::string fullPath = argv[0];
        size_t lastSlash = fullPath.find_last_of("/\\"); // should work on Unix- and Windows-style paths
        m_appname = fullPath.substr(lastSlash + 1);
        
        // store remaining arguments from offset to end
        for (int i = offset + 1; i < argc; ++i) {
            m_args.push_back( argv[i] );
        }
        
    } else {
        std::cerr << "GetOpt ERROR: No arguments given." << std::endl;
        exit(1);
    }
}

// query if particular 'bare-word' argument is set
inline
bool GetOpt::isSet(const std::string& name) const {
    return ( m_setOptions.find(name) != m_setOptions.end() );
}

// runs parser (does validation and assign values to arguments) 
// returns success/fail
inline
bool GetOpt::parse(void) {
  
    // initialize argument stack (reversed input args)
    std::vector<std::string> argStack( m_args.rbegin(), m_args.rend() );
    
    // initialize state
    enum State { StartingState, ExpectingState, OptionalState };
    State state = StartingState;
    
    // initialize token types
    enum TokenType { LongOpt, ShortOpt, Arg, End };
    TokenType token = End;
    TokenType currentType = End;
    
    // store option list bounds
    std::vector<Option>::const_iterator optBegin = m_options.begin();
    std::vector<Option>::const_iterator optEnd   = m_options.end();
    
    // declare currentOption
    Option currentOption;
  
    // we're going to fake an 'End' argument
    bool isExtraLoopNeeded = true;
    
    // iterate through stack contents & do one extra loop for the fake 'End'
    while ( !argStack.empty() || isExtraLoopNeeded ) {
      
        std::string arg;
        std::string originalArg;  // store the original arg because we're going to mangle 'arg'
        
        // if contents on the arg stack
        if ( !argStack.empty() ) {
          
            arg = argStack.back();
            argStack.pop_back();
            ++m_currentArgument;
            originalArg = arg;
          
            // long option version
            if ( arg.substr(0,2) == "--" ) {
              
                // set token type
                token = LongOpt;
              
                // strip the '--'
                arg = arg.substr(2);
                
                // make sure there's still somthing there
                if ( arg.empty() ) {
                    std::cerr << "'--' feature is not supported, yet." << std::endl;
                    exit(1);
                }
                
                // split any key=value style args
                size_t foundEqual = arg.find('=');
                if ( foundEqual != std::string::npos ) {
                  
                    // push value back onto stack
                    argStack.push_back( arg.substr(foundEqual+1) );
                    --m_currentArgument;
                    
                    // save key as current arg
                    arg = arg.substr(0, foundEqual);
                }
              
            }
            
            // short option version
            else if ( arg.at(0) == '-' ) {
              
                // set token type
                token = ShortOpt;
                
                // if option is directly followed by argument (eg -Wall), push that arg back onto stack
                if ( arg.length() > 2 ) {
                    argStack.push_back( arg.substr(2) );
                    --m_currentArgument;
                }
                
                // strip the '-'
                arg = arg[1];
            } 
            
            // bare-word argument
            else { token = Arg; }
        } 
        
        // in fake End iteration
        else { token = End;  }
      
        // look up arg in list of known options, modify token type if necessary
        Option opt;
        if ( token != End ) {
          
            // look up arg in option list
            std::vector<Option>::const_iterator optIter = optBegin;
            for ( ; optIter != optEnd; ++optIter ) {
                const Option& o = (*optIter);
                if ( (token == LongOpt && arg == o.LongName) ||
                    (token == ShortOpt && arg.at(0) == o.ShortName) ) {
                    opt = o;
                    break; 
                } 
            }
            
            // modify token type if needed
            if ( token == LongOpt && opt.Type == OptUnknown ) {
                if ( currentOption.Type != OptVariable ) {
                    std::cerr << "GetOpt ERROR: Unknown option --" << arg << std::endl;
                    return false;
                } else { 
                  token = Arg; 
                }
            } else if ( token == ShortOpt && opt.Type == OptUnknown ) {
                if ( currentOption.Type != OptVariable ) {
                    std::cerr << "GetOpt ERROR: Unknown option -" << arg.at(0) << std::endl;
                    return false;
                } else { 
                  token = Arg; 
                } 
            } 
        } else { opt = Option(OptEnd); }
        
        
         // interpret result
         switch ( state ) {
            
            case ( StartingState ) :
              
                if ( opt.Type == OptSwitch ) {
                    setSwitch(opt);
                    m_setOptions.insert( std::pair<std::string, int>(opt.LongName, 1) );
                    m_setOptions.insert( std::pair<std::string, int>((const char*)&opt.ShortName, 1) );
                } else if ( opt.Type == OptArg1 || opt.Type == OptRepeat ) {
                    state = ExpectingState;
                    currentOption = opt;
                    currentType = token;
                    m_setOptions.insert( std::pair<std::string, int>(opt.LongName, 1) );
                    m_setOptions.insert( std::pair<std::string, int>((const char*)&opt.ShortName, 1) );
                } else if ( opt.Type == OptOptional || opt.Type == OptVariable ) {
                    state = OptionalState;
                    currentOption = opt;
                    currentType = token;
                    m_setOptions.insert( std::pair<std::string, int>(opt.LongName, 1) );
                    m_setOptions.insert( std::pair<std::string, int>((const char*)&opt.ShortName, 1) );
                } else if ( opt.Type == OptEnd ) {
                    // do nothing (we're almost done here)
                } else if ( opt.Type == OptUnknown && token == Arg ) {
                    if ( m_numberRequiredArguments > 0 ) {
                        if ( (*m_requiredArgument.StringValue).empty() ) {
                            *m_requiredArgument.StringValue = arg;
                        } else {
                            std::cerr << "Too many bare arguments" << std::endl;
                            return false;
                        }
                    } 
                    
                    else if ( m_numberOptionalArguments > 0 ) {
                        if ( (*m_optionalArgument.StringValue).empty() ) {
                            *m_optionalArgument.StringValue = arg;
                        } else {
                            std::cerr << "Too many bare arguments" << std::endl;
                            return false;
                        }
                    }
                } else {
                    std::cerr << "GetOpt ERROR: Unhandled StartingState case: " << opt.Type << std::endl;
                    exit(1);
                }
                
                break;
              
            case ( ExpectingState ) :
             
                if ( token == Arg ) {    
                    if ( currentOption.Type == OptArg1 ) {
                        *currentOption.StringValue = arg;
                        state = StartingState;
                    }  else if ( currentOption.Type == OptRepeat ) {
                        currentOption.ListValue->push_back(arg);
                        state = StartingState;
                    } else {
                        std::cerr << "GetOpt ERROR: Unhandled ExpectingState case: " << currentOption.Type << std::endl;
                        exit(1);
                    }
                } else {
                    std::string name = (currentType == LongOpt) ? currentOption.LongName : (const char*)&currentOption.ShortName;
                    std::cerr << "GetOpt ERROR: Expected an argument after option: " << name << std::endl;
                    exit(1);
                }
        
                break;
                
            case ( OptionalState ) : 
              
                if ( token == Arg ) {
                    if ( currentOption.Type == OptOptional ) {
                        *currentOption.StringValue = arg;
                        state = StartingState;
                    } else if ( currentOption.Type == OptVariable ) {
                        currentOption.ListValue->push_back(originalArg);
                        // stay in this state
                    } else {
                        std::cerr << "GetOpt ERROR: Unhandled OptionalState case: " << currentOption.Type << std::endl;
                        exit(1);
                    }
                } else {
                  
                    // optional argument not specified
                    if ( currentOption.Type == OptOptional ) {
                        *currentOption.StringValue = currentOption.Default;
                    }
                    
                    if ( token != End ) { 
                        // re-evaluate current argument
                        argStack.push_back( originalArg );
                        --m_currentArgument;
                    }
                    
                    state = StartingState;
                }
        
                break;
        }
        
        if ( token == End ) {
            isExtraLoopNeeded = false;
        }
    }
    
    // check that required argument has been satisfied
    if ( m_numberRequiredArguments > 0 && (*m_requiredArgument.StringValue).empty() ) {
        std::cerr << "Lacking required argument" << std::endl;
        return false;
    }
  
    return true;
}

inline
void GetOpt::print(void) {
 
    std::cout << "---------------------------------" << std::endl;
    std::cout << "Options for app: " << m_appname << std::endl;
    std::cout << std::endl;
    std::cout << "Args: ";
    std::vector<std::string>::const_iterator argIter = m_args.begin();
    std::vector<std::string>::const_iterator argEnd  = m_args.end();
    for ( ; argIter != argEnd; ++argIter ) {
        std::cout << (*argIter) << " ";
    }
    std::cout << std::endl;
}    
    
inline
void GetOpt::saveOption(const Option& opt) {
    // check for conflicts (duplicating options) ??
    m_options.push_back(opt);
} 

inline
void GetOpt::setSwitch(const Option& opt) {
    assert( opt.Type == OptSwitch );
    *opt.BoolValue =  true;
}

} // namespace BamTools 

#endif // BAMTOOLS_GETOPT_H