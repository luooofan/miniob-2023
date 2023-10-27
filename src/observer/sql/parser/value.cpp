/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by WangYunlai on 2023/06/28.
//

#include <sstream>
#include "sql/parser/value.h"
#include "storage/field/field.h"
#include "common/log/log.h"
#include "common/lang/comparator.h"
#include "common/lang/string.h"
#include "common/time/date.h"

const char *ATTR_TYPE_NAME[] = {"undefined", "chars", "ints", "floats","doubles", "dates", "long", "texts", "nulls", "booleans"};

const char *attr_type_to_string(AttrType type)
{
  if (type >= UNDEFINED && type <= NULLS) {
    return ATTR_TYPE_NAME[type];
  }
  return "unknown";
}
AttrType attr_type_from_string(const char *s)
{
  for (unsigned int i = 0; i < sizeof(ATTR_TYPE_NAME) / sizeof(ATTR_TYPE_NAME[0]); i++) {
    if (0 == strcmp(ATTR_TYPE_NAME[i], s)) {
      return (AttrType)i;
    }
  }
  return UNDEFINED;
}

Value::Value(int val)
{
  set_int(val);
}

Value::Value(float val)
{
  set_float(val);
}
Value::Value(double val)
{
  set_double(val);
}
Value::Value(bool val)
{
  set_boolean(val);
}
Value::Value(int64_t val)
{
  set_long(val);
}
Value::Value(const char *s, int len /*= 0*/)
{
  set_string(s, len);
}

// NULLS can not set data
void Value::set_data(char *data, int length)
{
  switch (attr_type_) {
    case CHARS: {
      set_string(data, length);
    } break;
    case INTS: {
      num_value_.int_value_ = *(int *)data;
      length_ = length;
    } break;
    case FLOATS: {
      num_value_.float_value_ = *(float *)data;
      length_ = length;
    } break;
    case DOUBLES:{
      num_value_.double_value_ = *(double *)data;
      length_ = length;
    } break;
    case BOOLEANS: {
      num_value_.bool_value_ = *(int *)data != 0;
      length_ = length;
    } break;
    case DATES:{
      num_value_.int_value_ = *(int *)data;
      length_ = length;
    } break;
    case LONGS: {
      num_value_.long_ = *(int64_t *)data;
      length_ = length;
    } break;
    default: {
      LOG_WARN("unknown data type: %d", attr_type_);
    } break;
  }
}
void Value::set_int(int val)
{
  attr_type_ = INTS;
  num_value_.int_value_ = val;
  length_ = sizeof(val);
}

void Value::set_date(int val)
{
  attr_type_ = DATES;
  num_value_.int_value_ = val;
  length_ = sizeof(val);
}

void Value::set_float(float val)
{
  attr_type_ = FLOATS;
  num_value_.float_value_ = val;
  length_ = sizeof(val);
}
void Value::set_double(double val)
{
  attr_type_ = DOUBLES;
  num_value_.double_value_ = val;
  length_ = sizeof(val);
}
void Value::set_long(int64_t val)
{
  attr_type_ = LONGS;
  num_value_.long_ = val;
  length_ = sizeof(int64_t);
}
void Value::set_boolean(bool val)
{
  attr_type_ = BOOLEANS;
  num_value_.bool_value_ = val;
  length_ = sizeof(val);
}
void Value::set_string(const char *s, int len /*= 0*/)
{
  attr_type_ = CHARS;
  if (len > 0) {
    len = strnlen(s, len);
    str_value_.assign(s, len);
  } else {
    str_value_.assign(s);
  }
  length_ = str_value_.length();
}

void Value::set_value(const Value &value)
{
  switch (value.attr_type_) {
    case INTS: {
      set_int(value.get_int());
    } break;
    case DATES: {
      set_date(value.get_int());
    } break;
    case FLOATS: {
      set_float(value.get_float());
    } break;
    case DOUBLES: {
      set_double(value.get_double());
    } break;
    case CHARS: {
      set_string(value.get_string().c_str());
    } break;
    case BOOLEANS: {
      set_boolean(value.get_boolean());
    } break;
    case NULLS: {
      set_null();
    } break;
    case UNDEFINED: {
      ASSERT(false, "got an invalid value type");
    } break;
  }
}

const char *Value::data() const
{
  switch (attr_type_) {
    case CHARS: {
      return str_value_.c_str();
    } break;
    default: {
      return (const char *)&num_value_;
    } break;
  }
}

