// GatewayEsamToInfoteamSerial.h: Schnittstelle f�r die Klasse CGatewayEsamToInfoteamSerial.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATEWAYEsamTOEsamRS232_H__10BC918F_ECD4_465F_A861_6942A4B78334__INCLUDED_)
#define AFX_GATEWAYEsamTOEsamRS232_H__10BC918F_ECD4_465F_A861_6942A4B78334__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <CommunicationModel/Common/CommunicationModelDefinitions.h>



#include "../BaseClasses/GatewayDCStoPS.h"
class CCommand_PS_InfoteamSerial;

class CGatewayEsamToInfoteamSerial : public CGatewayDCStoPS
{
public:
    CGatewayEsamToInfoteamSerial();
    virtual ~CGatewayEsamToInfoteamSerial();

    virtual CGateway* Clone();
    CGatewayEsamToInfoteamSerial& operator=(CGatewayEsamToInfoteamSerial& other);

    virtual BOOL ProcessCommand(CCommandRoot*pCommand, CLayerManagerBase* pManager, HANDLE h_Handle, HANDLE hTransactionHandle);

private:
    void DeleteCommands();
    void InitCommands();

    BOOL Process_ReadObject(CCommand_DCS* pCommand, CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle);
    BOOL Process_WriteObject(CCommand_DCS* pCommand, CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle);
    BOOL Process_InitiateSegmentedRead(CCommand_DCS* pCommand, CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle);
    BOOL Process_InitiateSegmentedWrite(CCommand_DCS* pCommand, CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle);
    BOOL Process_SegmentRead(CCommand_DCS* pCommand, CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle);
    BOOL Process_SegmentWrite(CCommand_DCS* pCommand, CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle);
    BOOL Process_AbortSegmentedTransfer(CCommand_DCS* pCommand, CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle);
    BOOL Process_SendNMTService(CCommand_DCS* pCommand, CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle);
    BOOL Process_SendCANFrame(CCommand_DCS* pCommand, CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle);
    BOOL Process_ReadCANFrame(CCommand_DCS* pCommand, CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle);
    BOOL Process_RequestCANFrame(CCommand_DCS* pCommand, CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle);
    BOOL Process_SendLSSFrame(CCommand_DCS* pCommand, CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle);
    BOOL Process_ReadLSSFrame(CCommand_DCS* pCommand, CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle);
    BOOL Process_Login(CCommand_DCS* pCommand, CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle);
    BOOL Process_Logout(CCommand_DCS* pCommand, CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle);
    BOOL Process_SetState(CCommand_DCS* pCommand, CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle);

    BOOL PS_ProcessProtocolMaxon(CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle, BYTE uOpCode, void* pDataBuffer, DWORD dDataBufferLength, BYTE ubKeepLock, void** ppRetDataBuffer, DWORD* pdRetDataBufferLength, CErrorInfo* pErrorInfo = NULL);
	BOOL PS_AbortProtocolMaxon(CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle, CErrorInfo* pErrorInfo = NULL);
    BOOL PS_ProcessProtocol(CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle, DWORD dTimeout, void* pDataBuffer, DWORD dDataBufferLength, void** ppRetDataBuffer, DWORD* pdRetDataBufferLength, CErrorInfo* pErrorInfo = NULL);
    BOOL PS_SendData(CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle, void* pDataBuffer, DWORD dDataBufferLength, CErrorInfo* pErrorInfo = NULL);
    BOOL PS_ReceiveData(CProtocolStackManagerBase* pProtocolStackManager, HANDLE hPS_Handle, HANDLE hTransactionHandle, void** ppRetDataBuffer, DWORD* pdRetDataBufferLength, CErrorInfo* pErrorInfo = NULL);

	//Layer Parameter Stack
	BOOL InitLayerParameterStack(CCommandRoot* p_pCommand);

    BOOL EvaluateErrorCode(BOOL oResult, DWORD dDeviceErrorCode, CErrorInfo* pComErrorInfo, CErrorInfo* pCmdErrorInfo);

    BOOL CopyData(void*& pDest, void* pSource, DWORD sourceSize);
    BOOL CopyReturnData(void* pDest, DWORD destSize, void*& pSource, DWORD& sourceSize);

    BOOL InitErrorHandling();

private:
    CCommand_PS_InfoteamSerial* m_pCommand_ProcessProtocolMaxon;
	CCommand_PS_InfoteamSerial* m_pCommand_AbortProtocolMaxon;
	CCommand_PS_InfoteamSerial* m_pCommand_ProcessProtocol;
    CCommand_PS_InfoteamSerial* m_pCommand_SendChunk;
    CCommand_PS_InfoteamSerial* m_pCommand_ReceiveData;

};


#endif // !defined(AFX_GATEWAYEsamTOEsamRS232_H__10BC918F_ECD4_465F_A861_6942A4B78334__INCLUDED_)
