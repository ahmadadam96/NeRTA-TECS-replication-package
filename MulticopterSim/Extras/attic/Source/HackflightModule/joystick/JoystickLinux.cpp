/*
 * Linux support for joysticks and other game controllers
 *
 * Copyright (C) 2021 Simon D. Levy
 *
 * MIT License
 */


#ifdef __linux__

#include "MainModule/joystick/Joystick.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#ifndef debug
#define debug printf
#endif

static char _productName[128];

IJoystick::IJoystick(const char * devname)
{
    _joystickId = open(devname, O_RDONLY);

    if (_joystickId <= 0) return;

    fcntl(_joystickId, F_SETFL, O_NONBLOCK);

    *_productName = 0;

    if (ioctl(_joystickId, JSIOCGNAME(sizeof(_productName)), _productName) < 0) {
        return;
    }

    if (strstr(_productName, "Taranis") || strstr(_productName, "DeviationTx Deviation GamePad")) {
        _productId = PRODUCT_TARANIS;
    }
    else if (strstr(_productName, "Horizon Hobby SPEKTRUM")) {
        _productId = PRODUCT_SPEKTRUM;
    }
    else if (strstr(_productName, "GREAT PLANES InterLink Elite")) {
        _productId = PRODUCT_INTERLINK;
    }
    else if (strstr(_productName, "Extreme 3D")) {
        _productId = PRODUCT_EXTREMEPRO3D;
        _isGameController = true;
    }
    else if (strstr(_productName, "Generic X-Box pad")) {
        _productId = PRODUCT_XBOX360_CLONE;
        _isGameController = true;
    }
    else if (strstr(_productName, "Logitech Logitech Dual Action")) {
        _productId = PRODUCT_F310;
        _isGameController = true;
    }
    else if (strstr(_productName, "Xbox 360 Wireless Receiver")) {
        _productId = PRODUCT_XBOX360;
        _isGameController = true;
    }
    else if (strstr(_productName, "Microsoft X-Box 360 pad")) {
        _productId = PRODUCT_XBOX360;
        _isGameController = true;
    }

}

// Convert InterLink aux switches to unique gamepad buttons
static void getAuxInterlink(float * axes, uint8_t number, uint8_t value, uint8_t aux1, uint8_t aux2, float auxMid)
{
    if (number == 0) {
        axes[aux2] = (float)!value;
    }

    if (number == 3 && value == 1) {
        axes[aux1] = -1.0;
    }

    if ((number == 3 || number == 4) && value == 0) {
        axes[aux1] = auxMid;
    }

    if (number == 4 && value == 1) {
        axes[aux1] = 1.0;
    }
}

uint16_t IJoystick::pollProduct(float axes[6], uint8_t & buttons)
{
    // ------------------------------- 0       1       2       3       4       5       6       7 -----
    static uint8_t F310_MAP[8]      = {AX_YAW, AX_THR, AX_ROL, AX_PIT, AX_NIL, AX_NIL, AX_NIL, AX_NIL};
    static uint8_t SPEKTRUM_MAP[8]  = {AX_YAW, AX_THR, AX_ROL, AX_PIT, AX_AU2, AX_NIL, AX_AU1, AX_NIL};
    static uint8_t XBOX360_MAP[8]   = {AX_YAW, AX_THR, AX_NIL, AX_ROL, AX_PIT, AX_NIL, AX_NIL, AX_NIL};
    static uint8_t INTERLINK_MAP[8] = {AX_ROL, AX_PIT, AX_THR, AX_NIL, AX_YAW, AX_AU1, AX_NIL, AX_NIL};

    if (_joystickId <= 0) return 0xFFFF;

    struct js_event js;

    read(_joystickId, &js, sizeof(struct js_event));

    if (js.type & JS_EVENT_INIT) return 0;

    uint8_t * axisMap = NULL;

    switch (_productId) {

        case PRODUCT_F310:
            axisMap = F310_MAP;
            break;

        case PRODUCT_SPEKTRUM:
            axisMap = SPEKTRUM_MAP;
            break;

        case PRODUCT_XBOX360:
            axisMap = XBOX360_MAP;
            break;

        case PRODUCT_INTERLINK:
            axisMap = INTERLINK_MAP;
            break;

        default:
            debug("JOYSTICK '%s' NOT RECOGNIZED\n", _productName);
            return 0xFFFF; // dummy value
    }

    static float _axes[6];

    // Initalize aux switches
    if (!_axes[AX_AU1]) {
        _axes[AX_AU1] = -1;
        _axes[AX_AU2] = -1;
    }

    switch (js.type) {

        case JS_EVENT_AXIS: 
            _axes[axisMap[js.number]] = js.value / 32768.f;
            break;

        case JS_EVENT_BUTTON:
            if (_productId == PRODUCT_INTERLINK)  {
                getAuxInterlink(_axes, js.number, js.value, AX_AU1, AX_AU2, AUX1_MID);
            }
            else {
                buttons = js.value ? 1 << js.number : 0; // for Windows compatibility
            }
            break;
    }

    for (uint8_t k=0; k<6; ++k) {
        axes[k] = _axes[k];
    }

    return 0x0000; // no error
}  

#endif
