#import "Object.h"
#include <stdio.h>
#include </opt/devkitpro/libtonc/include/tonc_tte.h>
#include </opt/devkitpro/libtonc/include/tonc_video.h>
#include </opt/devkitpro/libtonc/include/tonc_irq.h>


@interface HelloWorld:Object {
}
+(void) hello: (const char*) param;
@end


@implementation HelloWorld
+(void) hello: (const char*) param {

    irq_init(NULL);
    irq_add(II_VBLANK, NULL);
    REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;

    tte_init_se(
        0,
        BG_CBB(0)|BG_SBB(31),
        0,
        CLR_YELLOW,
        14,
        NULL,
        NULL);

    pal_bg_bank[1][15]= CLR_RED;
    pal_bg_bank[2][15]= CLR_GREEN;
    pal_bg_bank[3][15]= CLR_BLUE;
    pal_bg_bank[4][15]= CLR_WHITE;
    pal_bg_bank[5][15]= CLR_MAG;
    pal_bg_bank[4][14]= CLR_GRAY;

    tte_write(" #{cx:0x1000}Hello from Objc\n");
    tte_write(param);

    key_wait_till_hit(KEY_ANY);
}
@end


int main()
{
    [HelloWorld hello:"some text"];
}
