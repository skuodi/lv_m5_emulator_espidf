#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lvgl_port_m5stack.hpp"

#if defined(ESP_PLATFORM)
#include "M5Unified.h"
#endif

extern void user_app(void);

M5GFX gfx;
#if (__has_include(<SDL2/SDL.h>) || __has_include(<SDL.h>))
void setup(void) {
    gfx.init();

    lvgl_port_init(gfx);

    user_app();
}

void loop(void) {
    usleep(10 * 000);
}

#elif defined(ESP_PLATFORM)

extern "C" void app_main()
{
    M5.begin();
    gfx.init();
    
    lvgl_port_init(gfx);
    
    // ARDUINO_M5STACK_FIRE and ARDUINO_M5STACK_Fire are defined for M5Fire
    // even when Arduino framework is not used
#if defined ( ARDUINO_M5STACK_FIRE ) || defined ( ARDUINO_M5STACK_Fire )
    // When an lvgl object is created, it is automatically added to the default
    // group if one is set. So here we set one so that we can send button press
    // input to the group to implement navigation. See src/utility/lvgl_port_m5stack.cpp
    lv_group_set_default(lv_group_create());
#endif

    // Call the function that creates your UI 
    user_app();
    
    for(;;)
    {
        // Infinite loop goes here
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

#endif
