#include "module.h"
#include "class.h"
#include "selector.h"
#include "method.h"
#include <string.h>


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


void* objc_load_method(struct objc_class_gsv1* class, SEL selector)
{
    struct objc_method_list_gcc* methods = class->methods;

    if (methods == NULL) {
        while (1) ;
    }

    for (int i = 0; i < methods->count; ++i) {
        struct objc_method_gcc* method = &methods->methods[i];

        // FIXME: this comparison doesn't work correctly yet!
        if (((struct objc_selector*)method->selector)->index ==
            ((struct objc_selector*)selector)->index) {
            return method->imp;
        }
        // FIXME
        return method->imp;
    }
    while (1) ;
}


id objc_msg_lookup(id receiver, SEL selector)
{
    Class c = receiver->class_pointer;

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

    void* defs = symbols->definitions;

    for (int i = 0; i < symbols->class_count; ++i) {
        objc_load_class(defs);
        ++defs;
    }

    for (int i = 0; i < symbols->category_count; ++i) {
        ++defs;
    }
}
