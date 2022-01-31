#define IS_TOKEN_TYPE(token_type)           \
(*tokens_array->ptr)->type == token_type

#define IS_NEXT_TOKEN_TYPE(token_type)           \
(*(tokens_array->ptr + 1))->type == token_type

#define IS_TOKEN_OP(op)            \
IS_TOKEN_TYPE(OP) && (*tokens_array->ptr)->data.ch == op

#define IS_NEXT_TOKEN_OP(op)       \
IS_NEXT_TOKEN_TYPE(OP) && (*(tokens_array->ptr + 1))->data.ch == op