// DataFile.cpp

#include "DataFile.h"

#include "Files.h"
#include "Utf8.h"

using namespace std;



// Constructor, taking a file path (in UTF-8).
DataFile::DataFile(const string &path)
{
  Load(path);
}



// Constructor, taking an istream. This can be cin or a file.
DataFile::DataFile(istream &in)
{
  Load(in);
}



// Load from a file path (in UTF-8).
void DataFile::Load(const string &path)
{
  string data = Files::Read(path);
  if(data.empty())
    return;
  
  // As a sentinel, make sure the file always ends in a newline.
  if(data.empty() || data.back() != '\n')
    data.push_back('\n');
  
  // Note what file this node is in, so it will show up in error traces.
  root.tokens.push_back("file");
  root.tokens.push_back(path);

  LoadData(data);
}



// Constructor, taking an istream. This can be cin or a file.
void DataFile::Load(istream &in)
{
  string data;
  
  static const size_t BLOCK = 4096;
  while(in)
  {
    size_t currentSize = data.size();
    data.resize(currentSize + BLOCK);
    in.read(&*data.begin() + currentSize, BLOCK);
    data.resize(currentSize + in.gcount());
  }
  // As a sentinel, make sure the file always ends in a newline.
  if(data.empty() || data.back() != '\n')
    data.push_back('\n');
  
  LoadData(data);
}



// Get an iterator to the start of the list of nodes in this file.
list<DataNode>::const_iterator DataFile::begin() const
{
  return root.begin();
}



// Get an iterator to the end of the list of nodes in this file.
list<DataNode>::const_iterator DataFile::end() const
{
  return root.end();
}



// Parse the given text.
void DataFile::LoadData(const string &data)
{
  // Keep track of the current stack of indentation levels and the most recent
  // node at each level - that is, the node that will be the "parent" of any
  // new node added at the next deeper indentation level.
  vector<DataNode *> stack(1, &root);
  vector<int> whiteStack(1, -1);
  bool fileIsSpaces = false;
  bool warned = false;
  
  size_t end = data.length();
  for(size_t pos = 0; pos < end; pos = Utf8::NextCodePoint(data, pos))
  {
    char32_t c = Utf8::DecodeCodePoint(data, pos);

    // Find the first non-white character in this line.
     bool isSpaces = false;
    int white = 0;
    while(c <= ' ' && c != '\n')
    {
      // Warn about mixed indentations when parsing files.
      if(!isSpaces && c == ' ')
      {
        // If we've parsed whitespace that wasn't a space, issue a warning.
        if(white)
          stack.back()->PrintTrace("Mixed whitespace usage in line");
        else
          fileIsSpaces = true;
        
        isSpaces = true;
      }
      else if(fileIsSpaces && !warned && c != ' ')
      {
        warned = true;
        stack.back()->PrintTrace("Mixed whitespace usage in file");
      }

      ++white;
      pos = Utf8::NextCodePoint(data, pos);
      c = Utf8::DecodeCodePoint(data, pos);
    }
    
    // If the line is a comment, skip to the end of the line.
    if(c == '#')
    {
      while(c != '\n')
      {
        pos = Utf8::NextCodePoint(data, pos);
        c = Utf8::DecodeCodePoint(data, pos);
      }
    }
    // Skip empty lines (including comment lines).
    if(c == '\n')
      continue;
    
    // Determine where in the node tree we are inserting this node, based on
    // whether it has more indentation that the previous node, less, or the same.
    while(whiteStack.back() >= white)
    {
      whiteStack.pop_back();
      stack.pop_back();
    }
    
    // Add this node as a child of the proper node.
    list<DataNode> &children = stack.back()->children;
    children.emplace_back(stack.back());
    DataNode &node = children.back();
    
    // Remember where in the tree we are.
    stack.push_back(&node);
    whiteStack.push_back(white);
    
    // Tokenize the line. Skip comments and empty lines.
    while(c != '\n')
    {
      // Check if this token begins with a quotation mark. If so, it will
      // include everything up to the next instance of that mark.
      char32_t endQuote = c;
      bool isQuoted = (endQuote == '"' || endQuote == '`');
      if(isQuoted)
      {
        pos = Utf8::NextCodePoint(data, pos);
        c = Utf8::DecodeCodePoint(data, pos);
      }
      
      const size_t start = pos;
      
      // Find the end of this token.
      while(c != '\n' && (isQuoted ? (c != endQuote) : (c > ' ')))
      {
        pos = Utf8::NextCodePoint(data, pos);
        c = Utf8::DecodeCodePoint(data, pos);
      }
      
      // It ought to be legal to construct a string from an empty iterator
      // range, but it appears that some libraries do not handle that case
      // correctly. So:
      if(start == pos)
        node.tokens.emplace_back();
      else
        node.tokens.emplace_back(data, start, pos - start);
      // This is not a fatal error, but it may indicate a format mistake:
      if(isQuoted && c == '\n')
        node.PrintTrace("Closing quotation mark is missing:");
      
      if(c != '\n')
      {
        // If we've not yet reached the end of the line of text, search
        // forward for the next non-whitespace character.
        if(isQuoted)
        {
          pos = Utf8::NextCodePoint(data, pos);
          c = Utf8::DecodeCodePoint(data, pos);
        }
        while(c != '\n' && c <= ' ' && c != '#')
        {
          pos = Utf8::NextCodePoint(data, pos);
          c = Utf8::DecodeCodePoint(data, pos);
        }
        
        // If a comment is encountered outside of a token, skip the rest
        // of this line of the file.
        if(c == '#')
        {
          while(c != '\n')
          {
            pos = Utf8::NextCodePoint(data, pos);
            c = Utf8::DecodeCodePoint(data, pos);
          }
        }
      }
    }
  }
}
