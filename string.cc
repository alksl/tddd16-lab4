#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <strstream>
#include "string.hh"

using std::ostrstream;

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif


void string::ensure_size(int request)
{
    if (request > size)
    {
        if (request - size < chunk_size)
            request = size + chunk_size;
        if (text == NULL)
        {
            text = (char *)malloc(request);
            if (text == NULL)
                abort();
        }
        else
        {
            text = (char *)realloc(text, request);
            if (text == NULL)
                abort();
        }
        size = request;
    }
}

void string::upcase(void)
{
    int i;

    for (i = 0; i < position; i++)
    {
        text[i] = toupper(text[i]);
    }
}

void string::downcase(void)
{
    int i;

    for (i = 0; i < position; i++)
    {
        text[i] = tolower(text[i]);
    }
}


string::string()
{
    text = NULL;
    size = 0;
    position = 0;
    chunk_size = 10;
}

string::string(char *s)
{
    text = strdup(s);
    if (text == NULL)
        abort();
    size = strlen(s) + 1;
    position = strlen(s);
    chunk_size = 10;
}

string::string(char c, int sz)
{
    text = (char *)malloc(sz);
    if (text == NULL)
        abort();
    memset(text, c, sz);
    size = sz;
    position = 0;
    chunk_size = 10;
}

string::string(int s)
{
    char        buf[1024];
    ostrstream  str(buf, 1024);

    str << s;
    text = (char *)malloc(strlen(buf) + 1);
    if (text == NULL)
        abort();
    memcpy(text, buf, strlen(buf) + 1);
    size = strlen(buf) + 1;
    position = strlen(buf);
    chunk_size = 10;
}

string::string(const string& s)
{
    if (s.text)
    {
        text = (char *)malloc(s.size);
        memcpy(text, s.text, s.size);
        position = s.position;
        size = s.size;
    }
    else
    {
        text = NULL;
        size = 0;
        position = 0;
    }
}

string::~string(void)
{
    if (text)
        free(text);
}

string& string::operator=(const string& s)
{
    char *tmp;
    int   sz, pos;

    if (s.size > 0)
    {
        tmp = (char *)malloc(s.size);
        memcpy(tmp, s.text, s.size);
    }
    else
    {
        tmp = NULL;
    }
    sz = s.size;
    pos = s.position;

    if (text != NULL)
        free(text);
    text = tmp;
    position = pos;
    size = sz;

    return *this;
}


string& string::operator=(const char *s)
{
    ensure_size(strlen(s) + 1);
    strcpy(text, s);
    position = strlen(s);

    return *this;
}

string& string::operator=(const char c)
{
    ensure_size(1);
    text[0] = c;
    position = 1;

    return *this;
}

string& string::operator+=(const string& s)
{
    ensure_size(position + s.position);
    memcpy(&text[position], s.text, s.position);
    position += s.position;

    return *this;
}

string& string::operator+=(const char *s)
{
    ensure_size(position + strlen(s));
    memcpy(&text[position], s, strlen(s));
    position += strlen(s);

    return *this;
}

string& string::operator+=(const char c)
{
    ensure_size(position + 1);
    text[position] = c;
    position += 1;

    return *this;
}

string& operator+(const string& s1, const string& s2)
{
    string *res;

    res = new string;
    res->ensure_size(s1.position + s2.position);
    memcpy(&res->text[0], s1.text, s1.position);
    memcpy(&res->text[s1.position], s2.text, s2.position);
    res->position = s1.position + s2.position;

    return *res;
}

string& operator+(const string& s1, const char c)
{
    string *res;

    res = new string;
    res->ensure_size(s1.position + 1);
    memcpy(&res->text[0], s1.text, s1.position);
    res->text[s1.position] = c;
    res->position = s1.position + 1;

    return *res;
}


string& operator+(const string& s1, const int i )
{
    string      *res;
    char         buf[1024];
    ostrstream   str(buf, 1024);
    
    str << i << '\0';
    res = new string;
    res->ensure_size(s1.position + strlen(buf) + 1);
    memcpy(&res->text[0], s1.text, s1.position);
    memcpy(&res->text[s1.position], buf, strlen(buf) + 1);
    res->position = s1.position + strlen(buf);

    return *res;
}

int operator==(const string& s1, const string& s2)
{
    return !strncasecmp(s1.text, s2.text, MAX(s1.position, s2.position));
}

int operator!=(const string& s1, const string& s2)
{
    return !!strncasecmp(s1.text, s2.text, MAX(s1.position, s2.position));
}

int operator>=(const string& s1, const string& s2)
{
    return strncasecmp(s1.text, s2.text, MAX(s1.position, s2.position)) >= 0;
}

int operator<=(const string& s1, const string& s2)
{
    return strncasecmp(s1.text, s2.text, MAX(s1.position, s2.position)) <= 0;
}

int operator>(const string& s1, const string& s2)
{
    return strncasecmp(s1.text, s2.text, MAX(s1.position, s2.position)) > 0;
}

int operator<(const string& s1, const string& s2)
{
    return strncasecmp(s1.text, s2.text, MAX(s1.position, s2.position)) < 0;
}

unsigned long string::hash(void) const
{
    unsigned long res;
    int i;

    res = 0;

    for (i = 0; i < position; i++)
    {
        res = res * 65599 + text[i];
    }

    return res;
}

int string::length(void) const
{
    return position;
}

char& string::operator[](int ix)
{
    if (ix == position)
    {
        ensure_size(position + 1);
        text[position] = '\0';
        return text[position];
    }
    if (ix > position)
        abort();
    else
        return text[ix];
}

const char string::operator[](const int ix) const
{
    if (ix == position)
    {
        return '\0';
    }
    if (ix > position)
        abort();
    else
        return text[ix];
}

ostream& operator<<(ostream& o, const string& s)
{
    char *tmp;

    tmp = (char *)malloc(s.position + 1);
    memcpy(tmp, s.text, s.position);
    tmp[s.position] = '\0';

    return o << tmp;
}

ostream& operator<<(ostream& o, const string* s)
{
    char *tmp;

    tmp = (char *)malloc(s->position + 1);
    memcpy(tmp, s->text, s->position);
    tmp[s->position] = '\0';

    return o << tmp;
}
