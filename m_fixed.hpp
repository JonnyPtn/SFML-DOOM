#pragma once

//
// Fixed point, 32bit as 16.16.
//
#define FRACBITS		16
#define FRACUNIT		(1<<FRACBITS)

int FixedMul	(int a, int b);
int FixedDiv	(int a, int b);
int FixedDiv2	(int a, int b);