/*
This program is free software: you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "XCWrite.h"
#include "Common.h"
#include "Conversion.h"
#include <stdio.h>
#include <stdint.h>


void SetFloat(const char* dataRef, float value)
{
	XPLMDataRef intvar;
	intvar = XPLMFindDataRef(dataRef);

	XPLMSetDataf(intvar, value);
}

void SetFloat(const char* dataRef, float value, int index)
{
	XPLMDataRef intvar;
	intvar = XPLMFindDataRef(dataRef);

	XPLMSetDatavf(intvar, &value, index, 1);
}

void SetInt(const char* dataRef, int value)
{
	XPLMDataRef intvar;
	intvar = XPLMFindDataRef(dataRef);

	XPLMSetDatai(intvar, value);
}

void WriteVariable(BYTE* source, DWORD offset, DWORD size)
{

	FILE* str = fopen("FDSConnectX.out", "a+");

	if(offset==0x0330)
	{
		int16_t val;
		CopyMemory(&val, source, size);
		float fVal = HPAToInch(val / 16.0f);
		SetFloat("sim/cockpit/misc/barometer_setting", fVal);
	}
	else if(offset==0x088C)
	{
		int16_t val;
		CopyMemory(&val, source, size);

		float fVal = val;
		fVal /= 16384.0;

		SetInt("sim/operation/override/override_throttles", 1);
		SetFloat("sim/flightmodel/engine/ENGN_thro_use", (float)fVal, 0);
		SetInt("sim/operation/override/override_throttles", 0);
	}
	else if(offset==0x0924)
	{
		int16_t val;
		CopyMemory(&val, source, size);

		float fVal = val;
		fVal /= 16384.0;

		SetInt("sim/operation/override/override_throttles", 1);
		SetFloat("sim/flightmodel/engine/ENGN_thro_use", (float)fVal, 1);
		SetInt("sim/operation/override/override_throttles", 0);
	}
	else if(offset==0x0BB2)
	{
		if(PitchDisconnect)
		{
			SetInt("sim/operation/override/override_joystick_pitch", 1);

			int16_t val;
			CopyMemory(&val, source, size);
			float ratio = val / 16383.0f;
			SetFloat("sim/joystick/yoke_pitch_ratio", ratio);
		}
		else
			SetInt("sim/operation/override/override_joystick_pitch", 0);
	}
	else if(offset==0x0BB6)
	{
		if(RollDisconnect)
		{
			SetInt("sim/operation/override/override_joystick_roll", 1);

			int16_t val;
			CopyMemory(&val, source, size);
			float ratio = val / 16383.0f;
			SetFloat("sim/joystick/yoke_roll_ratio", ratio);
		}
		else
		{
			SetInt("sim/operation/override/override_joystick_roll", 0);
		}
	}
	
	else if(offset==0x0C4E)
	{
		int16_t val;
		CopyMemory(&val, source, size);
		SetFloat("sim/cockpit/radios/nav1_obs_degm", (float)val);
	
		//fprintf(str, "Set Nav1 OBS: %d\n", val);


	}
	else if(offset==0x0C5E)
	{
		int16_t val;
		CopyMemory(&val, source, size);
		SetFloat("sim/cockpit/radios/nav2_obs_degm", (float)val);
	}
	else if(offset==0x3542)
	{
		int16_t val;
		CopyMemory(&val, source, size);
		float fVal = HPAToInch(val / 16.0f);
		SetFloat("sim/cockpit/misc/barometer_setting2", fVal);	
	}
	else if(offset==0x310A)
	{
		char val;
		CopyMemory(&val, source, size);

		Throttle1Disconnect = false;
		Throttle2Disconnect = false;
		PitchDisconnect = false;
		RollDisconnect = false;

		if(val & 1)
		{
			PitchDisconnect = true;
		}
		if(val & 2)
		{
			RollDisconnect = true;
		}
		if(val & 8)
		{
			Throttle1Disconnect = true;
			Throttle2Disconnect = true;
		}
		if(val & 64)
		{
			Throttle1Disconnect = true;
		}
		if(val & 128)
		{
			Throttle2Disconnect = true;
		}

		if(Throttle1Disconnect || Throttle2Disconnect)
		{
			SetInt("sim/operation/override/override_throttles", 1);
		}
		else
		{
			SetInt("sim/operation/override/override_throttles", 0);
		}

		if(PitchDisconnect)
		{
			SetInt("sim/operation/override/override_joystick_pitch", 1);
		}
		else
			SetInt("sim/operation/override/override_joystick_pitch", 0);

		if(RollDisconnect)
		{
			SetInt("sim/operation/override/override_joystick_roll", 1);
		}
		else
			SetInt("sim/operation/override/override_joystick_roll", 0);
	}
	else
	{
		fprintf(str, "Unknown offset: %x\n", offset);
		BYTE* memory = XConnectMemBlock;
		memory+=offset;
		CopyMemory(memory, source, size);
	}

	fclose(str);
}