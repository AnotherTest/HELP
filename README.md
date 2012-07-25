···············································
: HELP - Handy Esoteric Language Preprocessor :
···············································

· For developers:
    - Compile with -std=c++11 and link to libboost_regex (unforutnately stdlib's regex isn't working properly yet).
    - TODO:
        Make the initial character that indicates a HELP macro user-specified. ('#' can't always be used)
        Documentation!
        Better code! (more C++11)

· What is HELP?
    HELP is a preprocessor intended to allow faster writing of programs in esoteric programming languages.
    Its macro's are actually regular expressions, allowing the esolang's syntax to be extended.
· HELP example:
    # something (\d+) := $1 something
    something 10
    Will be substituted to: 10 something
