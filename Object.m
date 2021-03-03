#include </opt/devkitpro/devkitARM/lib/gcc/arm-none-eabi/10.2.0/include/objc/objc.h>
#import "Object.h"


@implementation Object

+ (id)alloc
{
    return nil; // __objc_allocate_instance(self);
}


- (id)free
{
    return nil; //__objc_free_instance(self);
}

@end
