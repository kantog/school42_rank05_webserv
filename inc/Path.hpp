


#pragma once

#include <string>
#include <vector>

class Path
{
private:
    std::string _path;
    
    void normalize();
    
public:
    Path();
    Path(const std::string& path);         
    Path(const char* path);                 
    Path(const Path& other);                
    
    Path& operator=(const std::string& path);
    Path& operator=(const Path& other);
    
    Path operator+(const Path& other) const;
    Path operator+(const std::string& other) const;
    Path& operator+=(const Path& other);
    Path& operator+=(const std::string& other);
    
    bool operator==(const Path& other) const;
    bool operator!=(const Path& other) const;
    
    Path join(const Path& other) const;
    Path join(const std::string& other) const;
    Path removePrefix(const Path& prefix) const;
    Path removePrefix(const std::string& prefix) const;
    bool startsWith(const Path& prefix) const;
    bool startsWith(const std::string& prefix) const;
    bool isEmpty() const;
    
    std::string getFilename() const;
    Path getDirectory() const;
    
    const std::string& toString() const;
    const char* c_str() const;
    
    bool isAbsolute() const;
    bool isRelative() const;
    Path makeRelative() const;
    Path makeAbsolute() const;
};