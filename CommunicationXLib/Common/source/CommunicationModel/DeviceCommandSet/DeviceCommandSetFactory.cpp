#include "stdafx.h"
#include "DeviceCommandSetFactory.h"

/*DON'T EDIT THIS FILE! autogenerated code by XlibFactoryBuilder 1.0.2.3*/

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BOOL CDeviceCommandSetFactory::GetAllIdentifiers(std::list<CStdString>& p_rTargets)
{
	BOOL oResult = FALSE;

	p_rTargets.clear();

	p_rTargets.push_back(DEVICE_EPOS);
	p_rTargets.push_back(DEVICE_ESAM);
	p_rTargets.push_back(DEVICE_EPOS2);
	p_rTargets.push_back(DEVICE_ESAM2);

	oResult = TRUE;

	return oResult;
}

BOOL CDeviceCommandSetFactory::CreateInstance(CStdString p_Name, CDeviceBase*& p_rpNewInstance)
{
	BOOL oResult = FALSE;

	if(p_Name.CompareNoCase(DEVICE_EPOS) == 0)
	{
		p_rpNewInstance = new CDevice_Epos();
		oResult = TRUE;
	}
	else if(p_Name.CompareNoCase(DEVICE_ESAM) == 0)
	{
		p_rpNewInstance = new CDevice_Esam();
		oResult = TRUE;
	}
	else if(p_Name.CompareNoCase(DEVICE_EPOS2) == 0)
	{
		p_rpNewInstance = new CDevice_Epos2();
		oResult = TRUE;
	}
	else if(p_Name.CompareNoCase(DEVICE_ESAM2) == 0)
	{
		p_rpNewInstance = new CDevice_Esam2();
		oResult = TRUE;
	}

	return oResult;
}