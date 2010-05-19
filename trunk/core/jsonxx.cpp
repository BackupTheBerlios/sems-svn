// Author: Hong Jiang <hong@hjiang.net>
/* 
source: http://github.com/hjiang/jsonxx/

Copyright (c) 2010 Hong Jiang

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

*/

#include "jsonxx.h"

#include <cctype>
#include <iostream>
#include <sstream>

namespace jsonxx {

void eat_whitespaces(std::istream& input) {
    char ch;
    do {
        input.get(ch);
    } while(isspace(ch));
    input.putback(ch);
}

// Try to consume characters from the input stream and match the
// pattern string. Leading whitespaces from the input are ignored if
// ignore_ws is true.
bool match(const std::string& pattern, std::istream& input,
           bool ignore_ws) {
    if (ignore_ws) {
        eat_whitespaces(input);
    }
    std::string::const_iterator cur(pattern.begin());
    char ch(0);
    while(input && !input.eof() && cur != pattern.end()) {
        input.get(ch);
        if (ch != *cur) {
            input.putback(ch);
            return false;
        } else {
            cur++;
        }
    }
    return cur == pattern.end();
}

bool parse_string(std::istream& input, std::string* value) {
    if (!match("\"", input))  {
        return false;
    }
    char ch;
    while(!input.eof() && input.good()) {
        input.get(ch);
        if (ch == '"') {
            break;
        }
        value->push_back(ch);
    }
    if (input && ch == '"') {
        return true;
    } else {
        return false;
    }
}

bool parse_bool(std::istream& input, bool* value) {
    if (match("true", input))  {
        *value = true;
        return true;
    }
    if (match("false", input)) {
        *value = false;
        return true;
    }
    return false;
}

bool parse_null(std::istream& input) {
    if (match("null", input))  {
        return true;
    }
    return false;
}

bool parse_float(std::istream& input, double* value) {
    eat_whitespaces(input);
    char ch;
    bool has_dot = false;
    std::string value_str;
    int sign = 1;
    if (match("-", input)) {
        sign = -1;
    } else {
        match("+", input);
    }
    while(input && !input.eof()) {
        input.get(ch);
	if (ch=='.')
	  has_dot = true;
        if (!isdigit(ch) && (!(ch == '.'))) {
            input.putback(ch);
            break;
        }
        value_str.push_back(ch);
    }
    if (!has_dot) {
      for (std::string::reverse_iterator r_it=
	     value_str.rbegin(); r_it != value_str.rend(); r_it++)
	input.putback(*r_it);
      return false;
    }
    if (value_str.size() > 0) {
        std::istringstream(value_str) >> *value;
	*value*=sign;
        return true;
    } else {
        return false;
    }
}

bool parse_number(std::istream& input, long* value) {
    eat_whitespaces(input);
    char ch;
    std::string value_str;
    int sign = 1;
    if (match("-", input)) {
        sign = -1;
    } else {
        match("+", input);
    }
    while(input && !input.eof()) {
        input.get(ch);
        if (!isdigit(ch)) {
            input.putback(ch);
            break;
        }
        value_str.push_back(ch);
    }
    if (value_str.size() > 0) {
        std::istringstream(value_str) >> *value;
	*value*=sign;
        return true;
    } else {
        return false;
    }
}

bool parse_number(std::istream& input, int* value) {
    eat_whitespaces(input);
    char ch;
    std::string value_str;
    int sign = 1;
    if (match("-", input)) {
        sign = -1;
    } else {
        match("+", input);
    }
    while(input && !input.eof()) {
        input.get(ch);
        if (!isdigit(ch)) {
            input.putback(ch);
            break;
        }
        value_str.push_back(ch);
    }
    if (value_str.size() > 0) {
        std::istringstream(value_str) >> *value;
	*value*=sign;
        return true;
    } else {
        return false;
    }
}

Object::Object() : value_map_() {}

Object::~Object() {
    std::map<std::string, Value*>::iterator i;
    for (i = value_map_.begin(); i != value_map_.end(); ++i) {
        delete i->second;
    }
}

bool Object::parse(std::istream& input) {
    if (!match("{", input)) {
        return false;
    }

    do {
        std::string key;
        if (!parse_string(input, &key)) {
            return false;
        }
        if (!match(":", input)) {
            return false;
        }
        Value* v = new Value();
        if (!v->parse(input)) {
            delete v;
            break;
        }
        value_map_[key] = v;
    } while (match(",", input));

    if (!match("}", input)) {
        return false;
    }
    return true;
}

Value::Value() : type_(INVALID_) {}

Value::~Value() {
    if (type_ == STRING_) {
        delete string_value_;
    }
    if (type_ == OBJECT_) {
        delete object_value_;
    }
    if (type_ == ARRAY_) {
        delete array_value_;
    }
}

bool Value::parse(std::istream& input) {
    std::string string_value;
    if (parse_string(input, &string_value)) {
        string_value_ = new std::string();
        string_value_->swap(string_value);
        type_ = STRING_;
        return true;
    }
    if (parse_number(input, &integer_value_)) {
        type_ = INTEGER_;
        return true;
    }

    if (parse_bool(input, &bool_value_)) {
        type_ = BOOL_;
        return true;
    }
    if (parse_null(input)) {
        type_ = NULL_;
        return true;
    }
    array_value_ = new Array();
    if (array_value_->parse(input)) {
        type_ = ARRAY_;
        return true;
    }
    delete array_value_;
    object_value_ = new Object();
    if (object_value_->parse(input)) {
        type_ = OBJECT_;
        return true;
    }
    delete object_value_;
    return false;
}

Array::Array() : values_() {}

Array::~Array() {
    for (unsigned int i = 0; i < values_.size(); ++i) {
        delete values_[i];
    }
}

bool Array::parse(std::istream& input) {
    if (!match("[", input)) {
        return false;
    }

    do {
        Value* v = new Value();
        if (!v->parse(input)) {
            delete v;
            break;
        }
        values_.push_back(v);
    } while (match(",", input));

    if (!match("]", input)) {
        return false;
    }
    return true;
}

}  // namespace jsonxx
