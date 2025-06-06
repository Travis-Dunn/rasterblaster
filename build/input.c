#include "stdio.h"
#include "input.h"

static KeyMapping g_GameMappings[32];

void InputInit(InputSystem* input){
    memset(input, 0, sizeof(InputSystem));
    input->currentMode = INPUT_MODE_GAME;
    InputLoadDefaultMappings(input);
}

void InputUpdate(InputSystem* input, float dt){
    for (int i = 0; i < MAX_KEYS; i++){
        input->keys[i].justPressed = 0;
        input->keys[i].justReleased = 0;
    }
    for (int i = 0; i < ACTION_COUNT; i++){
        input->actionJustPressed[i] = 0;
        input->actionJustReleased[i] = 0;
        input->actionPressed[i] = 0;
    }
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

void InputClearMappings(InputSystem* input){
    input->mappingCount = 0;
}

void InputLoadDefaultMappings(InputSystem* input){
    InputClearMappings(input);
    if (input->currentMode == INPUT_MODE_GAME){
        input->mappings = g_GameMappings;
        input->mappingCount = 0;
        InputAddMapping(input, 'A', 0, ACTION_CAM_TRANS_X);
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
