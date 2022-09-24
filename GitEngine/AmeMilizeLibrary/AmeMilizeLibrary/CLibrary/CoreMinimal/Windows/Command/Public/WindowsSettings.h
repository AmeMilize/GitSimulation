#pragma once

#include "../../../Core/CoreMinimal.h"

_CRT_BEGIN_C_HEADER
typedef enum
{
	BLACK = 0,
	BLUE,
	GREEN,
	LIGHT_BLUE,
	RED,
	VIOLET,
	YELLOW,
	WHITE,
	GREY,
	PALE_BULE,
	PALE_GREEN,
	LIGHT_GREEN,
	PALE_RED,
	LAVENDER,
	CANARY_YELLOW,
	BRIGHT_WHITE
} EWindowsFontColor;

void SetWindowFontRepresentColor(EWindowsFontColor FontColor, EWindowsFontColor BackColor);
_CRT_END_C_HEADER