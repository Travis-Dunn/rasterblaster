#ifndef INPUT_H
#define INPUT_H

#include "string.h"
#include "event.h"

#define MAX_KEYS 256

typedef enum {
    ACTION_NONE = 0,
    ACTION_CAM_TRANS_G_X_MINUS,
    ACTION_CAM_TRANS_G_X_PLUS,
    ACTION_CAM_TRANS_G_Y_MINUS,
    ACTION_CAM_TRANS_G_Y_PLUS,
    ACTION_CAM_TRANS_G_Z_MINUS,
    ACTION_CAM_TRANS_G_Z_PLUS,
    ACTION_CAM_TRANS_L_X_MINUS,
    ACTION_CAM_TRANS_L_X_PLUS,
    ACTION_CAM_TRANS_L_Y_MINUS,
    ACTION_CAM_TRANS_L_Y_PLUS,
    ACTION_CAM_TRANS_L_Z_MINUS,
    ACTION_CAM_TRANS_L_Z_PLUS,
    ACTION_CAM_ROT_G_X_MINUS,
    ACTION_CAM_ROT_G_X_PLUS,
    ACTION_CAM_ROT_G_Y_MINUS,
    ACTION_CAM_ROT_G_Y_PLUS,
    ACTION_CAM_ROT_G_Z_MINUS,
    ACTION_CAM_ROT_G_Z_PLUS,
    ACTION_CAM_ROT_L_X_MINUS,
    ACTION_CAM_ROT_L_X_PLUS,
    ACTION_CAM_ROT_L_Y_MINUS,
    ACTION_CAM_ROT_L_Y_PLUS,
    ACTION_CAM_ROT_L_Z_MINUS,
    ACTION_CAM_ROT_L_Z_PLUS,
    ACTION_CAM_ROT_L_X,
    ACTION_CAM_ROT_L_Y,
    ACTION_CAM_ROT_SNAP_L_Y_MINUS,
    ACTION_COUNT
} InputAction;

typedef enum {
    INPUT_MODE_NONE = 0,
    INPUT_MODE_GAME,
    INPUT_MODE_COUNT
} InputMode;

typedef struct {
    int isPressed;
    int wasPressed;
    int justPressed;
    int justReleased;
} KeyState;

typedef struct {
    float dx, dy;
    float accumX, accumY;
    float sensitivity;
} MouseState;

typedef struct {
    int keyCode;
    int modifiers;
    InputAction action;
} KeyMapping;

typedef struct {
    int axis;
    InputAction action;
} MouseMapping;

typedef struct {
    KeyState keys[MAX_KEYS];
    InputMode currentMode;
    KeyMapping* mappings;
    MouseMapping* mouseMappings;
    int mappingCount;
    int mouseMappingCount;

    MouseState mouseState;

    /* these arrays are always oversized because of adding the mouse move */
    int actionPressed[ACTION_COUNT];
    int actionJustPressed[ACTION_COUNT];
    int actionJustReleased[ACTION_COUNT];
    float actionMouseMoved[ACTION_COUNT];
} InputSystem;

void InputInit(InputSystem* input);
void InputUpdate(InputSystem* input, float dt);
void InputHandleKeyEvent(InputSystem* input, int keyCode, int isPressed);
void InputHandleMouseEvent(InputSystem* input, float dx, float dy);
void InputSetMode(InputSystem* input, InputMode mode);
void InputAddMapping(InputSystem* input, int keyCode, int modifiers,
        InputAction action);
void InputAddMouseMapping(InputSystem* input, int axis, InputAction action);
void InputClearMappings(InputSystem* input);
void InputLoadDefaultMappings(InputSystem* input);
int InputIsActionPressed(InputSystem* input, InputAction action);
int InputIsActionJustPressed(InputSystem* input, InputAction action);
int InputIsActionJustReleased(InputSystem* input, InputAction action);
float InputIsActionMouseMoved(InputSystem* input, InputAction action);

#endif /* INPUT_H */
