#pragma once

#include <string>
#include <vector>
#include "sql/parser/parse_defs.h"
#include "storage/table/base_table.h"
#include "storage/table/table_meta.h"

class Db;
class Field;
class FieldMeta;
class TableMeta;
class SelectStmt;

class View : public BaseTable
{
public:
  View() = default;
  ~View() = default;

  RC create(int32_t table_id, 
            bool allow_write, 
            const char *path,       // .view文件路径、名称
            const char *name,       // view_name
            const char *base_dir,   // db/sys
            const std::vector<AttrInfoSqlNode> &attr_infos, 
            const std::vector<Field> &map_fields, 
            SelectSqlNode *select_sql);
  RC open();
  RC drop();
  void set_db(Db *db) { db_ = db; }

public:
  virtual int32_t table_id() const { return table_meta_.table_id(); }
  virtual const char *name() const { return table_meta_.name(); }
  virtual const TableMeta &table_meta() const { return table_meta_; }
  
  Db *get_db() const { return db_; }
  bool allow_write() const { return allow_write_; }
  const std::vector<Field> &get_map_fields() const { return map_fields_; }
  SelectSqlNode get_select_sql() const { return select_sql_; }

private:
  int get_serialize_size();
  RC serialize(std::fstream &fs);
  RC deserialize();

private:
  Db *db_;
  std::string base_dir_;
  bool allow_write_ = false;            // 是否允许对视图进行写操作
  TableMeta   table_meta_;
  std::vector<Field> map_fields_;       // view列映射的原始表中的列，可写view中所有col映射的都是Field，只读view不需要用到这个映射
  SelectSqlNode select_sql_;
};