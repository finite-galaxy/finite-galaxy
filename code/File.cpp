// File.cpp

#include "File.h"

#include "Files.h"

using namespace std;



File::File(const string &path, bool write)
{
  file = Files::Open(path, write);
}



File::File(File &&other)
  : file(other.file)
{
  other.file = nullptr;
}



File::~File()
{
  if(file)
    fclose(file);
}



File &File::operator=(File &&other)
{
  this->file = std::move(other.file);
  other.file = nullptr;
  return *this;
}



File::operator bool() const
{
  return file;
}



File::operator FILE*() const
{
  return file;
}
