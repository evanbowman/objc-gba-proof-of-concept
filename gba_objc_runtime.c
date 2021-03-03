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

/* static struct ClassTableEntry { */
/*     struct objc_class_gsv1* class_; */
/* } classes_[CLASS_TABLE_SIZE]; */


typedef struct TinyObjcClass {
    struct TinyObjcClass* class_;
    struct TinyObjcClass* super_class_;
    struct objc_method_list_gcc* method_list_;
    const char* name_;
    unsigned char resolved_;
} TinyObjcClass;


static struct TinyObjcClassTable {
    TinyObjcClass* class_;
} tinyobjc_classes[CLASS_TABLE_SIZE];


const char* class_name(struct objc_class_gsv1* class)
{
    return ((struct objc_class_gsv1*)class->isa)->name;
}


static TinyObjcClass* tinyobjc_get_class(const char* name)
{
    for (int i = 0; i < CLASS_TABLE_SIZE; ++i) {
        printf("in table... %s\n", tinyobjc_classes[i].class_->name_);
        if (strcmp(name, tinyobjc_classes[i].class_->name_) == 0) {
            return tinyobjc_classes[i].class_;
        }
    }

    return NULL;
}


static void tinyobjc_resolve_class(TinyObjcClass* class)
{
    if (class->resolved_) {
        return;
    }

    printf("%s is unresolved\n", class->name_);

    printf("fetching base class %s\n", (const char*)class->super_class_);

    // The unresolved class could be a metaclass, in which case, we want to just
    // go ahead and resolve the whole class chain, so we don't need complicated
    // logic to figure out whether we're a metaclass or not, to link to the
    // correct metaclass in the parallel superclass meta chain.
    class = tinyobjc_get_class(class->name_);

    TinyObjcClass* super = tinyobjc_get_class((const char*)class->super_class_);

    if (!super) {
        // Hmm... what? The linker should have raised an error...
        while (1) ;
    }

    class->super_class_ = super;
    class->class_->super_class_ = super->class_;

    class->resolved_ = 1;
}


