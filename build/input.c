#include "stdio.h"
#include "math.h"
#include "input.h"

static KeyMapping g_GameMappings[32];
static MouseMapping g_GameMouseMappings[2];

#define DEFAULT_MOUSE_SENSITIVITY 4.f

void InputInit(InputSystem* input){
    memset(input, 0, sizeof(InputSystem));
    input->currentMode = INPUT_MODE_GAME;
    InputLoadDefaultMappings(input);
    input->mouseState.sensitivity = DEFAULT_MOUSE_SENSITIVITY;
}

void InputUpdate(InputSystem* input, float dt){
   for (int i = 0; i < ACTION_COUNT; i++){
        input->actionJustPressed[i] = 0;
        input->actionJustReleased[i] = 0;
        input->actionPressed[i] = 0;
        input->actionMouseMoved[i] = 0.f;
    }
    /*
    input->mouseState.dx = 0.f;
    input->mouseState.dy = 0.f;
    */
    for (int i = 0; i < input->mappingCount; i++){
        KeyMapping* mapping = &input->mappings[i];
        KeyState* keyState = &input->keys[mapping->keyCode];
        int comboActive = keyState->isPressed;
        /* TODO: add modifiers support for actual combos */
        if (comboActive){
            input->actionPressed[mapping->action] = 1;
            if (keyState->justPressed){
                input->actionJustPressed[mapping->action] = 1;
            }
        }
        if (keyState->justReleased && input->actionPressed[mapping->action]
                == 0){
            input->actionJustReleased[mapping->action] = 1;    
        }
    }
    for (int i = 0; i < input->mouseMappingCount; i++){
        MouseMapping* mapping = &input->mouseMappings[i];
        MouseState* mouseState = &input->mouseState; 
        if (mapping->axis == 0){
            input->actionMouseMoved[mapping->action] = mouseState->dx;
        } else {
            input->actionMouseMoved[mapping->action] = mouseState->dy;
        }
    }
    for (int i = 0; i < MAX_KEYS; i++){
        input->keys[i].justPressed = 0;
        input->keys[i].justReleased = 0;
    }
    input->mouseState.dx = 0.f;
    input->mouseState.dy = 0.f;
}

void InputHandleKeyEvent(InputSystem* input, int keyCode, int isPressed){
    if (keyCode < 0 || keyCode >= MAX_KEYS){
        printf("key code out of bounds\n");
        return;
    }
    KeyState* key = &input->keys[keyCode];
    key->wasPressed = key->isPressed;
    key->isPressed = isPressed;
    key->justPressed = (!key->wasPressed && key->isPressed);
    key->justReleased = (key->wasPressed && !key->isPressed);
}

void InputHandleMouseEvent(InputSystem* input, float dx, float dy){
    input->mouseState.accumX += dx * input->mouseState.sensitivity;
    input->mouseState.accumY += dy * input->mouseState.sensitivity;
    input->mouseState.dx = floorf(input->mouseState.accumX);
    input->mouseState.dy = floorf(input->mouseState.accumY);
    input->mouseState.accumX -= input->mouseState.dx;
    input->mouseState.accumY -= input->mouseState.dy;
    /*
    input->mouseState.dx = dx;
    input->mouseState.dy = dy;
    */
}

void InputSetMode(InputSystem* input, InputMode mode){
    if (mode >= INPUT_MODE_COUNT){
        printf("invalid input mode\n");
        return;
    }
    input->currentMode = mode;
    switch (mode){
    case INPUT_MODE_GAME:{
        input->mappings = g_GameMappings;
        input->mappingCount = sizeof(g_GameMappings) /
            sizeof(g_GameMappings[0]);
        input->mouseMappings = g_GameMouseMappings;
        input->mouseMappingCount = sizeof(g_GameMouseMappings) /
            sizeof(g_GameMouseMappings[0]);
    } break;
    }
}

void InputAddMapping(InputSystem* input, int keyCode, int modifiers,
        InputAction action){
    if (input->mappingCount >= 32){
        printf("Can't add action mapping, out of space\n");
        return;
    }
    KeyMapping* mapping = &input->mappings[input->mappingCount];
    mapping->keyCode = keyCode;
    mapping->modifiers = modifiers;
    mapping->action = action;
    input->mappingCount++;
}

void InputAddMouseMapping(InputSystem* input, int axis, InputAction action){
    MouseMapping* mapping = &input->mouseMappings[input->mouseMappingCount];
    mapping->axis = axis;
    mapping->action = action;
    input->mouseMappingCount++;
}

void InputClearMappings(InputSystem* input){
    input->mappingCount = 0;
    input->mouseMappingCount = 0;
}

void InputLoadDefaultMappings(InputSystem* input){
    InputClearMappings(input);
    if (input->currentMode == INPUT_MODE_GAME){
        input->mappings = g_GameMappings;
        input->mouseMappings = g_GameMouseMappings;
        input->mappingCount = 0;
        InputAddMapping(input, 'A', 0, ACTION_CAM_TRANS_L_X_MINUS);
        InputAddMapping(input, 'D', 0, ACTION_CAM_TRANS_L_X_PLUS);
        InputAddMapping(input, 'E', 0, ACTION_CAM_TRANS_L_Y_MINUS);
        InputAddMapping(input, 'Q', 0, ACTION_CAM_TRANS_L_Y_PLUS);
        InputAddMapping(input, 'W', 0, ACTION_CAM_TRANS_L_Z_MINUS);
        InputAddMapping(input, 'S', 0, ACTION_CAM_TRANS_L_Z_PLUS);
        InputAddMouseMapping(input, 0, ACTION_CAM_ROT_L_X);
        InputAddMouseMapping(input, 1, ACTION_CAM_ROT_L_Y);
        InputAddMapping(input, 'M', 0, ACTION_CAM_ROT_L_Y_MINUS);
        InputAddMapping(input, 'B', 0, ACTION_CAM_ROT_L_Y_PLUS);
        InputAddMapping(input, 'H', 0, ACTION_CAM_ROT_L_X_MINUS);
        InputAddMapping(input, 'N', 0, ACTION_CAM_ROT_L_X_PLUS);
        /*
        InputAddMapping(input, 'V', 0, ACTION_CAM_ROT_SNAP_L_Y_MINUS);
        */
    }
}

int InputIsActionPressed(InputSystem* input, InputAction action){
    if (action >= ACTION_COUNT){
        printf("querying the state of an invalid action\n");
        return 0;
    }
    return input->actionPressed[action];
}

int InputIsActionJustPressed(InputSystem* input, InputAction action){
    if (action >= ACTION_COUNT){
        printf("querying the state of an invalid action\n");
        return 0;
    }
    return input->actionJustPressed[action]; 
}

int InputIsActionJustReleased(InputSystem* input, InputAction action){
    if (action >= ACTION_COUNT){
        printf("querying the state of an invalid action\n");
        return 0;
    }
    return input->actionJustReleased[action]; 
}

float InputIsActionMouseMoved(InputSystem* input, InputAction action){
     if (action >= ACTION_COUNT){
        printf("querying the state of an invalid action\n");
        return 0;
    }
    return input->actionMouseMoved[action];   
}



