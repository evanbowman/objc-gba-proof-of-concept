#import "Object.h"
#include <stdio.h>


@interface HelloWorld:Object {
    int field_;
}
+(id) hello: (const char*) param;
+(id) world: (const char*) param;
-(id) test1;
-(id) test;
@end


@implementation HelloWorld
+(id) hello: (const char*) param {
    printf("hello %s\n", param);
    return self;
}

+(id) world: (const char*) param {
    printf("world %s\n", param);
    return self;
}

-(id) test1 {
    field_ = 42;

    return self;
}

-(id) test {
    printf("%d\n", field_);
    puts("test");
    return self;
}
@end


int main()
{
    id hw = [[[HelloWorld alloc] test1] test];

    [HelloWorld hello: "test"];

    [hw free];

    return 0;
}
