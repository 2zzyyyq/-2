
/******************************************************************************/
/** \file system_hc32l13x.h
 **
 ** A detailed description is available at
 ** @link SampleGroup Some description @endlink
 **
 **   - 2019-03-01  1.0  Lux First version.
 **
 ******************************************************************************/

#ifndef __SYSTEM_HC32L13X_H__
#define __SYSTEM_HC32L13X_H__

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Global pre-processor symbols/macros ('define')                             */
/******************************************************************************/
#define HWWD_DISABLE        (1)

//#define HC32L13xKxxx           //64PIN
#define HC32L13xJxxx           //48PIN
//#define HC32L13xFxxx           //32PIN
//#define HC32L13xExxx           //28PIN

/**
 ******************************************************************************
 ** \brief Clock Setup macro definition
 **
 ** - 0: CLOCK_SETTING_NONE  - User provides own clock setting in application
 ** - 1: CLOCK_SETTING_CMSIS - 
 ******************************************************************************/
#define CLOCK_SETTING_NONE  0u
#define CLOCK_SETTING_CMSIS 1u


/******************************************************************************/
/*                                                                            */
/*                      START OF USER SETTINGS HERE                           */
/*                      ===========================                           */
/*                                                                            */
/*                 All lines with '<<<' can be set by user.                   */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Global function prototypes ('extern', definition in C source)              */
/******************************************************************************/


extern uint32_t SystemCoreClock;          // System Clock Frequency (Core Clock)
extern void SystemInit (void);            // Initialize the system
extern void SystemCoreClockUpdate (void); // Update SystemCoreClock variable

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_HC32L13X_H__ */







