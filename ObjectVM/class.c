//
//  class.c
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-20.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "class.h"

#include "object.h"
#include "primitive_table.h"
#include "vm.h"
#include "block.h"
#include "str.h"

#include <stdlib.h>
#include <memory.h>
#include <string.h>

struct class
{
    struct class* isa;
    object* super;
    primitive_table* ivars;

    primitive_table* instanceMethods;
    primitive_table* classMethods;
    char* name;
};

class* class_init(clockwork_vm* vm, char* name, char* superclass)
{
    class* klass = vm_allocate(vm, sizeof(class));

    if (superclass && strlen(superclass) > 0)
    {
        object* sup = vm_getConstant(vm, superclass);
        klass->super = sup;
    }

    klass->isa = klass;
    klass->name = vm_allocate(vm, strlen(name));
    strcpy(klass->name, name);

    return klass;
}

void class_addInstanceMethod(class* klass, clockwork_vm* vm, char* name, block* meth)
{
    if (klass->instanceMethods == NULL)
    {
        klass->instanceMethods = primitive_table_init(vm, 16);
    }

#warning GROW INSTANCE METHOD TABLE AS NEEDED.

    primitive_table_set(klass->instanceMethods, vm, name, (object*)meth);
}

void class_addClassMethod(class* klass, clockwork_vm* vm, char* name, block* meth)
{
    if (klass->classMethods == NULL)
    {
        klass->classMethods = primitive_table_init(vm, 16);
    }

#warning GROW CLASS METHOD TABLE AS NEEDED.

    primitive_table_set(klass->classMethods, vm, name, (object*)meth);
}

char* class_name(class* klass, clockwork_vm* vm)
{
    return klass->name;
}

block* class_getClassMethod(class* klass, clockwork_vm* vm, char* selector)
{
    if (klass->classMethods)
    {
        return (block*)primitive_table_get(klass->classMethods, vm, selector);
    }

    return NULL;
}

block* class_getInstanceMethod(class* klass, clockwork_vm* vm, char* selector)
{
    if (klass->instanceMethods)
    {
        return (block*)primitive_table_get(klass->instanceMethods, vm, selector);
    }
    
    return NULL;
}

block* class_findClassMethod(class* klass, clockwork_vm* vm, char* selector)
{
    block* m = NULL;
    if (klass)
    {
        m = klass->classMethods ? (block*)primitive_table_get(klass->classMethods, vm, selector) : NULL;
        if (!m)
        {
            m = class_findClassMethod((class*)klass->super, vm, selector);
        }
    }

    return m;
}