std::string Value::to_string() const
{
  std::stringstream os;
  switch (attr_type_) {
    case INTS: {
      os << num_value_.int_value_;
    } break;
    case FLOATS: {
      os << common::double_to_str(num_value_.float_value_);
    } break;
    case DOUBLES: {
      os << common::double_to_str(num_value_.double_value_);
    } break;
    case BOOLEANS: {
      os << num_value_.bool_value_;
    } break;
    case CHARS: {
      os << str_value_;
    } break;
    case DATES:{
      os << date_to_string(num_value_.int_value_);
    } break;
    // TODO(wbj) used to cast ?
    case NULLS: {
      os << "NULL";
    } break;
    default: {
      LOG_WARN("unsupported attr type: %d", attr_type_);
    } break;
  }
  return os.str();
}

int Value::compare(const Value &other) const
{
  //ASSERT(!this->is_null() && !other.is_null(), "Cound Not Be Null!");
  // null compare null ,return 0 
  if(this->is_null() || other.is_null())
  {
    if(this->is_null() && other.is_null() )
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
  if (this->attr_type_ == other.attr_type_) {
    switch (this->attr_type_) {
      case INTS: {
        return common::compare_int((void *)&this->num_value_.int_value_, (void *)&other.num_value_.int_value_);
      } break;
      case DATES: {
        return common::compare_int((void *)&this->num_value_.int_value_, (void *)&other.num_value_.int_value_);
      } break;
      case FLOATS: {
        return common::compare_float((void *)&this->num_value_.float_value_, (void *)&other.num_value_.float_value_);
      } break;
      case DOUBLES: {
        return common::compare_double((void *)&this->num_value_.double_value_, (void *)&other.num_value_.double_value_);
      } break;
      case CHARS: {
        return common::compare_string((void *)this->str_value_.c_str(),
            this->str_value_.length(),
            (void *)other.str_value_.c_str(),
            other.str_value_.length());
      } break;
      case BOOLEANS: {
        return common::compare_int((void *)&this->num_value_.bool_value_, (void *)&other.num_value_.bool_value_);
      }
      default: {
        LOG_WARN("unsupported type: %d", this->attr_type_);
      }
    }
  } else if (this->attr_type_ == INTS && other.attr_type_ == FLOATS) {
    float this_data = this->num_value_.int_value_;
    return common::compare_float((void *)&this_data, (void *)&other.num_value_.float_value_);
  } else if (this->attr_type_ == FLOATS && other.attr_type_ == INTS) {
    float other_data = other.num_value_.int_value_;
    return common::compare_float((void *)&this->num_value_.float_value_, (void *)&other_data);
  // } else if (this->attr_type_ == INTS && other.attr_type_ == DOUBLES) {
  //   double this_data = this->num_value_.int_value_;
  //   return common::compare_double((void *)&this_data, (void *)&other.num_value_.double_value_);
  // } else if (this->attr_type_ == DOUBLES && other.attr_type_ == INTS) {
  //   double other_data = other.num_value_.int_value_;
  //   return common::compare_double((void *)&this->num_value_.double_value_, (void *)&other_data);
  // } else if (this->attr_type_ == FLOATS && other.attr_type_ == DOUBLES) {
  //   float this_data = this->num_value_.float_value_;
  //   return common::compare_double((void *)&this_data, (void *)&other.num_value_.double_value_);
  // } else if (this->attr_type_ == DOUBLES && other.attr_type_ == FLOATS) {
  //   float other_data = other.num_value_.float_value_;
  //   return common::compare_double((void *)&this->num_value_.double_value_, (void *)&other_data);
  } else {
    double this_data = this->get_double();
    double other_data = other.get_double();
    return common::compare_double((void *)&this_data, (void *)&other_data);
  }
  LOG_WARN("not supported");
  return -1;  // TODO return rc?
}

int Value::get_int() const
{
  //ASSERT(attr_type_ != DATES,"date can not get_int()"); date_format 需要调用 get_int()
  switch (attr_type_) {
    case CHARS: {
      try {
        return (int)(std::stol(str_value_));
      } catch (std::exception const &ex) {
        LOG_TRACE("failed to convert string to number. s=%s, ex=%s", str_value_.c_str(), ex.what());
        return 0;
      }
    }
    case INTS:
    case DATES: {
      return num_value_.int_value_;
    }
    case FLOATS: {
      return (int)(num_value_.float_value_ + 0.5);
    }
    case DOUBLES: {
      return (int)(num_value_.double_value_ + 0.5);
    }
    case BOOLEANS: {
      return (int)(num_value_.bool_value_);
    }
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return 0;
    }
  }
  return 0;
}

