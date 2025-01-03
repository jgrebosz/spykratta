#ifndef __INCi2oh
#define __INCi2oh

/*
#define BIG_ENDIAN__
*/
#define LITTLE_ENDIAN__
#define I2O_64BIT_CONTEXT 1


#define XDAQ_ORGANIZATION_ID    0x100
#define I2O_BU_PUT_EVENT    0x17

#define I2O_PRIVATE_MESSAGE 0xFF


typedef unsigned char   U8;
typedef unsigned short  U16;
typedef unsigned int    U32;
typedef U32 BF;


typedef struct _U64
{
    U32 LowPart;
    U32 HighPart;
} U64;


#define I2O_TID_SZ  12
#define I2O_FUNCTION_SZ 8


#if I2O_64BIT_CONTEXT
typedef U64 I2O_INITIATOR_CONTEXT;
typedef U64 I2O_TRANSACTION_CONTEXT;
#else
typedef U32 I2O_INITIATOR_CONTEXT;
typedef U32 I2O_TRANSACTION_CONTEXT;
#endif


typedef struct _I2O_MESSAGE_FRAME
{
#if defined(LITTLE_ENDIAN__)
    U8          VersionOffset;
    U8          MsgFlags;
    U16         MessageSize;
BF          TargetAddress:
    I2O_TID_SZ;
BF          InitiatorAddress:
    I2O_TID_SZ;
BF          Function:
    I2O_FUNCTION_SZ;
    I2O_INITIATOR_CONTEXT   InitiatorContext;
#elif defined(BIG_ENDIAN__)
    U16         MessageSize;
    U8          MsgFlags;
    U8          VersionOffset;
BF          Function:
    I2O_FUNCTION_SZ;
BF          InitiatorAddress:
    I2O_TID_SZ;
BF          TargetAddress:
    I2O_TID_SZ;
    I2O_INITIATOR_CONTEXT   InitiatorContext;
#endif
} I2O_MESSAGE_FRAME, *PI2O_MESSAGE_FRAME;


typedef struct _I2O_PRIVATE_MESSAGE_FRAME
{
#if defined(LITTLE_ENDIAN__)
    I2O_MESSAGE_FRAME   StdMessageFrame;
    I2O_TRANSACTION_CONTEXT TransactionContext;
    U16         XFunctionCode;
    U16         OrganizationID;
    U32         Padding;           /* Padding for 64 bit */
    /*                              PrivatePayload[]        */
#elif defined(BIG_ENDIAN__)
    I2O_MESSAGE_FRAME   StdMessageFrame;
    I2O_TRANSACTION_CONTEXT TransactionContext;
    U16         OrganizationID;
    U16         XFunctionCode;
    U32         Padding;           /* Padding for 64 bit */
    /*                              PrivatePayload[]        */
#endif
} I2O_PRIVATE_MESSAGE_FRAME, *PI2O_PRIVATE_MESSAGE_FRAME;


typedef struct _I2O_SEND_TO_BU_MESSAGE_FRAME
{
    I2O_PRIVATE_MESSAGE_FRAME PvtMessageFrame;

    struct
    {
        U32 RunId;
        U32 BlockId;
    } info;
} I2O_SEND_TO_BU_MESSAGE_FRAME, *PI2O_READOUT_MESSAGE_FRAME;


#endif /* __INCi2oh */

