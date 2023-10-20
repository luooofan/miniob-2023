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
// Created by Wangyunlai.wyl on 2021/5/18.
//

#include "storage/index/index_meta.h"
#include "storage/field/field_meta.h"
#include "storage/table/table_meta.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "json/json.h"

const static Json::StaticString FIELD_NAME("name");
const static Json::StaticString FIELD_UNIQUE("unique");
const static Json::StaticString FIELD_FIELD_NUM("field_num");
const static Json::StaticString FIELD_FIELD_NAME("field_name");

RC IndexMeta::init(const char *name, bool unique, const std::vector<const FieldMeta*> &fields)
{
  if (common::is_blank(name)) {
    LOG_ERROR("Failed to init index, name is empty.");
    return RC::INVALID_ARGUMENT;
  }

  name_ = name;
  unique_ = unique;
  for (const FieldMeta *field : fields) {
    field_.emplace_back(field->name());
  }
  return RC::SUCCESS;
}

void IndexMeta::to_json(Json::Value &json_value) const
{
  json_value[FIELD_NAME] = name_;
  json_value[FIELD_UNIQUE] = unique_;
  json_value[FIELD_FIELD_NUM] = field_.size();
  Json::Value fields;
  for (int i = 0; i < field_.size(); i++) {
    fields[i] = field_[i];
  }
  json_value[FIELD_FIELD_NAME] = std::move(fields);
}

RC IndexMeta::from_json(const TableMeta &table, const Json::Value &json_value, IndexMeta &index)
{
  const Json::Value &name_value = json_value[FIELD_NAME];
  const Json::Value &unique = json_value[FIELD_UNIQUE];
  const Json::Value &field_num = json_value[FIELD_FIELD_NUM];
  const Json::Value &field_value = json_value[FIELD_FIELD_NAME];
  if (!name_value.isString()) {
    LOG_ERROR("Index name is not a string. json value=%s", name_value.toStyledString().c_str());
    return RC::INTERNAL;
  }

  if (!unique.isBool()) {
    LOG_ERROR("Index unique_option is not a bool. json value=%s", unique.toStyledString().c_str());
    return RC::INTERNAL;
  }

  if (!field_num.isInt()) {
    LOG_ERROR("Field num of index [%s] is not a integer. json value=%s",
        name_value.asCString(),
        field_num.toStyledString().c_str());
        return RC::INTERNAL;
  }

  if (field_num.asInt() != field_value.size()) {
    LOG_ERROR("Field num of index [%s] is %d, not equal to field size %d",
        name_value.asCString(),
        field_num.asInt(),
        field_value.size());
        return RC::INTERNAL;
  }

  for (int i = 0; i < field_num.asInt(); i++) {
    if (!field_value[i].isString()) {
      LOG_ERROR("Field name of index [%s] is not a string. json value=%s",
          name_value.asCString(),
          field_value.toStyledString().c_str());
      return RC::INTERNAL;
    }
  }

  std::vector<const FieldMeta*> fields;
  for (int i = 0; i < field_value.size(); i++) {
    const FieldMeta *field = table.field(field_value[i].asCString());
    if (nullptr == field) {
      LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), field_value.asCString());
      return RC::SCHEMA_FIELD_MISSING;
    }
    fields.emplace_back(field);
  }

  return index.init(name_value.asCString(), unique.asBool(), fields);
}

const char *IndexMeta::name() const
{
  return name_.c_str();
}

const bool IndexMeta::unique() const
{
  return unique_;
}

const std::vector<std::string> &IndexMeta::field() const
{
  return field_;
}

void IndexMeta::desc(std::ostream &os) const
{
  std::string fields;
  for (const std::string &field : field_) {
    fields += field;
    fields += " ";
  }
  os << "index name=" << name_ << ", field=" << fields;
}