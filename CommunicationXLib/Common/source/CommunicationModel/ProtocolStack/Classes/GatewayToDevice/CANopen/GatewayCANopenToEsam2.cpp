// GatewayCANopenToEsam2.cpp: Implementierung der Klasse CGatewayCANopenToEsam2.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GatewayCANopenToEsam2.h"

#include <malloc.h>
#include <Process/MmcProcess.h>
#include <memory.h>

#include <CommunicationModel/CommonLayer/Classes/Commands/ProtocolStack/BaseClasses/Command_PS.h>
#include <CommunicationModel/CommonLayer/Classes/Commands/ProtocolStack/Command_PS_CANopen.h>

#include "../../../../DeviceCommandSet/BaseClasses/DeviceCommandSetManagerBase.h"
#include <CommunicationModel/CommonLayer/Classes/Commands/DeviceCommandSet/Command_DCS_Esam2.h>
#include <CommunicationModel/CommonLayer/Classes/Commands/DeviceCommandSet/DcsEsam2Def.h>

#include <CommunicationModel/CommonLayer/Classes/Commands/Command/LayerParameter/LayerParameterStack.h>
#include <CommunicationModel/CommonLayer/Classes/Commands/Command/LayerParameter/LayerParameterSet.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CGatewayCANopenToEsam2::CGatewayCANopenToEsam2():
    k_MaxEsam2DataSize(255)
{
    m_pCommand_WriteObject = NULL;
    m_pCommand_InitiateSegmentedWrite = NULL;
    m_pCommand_SegmentedWrite = NULL;
    m_pCommand_ReadObject = NULL;
    m_pCommand_InitiateSegmentedRead = NULL;
    m_pCommand_SegmentedRead = NULL;
    m_pCommand_SendNMTService = NULL;
    m_pCommand_RequestCANFrame = NULL;
    m_pCommand_SendCANFrame = NULL;
    m_pCommand_SendLSSFrame = NULL;
    m_pCommand_ReadLSSFrame = NULL;
    m_pCommand_AbortSegmentedTransfer = NULL;

    m_pBufferedData = NULL;
    m_dBufferedDataSize = 0;
    m_dBufferedDataRead  = 0;
    m_oNoMoreSegments = FALSE;
    m_oToggle = FALSE;
    m_WriteSegmentSize = 0;
    ResetBufferedData();

    m_ulNetworkIndicationThreadId = 0;
    m_wTargetNetworkId = 0;
    m_uTargetNodeId = 0;

    InitCommands();
}

CGatewayCANopenToEsam2::~CGatewayCANopenToEsam2()
{
    ResetBufferedData();
    DeleteCommands();
}

CGateway* CGatewayCANopenToEsam2::Clone()
{
    CGatewayCANopenToEsam2* pClonedGateway;

    pClonedGateway = new CGatewayCANopenToEsam2();
    *pClonedGateway = *this;

    return pClonedGateway;
}

void CGatewayCANopenToEsam2::InitCommands()
{
    DeleteCommands();

    //WriteObject
    m_pCommand_WriteObject = new CCommand_DCS_Esam2();
    m_pCommand_WriteObject->InitCommand(ESAM2_WRITE_OBJECT);

    //ReadObject
    m_pCommand_ReadObject = new CCommand_DCS_Esam2();
    m_pCommand_ReadObject->InitCommand(ESAM2_READ_OBJECT);

    //InitiateSegmentedWrite
    m_pCommand_InitiateSegmentedWrite = new CCommand_DCS_Esam2();
    m_pCommand_InitiateSegmentedWrite->InitCommand(ESAM2_INITIATE_SEGMENT_WRITE);

    //InitiateSegmentedRead
    m_pCommand_InitiateSegmentedRead = new CCommand_DCS_Esam2();
    m_pCommand_InitiateSegmentedRead->InitCommand(ESAM2_INITIATE_SEGMENT_READ);

    //SegmentedWrite
    m_pCommand_SegmentedWrite = new CCommand_DCS_Esam2();
    m_pCommand_SegmentedWrite->InitCommand(ESAM2_SEGMENT_WRITE);

    //SegmentedRead
    m_pCommand_SegmentedRead = new CCommand_DCS_Esam2();
    m_pCommand_SegmentedRead->InitCommand(ESAM2_SEGMENT_READ);

    //AbortSegmentedTransfer
    m_pCommand_AbortSegmentedTransfer = new CCommand_DCS_Esam2();
    m_pCommand_AbortSegmentedTransfer->InitCommand(ESAM2_ABORT_SEGMENT_TRANSFER);

    //Init Command SendNMTService
    m_pCommand_SendNMTService = new CCommand_DCS_Esam2();
    m_pCommand_SendNMTService->InitCommand(ESAM2_SEND_NMT_SERVICE);

    //Init Command Request
    m_pCommand_RequestCANFrame = new CCommand_DCS_Esam2();
    m_pCommand_RequestCANFrame->InitCommand(ESAM2_REQUEST_CAN_FRAME);

    //Init Command Send
    m_pCommand_SendCANFrame = new CCommand_DCS_Esam2();
    m_pCommand_SendCANFrame->InitCommand(ESAM2_SEND_CAN_FRAME);

    //SendLSSFrame
    m_pCommand_SendLSSFrame = new CCommand_DCS_Esam2();
    m_pCommand_SendLSSFrame->InitCommand(ESAM2_SEND_LSS_FRAME);

    //ReadLSSFrame
    m_pCommand_ReadLSSFrame = new CCommand_DCS_Esam2();
    m_pCommand_ReadLSSFrame->InitCommand(ESAM2_READ_LSS_FRAME);
}

void CGatewayCANopenToEsam2::DeleteCommands()
{
    //WriteObject
    if(m_pCommand_WriteObject)
    {
        delete m_pCommand_WriteObject;
        m_pCommand_WriteObject = NULL;
    }

    //InitiateSegmentedWrite
    if(m_pCommand_InitiateSegmentedWrite)
    {
        delete m_pCommand_InitiateSegmentedWrite;
        m_pCommand_InitiateSegmentedWrite = NULL;
    }

    //SegmentedWrite
    if(m_pCommand_SegmentedWrite)
    {
        delete m_pCommand_SegmentedWrite;
        m_pCommand_SegmentedWrite = NULL;
    }

    //ReadObject
    if(m_pCommand_ReadObject)
    {
        delete m_pCommand_ReadObject;
        m_pCommand_ReadObject = NULL;
    }

    //AbortSegmentedTransfer
    if(m_pCommand_AbortSegmentedTransfer)
    {
        delete m_pCommand_AbortSegmentedTransfer;
        m_pCommand_AbortSegmentedTransfer = NULL;
    }

    //InitateSegmentedRead
    if(m_pCommand_InitiateSegmentedRead)
    {
        delete m_pCommand_InitiateSegmentedRead;
        m_pCommand_InitiateSegmentedRead = NULL;
    }

    //SegmentedRead
    if(m_pCommand_SegmentedRead)
    {
        delete m_pCommand_SegmentedRead;
        m_pCommand_SegmentedRead = NULL;
    }

    //NMTService
    if(m_pCommand_SendNMTService)
    {
        delete m_pCommand_SendNMTService;
        m_pCommand_SendNMTService = NULL;
    }

    //RequestCANFrame
    if(m_pCommand_RequestCANFrame)
    {
        delete m_pCommand_RequestCANFrame;
        m_pCommand_RequestCANFrame = NULL;
    }

    //SendCANFrame
    if(m_pCommand_SendCANFrame)
    {
        delete m_pCommand_SendCANFrame;
        m_pCommand_SendCANFrame = NULL;
    }

    //SendLSSFrame
    if(m_pCommand_SendLSSFrame)
    {
        delete m_pCommand_SendLSSFrame;
        m_pCommand_SendLSSFrame = NULL;
    }

    //ReadLSSFrame
    if(m_pCommand_ReadLSSFrame)
    {
        delete m_pCommand_ReadLSSFrame;
        m_pCommand_ReadLSSFrame = NULL;
    }
}

CGatewayCANopenToEsam2& CGatewayCANopenToEsam2::operator=(CGatewayCANopenToEsam2& other)
{
    if(this != &other)
    {
        *((CGatewayPStoDCS*)this) = *((CGatewayPStoDCS*)&other);
    }

    return *this;
}

