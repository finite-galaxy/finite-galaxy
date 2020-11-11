// File.h

#ifndef FILE_H_
#define FILE_H_

#include <cstdio>
#include <string>



// RAII wrapper for FILE, to make sure it gets closed if an error occurs.
class File {
public:
  File() = default;
  explicit File(const std::string &path, bool write = false);
  File(const File &) = delete;
  File(File &&other);
  ~File();

  // Do not allow copying the FILE pointer.
  File &operator=(const File &) = delete;
  // Move assignment is OK though.
  File &operator=(File &&other);

  operator bool() const;
  operator FILE*() const;

private:
  FILE *file = nullptr;
};



#endif
