#include "module.h"
#include "class.h"
#include "selector.h"
#include "method.h"
#include <string.h>

#include <stdio.h>


enum {
    CLASS_TABLE_SIZE = 100
};

static struct ClassTableEntry {
    struct objc_class_gsv1* class_;
} classes_[CLASS_TABLE_SIZE];


const char* class_name(struct objc_class_gsv1* class)
{
    return ((struct objc_class_gsv1*)class->isa)->name;
}


id objc_get_class(const char* name)
{
    for (int i = 0; i < CLASS_TABLE_SIZE; ++i) {
        if (strcmp(name, class_name(classes_[i].class_)) == 0) {
            return (id)classes_[i].class_;
        }
    }
    return nil;
}


id __objc_allocate_instance(struct objc_class_gsv1* class)
{
    return nil;
}


id __objc_free_instance(id self)
{
    return nil;
}


static int sel_is_equal(SEL sel_1, SEL sel_2)
{
    if (sel_1 == 0 || sel_2 == 0) {
        return sel_1 == sel_2;
    }

    if (sel_1->name == sel_2->name) {
        return 1;
    }

    // FIXME...
    return 1;
}


void* objc_load_method(struct objc_class_gsv1* class, SEL selector)
{
    struct objc_method_list_gcc* methods = class->methods;

    if (methods == NULL) {
        while (1) ;
    }

    for (int i = 0; i < methods->count; ++i) {
        struct objc_method_gcc* method = &methods->methods[i];

        // printf("%s %s\n", selector->types, method->selector->types);

        // FIXME: this comparison doesn't work correctly yet!
        if (sel_is_equal(method->selector, selector)) {
            return method->imp;
        }
        // FIXME
        return method->imp;
    }
    while (1) ;
}


void debug_walk_class(struct objc_class_gsv1* class, int depth)
{
    printf("name: %s, info: %zu, inst_size: %zu, abi: %zu\n",
           class->name,
           class->info,
           class->instance_size,
           class->abi_version);

    if (class->methods) {
        struct objc_method_list_gcc* methods = class->methods;

        for (int i = 0; i < methods->count; ++i) {
            struct objc_method_gcc* method = &methods->methods[i];
            printf("method: %s %zu\n",
                   method->types ? method->types : "no types",
                   method->selector->index);
        }
    }

    if (class->isa && depth < 2) {
        debug_walk_class((struct objc_class_gsv1*)class->isa, depth + 1);
    }
}


id objc_msg_lookup(id receiver, SEL selector)
{
    Class c = receiver->class_pointer;

    debug_walk_class((struct objc_class_gsv1*)receiver, 0);

    // This is super lazy. We should be correctly doing message dispatch by
    // walking up the class hierarchy.
    return objc_load_method((struct objc_class_gsv1*)((struct objc_class_gsv1*)c)->isa, selector);
}


static void objc_load_class(struct objc_class_gsv1* class)
{
    for (int i = 0; i < CLASS_TABLE_SIZE; ++i) {
        if (classes_[i].class_ == NULL) {
            classes_[i].class_ = class;
            break;
        }
    }
}


void __objc_exec_class(struct objc_module_abi_8* module)
{
    struct objc_symbol_table_abi_8* symbols = module->symbol_table;

    if (module->version != 8) {
        while (1) ; // TODO: raise error for incompatible version... but main
                    // hasn't even been called yet, so how to signal the error
                    // to the user?
    }

    while (symbols->selector_count) ;

    void* defs = symbols->definitions;

    for (int i = 0; i < symbols->class_count; ++i) {
        objc_load_class(defs);
        ++defs;
    }

    // TODO: load categories
    while (symbols->category_count) ;

    for (int i = 0; i < symbols->category_count; ++i) {
        ++defs;
    }
}
