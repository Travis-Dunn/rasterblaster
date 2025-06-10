#ifndef INPUT_H
#define INPUT_H

#include "string.h"
#include "event.h"

#define MAX_KEYS 256

typedef enum {
    ACTION_NONE = 0,
    ACTION_CAM_TRANS_X_L,
    ACTION_CAM_TRANS_X_R,
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
    int keyCode;
    int modifiers;
    InputAction action;
} KeyMapping;

typedef struct {
    KeyState keys[MAX_KEYS];
    InputMode currentMode;
    KeyMapping* mappings;
    int mappingCount;

    int actionPressed[ACTION_COUNT];
    int actionJustPressed[ACTION_COUNT];
    int actionJustReleased[ACTION_COUNT];
} InputSystem;

void InputInit(InputSystem* input);
void InputUpdate(InputSystem* input, float dt);
void InputHandleKeyEvent(InputSystem* input, int keyCode, int isPressed);
void InputSetMode(InputSystem* input, InputMode mode);
void InputAddMapping(InputSystem* input, int keyCode, int modifiers,
        InputAction action);
void InputClearMappings(InputSystem* input);
void InputLoadDefaultMappings(InputSystem* input);
int InputIsActionPressed(InputSystem* input, InputAction action);
int InputIsActionJustPressed(InputSystem* input, InputAction action);
int InputIsActionJustReleased(InputSystem* input, InputAction action);

#endif /* INPUT_H */
