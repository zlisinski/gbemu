#pragma once

#include <stdio.h>
#include <stdint.h>

#include "Exceptions.h"
#include "Utils.h"

// CPU speed in Hz.
const int CLOCKS_PER_SECOND = 4194304;

// There are 4 clocks per instruction cycle.
const int CLOCKS_PER_CYCLE = 4;