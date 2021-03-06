#pragma once

/*DONT EDIT THIS FILE! autogenerated code by XlibFactoryBuilder 1.0.2.3*/

#include <list>
#include <StdString.h>

#include <CommunicationModel/Interface/Classes/Gateway/BaseClasses/DeviceInfoHandlingBase.h>
#include <CommunicationModel/Interface/Classes/Gateway/USB/Classes/Ftd2xxDeviceInfoHandling.h>

class CDeviceInfoHandlingFactory
{
public:
	static BOOL GetAllIdentifiers(std::list<CStdString>& p_rTargets);
	static BOOL CreateInstance(CStdString p_Name, CDeviceInfoHandlingBase*& p_rpInstance);
};
