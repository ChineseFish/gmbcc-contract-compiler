#include "../common.h"

/* alloc memory from tree arena. */
static int where = TREE;
static int print_level = 0;

/* new tree or node */
Tree new_tree(int op, Type type, Tree left, Tree right)
{

    Tree p;

    NEW0(p, where);
    if(!p)
    {
        internal_error("insufficient memory");
    }

    p->op = op; /* AST节点的类型 */
    p->result_type = type;
    p->kids[0] = left;
    p->kids[1] = right;
    return p;
}

/* new header tree. */
Tree header_tree(symtab *ptab)
{
    Tree t;
    t = new_tree(HEADER, ptab->type, NULL, NULL);
    t->u.generic.symtab = ptab;
    return t;
}

/* type conversion tree. */
Tree conversion_tree(Tree source, Type target)
{
    Tree t;
    switch (target->type_id)
    {
        case TYPE_INTEGER:
            t = new_tree(CVI, target, source, NULL);
            break;
        case TYPE_REAL:
            t = new_tree(CVF, target, source, NULL);
            break;
        case TYPE_BOOLEAN:
            t = new_tree(CVB, target, source, NULL);
            break;
        case TYPE_UINTEGER:
            t = new_tree(CVUI, target, source, NULL);
            break;
        case TYPE_UCHAR:
            t = new_tree(CVUC, target, source, NULL);
            break;
        case TYPE_CHAR:
            t = new_tree(CVC, target, source, NULL);
            break;
        default:
        {
            parse_error("conversion_tree, invalid conversion", target->name);
            exit(1);
        }
    }

    return t;
}

/* 获取变量值 */
Tree id_factor_tree(Tree source, Symbol sym)
{
    Tree t;

    if (source)
        /* 通过AST树取值（数组） */
        t = new_tree(LOAD, source->result_type, source, NULL);
    else
        /* 通过符号取值 */
        t = new_tree(LOAD, sym->type, NULL, NULL);

    /* 节点对应的symbol */
    t->u.generic.sym = sym;
    return t;
}

Tree address_tree(Symbol sym)
{
    Tree t;

    t = new_tree(ADDRG, sym->type, NULL, NULL);
    
    /* 变量对应的符号 */
    t->u.generic.sym = sym;
    
    return t;
}

Tree neg_tree(Tree source)
{
    Tree t;

    t = new_tree(NEG, source->result_type, source, NULL);
    return t;
}

Tree not_tree(Tree source)
{
    Tree t;

    switch(source->result_type->type_id)
    {
        case TYPE_BOOLEAN:
        {

        }
        break;
        case TYPE_INTEGER:
        case TYPE_UINTEGER:
        case TYPE_CHAR:
        case TYPE_UCHAR:
        case TYPE_REAL:
        {
            source = conversion_tree(source, find_system_type_by_id(TYPE_BOOLEAN));
        }
        break;
        default:
        {
            parse_error("not_tree, invalid source", "");
            exit(1);
        }
    }

    t = new_tree(NOT, find_system_type_by_id(TYPE_BOOLEAN), source, NULL);
    return t;
}

Tree com_tree(Tree source)
{
    Tree t;

    t = new_tree(BCOM, source->result_type, source, NULL);
    return t;
}

/* arguments tree.
 * set argtree to NULL when first called.
 * argtree表示可以参数AST树、function表示函数对应的符号表、arg指向符号表中的参数链表的头部、expr表示实参
 */
