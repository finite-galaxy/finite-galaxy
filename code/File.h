// File.h

#ifndef FILE_H_
#define FILE_H_

#include <cstdio>
#include <string>



// RAII wrapper for FILE, to make sure it gets closed if an error occurs.
class File {
public:
  explicit File(const std::string &path, bool write = false);
  File(const File &) = delete;
  File(File &&other);
  ~File();

  File &operator=(const File &) = delete;

  operator bool() const;
  operator FILE*() const;

private:
  FILE *file = nullptr;
};



#endif
