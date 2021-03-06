// Copyright 2022, Mikhael Khrustik <misha@myrt.co>
//
// All components of Myrt Desk Firmware are licensed under the BSD 3-Clause
// License. See LICENSE.txt for details.

#pragma once

#define SYNC_CRC_EOP 0x20

#define STK_READ_SIGN 0x75
#define STK_GET_SYNCH 0x30
#define STK_INSYNC 0x14
#define STK_OK 0x10
#define STK_ENTER_PROGMODE 0x50
#define STK_LOAD_ADDRESS 0x55
#define STK_PROGRAM_PAGE 0x64
#define STK_LEAVE_PROGMODE 0x51

#define MEMORY_TYPE_FLASH 0x46
