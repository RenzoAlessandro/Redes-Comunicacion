#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <string>
#include <sstream>

using namespace std;


class Point
{
public:
  int x,y;
  Point(int _x, int _y)
  {
    x = _x;
    y = _y;
  }
  Point()
  {
    x = 0;
    y = 0;
  }
};


string int_to_char_spaces(int input_number, int cant_spaces)
  {
  string result_char;

  stringstream ss;
  ss << input_number;

  string prev_string = ss.str();

  string temp_string;

  if (prev_string.size() <= cant_spaces)
  {

  temp_string.append((cant_spaces - prev_string.size()), '0');
  }

  else
  {
  string error_string = "Invalid parameters.";

  return error_string;
  }

  prev_string = temp_string + prev_string;

  return prev_string;
}

#endif /* ADD_H */
