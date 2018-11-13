#include "../common.h"

#define binary_xx(optype, realop) case optype: \
switch(con->kids[0]->type->type_id) { \
        case TYPE_INTEGER: \
                con->val.i = (con->kids[0]->val.i) realop (con->kids[1]->val.i); \
                break; \
        case TYPE_UINTEGER: \
                con->val.ui = (con->kids[0]->val.ui) realop (con->kids[1]->val.ui); \
                break; \
        case TYPE_BOOLEAN: \
                con->val.b = (con->kids[0]->val.b) realop (con->kids[1]->val.b); \
                break; \
        case TYPE_CHAR: \
                con->val.c = (con->kids[0]->val.c) realop (con->kids[1]->val.c); \
                break; \
        case TYPE_UCHAR: \
                con->val.uc = (con->kids[0]->val.uc) realop (con->kids[1]->val.uc); \
                break; \
        default: \
        { \
            parse_error("compute error", get_op_name(optype)); \
            assert(0); \
        } \
        break; \
} \
break;

#define real_binary_xx(optype, realop) case optype: \
switch(con->kids[0]->type->type_id) { \
        case TYPE_INTEGER: \
                con->val.i = (con->kids[0]->val.i) realop (con->kids[1]->val.i); \
                break; \
        case TYPE_UINTEGER: \
                con->val.ui = (con->kids[0]->val.ui) realop (con->kids[1]->val.ui); \
                break; \
        case TYPE_BOOLEAN: \
                con->val.b = (con->kids[0]->val.b) realop (con->kids[1]->val.b); \
                break; \
        case TYPE_CHAR: \
                con->val.c = (con->kids[0]->val.c) realop (con->kids[1]->val.c); \
                break; \
        case TYPE_UCHAR: \
                con->val.uc = (con->kids[0]->val.uc) realop (con->kids[1]->val.uc); \
                break; \
        case TYPE_REAL: \
                con->val.f = (con->kids[0]->val.f) realop (con->kids[1]->val.f); \
                break; \
        default: \
        { \
            parse_error("compute error", get_op_name(optype)); \
            assert(0); \
        } \
        break; \
} \
break;

#define single_xx(optype, realop) case optype: \
switch(con->kids[0]->type->type_id) { \
        case TYPE_INTEGER: \
                con->val.i = realop (con->kids[0]->val.i); \
                break; \
        case TYPE_UINTEGER: \
                con->val.ui = realop (con->kids[0]->val.ui); \
                break; \
        case TYPE_BOOLEAN: \
                con->val.b = realop (con->kids[0]->val.b); \
                break; \
        case TYPE_CHAR: \
                con->val.c = realop (con->kids[0]->val.c);\
                break; \
        case TYPE_UCHAR: \
                con->val.uc = realop (con->kids[0]->val.uc); \
                break; \
        default: \
        { \
            parse_error("compute error", get_op_name(optype)); \
            assert(0); \
        } \
        break; \
} \
break;

#define real_single_xx(optype, realop) case optype: \
switch(con->kids[0]->type->type_id) { \
        case TYPE_INTEGER: \
                con->val.i = realop (con->kids[0]->val.i); \
                break; \
        case TYPE_UINTEGER: \
                con->val.ui = realop (con->kids[0]->val.ui); \
                break; \
        case TYPE_BOOLEAN: \
                con->val.b = realop (con->kids[0]->val.b); \
                break; \
        case TYPE_CHAR: \
                con->val.c = realop (con->kids[0]->val.c);\
                break; \
        case TYPE_UCHAR: \
                con->val.uc = realop (con->kids[0]->val.uc); \
                break; \
        case TYPE_REAL: \
                con->val.f = realop (con->kids[0]->val.f); \
                break; \
        default: \
        { \
            parse_error("compute error", get_op_name(optype)); \
            assert(0); \
        } \
        break; \
} \
break;

void arithmetical_operate(Node con)
{
  switch(con->op)
  {
    binary_xx(BAND, &);
    binary_xx(BOR, |);
    binary_xx(BXOR, ^);
    binary_xx(MOD, %);
    binary_xx(RSH, >>);
    binary_xx(LSH, <<);
    real_binary_xx(AND, &&);
    real_binary_xx(OR, ||);
    real_binary_xx(EQ, ==);
    real_binary_xx(NE, !=);
    real_binary_xx(GT, >);
    real_binary_xx(GE, >=);
    real_binary_xx(LT, <);
    real_binary_xx(LE, <=);
    real_binary_xx(ADD, +);
    real_binary_xx(SUB, -);
    real_binary_xx(DIV, /);
    real_binary_xx(MUL, *);
    single_xx(NOT, !);
    single_xx(BCOM, ~);
    single_xx(INCR, ++);
    single_xx(DECR, --);
    real_single_xx(NEG, -);
    default:
    {
        parse_error("compute error", "");
        assert(0);
    }
    break;
  }
}