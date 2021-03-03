#import "Object.h"
#include <stdio.h>


@interface HelloWorld:Object {
    int field_;
}
+(id) hello: (const char*) param;
+(id) world: (const char*) param;
-(id) test1;
-(void) test;
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

-(void) test {
    printf("%d\n", field_);
    puts("test");
}
@end


int main()
{
    // [[HelloWorld hello: "some text"] world: "some more text!"];

    [[[HelloWorld alloc] test1] test];
}
