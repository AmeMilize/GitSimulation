#include "../Public/WindowsSettings.h"

_CRT_BEGIN_C_HEADER
void SetWindowFontRepresentColor(EWindowsFontColor FontColor, EWindowsFontColor BackgroundColor)
{
	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(Handle, (unsigned short)FontColor + (unsigned short)BackgroundColor * 0x10);
}
_CRT_END_C_HEADER