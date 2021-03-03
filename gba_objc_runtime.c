#include "module.h"
#include "class.h"
#include "selector.h"
#include "method.h"
#include <string.h>
#include <stdlib.h>
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


id __objc_allocate_instance(id class)
{
    // FIXME: actually calculate the instance size from the class.
    id mem = malloc(100);
    mem->class_pointer = (Class)class;
    return mem;
}


id __objc_free_instance(id self)
{
    free(self);
    return nil;
}


static int type_compare(const char* lhs, const char* rhs)
{
    return lhs == rhs || strcmp(lhs, rhs) == 0;
}


static void* objc_load_method_slow(struct objc_class_gsv1* class, SEL selector)
{
    while (1) {
        struct objc_class_gsv1* meta = (struct objc_class_gsv1*)class->isa;

        if (meta->methods) {
            struct objc_method_list_gcc* methods = meta->methods;

            for (int i = 0; i < methods->count; ++i) {
                struct objc_method_gcc* method = &methods->methods[i];

                if (type_compare(method->types, selector->types) &&
                    strcmp(selector->name, ((char*)&method->selector->index)) == 0) {
                    return method->imp;
                }
            }
        }

        puts("checking super class");
        if (class->super_class) {
            // NOTE: the ABI stores a string classname in the super_class field.
            id super = (id)objc_get_class((const char*)class->super_class);
            class = (struct objc_class_gsv1*)super->class_pointer;
        } else {
            puts("has no superclass?!");
            return nil;
        }
    }
}


id objc_msg_lookup(id receiver, SEL selector)
{
    printf("msg lookup starting at %p\n", receiver);
    Class class = receiver->class_pointer;
    printf("class is %p\n", class);

    return objc_load_method_slow((struct objc_class_gsv1*)class, selector);
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
