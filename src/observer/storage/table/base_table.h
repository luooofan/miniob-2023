/* 
 * 用来区分Table和View
 */

#pragma once

class TableMeta;

enum class BaseTabletype
{
  TABLE_TYPE,
  VIEW_TYPE
};

class BaseTable
{
public:
  BaseTable() = default;

  virtual int32_t table_id() const = 0;
  virtual const char *name() const = 0;
  virtual const TableMeta &table_meta() const = 0;
public:
  void set_view_type()
  {
    base_type_ = BaseTabletype::VIEW_TYPE;
  }
  bool is_view() const
  {
    return BaseTabletype::VIEW_TYPE == base_type_;
  }
  bool is_table() const
  {
    return BaseTabletype::TABLE_TYPE == base_type_;
  }

private:
  BaseTabletype base_type_ = BaseTabletype::TABLE_TYPE;
};