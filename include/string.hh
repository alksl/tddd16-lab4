#ifndef KOMP_STRING_H
#define KOMP_STRING_H

#include <iostream>


//
// Note that the implementation of this class is rather inefficient.
// When operating on strings and character pointers, this
// implementation will create a lot of temporary objects. By
// overloading all the operators on combinations of strings and
// character pointers this could be avoided.
//
// The append and assignment operators (+=) are defined this way
// because it is used a lot. Something similar should be done for all
// other operators as well.
//
// Finally, the class should avoid actually copying data unless it is
// really necessary. If strings were considered read only (no
// destructive update), then it would be possible to keep a pool of
// strings with reference counts and just pass along references to
// that pool.
//

class string
{
private:
    char        *text;
    int          size;
    int          position;
    int          chunk_size;

    void ensure_size(int);

public:
    class error {};             // Exception thrown when out of memory

    string();                   // Default constructor creates empty string
    string(char *);             // Create string from character pointer
    string(char, int);          // Create string filles with characters
    string(const string &);     // Copy constructor
    string(int);                // Convert an integer

    ~string();                  // Destructor

    void upcase(void);
    void downcase(void);

    string& operator=(const string&); // Assignment operator
    string& operator=(const char *);  // Assignment operator
    string& operator=(const char);    // Assignment operator

    string& operator+=(const string&); // Append operator
    string& operator+=(const char);    // Append operator
    string& operator+=(const char *);  // Append operator

    friend string& operator+(const string&, const string&); // Concatenate
    friend string& operator+(const string&, const char);    // Concatenate
    friend string& operator+(const string&, const int);     // Concatenate

    //
    // Comparison operators
    //

    friend int operator==(const string&, const string&);
    friend int operator!=(const string&, const string&);
    friend int operator< (const string&, const string&);
    friend int operator> (const string&, const string&);
    friend int operator>=(const string&, const string&);
    friend int operator<=(const string&, const string&);

    //
    // I/O
    //

    friend std::ostream& operator<<(std::ostream&, const string&);
    friend std::ostream& operator<<(std::ostream&, const string*);

    //
    // Miscellaneous operators and methods
    //

    unsigned long hash(void) const;   // Compute hash value
    int length(void) const;           // Length of the string
    char& operator[](int);       // Extract a character
    const char operator[](const int) const;
};

#endif