Tree arg_tree(Tree argtree, Symtab function, Symbol arg, Tree expr)
{
    Tree t, right;

    /* 检查arg类型是否与expr类型相同 */
    if(arg != NULL && arg->type->type_id != expr->result_type->type_id)
    {
        if(arg->type->type_id == TYPE_ARRAY)
        {
            if(arg->type->last->type->type_id != TYPE_CHAR || expr->result_type->type_id != TYPE_STRING)
            {
                parse_error("arg_tree, type miss match.", "");
                exit(1);
            }
        }
        else
        {
            expr = conversion_tree(expr, arg->type);
        }
        
    }

    if(argtree == NULL)
    {
        /* 函数参数树没有进行初始化 */
        if (arg != NULL) 
            /* 符号表中的参数符号类型 */
            t = new_tree(ARG, arg->type, expr, NULL);
        else
            /* expr中的类型 */
            t = new_tree(ARG, expr->result_type, expr, NULL);

        t->u.generic.sym = arg; /* 参数符号 */
        t->u.generic.symtab = function; /* 函数符号表 */
        return t;
    }

    /* 函数参数树已经初始化 */
    if(argtree->kids[1] == NULL)
    {
        if(arg != NULL)
        {
            argtree->kids[1] = new_tree(RIGHT, arg->type, expr, NULL);
        }
        else
        {
            argtree->kids[1] = new_tree(RIGHT, expr->result_type, expr, NULL);
        }
        
        right = argtree->kids[1];
    }
    else
    {
        right = argtree->kids[1];

        /* 移动到参数AST树最右端 */
        while(right->kids[1] != NULL)
        {
            right = right->kids[1];
        }

        /* 初始化一个最右AST树 */
        if(arg != NULL)
        {
            right->kids[1] = new_tree(RIGHT, arg->type, expr, NULL);
        }
        else
        {
            right->kids[1] = new_tree(RIGHT, expr->result_type, expr, NULL);
        }

        right = right->kids[1];
    }

    right->u.generic.sym = arg; /* 参数符号 */
    right->u.generic.symtab = function; /* 函数符号表 */

    return argtree;
}

/* 记录的属性 */
Tree record_field_tree(Symbol record, Symbol field)
{
    Tree t;

    t = new_tree(FIELD, field->type, NULL, NULL);
    /* 记录符号 */
    t->u.field.record = record;
    /* 属性对应的符号 */
    t->u.field.field = field;

    return t;
}

/* 数组下标 */
Tree array_index_tree(Symbol array, Tree expr)
{
    Tree t;
    /* expr用于表示array的下标 */
    t = new_tree(ARRAY, array->type->last->type, expr, NULL);
    /* 对应的array符号 */
    t->u.generic.sym = array;

    return t;
}

/*  常量树 */
Tree const_tree(Symbol const_val)
{
    Tree t;

    t = new_tree(CNST, const_val->type, NULL, NULL);
    t->u.generic.sym = const_val;

    return t;
}

/*  自定义函数或过程调用 */
Tree call_tree(Symtab routine, Tree argstree)
{
    Tree t;

    t = new_tree(CALL, routine->type, argstree, NULL);
    t->u.generic.symtab = routine;
    return t;
}

/*  系统函数或过程调用 */
Tree sys_tree(int sys_id, Tree argstree)
{
    Tree t;
    Symtab ptab;

    ptab = find_sys_routine(sys_id);
    if(ptab)
    {
        t = new_tree(SYS, ptab->type, argstree, NULL);
    }
    else
    {
        /*  */
        char c_err[MAX_ERR_STR_LEN];
        snprintf(c_err, MAX_ERR_STR_LEN, "sys routine is not exist %d", sys_id);
        parse_error(c_err, "");
        /* 对应的系统函数或过程没有进行初始化，初始化一个空的AST树 */
        t = new_tree(SYS, find_system_type_by_id(TYPE_VOID), argstree, NULL);
    }

    /* 初始化系统函数或过程的id */
    t->u.sys.sys_id = sys_id;
    return t;
}

Tree binary_expr_tree(int op, Tree left, Tree right)
{
    Tree t;

    /*  */
    if(left->result_type->type_id == TYPE_STRING || right->result_type->type_id == TYPE_STRING)
    {
        parse_error("binary_expr_tree err, string type is invalid", "");
        exit(1);
    }

    /*  */
    if(op == LSH || op == RSH || op == MOD)
    {
        if(right->result_type->type_id != TYPE_INTEGER)
        {
            right = conversion_tree(right, find_system_type_by_id(TYPE_INTEGER));
        }

        if(op == MOD)
        {
            /*  */
            if(left->result_type->type_id != TYPE_INTEGER)
            {
                left = conversion_tree(left, find_system_type_by_id(TYPE_INTEGER));
            }
        }
    }
    else
    {
        if(left->result_type->type_id != right->result_type->type_id)
        {
            right = conversion_tree(right, left->result_type);
        }
    }

    /* AST树的类型等于左AST树类型 */
    t = new_tree(op, left->result_type, left, right);
    t->result_type = left->result_type;
    return t;
}

