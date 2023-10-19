/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once

#include <vector>
#include "sql/optimizer/rewrite_rule.h"

/**
 * @brief 将父子关系的谓词合并成一个
 * @ingroup Rewriter
 * @details 使得能够用其他规则
 */
class PredicateMergeRewriter : public RewriteRule 
{
public:
  PredicateMergeRewriter() = default;
  virtual ~PredicateMergeRewriter() = default;

  RC rewrite(std::unique_ptr<LogicalOperator> &oper, bool &change_made) override;

private:
  RC merge(std::unique_ptr<Expression>& source, std::unique_ptr<Expression>& target);
};
