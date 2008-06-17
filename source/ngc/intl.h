/*
 * This file holds collections of strings to make localization easier.
 * Keep the max string length to 30 characters or bad things will happen
 */
#define MENU_STRING_LENGTH      30
#define MENU_SNES9X_TITLE      "Snes9x 1.43 - GX Edition v0.1.2beta1"

#ifdef INTL_SPANISH
#include "intl/spanish.h"
#elif defined INTL_GERMAN
#include "intl/german.h"
#elif defined INTL_DUTCH
#include "intl/dutch.h"
#elif defined INTL_FRENCH
#include "intl/french.h"
#else
#include "intl/english.h"
#endif