BOOL CGatewayCANopenToEsam2::ProcessCommand(CCommandRoot* pCommand, CLayerManagerBase* pManager, HANDLE h_Handle, HANDLE hTransactionHandle)
{
    CCommand_PS* pCommand_PS;
    CDeviceCommandSetManagerBase* pDeviceCommandSetManager;

    if(CGateway::ProcessCommand(pCommand, pManager, h_Handle, hTransactionHandle))
    {
        if(CheckLayers(pCommand, pManager))
        {
            pCommand_PS = (CCommand_PS*)pCommand;
            pDeviceCommandSetManager = (CDeviceCommandSetManagerBase*)pManager;

            switch(pCommand->GetCommandId())
            {
                case CANOPEN_INITIATE_SDO_DOWNLOAD: return Process_InitiateSDODownload(pCommand_PS, pDeviceCommandSetManager, h_Handle, hTransactionHandle);
                case CANOPEN_DOWNLOAD_SDO_SEGMENT: return Process_DownloadSDOSegment(pCommand_PS, pDeviceCommandSetManager, h_Handle, hTransactionHandle);
                case CANOPEN_INITIATE_SDO_UPLOAD: return Process_InitiateSDOUpload(pCommand_PS, pDeviceCommandSetManager, h_Handle, hTransactionHandle);
                case CANOPEN_UPLOAD_SDO_SEGMENT: return Process_UploadSDOSegment(pCommand_PS, pDeviceCommandSetManager, h_Handle, hTransactionHandle);
                case CANOPEN_NETWORK_INDICATION: return Process_NetworkIndication(pCommand_PS, pDeviceCommandSetManager, h_Handle, hTransactionHandle);
                case CANOPEN_ABORT_SDO_TRANSFER: return Process_AbortSDOTransfer(pCommand_PS, pDeviceCommandSetManager, h_Handle, hTransactionHandle);
                case CANOPEN_SEND_NMT_SERVICE: return Process_SendNMTService(pCommand_PS, pDeviceCommandSetManager, h_Handle, hTransactionHandle);
                case CANOPEN_SEND_CAN_FRAME: return Process_SendCANFrame(pCommand_PS, pDeviceCommandSetManager, h_Handle, hTransactionHandle);
                case CANOPEN_READ_CAN_FRAME: return Process_ReadCANFrame(pCommand_PS, pDeviceCommandSetManager, h_Handle, hTransactionHandle);
                case CANOPEN_REQUEST_CAN_FRAME: return Process_RequestCANFrame(pCommand_PS, pDeviceCommandSetManager, h_Handle, hTransactionHandle);
                case CANOPEN_SEND_LSS_FRAME: return Process_SendLSSFrame(pCommand_PS, pDeviceCommandSetManager, h_Handle, hTransactionHandle);
                case CANOPEN_READ_LSS_FRAME: return Process_ReadLSSFrame(pCommand_PS, pDeviceCommandSetManager, h_Handle, hTransactionHandle);
            }
        }
    }

    return FALSE;
}
BOOL CGatewayCANopenToEsam2::Process_InitiateSDODownload(CCommand_PS* pCommand, CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle)
{
    const DWORD k_MaxDataSize = 4;

    //Constants CANopen
    const int k_ParameterIndex_CobIdClientServer = 0;
    const int k_ParameterIndex_CobIdServerClient = 1;
    const int k_ParameterIndex_ExpeditedTransfer = 2;
    const int k_ParameterIndex_SizeIndicated = 3;
    const int k_ParameterIndex_NonValidNbOfBytes = 4;
    const int k_ParameterIndex_Index = 5;
    const int k_ParameterIndex_SubIndex = 6;
    const int k_ParameterIndex_Data = 7;

    const int k_ReturnParameterIndex_CobIdServerClient = 0;
    const int k_ReturnParameterIndex_Index = 1;
    const int k_ReturnParameterIndex_SubIndex = 2;
    const int k_ReturnParameterIndex_AbortCode = 3;

    //Parameter CANopen
    DWORD dCobIdClientServer = 0;
    DWORD dCobIdServerClient = 0;
    BOOL oExpeditedTransfer = FALSE;
    BOOL oSizeIndicated = FALSE;
    BYTE uNonValidNbOfBytes = 0;
    WORD wIndex = 0;
    BYTE uSubIndex = 0;
    void* pDataBuffer = NULL;
    DWORD dDataBufferLength = 0;
    DWORD dObjectLength = 0;

    //ReturnParameter CANopen
    DWORD dAbortCode = 0;

    //Address Parameter
    WORD wNetworkId = 0;
    BYTE uNodeId = 0;

    BOOL oResult = FALSE;
    CErrorInfo errorInfo;

    if(pCommand)
    {
        //Lock CriticalSection
        if(IsNetworkIndicationThread())
        {
            //Already locked by NetworkIndication
            if(!IsLocked(pCommand)) return FALSE;
        }
        else
        {
            //No NetworkIndication received
            if(!Lock(pCommand)) return FALSE;
        }

        //Init
        oResult = TRUE;

        //Prepare DataBuffer
        dDataBufferLength = pCommand->GetParameterLength(k_ParameterIndex_Data);
        if(dDataBufferLength > 0) pDataBuffer = malloc(dDataBufferLength);

        //Get ParameterData CANopen
        pCommand->GetParameterData(k_ParameterIndex_CobIdClientServer, &dCobIdClientServer, sizeof(dCobIdClientServer));
        pCommand->GetParameterData(k_ParameterIndex_CobIdServerClient, &dCobIdServerClient, sizeof(dCobIdServerClient));
        pCommand->GetParameterData(k_ParameterIndex_ExpeditedTransfer, &oExpeditedTransfer, sizeof(oExpeditedTransfer));
        pCommand->GetParameterData(k_ParameterIndex_SizeIndicated, &oSizeIndicated, sizeof(oSizeIndicated));
        pCommand->GetParameterData(k_ParameterIndex_NonValidNbOfBytes, &uNonValidNbOfBytes, sizeof(uNonValidNbOfBytes));
        pCommand->GetParameterData(k_ParameterIndex_Index, &wIndex, sizeof(wIndex));
        pCommand->GetParameterData(k_ParameterIndex_SubIndex, &uSubIndex, sizeof(uSubIndex));
        pCommand->GetParameterData(k_ParameterIndex_Data, pDataBuffer, dDataBufferLength);

        //Get Address Parameter
        if(oResult) oResult = GetNetworkId(pCommand, pDeviceCommandSetManager, hDCS_Handle, wNetworkId, &errorInfo);
        if(oResult) oResult = GetNodeId(dCobIdClientServer, uNodeId, &errorInfo);

        //Check DataBuffer Length
        dDataBufferLength = CheckDataBufferLength(oSizeIndicated, uNonValidNbOfBytes, dDataBufferLength, k_MaxDataSize);

        //Translate Command
        if(oExpeditedTransfer)
        {
            //Expedited Transfer
            if(oResult) oResult = DCS_WriteObject(pDeviceCommandSetManager, hDCS_Handle, hTransactionHandle, wNetworkId, uNodeId, wIndex, uSubIndex, (BYTE*)pDataBuffer, dDataBufferLength, &errorInfo);
            dAbortCode = errorInfo.GetErrorCode();
        }
        else
        {
            //NonExpedited Transfer
            if(oSizeIndicated && (dDataBufferLength >= k_MaxDataSize))
            {
                dObjectLength = *((DWORD*)pDataBuffer);
                if(oResult) oResult = DCS_InitiateSegmentedWrite(pDeviceCommandSetManager, hDCS_Handle, hTransactionHandle, wNetworkId, uNodeId, wIndex, uSubIndex, dObjectLength, &errorInfo);
                dAbortCode = errorInfo.GetErrorCode();
                m_oToggle = FALSE;
                m_WriteSegmentSize = k_MaxEsam2DataSize;
            }
            else
            {
                oResult = FALSE;
                if(m_pErrorHandling) m_pErrorHandling->GetError(k_Error_Internal, &errorInfo);
                dAbortCode = errorInfo.GetErrorCode();
            }
        }

        //Set ReturnParameterData CANopen
        pCommand->SetStatus(oResult, &errorInfo);
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_CobIdServerClient, &dCobIdServerClient, sizeof(dCobIdServerClient));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_Index, &wIndex, sizeof(wIndex));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_SubIndex, &uSubIndex, sizeof(uSubIndex));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_AbortCode, &dAbortCode, sizeof(dAbortCode));

        //Free DataBuffer
        if(pDataBuffer) free(pDataBuffer);
        ResetBufferedData();

        //Unlock CriticalSection
        if(oExpeditedTransfer || !oResult)
        {
            ResetNetworkIndication();
            Unlock();
        }
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::Process_AbortSDOTransfer(CCommand_PS* pCommand, CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle)
{
    //*Constants PS*
    const int k_ParameterIndex_CobIdClientServer = 0;
    const int k_ParameterIndex_Index = 1;
    const int k_ParameterIndex_SubIndex = 2;
    const int k_ParameterIndex_AbortCode = 3;

    //*Variables PS*
    //Parameter
    DWORD dCobIdClientServer = 0;
    WORD wIndex = 0;
    BYTE uSubIndex = 0;
    DWORD dAbortCode = 0;

    //Address Parameter
    WORD wNetworkId = 0;
    BYTE uNodeId = 0;

    BOOL oResult = FALSE;
    CErrorInfo errorInfo;

    if(pCommand)
    {
        //Init
        oResult = TRUE;

        //Get PS Parameter Data
        pCommand->GetParameterData(k_ParameterIndex_CobIdClientServer, &dCobIdClientServer, sizeof(dCobIdClientServer));
        pCommand->GetParameterData(k_ParameterIndex_Index, &wIndex, sizeof(wIndex));
        pCommand->GetParameterData(k_ParameterIndex_SubIndex, &uSubIndex, sizeof(uSubIndex));
        pCommand->GetParameterData(k_ParameterIndex_AbortCode, &dAbortCode, sizeof(dAbortCode));

        //Get Address Parameter
        if(oResult) oResult = GetNetworkId(pCommand, pDeviceCommandSetManager, hDCS_Handle, wNetworkId, &errorInfo);
        if(oResult) oResult = GetNodeId(dCobIdClientServer, uNodeId, &errorInfo);

        //Execute Command
        if(oResult) oResult = DCS_AbortSegmentedTransfer(pDeviceCommandSetManager, hDCS_Handle, hTransactionHandle, wNetworkId, uNodeId, wIndex, uSubIndex, dAbortCode, &errorInfo);

        //Set PS ReturnParameter Data
        pCommand->SetStatus(oResult, &errorInfo);

        //Unlock CriticalSection
        ResetNetworkIndication();
        Unlock();
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::Process_NetworkIndication(CCommand_PS* pCommand, CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle)
{
    //*Constants PS*
    const int k_ParameterIndex_CobIdClientServer = 0;
    const int k_ParameterIndex_CobIdServerClient = 1;
    const int k_ParameterIndex_TargetNetworkId = 2;
    const int k_ParameterIndex_TargetNodeId = 3;

    const int k_ReturnParameterIndex_CobIdServerClient = 0;
    const int k_ReturnParameterIndex_TargetNetworkId = 1;
    const int k_ReturnParameterIndex_TargetNodeId = 2;
    const int k_ReturnParameterIndex_AbortCode = 3;

    //*Variables PS*
    //Parameter
    DWORD dCobIdClientServer = 0;
    DWORD dCobIdServerClient = 0;
    WORD wTargetNetworkId = 0;
    BYTE uTargetNodeId = 0;

    //ReturnParameter
    DWORD dRetCobIdServerClient = 0;
    WORD wRetTargetNetworkId = 0;
    BYTE uRetTargetNodeId = 0;
    DWORD dAbortCode = 0;

    BOOL oResult = FALSE;
    CErrorInfo errorInfo;

    if(pCommand)
    {
        //Lock CriticalSection
        if(!Lock(pCommand)) return FALSE;

        //Init
        oResult = TRUE;

        //Get PS Parameter Data
        pCommand->GetParameterData(k_ParameterIndex_CobIdClientServer, &dCobIdClientServer, sizeof(dCobIdClientServer));
        pCommand->GetParameterData(k_ParameterIndex_CobIdServerClient, &dCobIdServerClient, sizeof(dCobIdServerClient));
        pCommand->GetParameterData(k_ParameterIndex_TargetNetworkId, &wTargetNetworkId, sizeof(wTargetNetworkId));
        pCommand->GetParameterData(k_ParameterIndex_TargetNodeId, &uTargetNodeId, sizeof(uTargetNodeId));

        //No Corresonding Command -> Save Information for SDO Service
        if(oResult) oResult = SetNetworkIndication(wTargetNetworkId, uTargetNodeId);

        //Set PS ReturnParameter Data
        pCommand->SetStatus(oResult, &errorInfo);
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_CobIdServerClient, &dCobIdServerClient, sizeof(dCobIdServerClient));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_TargetNetworkId, &wTargetNetworkId, sizeof(wTargetNetworkId));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_TargetNodeId, &uTargetNodeId, sizeof(uTargetNodeId));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_AbortCode, &dAbortCode, sizeof(dAbortCode));

        //Unlock CriticalSection
        if(!oResult)
        {
            ResetNetworkIndication();
            Unlock();
        }
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::Process_DownloadSDOSegment(CCommand_PS* pCommand, CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle)
{
    const DWORD k_MaxCANopenDataSize = 7;

    //Constants PS CANopen
    const int k_ParameterIndex_CobIdClientServer = 0;
    const int k_ParameterIndex_CobIdServerClient = 1;
    const int k_ParameterIndex_Toggle = 2;
    const int k_ParameterIndex_NonValidNbOfBytes = 3;
    const int k_ParameterIndex_NoMoreSegments = 4;
    const int k_ParameterIndex_SegData = 5;

    const int k_ReturnParameterIndex_CobIdServerClient = 0;
    const int k_ReturnParameterIndex_Toggle = 1;
    const int k_ReturnParameterIndex_AbortCode = 2;

    //Parameter CANopen
    DWORD dCobIdClientServer = 0;
    DWORD dCobIdServerClient = 0;
    BOOL oToggle = FALSE;
    BYTE uNonValidNbOfBytes = 0;
    BOOL oNoMoreSegments = FALSE;
    void* pSegDataBuffer = NULL;
    DWORD dSegDataBufferLength = 0;

    //ReturnParameter CANopen
    DWORD dAbortCode = 0;

    //ReturnParameter Esam2
    void* pDataBuffer = NULL;
    DWORD dDataBufferLength = 0;
    DWORD dDataLengthRead = 0;
    DWORD dDataLengthWritten = 0;

    //Address Parameter
    WORD wNetworkId = 0;

    BOOL oResult = FALSE;
    CErrorInfo errorInfo;

    if(pCommand)
    {
        //Check CriticalSection
        if(!IsLocked(pCommand)) return FALSE;

        //Init
        oResult = TRUE;

        //Prepare DataBuffer
        dSegDataBufferLength = pCommand->GetParameterLength(k_ParameterIndex_SegData);
        if(dSegDataBufferLength > 0) pSegDataBuffer = malloc(dSegDataBufferLength);
        dDataBufferLength = m_WriteSegmentSize;
        if(dDataBufferLength > 0) pDataBuffer = malloc(dDataBufferLength);

        //Get ParameterData CANopen
        pCommand->GetParameterData(k_ParameterIndex_CobIdClientServer, &dCobIdClientServer, sizeof(dCobIdClientServer));
        pCommand->GetParameterData(k_ParameterIndex_CobIdServerClient, &dCobIdServerClient, sizeof(dCobIdServerClient));
        pCommand->GetParameterData(k_ParameterIndex_Toggle, &oToggle, sizeof(oToggle));
        pCommand->GetParameterData(k_ParameterIndex_NonValidNbOfBytes, &uNonValidNbOfBytes, sizeof(uNonValidNbOfBytes));
        pCommand->GetParameterData(k_ParameterIndex_NoMoreSegments, &oNoMoreSegments, sizeof(oNoMoreSegments));
        pCommand->GetParameterData(k_ParameterIndex_SegData, pSegDataBuffer, dSegDataBufferLength);

        //Get Address Parameter
        if(oResult) oResult = GetNetworkId(pCommand, pDeviceCommandSetManager, hDCS_Handle, wNetworkId, &errorInfo);

        //Check DataBuffer Length
        dSegDataBufferLength = CheckDataBufferLength(TRUE, uNonValidNbOfBytes, dSegDataBufferLength, k_MaxCANopenDataSize);

        //Translate Command
        oResult = AddBufferedData(pSegDataBuffer, dSegDataBufferLength, oNoMoreSegments);
        if(oResult)
        {
            while(GetBufferedData(pDataBuffer, dDataBufferLength, &dDataLengthRead, &oNoMoreSegments))
            {
                //AbortCommand
                if(m_oAbortCommands)
                {
                    if(m_pErrorHandling) m_pErrorHandling->GetError(k_Error_CommandAbortedByUser, &errorInfo);
                    oResult = FALSE;
                    break;
                }

                oResult = DCS_SegmentedWrite(pDeviceCommandSetManager, hDCS_Handle, hTransactionHandle, wNetworkId, m_oToggle, oNoMoreSegments, pDataBuffer, dDataLengthRead, &dDataLengthWritten, &errorInfo);
                m_oToggle = !m_oToggle;

                //Update SegmentLength
                if(dDataLengthWritten < dDataLengthRead)
                {
                    ReverseBufferedData(dDataLengthRead-dDataLengthWritten);
                    oNoMoreSegments = FALSE;
                    if(pDataBuffer)
                    {
                        free(pDataBuffer);
                        pDataBuffer = NULL;

                        m_WriteSegmentSize = dDataLengthWritten;
                        dDataBufferLength = dDataLengthWritten;

                        if(dDataBufferLength > 0) pDataBuffer = malloc(dDataBufferLength);
                    }
                }

                if(oNoMoreSegments) break;
            }
        }
        dAbortCode = errorInfo.GetErrorCode();

        //Set ReturnParameterData CANopen
        pCommand->SetStatus(oResult, &errorInfo);
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_CobIdServerClient, &dCobIdServerClient, sizeof(dCobIdServerClient));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_Toggle, &oToggle, sizeof(oToggle));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_AbortCode, &dAbortCode, sizeof(dAbortCode));

        //Free DataBuffer
        if(pSegDataBuffer) free(pSegDataBuffer);
        if(pDataBuffer) free(pDataBuffer);

        //Unlock CriticalSection
        if(oNoMoreSegments || !oResult)
        {
            ResetBufferedData();
            ResetNetworkIndication();
            Unlock();
        }
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::Process_InitiateSDOUpload(CCommand_PS* pCommand, CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle)
{
    const DWORD k_MaxDataSize = 4;

    //Constants CANopen
    const int k_ParameterIndex_CobIdClientServer = 0;
    const int k_ParameterIndex_CobIdServerClient = 1;
    const int k_ParameterIndex_Index = 2;
    const int k_ParameterIndex_SubIndex = 3;

    const int k_ReturnParameterIndex_CobIdServerClient = 0;
    const int k_ReturnParameterIndex_ExpeditedTransfer = 1;
    const int k_ReturnParameterIndex_SizeIndicated = 2;
    const int k_ReturnParameterIndex_NonValidNbOfBytes = 3;
    const int k_ReturnParameterIndex_Index = 4;
    const int k_ReturnParameterIndex_SubIndex = 5;
    const int k_ReturnParameterIndex_Data = 6;
    const int k_ReturnParameterIndex_AbortCode = 7;

    //Parameter CANopen
    DWORD dCobIdClientServer = 0;
    DWORD dCobIdServerClient = 0;
    WORD wIndex = 0;
    BYTE uSubIndex = 0;

    //ReturnParameter CANopen
    BOOL oRetExpeditedTransfer = FALSE;
    BOOL oRetSizeIndicated = FALSE;
    BYTE uRetNonValidNbOfBytes = 0;
    void* pRetDataBuffer = NULL;
    DWORD dRetDataBufferLength = 0;
    DWORD dAbortCode = 0;

    //Address Parameter
    WORD wNetworkId = 0;
    BYTE uNodeId = 0;

    //ReturnParameter
    DWORD dObjectLength = 0;

    BOOL oResult = FALSE;
    CErrorInfo errorInfo;

    if(pCommand)
    {
        //Lock CriticalSection
        if(IsNetworkIndicationThread())
        {
            //Already locked by NetworkIndication
            if(!IsLocked(pCommand)) return FALSE;
        }
        else
        {
            //No NetworkIndication received
            if(!Lock(pCommand)) return FALSE;
        }

        //Init
        oResult = TRUE;

        //Prepare DataBuffer
        dRetDataBufferLength = k_MaxDataSize;
        pRetDataBuffer = malloc(dRetDataBufferLength);
        memset(pRetDataBuffer, 0, dRetDataBufferLength);

        //Get ParameterData CANopen
        pCommand->GetParameterData(k_ParameterIndex_CobIdClientServer, &dCobIdClientServer, sizeof(dCobIdClientServer));
        pCommand->GetParameterData(k_ParameterIndex_CobIdServerClient, &dCobIdServerClient, sizeof(dCobIdServerClient));
        pCommand->GetParameterData(k_ParameterIndex_Index, &wIndex, sizeof(wIndex));
        pCommand->GetParameterData(k_ParameterIndex_SubIndex, &uSubIndex, sizeof(uSubIndex));

        //Get Address Parameter
        if(oResult) oResult = GetNetworkId(pCommand, pDeviceCommandSetManager, hDCS_Handle, wNetworkId, &errorInfo);
        if(oResult) oResult = GetNodeId(dCobIdClientServer, uNodeId, &errorInfo);

        //Translate Command
        if(oResult)
        {
            if(oResult = DCS_ReadObject(pDeviceCommandSetManager, hDCS_Handle, hTransactionHandle, wNetworkId, uNodeId, wIndex, uSubIndex, (BYTE*)pRetDataBuffer, dRetDataBufferLength, &dObjectLength, &errorInfo))
            {
                //Expedited Transfer
                oRetExpeditedTransfer = TRUE;
                oRetSizeIndicated = FALSE;
                uRetNonValidNbOfBytes = 0;
                dAbortCode = errorInfo.GetErrorCode();
            }
            else if(errorInfo.GetErrorCode() == ERROR_DEVICE_ESAM2_SEGMENTED_TRANSFER_REQUIRED_INIT_DONE)
            {
                //Non Expedited Transfer -> Initialisation already done
                oRetExpeditedTransfer = FALSE;
                oRetSizeIndicated = TRUE;
                uRetNonValidNbOfBytes = 0;
                dAbortCode = 0;
                oResult = TRUE;
                m_oToggle = FALSE;
            }
            else if(oResult = DCS_InitiateSegmentedRead(pDeviceCommandSetManager, hDCS_Handle, hTransactionHandle, wNetworkId, uNodeId, wIndex, uSubIndex, &dObjectLength, &errorInfo))
            {
                //Non Expedited Transfer
                oRetExpeditedTransfer = FALSE;
                oRetSizeIndicated = TRUE;
                uRetNonValidNbOfBytes = 0;
                *((DWORD*)pRetDataBuffer) = dObjectLength;
                dAbortCode = errorInfo.GetErrorCode();
                m_oToggle = FALSE;
            }
            else
            {
                //Failed
                dAbortCode = errorInfo.GetErrorCode();
            }
        }

        //Set ReturnParameterData CANopen
        pCommand->SetStatus(oResult, &errorInfo);
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_CobIdServerClient, &dCobIdServerClient, sizeof(dCobIdServerClient));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_ExpeditedTransfer, &oRetExpeditedTransfer, sizeof(oRetExpeditedTransfer));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_SizeIndicated, &oRetSizeIndicated, sizeof(oRetSizeIndicated));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_NonValidNbOfBytes, &uRetNonValidNbOfBytes, sizeof(uRetNonValidNbOfBytes));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_Index, &wIndex, sizeof(wIndex));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_SubIndex, &uSubIndex, sizeof(uSubIndex));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_Data, pRetDataBuffer, dRetDataBufferLength);
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_AbortCode, &dAbortCode, sizeof(dAbortCode));

        //Free DataBuffer
        if(pRetDataBuffer) free(pRetDataBuffer);
        ResetBufferedData();

        //Unlock CriticalSection
        if(oRetExpeditedTransfer || !oResult)
        {
            ResetNetworkIndication();
            Unlock();
        }
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::Process_UploadSDOSegment(CCommand_PS* pCommand, CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle)
{
    const DWORD k_MaxCANopenDataSize = 7;

    //Constants CANopen
    const int k_ParameterIndex_CobIdClientServer = 0;
    const int k_ParameterIndex_CobIdServerClient = 1;
    const int k_ParameterIndex_Toggle = 2;

    const int k_ReturnParameterIndex_CobIdServerClient = 0;
    const int k_ReturnParameterIndex_Toggle = 1;
    const int k_ReturnParameterIndex_NonValidNbOfBytes = 2;
    const int k_ReturnParameterIndex_NoMoreSegments = 3;
    const int k_ReturnParameterIndex_SegData = 4;
    const int k_ReturnParameterIndex_AbortCode = 5;

    //Parameter CANopen
    DWORD dCobIdClientServer = 0;
    DWORD dCobIdServerClient = 0;
    BOOL oToggle = FALSE;

    //ReturnParameter CANopen
    BYTE uRetNonValidNbOfBytes = 0;
    BOOL oRetNoMoreSegments = FALSE;
    void* pRetSegDataBuffer = NULL;
    DWORD dRetSegDataBufferLength = 0;
    DWORD dSegmentLengthRead = 0;
    DWORD dAbortCode = 0;

    //Address Parameter
    WORD wNetworkId = 0;

    //ReturnParameter Esam
    void* pDataBuffer = NULL;
    DWORD dDataBufferLength = 0;

    BOOL oResult = FALSE;
    CErrorInfo errorInfo;

    if(pCommand)
    {
        //Check CriticalSection
        if(!IsLocked(pCommand)) return FALSE;

        //Init
        oResult = TRUE;

        //Prepare DataBuffer
        dRetSegDataBufferLength = k_MaxCANopenDataSize;
        pRetSegDataBuffer = malloc(dRetSegDataBufferLength);
        memset(pRetSegDataBuffer, 0, dRetSegDataBufferLength);

        //Get ParameterData CANopen
        pCommand->GetParameterData(k_ParameterIndex_CobIdClientServer, &dCobIdClientServer, sizeof(dCobIdClientServer));
        pCommand->GetParameterData(k_ParameterIndex_CobIdServerClient, &dCobIdServerClient, sizeof(dCobIdServerClient));
        pCommand->GetParameterData(k_ParameterIndex_Toggle, &oToggle, sizeof(oToggle));

        //Get Address Parameter
        if(oResult) oResult = GetNetworkId(pCommand, pDeviceCommandSetManager, hDCS_Handle, wNetworkId, &errorInfo);

        //Translate Command
        oResult = GetBufferedData(pRetSegDataBuffer, dRetSegDataBufferLength, &dSegmentLengthRead, &oRetNoMoreSegments);
        if(!oResult)
        {
            oResult = DCS_SegmentedRead(pDeviceCommandSetManager, hDCS_Handle, hTransactionHandle, wNetworkId, m_oToggle, &oRetNoMoreSegments, &pDataBuffer, &dDataBufferLength, &errorInfo);
            if(oResult)
            {
                m_oToggle = !m_oToggle;
                AddBufferedData(pDataBuffer, dDataBufferLength, oRetNoMoreSegments);
                oResult = GetBufferedData(pRetSegDataBuffer, dRetSegDataBufferLength, &dSegmentLengthRead, &oRetNoMoreSegments);
            }
        }

        //CheckSize
        if(oResult && (dSegmentLengthRead <= k_MaxCANopenDataSize))
        {
            uRetNonValidNbOfBytes = (BYTE)(k_MaxCANopenDataSize-dSegmentLengthRead);
            dAbortCode = errorInfo.GetErrorCode();
        }
        else if(oResult)
        {
            oResult = FALSE;
            if(m_pErrorHandling) m_pErrorHandling->GetError(k_Error_Internal, &errorInfo);
            dAbortCode = errorInfo.GetErrorCode();
        }
        else
        {
            uRetNonValidNbOfBytes = k_MaxCANopenDataSize;
            dAbortCode = errorInfo.GetErrorCode();
        }

        //Set ReturnParameterData CANopen
        pCommand->SetStatus(oResult, &errorInfo);
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_CobIdServerClient, &dCobIdServerClient, sizeof(dCobIdServerClient));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_Toggle, &oToggle, sizeof(oToggle));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_NonValidNbOfBytes, &uRetNonValidNbOfBytes, sizeof(uRetNonValidNbOfBytes));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_NoMoreSegments, &oRetNoMoreSegments, sizeof(oRetNoMoreSegments));
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_SegData, pRetSegDataBuffer, dSegmentLengthRead);
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_AbortCode, &dAbortCode, sizeof(dAbortCode));

        //Free DataBuffer
        if(pRetSegDataBuffer) free(pRetSegDataBuffer);
        if(pDataBuffer) free(pDataBuffer);

        //Unlock CriticalSection
        if(oRetNoMoreSegments || !oResult)
        {
            ResetBufferedData();
            ResetNetworkIndication();
            Unlock();
        }
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::Process_SendNMTService(CCommand_PS* pCommand, CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle)
{
    //Constants CANopen
    const int k_ParameterIndex_CommandSpecifier = 0;
    const int k_ParameterIndex_NodeId = 1;

    //Parameter CANopen
    BYTE uCommandSpecifier = 0;
    BYTE uNodeId = 0;

    //Address Parameter
    WORD wNetworkId = 0;

    BOOL oResult = FALSE;
    CErrorInfo errorInfo;

    if(pCommand)
    {
        //Lock CriticalSection
        if(!Lock(pCommand)) return FALSE;

        //Init
        oResult = TRUE;

        //Get ParameterData CANopen
        pCommand->GetParameterData(k_ParameterIndex_CommandSpecifier, &uCommandSpecifier, sizeof(uCommandSpecifier));
        pCommand->GetParameterData(k_ParameterIndex_NodeId, &uNodeId, sizeof(uNodeId));

        //Get Address Parameter
        if(oResult) oResult = GetNetworkId(pCommand, pDeviceCommandSetManager, hDCS_Handle, wNetworkId, &errorInfo);

        //Translate Command
        if(oResult) oResult = DCS_SendNMTService(pDeviceCommandSetManager, hDCS_Handle, hTransactionHandle, wNetworkId, (WORD)uNodeId, (WORD)uCommandSpecifier, &errorInfo);

        //Set ReturnParameterData CANopen
        pCommand->SetStatus(oResult, &errorInfo);

        //Unlock CriticalSection
        Unlock();
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::Process_SendCANFrame(CCommand_PS* pCommand, CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle)
{
    //Constants CANopen
    const int k_ParameterIndex_CobId = 0;
    const int k_ParameterIndex_Length = 1;
    const int k_ParameterIndex_Data = 2;

    //Parameter CANopen
    DWORD dCobId = 0;
    BYTE uLength = 0;
    void* pDataBuffer = NULL;
    DWORD dDataBufferLength = 0;

    //Address Parameter
    WORD wRemoteNetworkId = 0;

    BOOL oResult = FALSE;
    CErrorInfo errorInfo;

    if(pCommand)
    {
        //Lock CriticalSection
        if(!Lock(pCommand)) return FALSE;

        //Init
        oResult = TRUE;

        //Prepare DataBuffer
        dDataBufferLength = pCommand->GetParameterLength(k_ParameterIndex_Data);
        if(dDataBufferLength > 0) pDataBuffer = malloc(dDataBufferLength);

        //Get ParameterData CANopen
        pCommand->GetParameterData(k_ParameterIndex_CobId, &dCobId, sizeof(dCobId));
        pCommand->GetParameterData(k_ParameterIndex_Length, &uLength, sizeof(uLength));
        pCommand->GetParameterData(k_ParameterIndex_Data, pDataBuffer, dDataBufferLength);

        //Get Address Parameter
        if(oResult) oResult = GetRemoteNetworkId(pCommand, pDeviceCommandSetManager, hDCS_Handle, wRemoteNetworkId, &errorInfo);

        //Check Length
        if(uLength > dDataBufferLength) uLength = (BYTE)dDataBufferLength;

        //Translate Command
        if(oResult) oResult = DCS_SendCANFrame(pDeviceCommandSetManager, hDCS_Handle, hTransactionHandle, wRemoteNetworkId, (WORD)dCobId, (WORD)uLength, pDataBuffer, &errorInfo);

        //Set ReturnParameterData CANopen
        pCommand->SetStatus(oResult, &errorInfo);

        //Free DataBuffer
        if(pDataBuffer) free(pDataBuffer);

        //Unlock CriticalSection
        Unlock();
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::Process_ReadCANFrame(CCommand_PS* pCommand, CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle)
{
    //Not Supported
    return FALSE;
}

BOOL CGatewayCANopenToEsam2::Process_RequestCANFrame(CCommand_PS* pCommand, CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle)
{
    //Constants CANopen
    const int k_ParameterIndex_CobId = 0;
    const int k_ParameterIndex_Length = 1;

    const int k_ReturnParameterIndex_Data = 0;

    //Parameter CANopen
    DWORD dCobId = 0;
    BYTE uLength = 0;

    //ReturnParameter CANopen
    void* pRetDataBuffer = NULL;
    DWORD dRetDataBufferLength = 0;

    //Address Parameter
    WORD wRemoteNetworkId = 0;

    BOOL oResult = FALSE;
    CErrorInfo errorInfo;

    if(pCommand)
    {
        //Lock CriticalSection
        if(!Lock(pCommand)) return FALSE;

        //Init
        oResult = TRUE;

        //Get ParameterData CANopen
        pCommand->GetParameterData(k_ParameterIndex_CobId, &dCobId, sizeof(dCobId));
        pCommand->GetParameterData(k_ParameterIndex_Length, &uLength, sizeof(uLength));

        //Get Address Parameter
        if(oResult) oResult = GetRemoteNetworkId(pCommand, pDeviceCommandSetManager, hDCS_Handle, wRemoteNetworkId, &errorInfo);

        //Prepare DataBuffer
        dRetDataBufferLength = uLength;
        if(dRetDataBufferLength > 0)
        {
            pRetDataBuffer = malloc(dRetDataBufferLength);
            memset(pRetDataBuffer, 0, dRetDataBufferLength);
        }

        //Translate Command
        oResult = DCS_RequestCANFrame(pDeviceCommandSetManager, hDCS_Handle, hTransactionHandle, wRemoteNetworkId, (WORD)dCobId, (WORD)uLength, pRetDataBuffer, &errorInfo);

        //Set ReturnParameterData CANopen
        pCommand->SetStatus(oResult, &errorInfo);
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_Data, pRetDataBuffer, dRetDataBufferLength);

        //Free DataBuffer
        if(pRetDataBuffer) free(pRetDataBuffer);

        //Unlock CriticalSection
        Unlock();
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::Process_SendLSSFrame(CCommand_PS* pCommand, CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle)
{
    //*Constants PS*
    const int k_ParameterIndex_Data = 0;

    //*Variables PS*
    //Parameter
    void* pDataBuffer = NULL;
    DWORD dDataBufferLength = 0;

    //Address Parameter
    WORD wRemoteNetworkId = 0;

    BOOL oResult = FALSE;
    CErrorInfo errorInfo;

    if(pCommand)
    {
        //Lock CriticalSection
        if(!Lock(pCommand)) return FALSE;

        //Init
        oResult = TRUE;

        //Prepare DataBuffer
        dDataBufferLength = pCommand->GetParameterLength(k_ParameterIndex_Data);
        if(dDataBufferLength > 0) pDataBuffer = malloc(dDataBufferLength);

        //Get PS Parameter Data
        pCommand->GetParameterData(k_ParameterIndex_Data, pDataBuffer, dDataBufferLength);

        //Get Address Parameter
        if(oResult) oResult = GetRemoteNetworkId(pCommand, pDeviceCommandSetManager, hDCS_Handle, wRemoteNetworkId, &errorInfo);

        //Translate Command
        if(oResult) oResult = DCS_SendLSSFrame(pDeviceCommandSetManager, hDCS_Handle, hTransactionHandle, wRemoteNetworkId, pDataBuffer, dDataBufferLength, &errorInfo);

        //Set PS ReturnParameter Data
        pCommand->SetStatus(oResult, &errorInfo);

        //Free DataBuffer
        if(pDataBuffer) free(pDataBuffer);

        //Unlock CriticalSection
        Unlock();
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::Process_ReadLSSFrame(CCommand_PS* pCommand, CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle)
{
    //*Constants PS*
    const int k_FrameSize = 8;
    const int k_ParameterIndex_Timeout = 0;
    const int k_ReturnParameterIndex_Data = 0;

    //*Variables PS*
    //Parameter
    WORD wTimeout = 0;

    //ReturnParameter
    void* pRetDataBuffer = NULL;
    DWORD dRetDataBufferLength = 0;

    //Address Parameter
    WORD wRemoteNetworkId = 0;

    BOOL oResult = FALSE;
    CErrorInfo errorInfo;

    if(pCommand)
    {
        //Lock CriticalSection
        if(!Lock(pCommand)) return FALSE;

        //Init
        oResult = TRUE;

        //Prepare RetDataBuffer
        dRetDataBufferLength = k_FrameSize;
        if(dRetDataBufferLength > 0) pRetDataBuffer = malloc(dRetDataBufferLength);

        //Get PS Parameter Data
        pCommand->GetParameterData(k_ParameterIndex_Timeout, &wTimeout, sizeof(wTimeout));

        //Get Address Parameter
        if(oResult) oResult = GetRemoteNetworkId(pCommand, pDeviceCommandSetManager, hDCS_Handle, wRemoteNetworkId, &errorInfo);

        //Translate Command
        oResult = DCS_ReadLSSFrame(pDeviceCommandSetManager, hDCS_Handle, hTransactionHandle, wRemoteNetworkId, wTimeout, pRetDataBuffer, dRetDataBufferLength, &errorInfo);

        //Set PS ReturnParameter Data
        pCommand->SetStatus(oResult, &errorInfo);
        pCommand->SetReturnParameterData(k_ReturnParameterIndex_Data, pRetDataBuffer, dRetDataBufferLength);

        //Free DataBuffer
        if(pRetDataBuffer) free(pRetDataBuffer);

        //Unlock CriticalSection
        Unlock();
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::DCS_ReadObject(CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle, WORD wNetworkId, BYTE uNodeId, WORD wIndex, BYTE uSubIndex, BYTE* pObjectBuffer, DWORD dObjectBufferLength, DWORD* pdObjectLengthRead, CErrorInfo* pErrorInfo)
{
    DWORD dDeviceErrorCode = 0;
    DWORD dObjectLength = 0;
    BOOL oResult = FALSE;

    if(m_pCommand_ReadObject && pDeviceCommandSetManager)
    {
        //Set Parameter Data
        m_pCommand_ReadObject->ResetStatus();
        m_pCommand_ReadObject->SetParameterData(0, &wNetworkId, sizeof(wNetworkId));
        m_pCommand_ReadObject->SetParameterData(1, &uNodeId, sizeof(uNodeId));
        m_pCommand_ReadObject->SetParameterData(2, &wIndex, sizeof(wIndex));
        m_pCommand_ReadObject->SetParameterData(3, &uSubIndex, sizeof(uSubIndex));

        //Execute Command
        oResult = pDeviceCommandSetManager->ExecuteCommand(m_pCommand_ReadObject, hDCS_Handle, hTransactionHandle);

        //Get ReturnParameter Length
        dObjectLength = m_pCommand_ReadObject->GetReturnParameterLength(1);
        if(dObjectBufferLength < dObjectLength) dObjectLength = dObjectBufferLength;
        if(pdObjectLengthRead) *pdObjectLengthRead = dObjectLength;

        //Get ReturnParameter Data
        m_pCommand_ReadObject->GetReturnParameterData(0, &dDeviceErrorCode, sizeof(dDeviceErrorCode));
        m_pCommand_ReadObject->GetReturnParameterData(1, pObjectBuffer, dObjectLength);

        //Get ErrorCode
        m_pCommand_ReadObject->GetErrorInfo(pErrorInfo);
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::DCS_InitiateSegmentedRead(CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle, WORD wNetworkId, BYTE uNodeId, WORD wIndex, BYTE uSubIndex, DWORD* pdObjectLength, CErrorInfo* pErrorInfo)
{
    DWORD dDeviceErrorCode;
    BOOL oResult = FALSE;

    if(m_pCommand_InitiateSegmentedRead && pDeviceCommandSetManager)
    {
        //Set Parameter Data
        m_pCommand_InitiateSegmentedRead->ResetStatus();
        m_pCommand_InitiateSegmentedRead->SetParameterData(0, &wNetworkId, sizeof(wNetworkId));
        m_pCommand_InitiateSegmentedRead->SetParameterData(1, &uNodeId, sizeof(uNodeId));
        m_pCommand_InitiateSegmentedRead->SetParameterData(2, &wIndex, sizeof(wIndex));
        m_pCommand_InitiateSegmentedRead->SetParameterData(3, &uSubIndex, sizeof(uSubIndex));

        //Execute command
        oResult = pDeviceCommandSetManager->ExecuteCommand(m_pCommand_InitiateSegmentedRead, hDCS_Handle, hTransactionHandle);

        //Get ReturnParameter Data
        m_pCommand_InitiateSegmentedRead->GetReturnParameterData(0, &dDeviceErrorCode, sizeof(dDeviceErrorCode));
        m_pCommand_InitiateSegmentedRead->GetReturnParameterData(1, pdObjectLength, sizeof(*pdObjectLength));

        //Get ErrorCode
        m_pCommand_InitiateSegmentedRead->GetErrorInfo(pErrorInfo);
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::DCS_SegmentedRead(CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle, WORD wNetworkId, BOOL oToggle, BOOL* poLastDataSegment, void** ppSegmentBuffer, DWORD* pdSegmentBufferLength, CErrorInfo* pErrorInfo)
{
    UEsam2ControlByte controlByte;
    BYTE uDummyByte = 0;

    DWORD dDeviceErrorCode;
    BYTE uLength = 0;
    UEsam2ControlByte retControlByte;

    BOOL oResult = FALSE;

    if(m_pCommand_SegmentedRead && pDeviceCommandSetManager && ppSegmentBuffer && pdSegmentBufferLength)
    {
        //Prepare Parameter
        controlByte.ubValue = 0;
        controlByte.structure.ubToggle = oToggle;

        //Set Parameter Data
        m_pCommand_SegmentedRead->ResetStatus();
        m_pCommand_SegmentedRead->SetParameterData(0, &controlByte, sizeof(controlByte));
        m_pCommand_SegmentedRead->SetParameterData(1, &uDummyByte, sizeof(uDummyByte));

        //Execute Command
        oResult = pDeviceCommandSetManager->ExecuteCommand(m_pCommand_SegmentedRead, hDCS_Handle, hTransactionHandle);

        //Get ReturnParameter Data
        m_pCommand_SegmentedRead->GetReturnParameterData(0, &dDeviceErrorCode, sizeof(dDeviceErrorCode));
        m_pCommand_SegmentedRead->GetReturnParameterData(1, &uLength, sizeof(uLength));
        m_pCommand_SegmentedRead->GetReturnParameterData(2, &retControlByte, sizeof(retControlByte));

        //Prepare DataBufer
        *pdSegmentBufferLength = m_pCommand_SegmentedRead->GetReturnParameterLength(3);
        if(uLength < *pdSegmentBufferLength) *pdSegmentBufferLength = uLength;
        if(*pdSegmentBufferLength > 0)
        {
            *ppSegmentBuffer = malloc(*pdSegmentBufferLength);
            memset(*ppSegmentBuffer, 0, *pdSegmentBufferLength);
        }
        m_pCommand_SegmentedRead->GetReturnParameterData(3, *ppSegmentBuffer, *pdSegmentBufferLength);

        //Get ErrorCode
        m_pCommand_SegmentedRead->GetErrorInfo(pErrorInfo);

        //Restore ControlByte
        if(poLastDataSegment) *poLastDataSegment = retControlByte.structure.ubLastDataSegment;
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::DCS_InitiateSegmentedWrite(CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle, WORD wNetworkId, BYTE uNodeId, WORD wIndex, BYTE uSubIndex, DWORD dObjectLength, CErrorInfo* pErrorInfo)
{
    DWORD dDeviceErrorCode;
    BOOL oResult = FALSE;

    if(m_pCommand_InitiateSegmentedWrite && pDeviceCommandSetManager)
    {
        //Set Parameter Data
        m_pCommand_InitiateSegmentedWrite->ResetStatus();
        m_pCommand_InitiateSegmentedWrite->SetParameterData(0, &wNetworkId, sizeof(wNetworkId));
        m_pCommand_InitiateSegmentedWrite->SetParameterData(1, &uNodeId, sizeof(uNodeId));
        m_pCommand_InitiateSegmentedWrite->SetParameterData(2, &wIndex, sizeof(wIndex));
        m_pCommand_InitiateSegmentedWrite->SetParameterData(3, &uSubIndex, sizeof(uSubIndex));
        m_pCommand_InitiateSegmentedWrite->SetParameterData(4, &dObjectLength, sizeof(dObjectLength));

        //Execute command
        oResult = pDeviceCommandSetManager->ExecuteCommand(m_pCommand_InitiateSegmentedWrite, hDCS_Handle, hTransactionHandle);

        //Get ReturnParameter Data
        m_pCommand_InitiateSegmentedWrite->GetReturnParameterData(0, &dDeviceErrorCode, sizeof(dDeviceErrorCode));

        //Get ErrorCode
        m_pCommand_InitiateSegmentedWrite->GetErrorInfo(pErrorInfo);
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::DCS_SegmentedWrite(CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle, WORD wNetworkId, BOOL oToggle, BOOL oLastDataSegment, void* pSegmentBuffer, DWORD dSegmentLength, DWORD* pdSegmentLengthWritten, CErrorInfo* pErrorInfo)
{
    BYTE uLength = 0;
    UEsam2ControlByte controlByte;

    DWORD dDeviceErrorCode = 0;
    BYTE uLengthWritten = 0;
    UEsam2ControlByte retControlByte;

    BOOL oResult = FALSE;

    if(m_pCommand_SegmentedWrite && pDeviceCommandSetManager)
    {
        //Prepare Parameters
        uLength = (BYTE)dSegmentLength;
        controlByte.ubValue = 0;
        controlByte.structure.ubToggle = oToggle;
        controlByte.structure.ubLastDataSegment = oLastDataSegment;

        //Set Parameter Data
        m_pCommand_SegmentedWrite->ResetStatus();
        m_pCommand_SegmentedWrite->SetParameterData(0, &uLength, sizeof(uLength));
        m_pCommand_SegmentedWrite->SetParameterData(1, &controlByte, sizeof(controlByte));
        m_pCommand_SegmentedWrite->SetParameterData(2, pSegmentBuffer, dSegmentLength);

        //Execute Command
        oResult = pDeviceCommandSetManager->ExecuteCommand(m_pCommand_SegmentedWrite, hDCS_Handle, hTransactionHandle);

        //Get ReturnParameter Data
        m_pCommand_SegmentedWrite->GetReturnParameterData(0, &dDeviceErrorCode, sizeof(dDeviceErrorCode));
        m_pCommand_SegmentedWrite->GetReturnParameterData(1, &uLengthWritten, sizeof(uLengthWritten));
        m_pCommand_SegmentedWrite->GetReturnParameterData(2, &retControlByte, sizeof(retControlByte));

        //Restore Parameters
        if(pdSegmentLengthWritten) *pdSegmentLengthWritten = uLengthWritten;

        //Get ErrorCode
        m_pCommand_SegmentedWrite->GetErrorInfo(pErrorInfo);
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::DCS_AbortSegmentedTransfer(CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle, WORD wNetworkId, BYTE uNodeId, WORD wIndex, BYTE uSubIndex, DWORD dAbortCode, CErrorInfo* pErrorInfo)
{
    BOOL oResult = FALSE;

    if(m_pCommand_AbortSegmentedTransfer && pDeviceCommandSetManager)
    {
        //Set Parameter Data
        m_pCommand_AbortSegmentedTransfer->ResetStatus();
        m_pCommand_AbortSegmentedTransfer->SetParameterData(0, &wNetworkId, sizeof(wNetworkId));
        m_pCommand_AbortSegmentedTransfer->SetParameterData(1, &uNodeId, sizeof(uNodeId));
        m_pCommand_AbortSegmentedTransfer->SetParameterData(2, &wIndex, sizeof(wIndex));
        m_pCommand_AbortSegmentedTransfer->SetParameterData(3, &uSubIndex, sizeof(uSubIndex));
        m_pCommand_AbortSegmentedTransfer->SetParameterData(4, &dAbortCode, sizeof(dAbortCode));

        //Execute Command
        oResult = pDeviceCommandSetManager->ExecuteCommand(m_pCommand_AbortSegmentedTransfer, hDCS_Handle, hTransactionHandle);

        //Get ErrorCode
        m_pCommand_AbortSegmentedTransfer->GetErrorInfo(pErrorInfo);
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::DCS_WriteObject(CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle, WORD wNetworkId, BYTE uNodeId, WORD wIndex, BYTE uSubIndex, BYTE* pObjectBuffer, DWORD dObjectLength, CErrorInfo* pErrorInfo)
{
    DWORD dDeviceErrorCode = 0;
    BOOL oResult = FALSE;

    if(m_pCommand_WriteObject && pDeviceCommandSetManager)
    {
        //Set Parameter Data
        m_pCommand_WriteObject->ResetStatus();
        m_pCommand_WriteObject->SetParameterData(0, &wNetworkId, sizeof(wNetworkId));
        m_pCommand_WriteObject->SetParameterData(1, &uNodeId, sizeof(uNodeId));
        m_pCommand_WriteObject->SetParameterData(2, &wIndex, sizeof(wIndex));
        m_pCommand_WriteObject->SetParameterData(3, &uSubIndex, sizeof(uSubIndex));
        m_pCommand_WriteObject->SetParameterData(4, pObjectBuffer, dObjectLength);

        //Execute Command
        oResult = pDeviceCommandSetManager->ExecuteCommand(m_pCommand_WriteObject, hDCS_Handle, hTransactionHandle);

        //Get ReturnParameter Data
        m_pCommand_WriteObject->GetReturnParameterData(0, &dDeviceErrorCode, sizeof(dDeviceErrorCode));

        //Get ErrorCode
        m_pCommand_WriteObject->GetErrorInfo(pErrorInfo);
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::DCS_SendNMTService(CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle, WORD wNetworkId, WORD wNodeId, WORD wCommandSpecifier, CErrorInfo* pErrorInfo)
{
    BOOL oResult = FALSE;
    DWORD dDeviceErrorCode = 0;

    if(m_pCommand_SendNMTService && pDeviceCommandSetManager)
    {
        //Set Parameter Data
        m_pCommand_SendNMTService->ResetStatus();
        m_pCommand_SendNMTService->SetParameterData(0, &wNetworkId, sizeof(wNetworkId));
        m_pCommand_SendNMTService->SetParameterData(1, &wNodeId, sizeof(wNodeId));
        m_pCommand_SendNMTService->SetParameterData(2, &wCommandSpecifier, sizeof(wCommandSpecifier));

        //Execute Command
        oResult = pDeviceCommandSetManager->ExecuteCommand(m_pCommand_SendNMTService, hDCS_Handle, hTransactionHandle);

        //Get ReturnParameter Data
        m_pCommand_SendNMTService->GetReturnParameterData(0, &dDeviceErrorCode, sizeof(dDeviceErrorCode));

        //Get ErrorCode
        m_pCommand_SendNMTService->GetErrorInfo(pErrorInfo);
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::DCS_SendCANFrame(CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle, WORD wNetworkId, WORD wCobID, WORD wLength, void* pData, CErrorInfo* pErrorInfo)
{
    DWORD dDeviceErrorCode = 0;
    BOOL oResult = FALSE;

    if(m_pCommand_SendCANFrame && pDeviceCommandSetManager)
    {
        //Set Parameter Data
        m_pCommand_SendCANFrame->ResetStatus();
        m_pCommand_SendCANFrame->SetParameterData(0, &wNetworkId, sizeof(wNetworkId));
        m_pCommand_SendCANFrame->SetParameterData(1, &wCobID, sizeof(wCobID));
        m_pCommand_SendCANFrame->SetParameterData(2, &wLength, sizeof(wLength));
        m_pCommand_SendCANFrame->SetParameterData(3, pData, wLength);

        //Execute Command
        oResult = pDeviceCommandSetManager->ExecuteCommand(m_pCommand_SendCANFrame, hDCS_Handle, hTransactionHandle);

        //Get ReturnParameter Data
        m_pCommand_SendCANFrame->GetReturnParameterData(0, &dDeviceErrorCode, sizeof(dDeviceErrorCode));

        //Get ErrorCode
        m_pCommand_SendCANFrame->GetErrorInfo(pErrorInfo);
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::DCS_RequestCANFrame(CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle, WORD wNetworkId, WORD wCobID, WORD wLength, void* pData, CErrorInfo* pErrorInfo)
{
    DWORD dDeviceErrorCode = 0;
    BOOL oResult = FALSE;

    if(m_pCommand_RequestCANFrame && pDeviceCommandSetManager)
    {
        //Set Parameter Data
        m_pCommand_RequestCANFrame->ResetStatus();
        m_pCommand_RequestCANFrame->SetParameterData(0, &wNetworkId, sizeof(wNetworkId));
        m_pCommand_RequestCANFrame->SetParameterData(1, &wCobID, sizeof(wCobID));
        m_pCommand_RequestCANFrame->SetParameterData(2, &wLength, sizeof(wLength));

        //Execute Command
        oResult = pDeviceCommandSetManager->ExecuteCommand(m_pCommand_RequestCANFrame, hDCS_Handle, hTransactionHandle);

        //Get ReturnParameter Data
        m_pCommand_RequestCANFrame->GetReturnParameterData(0, &dDeviceErrorCode, sizeof(dDeviceErrorCode));
        m_pCommand_RequestCANFrame->GetReturnParameterData(1, pData, wLength);

        //Get ErrorCode
        m_pCommand_RequestCANFrame->GetErrorInfo(pErrorInfo);
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::DCS_SendLSSFrame(CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle, WORD wNetworkId, void* pDataBuffer, DWORD dDataBufferLength, CErrorInfo* pErrorInfo)
{
    DWORD dDeviceErrorCode = 0;
    BOOL oResult = FALSE;

    if(m_pCommand_SendLSSFrame && pDeviceCommandSetManager)
    {
        //Set Parameter Data
        m_pCommand_SendLSSFrame->ResetStatus();
        m_pCommand_SendLSSFrame->SetParameterData(0, &wNetworkId, sizeof(wNetworkId));
        m_pCommand_SendLSSFrame->SetParameterData(1, pDataBuffer, dDataBufferLength);

        //Execute Command
        oResult = pDeviceCommandSetManager->ExecuteCommand(m_pCommand_SendLSSFrame, hDCS_Handle, hTransactionHandle);

        //Get ReturnParameter Data
        m_pCommand_SendLSSFrame->GetReturnParameterData(0, &dDeviceErrorCode, sizeof(dDeviceErrorCode));

        //Get ErrorCode
        m_pCommand_SendLSSFrame->GetErrorInfo(pErrorInfo);
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::DCS_ReadLSSFrame(CDeviceCommandSetManagerBase* pDeviceCommandSetManager, HANDLE hDCS_Handle, HANDLE hTransactionHandle, WORD wNetworkId, WORD wTimeout, void* pDataBuffer, DWORD dDataBufferLength, CErrorInfo* pErrorInfo)
{
    DWORD dDeviceErrorCode = 0;
    BOOL oResult = FALSE;

    if(m_pCommand_ReadLSSFrame && pDeviceCommandSetManager)
    {
        //Set Parameter Data
        m_pCommand_ReadLSSFrame->ResetStatus();
        m_pCommand_ReadLSSFrame->SetParameterData(0, &wNetworkId, sizeof(wNetworkId));
        m_pCommand_ReadLSSFrame->SetParameterData(1, &wTimeout, sizeof(wTimeout));

        //Execute Command
        oResult = pDeviceCommandSetManager->ExecuteCommand(m_pCommand_ReadLSSFrame, hDCS_Handle, hTransactionHandle);

        //Get ReturnParameter Data
        m_pCommand_ReadLSSFrame->GetReturnParameterData(0, &dDeviceErrorCode, sizeof(dDeviceErrorCode));
        m_pCommand_ReadLSSFrame->GetReturnParameterData(1, pDataBuffer, dDataBufferLength);

        //Get ErrorCode
        m_pCommand_ReadLSSFrame->GetErrorInfo(pErrorInfo);
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::GetNodeId(DWORD dDefaultSdoCobIdClientServer, BYTE& p_rNodeId, CErrorInfo* p_pErrorInfo)
{
    const DWORD k_Min_TxSDO     = 0x600;
    const DWORD k_Max_TxSDO     = 0x67F;
    const DWORD k_TxSDO_Offset  = 0x600;

    BOOL oResult = FALSE;

    if(IsNetworkIndicationActive())
    {
        //NetworkIndication Active
        p_rNodeId = m_uTargetNodeId;
        oResult = TRUE;
    }
    else
    {
        //No NetworkIndication Active
        if((dDefaultSdoCobIdClientServer >= k_Min_TxSDO) && (dDefaultSdoCobIdClientServer <= k_Max_TxSDO))
        {
            p_rNodeId =  (BYTE)(dDefaultSdoCobIdClientServer - k_TxSDO_Offset);
            oResult = TRUE;
        }
        else
        {
            if(m_pErrorHandling) m_pErrorHandling->GetError(k_Error_BadParameter, p_pErrorInfo);
            oResult = FALSE;
        }
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::GetNetworkId(CCommand_PS* p_pCommand, CDeviceCommandSetManagerBase* p_pManager, HANDLE p_hDCS_Handle, WORD& p_rRemoteNetworkId, CErrorInfo* p_pErrorInfo)
{
    BOOL oResult = FALSE;

    if(IsNetworkIndicationActive())
    {
        //NetworkIndication Active
        p_rRemoteNetworkId = m_wTargetNetworkId;
        oResult = TRUE;
    }
    else
    {
        //No NetworkIndication Active
        oResult = GetRemoteNetworkId(p_pCommand, p_pManager, p_hDCS_Handle, p_rRemoteNetworkId, p_pErrorInfo);
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::GetRemoteNetworkId(CCommand_PS* p_pCommand, CDeviceCommandSetManagerBase* p_pManager, HANDLE p_hDCS_Handle, WORD& p_rRemoteNetworkId, CErrorInfo* p_pErrorInfo)
{
    const CStdString REMOTE_NETWORK_ID = _T("RemoteNetworkId");

    CErrorInfo errorInfo;
    BOOL oResult = FALSE;

    //Get RemoteNetworkId from Command
    if(!oResult) oResult = p_pCommand->GetLayerParameter(DEVICE_COMMAND_SET_LAYER, REMOTE_NETWORK_ID, (BYTE*)&p_rRemoteNetworkId, sizeof(p_rRemoteNetworkId), &errorInfo);

    //Get RemoteNetworkId from Device
    if(!oResult) oResult = p_pManager->GetParameter(PT_DEVICE, p_hDCS_Handle, REMOTE_NETWORK_ID, (BYTE*)&p_rRemoteNetworkId, sizeof(p_rRemoteNetworkId), &errorInfo);

    //ErrorHandling
    if(!oResult && p_pErrorInfo) *p_pErrorInfo = errorInfo;

    return oResult;
}

DWORD CGatewayCANopenToEsam2::CheckDataBufferLength(BOOL oSizeIndicated, BYTE uNonValidNbOfBytes, DWORD dDataBufferLength, DWORD dMaxDataBufferLength)
{
    if(oSizeIndicated)
    {
        //Size Indicated
        if(dDataBufferLength > (dMaxDataBufferLength - uNonValidNbOfBytes)) dDataBufferLength = (dMaxDataBufferLength - uNonValidNbOfBytes);
    }
    else
    {
        //Size not Indicated
        if(dDataBufferLength > dMaxDataBufferLength) dDataBufferLength = dMaxDataBufferLength;
    }

    return dDataBufferLength;
}

BOOL CGatewayCANopenToEsam2::ResetBufferedData()
{
    if(m_pBufferedData) free(m_pBufferedData);
    m_pBufferedData = NULL;
    m_dBufferedDataSize = 0;
    m_dBufferedDataRead = 0;
    m_oNoMoreSegments = FALSE;

    return TRUE;
}

BOOL CGatewayCANopenToEsam2::AddBufferedData(void* pDataBuffer, DWORD dDataBufferSize, BOOL oNoMoreSegments)
{
    DWORD size;
    BYTE* p;

    if(dDataBufferSize > 0)
    {
        //Allocate Memory
        size = m_dBufferedDataSize + dDataBufferSize;
        m_pBufferedData = realloc(m_pBufferedData, size);
        p = (BYTE*)m_pBufferedData + m_dBufferedDataSize;
        memcpy(p, pDataBuffer, dDataBufferSize);
        m_dBufferedDataSize += dDataBufferSize;

        //NoMoreSegments
        m_oNoMoreSegments = oNoMoreSegments;

        return TRUE;
    }

    return FALSE;
}

BOOL CGatewayCANopenToEsam2::IsBufferedDataAvailable(DWORD dSize, DWORD* pdSizeAvailable, BOOL* poNoMoreSegments)
{
    DWORD dTotalSizeAvailable;

    if(m_dBufferedDataSize > m_dBufferedDataRead)
    {
        dTotalSizeAvailable = m_dBufferedDataSize-m_dBufferedDataRead;
        if(dTotalSizeAvailable > dSize)
        {
            if(poNoMoreSegments) *poNoMoreSegments = FALSE;
            if(pdSizeAvailable) *pdSizeAvailable = dSize;
            return TRUE;
        }
        else if(dTotalSizeAvailable == dSize)
        {
            if(poNoMoreSegments) *poNoMoreSegments = m_oNoMoreSegments;
            if(pdSizeAvailable) *pdSizeAvailable = dTotalSizeAvailable;
            return TRUE;
        }
        else if(dTotalSizeAvailable < dSize)
        {
            if(m_oNoMoreSegments)
            {
                if(poNoMoreSegments) *poNoMoreSegments = TRUE;
                if(pdSizeAvailable) *pdSizeAvailable = dTotalSizeAvailable;
                return TRUE;
            }
        }
    }

    if(poNoMoreSegments) *poNoMoreSegments = FALSE;
    return FALSE;
}

BOOL CGatewayCANopenToEsam2::GetBufferedData(void* pDataBuffer, DWORD dDataBufferSize, DWORD* pdDataRead, BOOL* poNoMoreSegments)
{
    DWORD dSizeAvailable;
    BYTE* p;

    if(pDataBuffer && pdDataRead && poNoMoreSegments)
    {
        if(IsBufferedDataAvailable(dDataBufferSize, &dSizeAvailable, poNoMoreSegments))
        {
            if(dSizeAvailable > dDataBufferSize) dSizeAvailable = dDataBufferSize;
            p = (BYTE*)m_pBufferedData + m_dBufferedDataRead;
            memcpy(pDataBuffer, p, dSizeAvailable);
            m_dBufferedDataRead += dSizeAvailable;
            *pdDataRead = dSizeAvailable;

            return TRUE;
        }
    }

    return FALSE;
}

BOOL CGatewayCANopenToEsam2::ReverseBufferedData(DWORD dReverseSize)
{
    if(m_dBufferedDataRead >= dReverseSize)
    {
        m_dBufferedDataRead -= dReverseSize;
        return TRUE;
    }

    return FALSE;
}

BOOL CGatewayCANopenToEsam2::SetNetworkIndication(WORD p_wTargetNetworkId, BYTE p_uTargetNodeId)
{
    BOOL oResult = FALSE;

    m_ulNetworkIndicationThreadId = MmcGetCurrentThreadId();

    m_wTargetNetworkId = p_wTargetNetworkId;
    m_uTargetNodeId = p_uTargetNodeId;
    oResult = TRUE;

    return oResult;
}

BOOL CGatewayCANopenToEsam2::ResetNetworkIndication()
{
    BOOL oResult = FALSE;

    m_ulNetworkIndicationThreadId = 0;
    m_wTargetNetworkId = 0;
    m_uTargetNodeId = 0;
    oResult = TRUE;

    return oResult;
}

BOOL CGatewayCANopenToEsam2::IsNetworkIndicationActive()
{
    return (m_ulNetworkIndicationThreadId != 0);
}

BOOL CGatewayCANopenToEsam2::IsNetworkIndicationThread()
{
    BOOL oResult = FALSE;

    if(IsNetworkIndicationActive())
    {
        oResult = (MmcGetCurrentThreadId() == m_ulNetworkIndicationThreadId);
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::InitLayerParameterStack(CCommandRoot* p_pCommand)
{
    CLayerParameterStack layerParameterStack;
    CLayerParameterSet layerParameterSet;
    BOOL oResult = FALSE;

    if(p_pCommand)
    {
        if(p_pCommand->GetLayerParameterStack(layerParameterStack))
        {
            //Pop PS Layer Parameter Set
            layerParameterStack.PopLayer(PROTOCOL_STACK_LAYER, layerParameterSet);

            //Set I Layer Commands
            if(m_pCommand_WriteObject) m_pCommand_WriteObject->SetLayerParameterStack(layerParameterStack);
            if(m_pCommand_InitiateSegmentedWrite) m_pCommand_InitiateSegmentedWrite->SetLayerParameterStack(layerParameterStack);
            if(m_pCommand_SegmentedWrite) m_pCommand_SegmentedWrite->SetLayerParameterStack(layerParameterStack);
            if(m_pCommand_ReadObject) m_pCommand_ReadObject->SetLayerParameterStack(layerParameterStack);
            if(m_pCommand_InitiateSegmentedRead) m_pCommand_InitiateSegmentedRead->SetLayerParameterStack(layerParameterStack);
            if(m_pCommand_SegmentedRead) m_pCommand_SegmentedRead->SetLayerParameterStack(layerParameterStack);
            if(m_pCommand_AbortSegmentedTransfer) m_pCommand_AbortSegmentedTransfer->SetLayerParameterStack(layerParameterStack);

            if(m_pCommand_SendCANFrame) m_pCommand_SendCANFrame->SetLayerParameterStack(layerParameterStack);
            if(m_pCommand_RequestCANFrame) m_pCommand_RequestCANFrame->SetLayerParameterStack(layerParameterStack);
            if(m_pCommand_SendNMTService) m_pCommand_SendNMTService->SetLayerParameterStack(layerParameterStack);

            if(m_pCommand_SendLSSFrame) m_pCommand_SendLSSFrame->SetLayerParameterStack(layerParameterStack);
            if(m_pCommand_ReadLSSFrame) m_pCommand_ReadLSSFrame->SetLayerParameterStack(layerParameterStack);
            oResult = TRUE;
        }
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::SetDeviceTimeout(CDeviceCommandSetManagerBase* p_pManager, HANDLE p_hHandle, DWORD p_ulTimeout)
{
    WORD usTimeout(0);
    BOOL oResult(FALSE);
    CErrorInfo errorInfo;
    HANDLE hTransactionHandle(NULL);
    WORD usNetworkId(0);
    BYTE ubNodeId(0);
    CStdString protocolStackName(_T(""));
    CStdString interfaceName(_T(""));

    if(p_pManager)
    {
        if(p_pManager->DCS_GetProtocolStackName(p_hHandle, &protocolStackName) && p_pManager->DCS_GetInterfaceName(p_hHandle, &interfaceName))
        {
            //Init
            usTimeout = (WORD)p_ulTimeout;
            
			if(GetAddressParameter(p_pManager, p_hHandle, ubNodeId, &errorInfo))
			{
				if(protocolStackName == PROTOCOL_CAN_OPEN)
				{
					//CAN Frame Timeout
					oResult = DCS_WriteObject(p_pManager, p_hHandle, hTransactionHandle, usNetworkId, ubNodeId, INDEX_CAN_FRAME_TIMEOUT, SUBINDEX_CAN_FRAME_TIMEOUT, (BYTE*)&usTimeout, sizeof(usTimeout), &errorInfo);
				}
				else if(protocolStackName == PROTOCOL_MAXON_SERIAL_V2)
				{
					if(interfaceName == INTERFACE_RS232)
					{
						//RS232 Frame Timeout
						   oResult = DCS_WriteObject(p_pManager, p_hHandle, hTransactionHandle, usNetworkId, ubNodeId, INDEX_RS232_FRAME_TIMEOUT, SUBINDEX_RS232_FRAME_TIMEOUT, (BYTE*)&usTimeout, sizeof(usTimeout), &errorInfo);
					}
					else if(interfaceName == INTERFACE_USB)
					{
						//USB Frame Timeout
						oResult = DCS_WriteObject(p_pManager, p_hHandle, hTransactionHandle, usNetworkId, ubNodeId, INDEX_USB_FRAME_TIMEOUT, SUBINDEX_USB_FRAME_TIMEOUT, (BYTE*)&usTimeout, sizeof(usTimeout), &errorInfo);
					}
				}
			}
        }
    }

    return oResult;
}

BOOL CGatewayCANopenToEsam2::GetAddressParameter(CDeviceCommandSetManagerBase* p_pManager, HANDLE p_hDCS_Handle, BYTE& p_rNodeId, CErrorInfo* p_pErrorInfo)
{
    const CStdString NODE_ID = _T("NodeId");

    BOOL oResult(FALSE);

    if(p_pManager)
    {
        //Get Address Parameter from Device
        oResult = p_pManager->GetParameter(PT_DEVICE, p_hDCS_Handle, NODE_ID, (BYTE*)&p_rNodeId, sizeof(p_rNodeId), p_pErrorInfo);
    }

    return oResult;
}

