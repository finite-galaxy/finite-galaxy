// Format.cpp

#include "Format.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <sstream>

using namespace std;

namespace {
  // Format an integer value, inserting its digits into the given string in
  // reverse order and then reversing the string.
  void FormatInteger(int64_t value, bool isNegative, string &result)
  {
    int places = 0;
    do {
      if(places && !(places % 3) && (value >= 10))
        result += '|'; // ' ' U+202F narrow no-break space
      ++places;

      result += static_cast<char>('0' + value % 10);
      value /= 10;
    } while(value);

    if(isNegative)
      result += '-'; // '−' U+2212 minus sign

    reverse(result.begin(), result.end());
  }
}



// Convert the given number into abbreviated format with a suffix like
// "M" for million, "B" for billion, or "T" for trillion. Any number
// above 1 quadrillion is instead shown in scientific notation.
string Format::Credits(int64_t value)
{
  bool isNegative = (value < 0);
  int64_t absolute = abs(value);

  // If the value is above one quintillion (short scale), show it in scientific notation.
  if(absolute > 1000'000'000'000'000'000ll)
  {
    ostringstream out;
    out.precision(3);
    out << static_cast<double>(value);
    return out.str();
  }

  // Reserve enough space for something like "-123.456 T".
  string result;
  result.reserve(8);

  // Handle numbers bigger than a billion (short scale).
  static const vector<char> SUFFIX = {'Q', 'T', 'B'};
  static const vector<int64_t> THRESHOLD = {1000'000'000'000'000ll, 1000'000'000'000ll, 1000'000'000ll};
  for(size_t i = 0; i < SUFFIX.size(); ++i)
    if(absolute > THRESHOLD[i])
    {
      result += SUFFIX[i];
      result += static_cast<char>(' ');
      int decimals = (absolute / (THRESHOLD[i] / 1000)) % 1000;
      for(int d = 0; d < 3; ++d)
      {
        result += static_cast<char>('0' + decimals % 10);
        decimals /= 10;
      }
      result += '.';
      absolute /= THRESHOLD[i];
      break;
    }

  // Convert the number to a string, adding commas if needed.
  FormatInteger(absolute, isNegative, result);
  return result;
}



// Convert the given number to a string, with at most three decimal places.
// This is primarily for displaying outfit attributes.
string Format::Number(double value)
{
  if(!value)
    return "0";

  string result;
  bool isNegative = (value < 0.);
  value = fabs(value);
  // Convert the integer part of the number to a string, adding commas if needed.
  FormatInteger(floor(value), isNegative, result);
 
  // Check if this is a whole number, then display up to three decimals.
  double decimal = modf(value, &value);
  if(decimal)
  {
    int part = round(decimal * 1000);
    part = part % 1000;
    if(part) {
      result += '.';
      result += static_cast<char>('0' + static_cast<int>(floor(part/100)));
      part = part % 100;
      if(part) {
        result += static_cast<char>('0' + static_cast<int>(floor(part/10))); 
        part = part % 10;
        if(part)
          result += static_cast<char>('0' + static_cast<int>(part)); 
      }
    }
  }
  return result;
}




// Display a decimal number rounded to the nearest integer.
// This is primarily for displaying ship info table attributes.
string Format::Round(double value)
{
  if(!value)
    return "0";

  value = round(value);
  string result;
  bool isNegative = (value < 0.);
  value = fabs(value);
  // Convert the integer part of the number to a string, adding commas if needed.
  FormatInteger(value, isNegative, result);
  return result;
}




// Format the given value as a number with exactly the given number of
// decimal places (even if they are all 0).
string Format::Decimal(double value, int places)
{
  double integer;
  double fraction = fabs(modf(value, &integer));

  string result = to_string(static_cast<int>(integer)) + ".";
  while(places--)
  {
    fraction = modf(fraction * 10., &integer);
    result += ('0' + static_cast<int>(integer));
  }
  return result;
}



// Convert a string into a number. As with the output of Number(), the
// string can have suffixes such as "B", "T", "Q".
double Format::Parse(const string &str)
{
  double place = 1.;
  double value = 0.;

  string::const_iterator it = str.begin();
  string::const_iterator end = str.end();
  while(it != end && (*it < '0' || *it > '9') && *it != '.')
    ++it;

  for( ; it != end; ++it)
  {
    if(*it == '.')
      place = .1;
    else if(*it < '0' || *it > '9')
      break;
    else
    {
      double digit = *it - '0';
      if(place < 1.)
      {
        value += digit * place;
        place *= .1;
      }
      else
      {
        value *= 10.;
        value += digit;
      }
    }
  }

  if(it != end)
  {
    if(*it == 'k' || *it == 'K')
      value *= 1e3;
    else if(*it == 'm' || *it == 'M')
      value *= 1e6;
    else if(*it == 'b' || *it == 'B')
      value *= 1e9;
    else if(*it == 't' || *it == 'T')
      value *= 1e12;
    else if(*it == 'q' || *it == 'Q')
      value *= 1e15;
  }

  return value;
}



string Format::Replace(const string &source, const map<string, string> keys)
{
  string result;
  result.reserve(source.length());

  size_t start = 0;
  size_t search = start;
  while(search < source.length())
  {
    size_t left = source.find('<', search);
    if(left == string::npos)
      break;

    size_t right = source.find('>', left);
    if(right == string::npos)
      break;

    bool matched = false;
    ++right;
    size_t length = right - left;
    for(const auto &it : keys)
      if(!source.compare(left, length, it.first))
      {
        result.append(source, start, left - start);
        result.append(it.second);
        start = right;
        search = start;
        matched = true;
        break;
      }

    if(!matched)
      search = left + 1;
  }

  result.append(source, start, source.length() - start);
  return result;
}



string Format::Capitalize(const string &str)
{
  string result = str;
  bool first = true;
  for(char &c : result)
  {
    if(!isalpha(c))
      first = true;
    else
    {
      if(first && islower(c))
        c = toupper(c);
      first = false;
    }
  }
  return result;
}



string Format::LowerCase(const string &str)
{
  string result = str;
  for(char &c : result)
    c = tolower(c);
  return result;
}



// Split a single string into substrings with the given separator.
vector<string> Format::Split(const string &str, const string &separator)
{
  vector<string> result;
  size_t begin = 0;
  while(true)
  {
    size_t pos = str.find(separator, begin);
    if(pos == string::npos)
      pos = str.length();
    result.emplace_back(str, begin, pos - begin);
    begin = pos + separator.size();
    if(begin >= str.length())
      break;
  }
  return result;
}
