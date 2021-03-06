// DRIVE_Plc.h: Schnittstelle f�r die Klasse CVirtualDevice_Plc.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRIVE_Plc_H__5699BAD0_1237_4AF2_9A16_ED32C2CACB84__INCLUDED_)
#define AFX_DRIVE_Plc_H__5699BAD0_1237_4AF2_9A16_ED32C2CACB84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <CommunicationModel/Common/CommunicationModelDefinitions.h>

#include "BaseClasses/VirtualDeviceBase.h"

class CGateway;
class CCommandGroupStandard_VCS_Plc;

class CVirtualDevice_Plc : public CVirtualDeviceBase
{
public:
    BOOL InitVirtualDevice(CErrorInfo* pErrorInfo = NULL);
    BOOL InitVirtualDevice(CStdString strDeviceName, CErrorInfo* pErrorInfo = NULL);
    BOOL InitVirtualDevice(CStdString strDeviceName, CStdString strProtocolStackName, CStdString strInterfaceName, CErrorInfo* pErrorInfo = NULL);
    BOOL InitGateway(CStdString strDeviceName);

//Funktionalit�t
    BOOL GetCommands(CStdString* pCommandInfo);

    CVirtualDevice_Plc();
    CVirtualDevice_Plc(const CVirtualDevice_Plc& rObject);
    virtual ~CVirtualDevice_Plc();
    virtual CVirtualDeviceBase* Clone();

//JournalManager
    void InitJournalManager(CJournalManagerBase *pJournalManager);
    void ResetJournalManager();

//ParameterSet
	BOOL InitParameterSet();

private:
    BOOL VerifyGateway(CStdString strDeviceName);
    BOOL InitErrorHandling();

    void DeleteGroupList();
    void FillGroupList();
    BOOL InitGroupList(CGateway* pGateway);

    CCommandGroupStandard_VCS_Plc* m_pCommandGroupStandard;
};

#endif // !defined(AFX_DRIVE_Plc_H__5699BAD0_1237_4AF2_9A16_ED32C2CACB84__INCLUDED_)
