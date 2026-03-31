#pragma once
#include "GameRegistry.h"

#define REGISTER_GAME(gameObj) __attribute__((constructor)) void autoRegister_##gameObj() {registerGame(&gameObj);}