#include "xcread.h"
#include "conversion.h"
#include "Common.h"
#include <stdio.h>


XPLMDataRef GetDataRef(const char* dataRef)
{
	XPLMDataRef intvar;

	StringMap::iterator it = VarMap.find(dataRef);

	if(it==VarMap.end())
	{
		intvar = XPLMFindDataRef(dataRef);
		VarMap[dataRef] = intvar;
	}
	else
	{
		intvar = it->second;
	}

	//intvar = XPLMFindDataRef(dataRef);
	return intvar;

}

float GetFloat(const char* dataRef)
{
	XPLMDataRef intvar = GetDataRef(dataRef);
	
	return XPLMGetDataf(intvar);
}

int GetInt(const char* dataRef)
{
	XPLMDataRef intvar;
	intvar = XPLMFindDataRef(dataRef);
	return XPLMGetDatai(intvar);
}

double GetDouble(const char* dataRef)
{

	XPLMDataRef intvar;
	intvar = XPLMFindDataRef(dataRef);
	return XPLMGetDatad(intvar);
}

float GetFloat(const char* dataRef, int offset)
{
	XPLMDataRef intvar;
	intvar = XPLMFindDataRef(dataRef);

	float ret;

	XPLMGetDatavf(intvar, &ret, offset, 1);

	return ret;
}
int GetInt(const char* dataRef, int offset)
{
	XPLMDataRef intvar;
	intvar = XPLMFindDataRef(dataRef);

	int ret;

	XPLMGetDatavi(intvar, &ret, offset, 1);

	return ret;
	
}

