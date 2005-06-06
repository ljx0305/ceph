#ifndef __FILEPATH_H
#define __FILEPATH_H


/*
 * BUG:  /a/b/c is equivalent to a/b/c in dentry-breakdown, but not string.
 *   -> should it be different?  how?  should this[0] be "", with depth 4?
 *
 */


#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include <ext/rope>
using namespace __gnu_cxx;

class filepath {
  string path;
  vector<string> bits;

  void parse() {
	bits.clear();
	int off = 0;
	while (off < path.length()) {
	  // skip trailing/duplicate slash(es)
	  int nextslash = path.find('/', off);
	  if (nextslash == off) {
		off++;
		continue;
	  }
	  if (nextslash < 0) 
		nextslash = path.length();  // no more slashes
	  
	  bits.push_back( path.substr(off,nextslash-off) );
	  off = nextslash+1;
	}
  }

 public:
  filepath() {}
  filepath(const string& s) {
	set_path(s);
  }
  filepath(const char* s) {
	set_path(s);
  }

  void set_path(const string& s) {
	path = s;
	parse();
  }
  void set_path(const char *s) {
	path = s;
	parse();
  }

  string& get_path() {
	return path;
  }
  int length() const {
	return path.length();
  }

  const char *c_str() const {
	return path.c_str();
  }


  filepath prefixpath(int s) const {
	filepath t;
	for (int i=0; i<s; i++)
	  t.add_dentry(bits[i]);
	return t;
  }
  filepath postfixpath(int s) const {
	filepath t;
	for (int i=s; i<bits.size(); i++)
	  t.add_dentry(bits[i]);
	return t;
  }
  void add_dentry(const string& s) {
	bits.push_back(s);
	if (path.length())
	  path += "/";
	path += s;
  }
  void append(const filepath& a) {
	for (int i=0; i<a.depth(); i++) 
	  add_dentry(a[i]);
  }


  void clear() {
    path = "";
    bits.clear();
  }

  const string& operator[](int i) const {
	return bits[i];
  }

  const string& last_bit() const {
	return bits[ bits.size()-1 ];
  }

  int depth() const {
	return bits.size();
  }
  bool empty() {
    return bits.size() == 0;
  }

  
  void _rope(crope& r) {
    char n = bits.size();
    r.append((char*)&n, sizeof(char));
    for (vector<string>::iterator it = bits.begin();
         it != bits.end();
         it++) { 
      r.append((*it).c_str(), (*it).length()+1);
    }
  }

  void _unrope(crope& r, int& off) {
    clear();

    char n;
    r.copy(off, sizeof(char), (char*)&n);
    off += sizeof(char);
    for (int i=0; i<n; i++) {
      string s = r.c_str() + off;
      off += s.length() + 1;
	  add_dentry(s);
    }
  }

};

inline ostream& operator<<(ostream& out, filepath& path)
{
  return out << path.get_path();
}

#endif
