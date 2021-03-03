#include </opt/devkitpro/devkitARM/lib/gcc/arm-none-eabi/10.2.0/include/objc/objc.h>

typedef struct objc_object
{
	/**
	 * Pointer to this object's class.  Accessing this directly is STRONGLY
	 * discouraged.  You are recommended to use object_getClass() instead.
	 */
#ifndef __OBJC_RUNTIME_INTERNAL__
	__attribute__((deprecated))
#endif
	Class isa;
} *id;