Tree compare_expr_tree(int op, Tree left, Tree right)
{
    Tree t;

    /*  */
    if(left->result_type->type_id == TYPE_STRING || right->result_type->type_id == TYPE_STRING)
    {
        parse_error("compare_expr_tree err, string type is invalid", "");
        exit(1);
    }

    /*  */
    if(left->result_type->type_id != right->result_type->type_id)
    {
        right = conversion_tree(right, left->result_type);
    }

    /* AST树类型为布尔型 */
    t = new_tree(op, find_system_type_by_id(TYPE_BOOLEAN), left, right);
    return t;
}

Tree assign_tree(Tree id, Tree expr)
{
    Tree t;

    if(id->result_type->type_id != expr->result_type->type_id)
    {
        if(id->result_type->type_id == TYPE_ARRAY)
        {
            if(id->result_type->last->type->type_id != TYPE_CHAR || expr->result_type->type_id != TYPE_STRING)
            {
                parse_error("assign_tree, type miss match", "");
                exit(1);
            }
        }
        else
        {
            expr = conversion_tree(expr, id->result_type);
        }
    }

    /* id表示一棵地址树，expr表示赋值内容 */
    t = new_tree(ASGN, id->result_type, id, expr);

    return t;
}

/* 标签 */
Tree label_tree(Symbol label)
{
    Tree t;

    t = new_tree(LABEL, label->type, NULL, NULL);
    t->u.generic.sym = label;
    return t;
}

/* 
 * jump without condition.
 */
Tree jump_tree(Symbol label)
{
    Tree t;

    t = new_tree(JUMP, label->type, NULL, NULL);
    t->u.generic.sym = label;
    return t;
}

/* 
 * 条件跳转
 */
Tree cond_jump_tree(Tree cond, int trueorfalse, Symbol label)
{
    Tree t;

    /*  */
    switch(cond->result_type->type_id)
    {
        case TYPE_BOOLEAN:
        {

        }
        break;
        case TYPE_INTEGER:
        case TYPE_UINTEGER:
        case TYPE_REAL:
        case TYPE_CHAR:
        case TYPE_UCHAR:
        {
          cond = conversion_tree(cond, find_system_type_by_id(TYPE_BOOLEAN));
        }
        break;
        default:
        {
          parse_error("cond_jump_tree, cond is valid", "");
          exit(1);
        }
    }

    t = new_tree(COND, label->type, cond, NULL);
    t->u.cond_jump.label = label;
    t->u.cond_jump.true_or_false = trueorfalse;
    return t;
}

/*
 * increment.
 */
Tree incr_one_tree(Tree source)
{
    Tree t;

    t = new_tree(INCR, source->result_type, source, NULL);
    return t;
}

/*
 * decrement.
 */
Tree decr_one_tree(Tree source)
{
    Tree t;

    t = new_tree(DECR, source->result_type, source, NULL);
    return t;
}

static void print_tree(Tree tp)
{
    int i;

    if (tp == NULL)
        return;

    printf("+");
    for (i = 0; i < print_level; i++)
    {
        printf("-");
    }

    switch(generic(tp->op))
    {
    case LOAD:
        printf("%s (%s)\n", get_op_name(generic(tp->op)), tp->u.generic.sym->name);
        break;
    case LABEL:
        printf("LABEL (%s)\n", tp->u.generic.sym->name);
        break;
    case JUMP:
        printf("JUMP (%s)\n", tp->u.generic.sym->name);
        break;
    default:
        printf("%s\n", get_op_name(generic(tp->op)));
        break;
    }

    print_level++;

    if (tp->kids[0])
        print_tree(tp->kids[0]);

    if (tp->kids[1])
        print_tree(tp->kids[1]);

    print_level--;
}


void print_forest(List ast_forest)
{
    int n, i;
    Tree *forest;

    n = list_length(ast_forest);
    forest = (Tree *)list_ltov(ast_forest, STMT);

    printf("\n---------------------forest-----------------------------------\n");
    for (i = 0; i < n ; i++)
    {
        printf("\n\n");
        print_level = 0;
        print_tree(forest[i]);
    }
}