float Value::get_float() const
{
  ASSERT(attr_type_ != DATES,"date can not get_float()");
  switch (attr_type_) {
    case CHARS: {
      try {
        return std::stof(str_value_);
      } catch (std::exception const &ex) {
        LOG_TRACE("failed to convert string to float. s=%s, ex=%s", str_value_.c_str(), ex.what());
        return 0.0;
      }
    } break;
    case INTS: {
      return float(num_value_.int_value_);
    } break;
    case FLOATS: {
      return num_value_.float_value_;
    } break;
    case DOUBLES:{
      return float(num_value_.double_value_);
    } break;
    case BOOLEANS: {
      return float(num_value_.bool_value_);
    } break;
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return 0;
    }
  }
  return 0;
}


double Value::get_double() const
{
  ASSERT(attr_type_ != DATES,"date can not get_double()");
  switch (attr_type_) {
    case CHARS: {
      try {
        return std::stod(str_value_);
      } catch (std::exception const &ex) {
        LOG_TRACE("failed to convert string to float. s=%s, ex=%s", str_value_.c_str(), ex.what());
        return 0.0;
      }
    } break;
    case INTS: {
      return double(num_value_.int_value_);
    } break;
    case FLOATS: {
      return double(num_value_.float_value_);
    } break;
    case DOUBLES: {
      return  num_value_.double_value_;
    } break;
    case BOOLEANS: {
      return double(num_value_.bool_value_);
    } break;
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return 0;
    }
  }
  return 0;
}

int64_t Value::get_long() const
{
  return num_value_.long_;
}


std::string Value::get_string() const
{
  return this->to_string();
}

bool Value::get_boolean() const
{
  ASSERT(attr_type_ != DATES,"date can not get_boolean()");
  switch (attr_type_) {
    case CHARS: {
      try {
        float val = std::stof(str_value_);
        if (val >= EPSILON || val <= -EPSILON) {
          return true;
        }

        int int_val = std::stol(str_value_);
        if (int_val != 0) {
          return true;
        }

        return !str_value_.empty();
      } catch (std::exception const &ex) {
        LOG_TRACE("failed to convert string to float or integer. s=%s, ex=%s", str_value_.c_str(), ex.what());
        return !str_value_.empty();
      }
    } break;
    case INTS: {
      return num_value_.int_value_ != 0;
    } break;
    case FLOATS: {
      float val = num_value_.float_value_;
      return val >= EPSILON || val <= -EPSILON;
    } break;
    case DOUBLES: {
      double val = num_value_.double_value_;
      return val >= EPSILON || val <= -EPSILON;
    } break;
    case BOOLEANS: {
      return num_value_.bool_value_;
    } break;
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return false;
    }
  }
  return false;
}

const Value& Value::min(const Value &a, const Value &b)
{
  if (a.is_null()) {
    return b;  // even if b is also null
  }
  return a.compare(b) <= 0 ? a : b;
}
const Value& Value::max(const Value &a, const Value &b)
{
  if (a.is_null()) {
    return b;  // even if b is also null
  }
  return a.compare(b) >= 0 ? a : b;
}

const Value Value::add(const Value &left, const Value &right)
{
  Value result_cell;
  if (left.is_null() || right.is_null()) {
    result_cell.set_null();
    return result_cell;
  }
  if (left.attr_type() == INTS && right.attr_type() == INTS) {
    int result = left.get_int()+right.get_int();
    result_cell.set_int(result);
  } else {
    double tmp_left = left.get_double();
    double tmp_right = right.get_double();
    double result = tmp_left + tmp_right;
    result_cell.set_double(result);
  }
  return result_cell;
}

const Value Value::div(const Value &left, const Value &right)
{
  Value result_cell;
  if (left.is_null() || right.is_null()) {
    result_cell.set_null();
    return result_cell;
  }
  if(right.get_double() == 0) {
    result_cell.set_null();
  }
  else {
    double tmp_left = left.get_double();
    double tmp_right = right.get_double();
    double result = tmp_left / tmp_right;
    result_cell.set_double(result);
  }
  return result_cell;
}