id objc_get_class(const char* name)
{
    return (id)tinyobjc_get_class(name);
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


/* static void* objc_load_method_slow(struct objc_class_gsv1* class, SEL selector) */
/* { */
/*     while (1) { */
/*         struct objc_class_gsv1* meta = (struct objc_class_gsv1*)class->isa; */

/*         if (meta->methods) { */
/*             struct objc_method_list_gcc* methods = meta->methods; */

/*             for (int i = 0; i < methods->count; ++i) { */
/*                 struct objc_method_gcc* method = &methods->methods[i]; */

/*                 if (type_compare(method->types, selector->types) && */
/*                     strcmp(selector->name, ((char*)&method->selector->index)) == 0) { */
/*                     return method->imp; */
/*                 } */
/*             } */
/*         } */

/*         puts("checking super class"); */
/*         if (class->super_class) { */
/*             // NOTE: the ABI stores a string classname in the super_class field. */
/*             id super = (id)objc_get_class((const char*)class->super_class); */
/*             class = (struct objc_class_gsv1*)super->class_pointer; */
/*         } else { */
/*             puts("has no superclass?!"); */
/*             return nil; */
/*         } */
/*     } */
/* } */


/* void debug_walk_class(struct objc_class_gsv1* class, int depth) */
/* { */
/*     puts("here"); */
/*     printf("name: %s, info: %zu, inst_size: %zu, abi: %zu, super: %s\n", */
/*            class->name, */
/*            class->info, */
/*            class->instance_size, */
/*            class->abi_version, */
/*            (const char*)class->super_class); */

/*     if (class->methods) { */
/*         struct objc_method_list_gcc* methods = class->methods; */

/*     WALK_METHODS: */
/*         for (int i = 0; i < methods->count; ++i) { */
/*             struct objc_method_gcc* method = &methods->methods[i]; */

/*             printf("method: %s %s\n", */
/*                    method->types ? method->types : "no types", */
/*                    ((char*)&method->selector->index)); */
/*         } */

/*         if (methods->next) { */
/*             methods = methods->next; */
/*             goto WALK_METHODS; */
/*         } */
/*     } */

/*     puts(""); */

/*     if (class->isa && depth < 2) { */
/*         debug_walk_class((struct objc_class_gsv1*)class->isa, depth + 1); */
/*     } */
/* } */


static void* objc_load_method_slow(TinyObjcClass* class, SEL selector)
{
    while (1) {
        puts("here");
        if (class->method_list_) {
            struct objc_method_list_gcc* methods = class->method_list_;

            for (int i = 0; i < methods->count; ++i) {
                struct objc_method_gcc* method = &methods->methods[i];
                printf("checking method %s %s\n",
                       selector->name,
                       (char*)&method->selector->index);
                if (type_compare(method->types, selector->types) &&
                    strcmp(selector->name, ((char*)&method->selector->index)) == 0) {
                    return method->imp;
                }
            }
        }

        if (class->super_class_) {
            class = class->super_class_;
        } else {
            puts("has no superclass?!");
            return nil;
        }
    }
}


id objc_msg_lookup(id receiver, SEL selector)
{
    Class class = receiver->class_pointer;

    tinyobjc_resolve_class((TinyObjcClass*)class);

    printf("looking up method in %s %p\n",
           ((TinyObjcClass*)class)->name_,
           class);

    return objc_load_method_slow((TinyObjcClass*)class, selector);
}


/* name: (null), info: 0, inst_size: 94667112882204, abi: 1 */

/* name: HelloWorld, info: 1, inst_size: 8, abi: 0 */

/* name: HelloWorld, info: 2, inst_size: 104, abi: 0 */
/* method: v24@0:8r*16worldworld: Obje */
/* method: v24@0:8r*16hello:      worl */


/* name: HelloWorld, info: 1, inst_size: 8, abi: 0 */
/* method: v16@0:8 test */

/* here */
/* name: HelloWorld, info: 2, inst_size: 104, abi: 0 */
/* method: @24@0:8r*16 world: */
/* method: @24@0:8r*16 hello: */


static TinyObjcClass* tinyobjc_make_class(struct objc_class_gsv1* class)
{
    TinyObjcClass* new = malloc(sizeof(TinyObjcClass));

    new->class_ = malloc(sizeof(TinyObjcClass));
    new->class_->name_ = ((struct objc_class_gsv1*)class->isa)->name;

    // The GCC ABI stores superclasses as strings. We will do the same, and
    // resolve the pointers to the actual classes after loading everything.
    new->class_->super_class_ = (TinyObjcClass*)
        ((struct objc_class_gsv1*)class->isa)->super_class;

    new->class_->method_list_ =
        ((struct objc_class_gsv1*)((struct objc_class_gsv1*)class->isa)->isa)->methods;
    new->class_->class_ = NULL;
    new->class_->resolved_ = 0;


    new->name_ = new->class_->name_;
    new->super_class_ = new->class_->super_class_;
    new->method_list_ = ((struct objc_class_gsv1*)class->isa)->methods;
    new->resolved_ = 0;

    new->super_class_ =
        (TinyObjcClass*)
        ((struct objc_class_gsv1*)class->isa)->super_class;

    return new;
}






/* // The GCC ABI stores superclass pointers as string names. After loading all of */
/* // the classes, we need to scan through the class table, and link classes with */
/* // their superclasses. */
/* static void tinyobjc_resolve_class(TinyObjcClass* class) */
/* { */
/*     if (class->resolved_) { */
/*         return; */
/*     } */


/*     for (int i = 0; i < CLASS_TABLE_SIZE; ++i) { */
/*         if (tinyobjc_classes[i].class_) { */

/*             TinyObjcClass* class = tinyobjc_classes[i].class_; */

/*             if (class->super_class_) { */
/*                 TinyObjcClass* super = */
/*                     tinyobjc_get_class((const char*)class->super_class_); */

/*                 if (super) { */
/*                     class->super_class_ = super; */
/*                     // Link the metaclass in a parallel chain. */
/*                     class->class_->super_class_ = super->class_; */
/*                 } */
/*             } */
/*         } */
/*     } */
/* } */


static void objc_load_class(struct objc_class_gsv1* class)
{
    for (int i = 0; i < CLASS_TABLE_SIZE; ++i) {
        if (tinyobjc_classes[i].class_ == NULL) {
            tinyobjc_classes[i].class_ = tinyobjc_make_class(class);
            return;
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
