
program
: program_item_list;

program_item_list
: program_item |
  program_item_list program_item;

import_statement
: IMPORT STRING_LIT;

program_item
: data_definition |
  func_definition |
  func_body |
  import_statement |
  exception_identifier;

exception_identifier
: EXCEPT IDENTIFIER;

type_name
: INTEGER |
  FLOAT |
  STRING |
  LIST |
  HASH;

formatted_string
: STRING_LIT expression_list_param |
  STRING_LIT;

data_declaration
: type_name IDENTIFIER |
  CONST type_name IDENTIFIER;

data_declaration_list
: data_declaration |
  data_declaration_list ',' data_declaration;

data_definition
: data_declaration |
  data_declaration '=' expression |
  data_declaration '=' list_init |
  data_declaration '=' dict_init;

func_name
: type_name IDENTIFIER |
  ITERATOR type_name IDENTIFIER |
  NOTHING IDENTIFIER;

func_definition
: func_name func_params func_body;

func_params
: '(' data_declaration_list ')' |
  '(' ')';

func_body
: '{' func_body_list '}';

func_body_list
: func_body_elem |
  func_body_list func_body_elem;

loop_body
: '{' '}' |
  '{' loop_body_list '}';

loop_body_list
: loop_body_elem |
  loop_body_list loop_body_elem;

loop_body_diffs
: BREAK |
  CONTINUE |
  YIELD '(' expression ')';

loop_body_elem
: func_body_elem |
  loop_body_diffs;

func_body_elem
: data_definition |
  func_reference |
  assignment |
  while_statement |
  do_statement |
  for_statement |
  ifelse_statement |
  tryexcept_statement |
  raise_statement |
  return_statement |
  exit_statement |
  print_statement |
  trace_statement |
  func_body;

trace_statement
: TRACE expression_list_param |
  TRACE;

print_statement
: PRINT expression_list_param |
  PRINT;

exit_statement
: EXIT expression_param;

return_statement
: RETURN expression_param |
  RETURN;

for_statement
: FOR '(' IDENTIFIER IN expression ')' func_body |
  FOR '(' type_name IDENTIFIER IN expression ')' func_body;

tryexcept_statement
: try_clause except_clause;

try_clause
: TRY func_body;

except_segment
: EXCEPT '(' IDENTIFIER ',' IDENTIFIER ')' func_body;

except_clause_list
: except_segment |
  except_clause_list except_segment;

final_except_clause
: EXCEPT '(' IDENTIFIER ')' func_body;

except_clause
: except_clause_list |
  except_clause_list final_except_clause |
  final_except_clause;

raise_statement
: RAISE '(' IDENTIFIER ',' formatted_string ')';

if_clause
: IF '(' expression ')' func_body;

ifelse_statement
: if_clause |
  if_clause else_clause;

else_segment
: ELSE '(' expression ')' func_body;

final_else_segment
: ELSE '(' ')' func_body |
  ELSE func_body;

else_clause_list
: else_segment |
  else_clause_list else_segment;

else_clause
: else_clause_list |
  else_clause_list final_else_segment |
  final_else_segment;

while_clause
: WHILE expression_param |
  WHILE;

while_statement
: while_clause loop_body;

do_statement
: DO loop_body while_clause;

assignment_right
: expression |
  '(' type_name ')' expression |
  list_init |
  dict_init;

list_init
: '[' expression_list ']';

dict_init_item
: STRING_LIT ':' expression;

dict_init_item_list
: dict_init_item |
  dict_init_item_list ',' dict_init_item;

dict_init
: '[' dict_init_item_list ']';

assignment_left
: IDENTIFIER |
  list_reference;

assignment
: assignment_left '=' assignment_right;

raw_value
: IDENTIFIER |
  INTEGER_LIT |
  FLOAT_LIT;

expr_primary
: raw_value |
  formatted_string |
  list_reference |
  func_reference;

list_reference
: IDENTIFIER list_ref_param_list;

list_ref_value
: IDENTIFIER |
  INTEGER_LIT |
  STRING_LIT |
  list_reference |
  func_reference |
  list_ref_param;

list_ref_param
: '[' list_ref_value ']';

list_ref_param_list
: list_ref_param |
  list_ref_param_list list_ref_param;

func_reference
: IDENTIFIER expression_list_param;

expression
: expr_primary |
  expression ADD_OPER expression |
  expression SUB_OPER expression |
  expression MUL_OPER expression |
  expression DIV_OPER expression |
  expression MOD_OPER expression |
  expression POW_OPER expression |
  expression GT_OPER expression |
  expression LT_OPER expression |
  expression EQU_OPER expression |
  expression NEQ_OPER expression |
  expression LTE_OPER expression |
  expression GTE_OPER expression |
  expression OR_OPER expression |
  expression AND_OPER expression |
  NOT_OPER expression % prec UNARY |
  SUB_OPER expression % prec UNARY |
  '(' expression ')';

expression_list
: expression |
  expression_list ',' expression;

expression_list_param
: '(' expression_list ')' |
  '(' ')';

expression_param
: '(' expression ')' |
  '(' ')';
