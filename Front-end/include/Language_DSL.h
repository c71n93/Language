#define IS_CHAR_TOKEN(character)           \
(*tokens_array->ptr)->data.ch == character

#define IS_CHAR_NEXT_TOKEN(character)       \
(*tokens_array->ptr + 1)->data.ch == character