void GetVariable(BYTE* target, DWORD offset, DWORD size)
{
	FILE* str = fopen("XConnect.out", "a+");

	XPLMDataRef intvar;

	if(offset == 0x0020)
	{	// Ground altitude meters x 256

		float agl = GetFloat("sim/flightmodel/position/y_agl");
		float msl = GetFloat("sim/flightmodel/position/elevation");

		int ga = (int)((msl - agl) * 256.0);

		CopyMemory(target, &ga, size);
	}
	
	if(offset==0x0238)
	{	// Hour local

		float sec = GetFloat("sim/time/local_time_sec");
		int iSec = (int)(sec / 3600);
		char cSec = (char)iSec;
		CopyMemory(target, &cSec, size);
	}
	if(offset==0x0239)
	{	// minute
		float sec = GetFloat("sim/time/local_time_sec");
		int iHour = (int)(sec / 3600);
		sec -= iHour * 3600;
		int min = (int)(sec / 60);

		char cMin = (char)min;

		CopyMemory(target, &cMin, size);
	}
	if(offset==0x023A)
	{	// second
		float sec = GetFloat("sim/time/local_time_sec");
		int iHour = (int)(sec / 3600);
		sec -= iHour * 3600;
		int min = (int)(sec / 60);
		sec -= min * 60;
		char cSec = (char)sec;

		CopyMemory(target, &cSec, size);
	}
	if(offset==0x023B)
	{	// Hour zulu

		//fprintf(str, "Read hour\n");

		
		float sec = GetFloat("sim/time/zulu_time_sec");

		//fprintf(str, "Hour: %f\n", sec);

		int iSec = (int)(sec / 3600);
		char cSec = (char)iSec;

		//fprintf(str, "Hour: %d\n", iSec);

		CopyMemory(target, &cSec, size);
	}
	if(offset==0x023C)
	{	// minute zulu
		float sec = GetFloat("sim/time/zulu_time_sec");
		int iHour = (int)(sec / 3600);
		sec -= iHour * 3600;
		int min = (int)(sec / 60);

		char cMin = (char)min;

		CopyMemory(target, &cMin, size);
	}
	else if(offset == 0x023E)
	{	// day in year

		int day = GetInt("sim/time/local_date_days");
		short sDay = (short)day;
		CopyMemory(target, &sDay, size);
	}
	else if(offset==0x0264)
	{   // Paused
		int pause = GetInt("sim/time/paused");
		short sPause = (short)pause;
		CopyMemory(target, &sPause, size);
	}
	else if(offset==0x02A0)
	{   // mag var
		float magvar = GetFloat("sim/flightmodel/position/magnetic_variation") / 360.0 * 65536;
		short sMagVar = -(short)magvar;
		CopyMemory(target, &sMagVar, size);
	}
	else if(offset==0x02B4)
	{	// Ground speed

		float gs = GetFloat("sim/flightmodel/position/groundspeed") * 65536.0;
		unsigned int iGS = (unsigned int)gs;

		CopyMemory(target, &iGS, size);

	}
	else if(offset==0x02B8)
	{	// TAS

		float ias = MetersToKnot(GetFloat("sim/flightmodel/position/true_airspeed") * 128.0);

		//fprintf(str, "TAS: %e\n", ias);
		unsigned int iIAS = (unsigned int)ias;

		CopyMemory(target, &iIAS, size);
	}
	else if(offset==0x02BC)
	{	// IAS

		float ias = GetFloat("sim/flightmodel/position/indicated_airspeed2") * 128.0;
		int iIAS = (int)ias;

		CopyMemory(target, &iIAS, size);
	}
	else if(offset==0x02C8)
	{	// vertical speed
		float vs = GetFloat("sim/cockpit2/gauges/indicators/vvi_fpm_pilot");
		vs = vs / 60.0 / 3.28084 * 256;
		int iVs = (int)vs;
		CopyMemory(target, &iVs, size);
	}	
	else if(offset==0x02D4)
	{	// adf2 freq
		int freq = GetInt("sim/cockpit/radios/adf2_freq_hz");
		
		short res = (short)GetBCD(freq);
		CopyMemory(target, &res, size);
	}
	else if(offset==0x02DC)
	{   // nav2 identity
		int freq = GetInt("sim/cockpit/radios/adf2_freq_hz");
		float longitude = (float)GetDouble("sim/flightmodel/position/longitude");
		float latitude = (float)GetDouble("sim/flightmodel/position/latitude");

		/*fprintf(str, "Freq: %d\n", freq);
		fprintf(str, "Lat: %f\n", latitude);
		fprintf(str, "Lon: %f\n", longitude);*/

		XPLMNavRef ref = XPLMFindNavAid(NULL, NULL, &latitude, &longitude, &freq, 2);

		char buf[6];
		for(int i = 0;i<6;i++)
			buf[i] = 0;


		if(ref != XPLM_NAV_NOT_FOUND)
		{
			XPLMGetNavAidInfo(ref, 
								NULL,	// type
								&latitude,	// lat
								&longitude,	// lon
								NULL,	// height
								&freq,	// freq
								NULL,	// heading
								buf,	// ID
								NULL,	// name
								NULL);	// reg
		}

		CopyMemory(target, buf, size);

	}
	else if(offset==0x0330)
	{	// altimeter
		float altimeter = GetFloat("sim/cockpit/misc/barometer_setting");
		short sAlt = ((short)InchToHPA(altimeter)) * 16;
		CopyMemory(target, &sAlt, size);
	}
	else if(offset==0x0366)
	{
		float agl = GetFloat("sim/flightmodel/position/y_agl");
		short onground = 0;
		if(agl<4)
		{
			onground = 1;
		}
		CopyMemory(target, &onground, size);
	}
	else if(offset==0x034C)
	{	// adf1 freq
		int freq = GetInt("sim/cockpit/radios/adf1_freq_hz");
		
		short res = (short)GetBCD(freq);
		CopyMemory(target, &res, size);
	}
	else if(offset==0x034E)
	{	// com1 freq
		int freq = GetInt("sim/cockpit/radios/com1_freq_hz") - 10000;
		
		//fprintf(str, "Nav 1 freq: %d\n", freq);

		short res = (short)GetBCD(freq);
		CopyMemory(target, &res, size);
	}
	else if(offset==0x0350)
	{	// nav1 freq
		int freq = GetInt("sim/cockpit/radios/nav1_freq_hz") - 10000;
		
		//fprintf(str, "Nav 1 freq: %d\n", freq);

		short res = (short)GetBCD(freq);
		CopyMemory(target, &res, size);
	}
	else if(offset==0x0352)
	{	// nav2 freq
		int freq = GetInt("sim/cockpit/radios/nav2_freq_hz") - 10000;
		
		short res = (short)GetBCD(freq);
		CopyMemory(target, &res, size);
	}
	else if(offset==0x0560)
	{
		double latitude = GetDouble("sim/flightmodel/position/latitude");

		latitude = latitude * (10001750.0 * 65536.0 * 65536.0) / 90.0;

		long long lLat = (long long) latitude;

		//fprintf(str, "Latitude: %e", latitude);

		BYTE* lTarget = XConnectMemBlock;
		lTarget += 0x0560;

		int dLo = (lLat & 0xFFFFFFFF);
		int dHi = (lLat & 0xFFFFFFFF00000000) >> 32;

		CopyMemory(lTarget, &dLo, 4);
		CopyMemory(lTarget + 0x0004, &dHi, 4);

		//CopyMemory(lTarget, &, 8);
		

		CopyMemory(target, lTarget, size);
	}
	else if(offset==0x0564)
	{
		double latitude = GetDouble("sim/flightmodel/position/latitude");

		latitude = latitude * (10001750.0 * 65536.0 * 65536.0) / 90.0;

		long long lLat = (long long) latitude;

		//fprintf(str, "Latitude: %e", latitude);

		BYTE* lTarget = XConnectMemBlock;
		lTarget += 0x0560;

		int dLo = (lLat & 0xFFFFFFFF);
		int dHi = (lLat & 0xFFFFFFFF00000000) >> 32;

		CopyMemory(lTarget, &dLo, 4);
		CopyMemory(lTarget + 0x0004, &dHi, 4);

		lTarget += 0x0004;

		CopyMemory(target, lTarget, size);
	}
	else if(offset==0x0568)
	{
		double longitude = GetDouble("sim/flightmodel/position/longitude");

		longitude = longitude * (65536.0 * 65536.0 * 65536.0 * 65536.0) / 360.0;

		long long lLon = (long long)longitude;
		
		BYTE* lTarget = XConnectMemBlock;
		lTarget += 0x0568;

		int dLo = (lLon & 0xFFFFFFFF);
		int dHi = (lLon & 0xFFFFFFFF00000000) >> 32;

		CopyMemory(lTarget, &dLo, 4);
		CopyMemory(lTarget + 0x0004, &dHi, 4);

		//CopyMemory(lTarget, &lLon, 8);

		CopyMemory(target, lTarget, size);
	}
	else if(offset==0x056C)
	{
		double longitude = GetDouble("sim/flightmodel/position/longitude");

		longitude = longitude * (65536.0 * 65536.0 * 65536.0 * 65536.0) / 360.0;

		long long lLon = (long long)longitude;
		
		BYTE* lTarget = XConnectMemBlock;
		lTarget += 0x0568;

		int dLo = (lLon & 0xFFFFFFFF);
		int dHi = (lLon & 0xFFFFFFFF00000000) >> 32;

		CopyMemory(lTarget, &dLo, 4);
		CopyMemory(lTarget + 0x0004, &dHi, 4);

		lTarget += 0x0004;

		CopyMemory(target, lTarget, size);
	}
	else if(offset==0x0578)
	{	// pitch
		float pitch = GetFloat("sim/flightmodel/position/theta");
		pitch = pitch / 360.0 * (65536.0 * 65536.0);
		int iPitch = (int)-pitch;
		CopyMemory(target, &iPitch, size);
	}
	else if(offset==0x057C)
	{	// bank
		float bank = GetFloat("sim/flightmodel/position/phi");
		bank = bank / 360.0 * (65536.0 * 65536.0);
		int iBank = (int)-bank;
		CopyMemory(target, &iBank, size);
	}
	else if(offset==0x0580)
	{	// heading
		double hdg = (double)GetFloat("sim/flightmodel/position/psi");

		hdg = hdg / 360.0 * (65536.0 * 65536.0);

		//fprintf(str, "Heading: %e\n", hdg);

		unsigned int iHdg = (unsigned int)hdg;

		//fprintf(str, "Heading int: %d\n", iHdg);
		CopyMemory(target, &iHdg, size);
	}
	else if(offset==0x0844)
	{	// runway heading nav2
		short crs = (short)(WrapHeading(GetFloat("sim/cockpit/radios/nav2_course_degm") + 180) / 360.0 * 65536.0);
		CopyMemory(target, &crs, size);
	}
	else if(offset==0x0858)
	{
		int freq = GetInt("sim/cockpit/radios/nav2_freq_hz");
		float longitude = (float)GetDouble("sim/flightmodel/position/longitude");
		float latitude = (float)GetDouble("sim/flightmodel/position/latitude");

		/*fprintf(str, "Freq: %d\n", freq);
		fprintf(str, "Lat: %f\n", latitude);
		fprintf(str, "Lon: %f\n", longitude);*/

		XPLMNavRef ref = XPLMFindNavAid(NULL, NULL, &latitude, &longitude, &freq, 1036 + 16 + 32);


		char buf[6];
		for(int i = 0;i<6;i++)
			buf[i] = 0;


		if(ref != XPLM_NAV_NOT_FOUND)
		{
			XPLMGetNavAidInfo(ref, 
								NULL,	// type
								&latitude,	// lat
								&longitude,	// lon
								NULL,	// height
								&freq,	// freq
								NULL,	// heading
								buf,	// ID
								NULL,	// name
								NULL);	// reg
		}

		int lat = (int)(latitude / 90.0 * 10001750.0);

		CopyMemory(target, &lat, size);
	}
	else if(offset==0x085C)
	{
		int freq = GetInt("sim/cockpit/radios/nav1_freq_hz");
		float longitude = (float)GetDouble("sim/flightmodel/position/longitude");
		float latitude = (float)GetDouble("sim/flightmodel/position/latitude");

		/*fprintf(str, "Freq: %d\n", freq);
		fprintf(str, "Lat: %f\n", latitude);
		fprintf(str, "Lon: %f\n", longitude);*/

		XPLMNavRef ref = XPLMFindNavAid(NULL, NULL, &latitude, &longitude, &freq, 1036 + 16 + 32);


		char buf[6];
		for(int i = 0;i<6;i++)
			buf[i] = 0;


		if(ref != XPLM_NAV_NOT_FOUND)
		{
			XPLMGetNavAidInfo(ref, 
								NULL,	// type
								&latitude,	// lat
								&longitude,	// lon
								NULL,	// height
								&freq,	// freq
								NULL,	// heading
								buf,	// ID
								NULL,	// name
								NULL);	// reg
		}

		int lat = (int)(latitude / 90.0 * 10001750.0);

		CopyMemory(target, &lat, size);
	}
	else if(offset==0x0860)
	{
		int freq = GetInt("sim/cockpit/radios/nav2_freq_hz");
		float longitude = (float)GetDouble("sim/flightmodel/position/longitude");
		float latitude = (float)GetDouble("sim/flightmodel/position/latitude");

		/*fprintf(str, "Freq: %d\n", freq);
		fprintf(str, "Lat: %f\n", latitude);
		fprintf(str, "Lon: %f\n", longitude);*/

		XPLMNavRef ref = XPLMFindNavAid(NULL, NULL, &latitude, &longitude, &freq, 1036 + 16 + 32);


		char buf[6];
		for(int i = 0;i<6;i++)
			buf[i] = 0;


		if(ref != XPLM_NAV_NOT_FOUND)
		{
			XPLMGetNavAidInfo(ref, 
								NULL,	// type
								&latitude,	// lat
								&longitude,	// lon
								NULL,	// height
								&freq,	// freq
								NULL,	// heading
								buf,	// ID
								NULL,	// name
								NULL);	// reg
		}

		int lon = (int)(longitude / 360.0 * (65536.0 * 65536.0));

		CopyMemory(target, &lon, size);
	}
	else if(offset==0x0864)
	{
		int freq = GetInt("sim/cockpit/radios/nav1_freq_hz");
		float longitude = (float)GetDouble("sim/flightmodel/position/longitude");
		float latitude = (float)GetDouble("sim/flightmodel/position/latitude");

		/*fprintf(str, "Freq: %d\n", freq);
		fprintf(str, "Lat: %f\n", latitude);
		fprintf(str, "Lon: %f\n", longitude);*/

		XPLMNavRef ref = XPLMFindNavAid(NULL, NULL, &latitude, &longitude, &freq, 1036 + 16 + 32);


		char buf[6];
		for(int i = 0;i<6;i++)
			buf[i] = 0;


		if(ref != XPLM_NAV_NOT_FOUND)
		{
			XPLMGetNavAidInfo(ref, 
								NULL,	// type
								&latitude,	// lat
								&longitude,	// lon
								NULL,	// height
								&freq,	// freq
								NULL,	// heading
								buf,	// ID
								NULL,	// name
								NULL);	// reg
		}

		int lon = (int)(longitude / 360.0 * (65536.0 * 65536.0));

		CopyMemory(target, &lon, size);
	}
	else if(offset==0x0870)
	{	// runway heading nav1

		short crs = (short)(WrapHeading(GetFloat("sim/cockpit/radios/nav1_course_degm") + 180) / 360.0 * 65536.0);
		CopyMemory(target, &crs, size);
	}
	else if(offset==0x0B74)
	{

		float ratio = GetFloat("sim/aircraft/overflow/acf_tank_rat", 1);
		float total = GetFloat("sim/aircraft/weight/acf_m_fuel_tot");
		float weightkg = GetFloat("sim/flightmodel/weight/m_fuel", 1);

		float proc = weightkg / (ratio * total);
		int iProc = (int)(proc * 128.0 * 65536.0);

		CopyMemory(target, &iProc, size);
	}
	else if(offset==0x0B78)
	{

		float ratio = GetFloat("sim/aircraft/overflow/acf_tank_rat", 1);
		float total = GetFloat("sim/aircraft/weight/acf_m_fuel_tot");
		int cap = (int)KilogramsToGallons(ratio * total);

		CopyMemory(target, &cap, size);
	}
	else if(offset==0x0B7C)
	{
		float ratio = GetFloat("sim/aircraft/overflow/acf_tank_rat", 0);
		float total = GetFloat("sim/aircraft/weight/acf_m_fuel_tot");
		float weightkg = GetFloat("sim/flightmodel/weight/m_fuel", 0);

		float proc = weightkg / (ratio * total);
		int iProc = (int)(proc * 128.0 * 65536.0);

		CopyMemory(target, &iProc, size);

		//fprintf(str, "Fuel total weight: %e\n", total);
	}
	else if(offset==0x0B80)
	{
		float ratio = GetFloat("sim/aircraft/overflow/acf_tank_rat", 0);
		float total = GetFloat("sim/aircraft/weight/acf_m_fuel_tot");
		int cap = (int)KilogramsToGallons(ratio * total);

		CopyMemory(target, &cap, size);
	}
	else if(offset==0x0B94)
	{
		float ratio = GetFloat("sim/aircraft/overflow/acf_tank_rat", 2);
		float total = GetFloat("sim/aircraft/weight/acf_m_fuel_tot");
		float weightkg = GetFloat("sim/flightmodel/weight/m_fuel", 2);

		float proc = weightkg / (ratio * total);
		int iProc = (int)(proc * 128.0 * 65536.0);

		CopyMemory(target, &iProc, size);
	}
	else if(offset==0x0B98)
	{
		float ratio = GetFloat("sim/aircraft/overflow/acf_tank_rat", 2);
		float total = GetFloat("sim/aircraft/weight/acf_m_fuel_tot");
		int cap = (int)KilogramsToGallons(ratio * total);

		CopyMemory(target, &cap, size);
	}
	else if(offset==0x0BB4)
	{
		short ratio = (short)(GetFloat("sim/joystick/yoke_pitch_ratio") * 16383.0);
		CopyMemory(target, &ratio, size);
	}
	else if(offset==0x0BB8)
	{
		short ratio = (short)(GetFloat("sim/joystick/yoke_roll_ratio") * 16383.0);
		CopyMemory(target, &ratio, size);
	}
	else if(offset==0x0C29)
	{
		float dist = GetFloat("sim/cockpit2/radios/indicators/nav1_dme_distance_nm");
		char dme[5];
		if(dist>99.9)
		{
			sprintf(dme, "%.0f", dist);
		}
		else
		{
			sprintf(dme, "%00.1f", dist);
		}
		dme[4] = 0;
		CopyMemory(target, dme, size);
	}
	else if(offset==0x0C33)
	{
		float dist = GetFloat("sim/cockpit2/radios/indicators/nav2_dme_distance_nm");
		char dme[5];
		if(dist>99.9)
		{
			sprintf(dme, "%.0f", dist);
		}
		else
		{
			sprintf(dme, "%00.1f", dist);
		}
		dme[4] = 0;
		CopyMemory(target, dme, size);
	}
	else if(offset==0x0C48)
	{
		char loc = (char)(GetFloat("sim/cockpit2/radios/indicators/nav1_hdef_dots_pilot") / 2.5 * 127);
		CopyMemory(target, &loc, size);
	}
	else if(offset==0x0C49)
	{
		char gs = (char)(GetFloat("sim/cockpit2/radios/indicators/nav1_vdef_dots_pilot") / 2.5 * 127);
		CopyMemory(target, &gs, size);
	}

	else if(offset==0x0C4B)
	{
		char fromto = (char)GetInt("sim/cockpit2/radios/indicators/nav1_flag_from_to_pilot");
		CopyMemory(target, &fromto, size);
	}
	else if(offset==0x0C4C)
	{
		char flag = (char)GetInt("sim/cockpit/radios/nav1_CDI");
		CopyMemory(target, &flag, size);
	}
	else if(offset==0x0C4E)
	{
		float fObs = GetFloat("sim/cockpit/radios/nav1_obs_degm");
		//fprintf(str, "Nav 1 obs: %f\n", fObs);

		short obs = (short)fObs;
		CopyMemory(target, &obs, size);

		/*fObs = GetFloat("sim/cockpit/radios/nav1_obs_degt");
		fprintf(str, "sim/cockpit/radios/nav1_obs_degt: %f\n", fObs);
		fObs = GetFloat("sim/cockpit2/radios/actuators/nav1_obs_deg_mag_pilot");
		fprintf(str, "sim/cockpit2/radios/actuators/nav1_obs_deg_mag_pilot: %f\n", fObs);
		fObs = GetFloat("sim/cockpit2/radios/actuators/nav1_course_deg_mag_pilot");
		fprintf(str, "sim/cockpit2/radios/actuators/nav1_course_deg_mag_pilot: %f\n", fObs);*/

	}
	else if(offset==0x0C50)
	{	// nav1 radial
		short bgn = (short)(WrapHeading(GetFloat("sim/cockpit2/radios/indicators/nav1_bearing_deg_mag") - 180) / 360.0 * 65536.0);
		CopyMemory(target, &bgn, size);
	}
	else if(offset==0x0C59)
	{
		char loc = (char)(GetFloat("sim/cockpit2/radios/indicators/nav2_hdef_dots_pilot") / 2.5 * 127);
		CopyMemory(target, &loc, size);
	}
	else if(offset==0x0C5B)
	{
		char fromto = (char)GetInt("sim/cockpit2/radios/indicators/nav2_flag_from_to_pilot");
		CopyMemory(target, &fromto, size);
	}
	else if(offset==0x0C5E)
	{
		short obs = (short)GetFloat("sim/cockpit/radios/nav2_obs_degm");
		CopyMemory(target, &obs, size);
	}
	else if(offset==0x0C60)
	{	// nav1 radial
		short bgn = (short)(WrapHeading(GetFloat("sim/cockpit2/radios/indicators/nav2_bearing_deg_mag") - 180) / 360.0 * 65536.0);
		CopyMemory(target, &bgn, size);
	}
	else if(offset==0x0C6E)
	{
		char gs = (char)(GetFloat("sim/cockpit2/radios/indicators/nav2_vdef_dots_pilot") / 2.5 * 127);
		CopyMemory(target, &gs, size);
	}
	else if(offset==0x0C6F)
	{
		char flag = (char)GetInt("sim/cockpit/radios/nav2_CDI");
		CopyMemory(target, &flag, size);
	}
	else if(offset==0x0E8C)
	{
		short oat = (short)(GetFloat("sim/weatcher/temperature_ambient_c") * 256.0);
		CopyMemory(target, &oat, size);
	}
	else if(offset==0x0E90)
	{	// wind speed
		short wsp = (short)GetFloat("sim/weather/wind_speed_kt");
		CopyMemory(target, &wsp, size);
	}
	else if(offset==0x0E92)
	{	// wind direction
		short wdir = (short)(GetFloat("sim/weather/wind_direction_degt") / 360.0 * 65536);
		CopyMemory(target, &wdir, size);
	}
	else if(offset==0x2000)
	{
		double n1 = (double)GetFloat("sim/cockpit2/engine/indicators/N1_percent", 0);
		CopyMemory(target, &n1, size);
	}
	else if(offset==0x2018)
	{
		double n2 = (double)GetFloat("sim/cockpit2/engine/indicators/N2_percent", 0);
		CopyMemory(target, &n2, size);
	}
	else if(offset==0x088C)
	{
		short thr = (short)(GetFloat("sim/flightmodel/engine/ENGN_thro", 0) * 16384.0);
		CopyMemory(target, &thr, size);




	}
	else if(offset==0x08BE)
	{
		short egt = (short)(GetFloat("sim/cockpit2/engine/indicators/EGT_deg_C", 0) / 860.0 * 16384.0);
		CopyMemory(target, &egt, size);
	}
	else if(offset==0x08BA)
	{
		short oilp = (short)(GetFloat("sim/flightmodel/engine/ENGN_oil_press_psi", 0) / 55.0 * 16384.0);
		CopyMemory(target, &oilp, size);
	}
	else if(offset==0x08B8)
	{
		short oilt = (short)(GetFloat("sim/flightmodel/engine/ENGN_oil_temp_c", 0) / 140.0 * 16384.0);
		CopyMemory(target, &oilt, size);
	}
	else if(offset==0x08D0)
	{
		int oilq = (int)(GetFloat("sim/cockpit2/engine/indicators/oil_quantity_ratio", 0) * 16384.0);
		CopyMemory(target, &oilq, size);
	}
	else if(offset==0x08DC)
	{
		int hydq = (int)(GetFloat("sim/cockpit/misc/hydraulic_quantity") / 100.0 * 16384.0);
		CopyMemory(target, &hydq, size);
	}
	else if(offset==0x0918)
	{
		double ff = (double)(GetFloat("sim/flightmodel/engine/ENGN_FF_", 0) * 3600 / 0.45359237);
		CopyMemory(target, &ff, size);
	}
	else if(offset==0x0924)
	{
		short thr = (short)(GetFloat("sim/flightmodel/engine/ENGN_thro", 1) * 16384.0);
		CopyMemory(target, &thr, size);
	}
	else if(offset==0x0BE0)
	{
		int flappos = (int)(GetFloat("sim/flightmodel2/controls/flap1_deploy_ratio") * 16383.0);
		CopyMemory(target, &flappos, size);
	}
	else if(offset==0x0BE4)
	{
		int flappos = (int)(GetFloat("sim/flightmodel2/controls/flap2_deploy_ratio") * 16383.0);
		CopyMemory(target, &flappos, size);
	}
	else if(offset==0x11C6)
	{
		short mach = (short)(GetFloat("sim/flightmodel/misc/machno") * 20480.0);
		CopyMemory(target, &mach, size);
	}
	else if(offset==0x11D0)
	{
		float oat = GetFloat("sim/weatcher/temperature_ambient_c");
		float mach = GetFloat("sim/flightmodel/misc/machno");

		float tat = oat * (1 + 0.2 * mach * mach) * 256.0;
		short sTat = (short)tat;
		CopyMemory(target, &sTat, size);
	}
	else if(offset==0x2100)
	{
		double n1 = (double)GetFloat("sim/cockpit2/engine/indicators/N1_percent", 1);
		CopyMemory(target, &n1, size);
	}
	else if(offset==0x2118)
	{
		double n2 = (double)GetFloat("sim/cockpit2/engine/indicators/N2_percent", 1);
		CopyMemory(target, &n2, size);
	}
	else if(offset==0x0956)
	{
		short egt = (short)(GetFloat("sim/cockpit2/engine/indicators/EGT_deg_C", 0) / 860.0 * 16384.0);
		CopyMemory(target, &egt, size);
	}
	else if(offset==0x0952)
	{
		short oilp = (short)(GetFloat("sim/flightmodel/engine/ENGN_oil_press_psi", 1) / 55.0 * 16384.0);
		CopyMemory(target, &oilp, size);
	}
	else if(offset==0x0950)
	{
		short oilt = (short)(GetFloat("sim/flightmodel/engine/ENGN_oil_temp_c", 1) / 140.0 * 16384.0);
		CopyMemory(target, &oilt, size);
	}
	else if(offset==0x0968)
	{
		int oilq = (int)(GetFloat("sim/cockpit2/engine/indicators/oil_quantity_ratio", 1) * 16384.0);
		CopyMemory(target, &oilq, size);
	}
	else if(offset==0x0974)
	{
		int hydq = (int)(GetFloat("sim/cockpit/misc/hydraulic_quantity2") / 100.0 * 16384.0);
		CopyMemory(target, &hydq, size);
	}
	else if(offset==0x09B0)
	{
		double ff = (double)(GetFloat("sim/flightmodel/engine/ENGN_FF_", 0) * 3600 / 0.45359237);
		CopyMemory(target, &ff, size);
	}
	else if(offset==0x3000)
	{   // nav1 identity

		int freq = GetInt("sim/cockpit/radios/nav1_freq_hz");
		float longitude = (float)GetDouble("sim/flightmodel/position/longitude");
		float latitude = (float)GetDouble("sim/flightmodel/position/latitude");

		/*fprintf(str, "Freq: %d\n", freq);
		fprintf(str, "Lat: %f\n", latitude);
		fprintf(str, "Lon: %f\n", longitude);*/

		XPLMNavRef ref = XPLMFindNavAid(NULL, NULL, &latitude, &longitude, &freq, 1036 + 16 + 32);


		char buf[6];
		for(int i = 0;i<6;i++)
			buf[i] = 0;


		if(ref != XPLM_NAV_NOT_FOUND)
		{
			XPLMGetNavAidInfo(ref, 
								NULL,	// type
								&latitude,	// lat
								&longitude,	// lon
								NULL,	// height
								&freq,	// freq
								NULL,	// heading
								buf,	// ID
								NULL,	// name
								NULL);	// reg
		}

		CopyMemory(target, buf, size);

	}
	else if(offset==0x301F)
	{   // nav2 identity
		int freq = GetInt("sim/cockpit/radios/nav2_freq_hz");
		float longitude = (float)GetDouble("sim/flightmodel/position/longitude");
		float latitude = (float)GetDouble("sim/flightmodel/position/latitude");

		/*fprintf(str, "Freq: %d\n", freq);
		fprintf(str, "Lat: %f\n", latitude);
		fprintf(str, "Lon: %f\n", longitude);*/

		XPLMNavRef ref = XPLMFindNavAid(NULL, NULL, &latitude, &longitude, &freq, 1036 + 16 + 32);

		char buf[6];
		for(int i = 0;i<6;i++)
			buf[i] = 0;


		if(ref != XPLM_NAV_NOT_FOUND)
		{
			XPLMGetNavAidInfo(ref, 
								NULL,	// type
								&latitude,	// lat
								&longitude,	// lon
								NULL,	// height
								&freq,	// freq
								NULL,	// heading
								buf,	// ID
								NULL,	// name
								NULL);	// reg
		}

		CopyMemory(target, buf, size);

	}
	else if(offset==0x303E)
	{   // nav2 identity
		int freq = GetInt("sim/cockpit/radios/adf1_freq_hz");
		float longitude = (float)GetDouble("sim/flightmodel/position/longitude");
		float latitude = (float)GetDouble("sim/flightmodel/position/latitude");

		/*fprintf(str, "Freq: %d\n", freq);
		fprintf(str, "Lat: %f\n", latitude);
		fprintf(str, "Lon: %f\n", longitude);*/

		XPLMNavRef ref = XPLMFindNavAid(NULL, NULL, &latitude, &longitude, &freq, 2);

		char buf[6];
		for(int i = 0;i<6;i++)
			buf[i] = 0;


		if(ref != XPLM_NAV_NOT_FOUND)
		{
			XPLMGetNavAidInfo(ref, 
								NULL,	// type
								&latitude,	// lat
								&longitude,	// lon
								NULL,	// height
								&freq,	// freq
								NULL,	// heading
								buf,	// ID
								NULL,	// name
								NULL);	// reg
		}

		CopyMemory(target, buf, size);

	}
	else if(offset==0x3118)
	{	// com2 freq
		int freq = GetInt("sim/cockpit/radios/com2_freq_hz") - 10000;
		
		//fprintf(str, "Nav 1 freq: %d\n", freq);

		short res = (short)GetBCD(freq);
		CopyMemory(target, &res, size);
	}
	else if(offset==0x31E4)
	{	// ground altitude
		int agl = (int)(GetFloat("sim/flightmodel/position/y_agl") * 65536.0);
		CopyMemory(target, &agl, size);
	}
	else if(offset==0x3300)
	{	// radio status flags
		
		bool goodnav1 = GetInt("sim/cockpit/radios/nav_type", 0) != 0; 
		bool goodnav2 = GetInt("sim/cockpit/radios/nav_type", 1) != 0;
		bool goodadf1 = GetInt("sim/cockpit/radios/nav_type", 2) != 0;
		bool goodadf2 = GetInt("sim/cockpit/radios/nav_type", 3) != 0;
		bool nav1isils = GetInt("sim/cockpit/radios/nav_type", 0) == 4;
		bool nav2isils = GetInt("sim/cockpit/radios/nav_type", 1) == 4;
		bool nav1hasdme = GetInt("sim/cockpit/radios/nav1_has_dme", 1) != 0;
		bool nav2hasdme = GetInt("sim/cockpit/radios/nav2_has_dme", 1) != 0;

		short ret = 0;

		if(goodnav1)
			ret += 2;
		if(goodnav2)
			ret += 4;
		if(goodadf1)
			ret += 8;
		if(nav1hasdme)
			ret += 16;
		if(nav2hasdme)
			ret += 32;
		if(nav1isils)
			ret += 64;
		if(goodadf2)
			ret += 2048;
		if(nav2isils)
			ret += 4096;

		CopyMemory(target, &ret, size);
	}
	else if(offset==0x3304)
	{	// FDSConnectX Version
		int version = 0x20000000;
		CopyMemory(target, &version, size);
	}
	else if(offset==0x3308)
	{	// X-Plane Version
		int version = 0xFADE0006;
		CopyMemory(target, &version, size);
	}
	else if(offset==0x3324)
	{   // Barometric altitude
		int alt = (int)GetFloat("sim/flightmodel/misc/h_ind2");
		CopyMemory(target, &alt, size);
	}
	else if(offset==0x3330)
	{
		short ratio = (short)(GetFloat("sim/cockpit2/engine/actuators/throttle_ratio",0) * 16384.0);
		CopyMemory(target, &ratio, size);
	}
	else if(offset==0x3332)
	{
		short ratio = (short)(GetFloat("sim/cockpit2/engine/actuators/throttle_ratio",1) * 16384.0);
		CopyMemory(target, &ratio, size);
	}
	else if(offset==0x3542)
	{
		float altimeter = GetFloat("sim/cockpit/misc/barometer_setting2");
		short sAlt = ((short)InchToHPA(altimeter)) * 16;
		CopyMemory(target, &sAlt, size);
	}
	else if(offset==0x3544)
	{
		int alt = (int)GetFloat("sim/flightmodel/misc/h_ind_copilot2");
		CopyMemory(target, &alt, size);
	}
	
	else 
	{
		BYTE* memory = XConnectMemBlock;
		memory+=offset;
		CopyMemory(target, memory, size);

	}

	fclose(str);
}