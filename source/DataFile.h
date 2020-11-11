// DataFile.h

#ifndef DATA_FILE_H_
#define DATA_FILE_H_

#include "DataNode.h"

#include <istream>
#include <list>
#include <string>



// A class which represents a hierarchical data file. Each line of the file that
// is not empty or a comment is a "node," and the relationship between the nodes
// is determined by indentation: if a node is more indented than the node before
// it, it is a "child" of that node. Otherwise, it is a "sibling." Each node is
// just a collection of one or more tokens that can be interpreted either as
// strings or as floating point values; see DataNode for more information.
class DataFile {
public:
  // A DataFile can be loaded either from a file path or an istream.
  DataFile() = default;
  explicit DataFile(const std::string &path);
  explicit DataFile(std::istream &in);

  void Load(const std::string &path);
  void Load(std::istream &in);

  // Functions for iterating through all DataNodes in this file.
  std::list<DataNode>::const_iterator begin() const;
  std::list<DataNode>::const_iterator end() const;


private:
  void LoadData(const std::string &data);


private:
  // This is the container for all DataNodes in this file.
  DataNode root;
};



#endif
