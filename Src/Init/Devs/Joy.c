
/*
** GameX engine
** Src > Init > Devs
*/

#include <devices/gameport.h>

#include <clib/exec_protos.h>

#include "Joy.h"

BOOL checkCon(struct IOStdReq *io)
{
    BYTE con;
    BOOL done = FALSE;

    Forbid();

    io->io_Command = GPD_ASKCTYPE;
    io->io_Data = &con;
    io->io_Length = sizeof(BYTE);
    io->io_Flags = IOF_QUICK;
    DoIO((struct IORequest *)io);

    if (con == GPCT_NOCONTROLLER)
    {
        con = GPCT_ABSJOYSTICK;
        done = TRUE;

        io->io_Command = GPD_SETCTYPE;
        io->io_Data = &con;
        io->io_Length = sizeof(BYTE);
        io->io_Flags = IOF_QUICK;
        DoIO((struct IORequest *)io);
    }

    Permit();

    return(done);
}

VOID clearCon(struct IOStdReq *io)
{
    BYTE con = GPCT_NOCONTROLLER;

    io->io_Command = GPD_SETCTYPE;
    io->io_Data = &con;
    io->io_Length = sizeof(BYTE);
    io->io_Flags = IOF_QUICK;
    DoIO((struct IORequest *)io);
}

VOID setTrig(struct IOStdReq *io)
{
    struct GamePortTrigger gpt;

    gpt.gpt_Keys = GPTF_UPKEYS|GPTF_DOWNKEYS;
    gpt.gpt_XDelta = 1;
    gpt.gpt_YDelta = 1;
    gpt.gpt_Timeout = TIMEOUT;

    io->io_Command = GPD_SETTRIGGER;
    io->io_Data = &gpt;
    io->io_Length = sizeof(gpt);
    io->io_Flags = IOF_QUICK;
    DoIO((struct IORequest *)io);
}

VOID clearIO(struct IOStdReq *io)
{
    io->io_Command = CMD_CLEAR;
    io->io_Data = NULL;
    io->io_Length = 0;
    io->io_Flags = IOF_QUICK;
    DoIO((struct IORequest *)io);
}

VOID readJoy(struct IOStdReq *io, struct InputEvent *ie)
{
    io->io_Command = GPD_READEVENT;
    io->io_Data = ie;
    io->io_Length = sizeof(*ie);
    io->io_Flags = 0;
    SendIO((struct IORequest *)io);
}

struct IOStdReq *openJoy(struct InputEvent *ie)
{
    struct IOStdReq *io;

    struct MsgPort *mp;

    if (mp = CreateMsgPort())
    {
        if (io = (struct IOStdReq *)CreateIORequest(mp, sizeof(*io)))
        {
            if (OpenDevice("gameport.device", 1, (struct IORequest *)io, 0) == 0)
            {
                if (checkCon(io))
                {
                    setTrig(io);
                    clearIO(io);
                    readJoy(io, ie);
                    return(io);
                }
                CloseDevice((struct IORequest *)io);
            }
            DeleteIORequest((struct IORequest *)io);
        }
        DeleteMsgPort(mp);
    }
    return(NULL);
}

VOID closeJoy(struct IOStdReq *io)
{
    struct MsgPort *mp = io->io_Message.mn_ReplyPort;

    if (CheckIO((struct IORequest *)io) == NULL)
    {
        AbortIO((struct IORequest *)io);
    }
    WaitIO((struct IORequest *)io);

    clearCon(io);
    CloseDevice((struct IORequest *)io);
    DeleteIORequest((struct IORequest *)io);
    DeleteMsgPort(mp);
}
