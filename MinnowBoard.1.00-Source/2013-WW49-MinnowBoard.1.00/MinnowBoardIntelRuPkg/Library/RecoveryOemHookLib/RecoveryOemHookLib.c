/*++
  This file contains 'Framework Code' and is licensed as such   
  under the terms of your license agreement with Intel or your  
  vendor.  This file may not be modified, except as allowed by  
  additional terms of your license agreement.                   
--*/
/** @file
  This file includes the function that can be customized by OEM.
  
Copyright (c) 2010, Intel Corporation.<BR>
All rights reserved.  This software and associated documentation
(if any) is furnished under a license and may only be used or
copied in accordance with the terms of the license.  Except as
permitted by such license, no part of this software or
documentation may be reproduced, stored in a retrieval system, or 
transmitted in any form or by any means without the express written 
consent of Intel Corporation.

**/


#include "CommonHeader.h"

/**
  The recovery capsule is determined by 2 factors,
    1. The device search order, if more than one Device Recovery Module PPI 
    was discovered
    2. The individual search order, if the device reported more than one recovery 
    DXE capsule was found generating a search order list.
  
  The 2 orders are decided by the RecoveryOemHook library function OemRecoveryRankCapsule().  
  
  @param[in, out]  DeviceInstance  Specifies which EFI_PEI_DEVICE_RECOVERY_MODULE_PPI instance
                   to retrieve when passed out.  Specifies from which EFI_PEI_DEVICE_RECOVERY_MODULE_PPI 
                   instance to start when passed in. The index start from zero.
  @param[in, out]  CapsuleInstance  Specifies which recovery capsule to retrieve when passed out.  
                   Specifies from which recovery capsule to start when passed in. The index start from zero.   
  @param[out]      DeviceRecoveryModule  If it's not NULL, it's EFI_PEI_DEVICE_RECOVERY_MODULE_PPI.
                   instance specified by DeviceInstance,  caller can use this instance directly.  The 
                   function will change DeviceRecoveryModule to NULL, if it doesn't return a PPI instance.                                                      
  @param[out]      CapsuleBuffer  If it's not NULL, it contains loaded capsule, caller can use it directly. 
                   The function will change Buffer to NULL, if it doesn't load a capsule.
                   
  @retval TRUE  If a recovery capsule is found.
  @retval FALSE If  no recovery capsule found.
                     
**/
BOOLEAN
EFIAPI
OemRecoveryRankCapsule (
  IN OUT UINTN                               *DeviceInstance,
  IN OUT UINTN                               *CapsuleInstance,
     OUT EFI_PEI_DEVICE_RECOVERY_MODULE_PPI  **DeviceRecoveryModule,  OPTIONAL
     OUT VOID                                **CapsuleBuffer OPTIONAL    
  ) 
{
  EFI_PEI_SERVICES         **PeiServices;
  UINTN                    NumberRecoveryCapsules;
  EFI_STATUS               DeviceFound;
  EFI_STATUS               CapsuleFound;  
  
  //
  // Return the first found capsule.  
  // OEM can override this implementation to support complex priority.
  //
 
  ASSERT (DeviceRecoveryModule != NULL);
  ASSERT (CapsuleBuffer != NULL);  
 
  if ((*DeviceInstance) != 0 || (*CapsuleInstance) != 1) {
    return FALSE;
  }
  
  PeiServices = (EFI_PEI_SERVICES  **) GetPeiServicesTablePointer ();
  NumberRecoveryCapsules = 0;  
  DeviceFound = EFI_SUCCESS;
  CapsuleFound = EFI_NOT_FOUND;
  (*DeviceRecoveryModule) = NULL;
  (*CapsuleBuffer) = NULL;
  
  //
  // If no new Device found, end search.
  // If device has at least 1 capsule, end search.
  //  
  while (!EFI_ERROR (DeviceFound) && NumberRecoveryCapsules == 0) {
    DeviceFound = PeiServicesLocatePpi (
                    &gEfiPeiDeviceRecoveryModulePpiGuid,
                    *DeviceInstance,
                    NULL,
                    (VOID **) DeviceRecoveryModule
                    );
               
    if (!EFI_ERROR (DeviceFound)) {
      DEBUG ((EFI_D_INFO | EFI_D_LOAD, "OEM Device Recovery PPI located\n"));
     
      CapsuleFound = (*DeviceRecoveryModule)->GetNumberRecoveryCapsules (
                                                PeiServices,
                                                *DeviceRecoveryModule,
                                                &NumberRecoveryCapsules
                                                );        
      if (NumberRecoveryCapsules == 0) {
        (*DeviceInstance) ++;
      } else {
        *CapsuleInstance = 1;
      }       
    }
  }   
  
  return (BOOLEAN)(NumberRecoveryCapsules > 0);
}

/**
  This function performe security check and check failure handling,
  
  @param  Buffer The caller allocated buffer that contains recovery capsule to be checked.                                 

  @retval TRUE  If it's secure.
**/
BOOLEAN
EFIAPI
OemRecoverySecurityCheck (
  VOID    *Buffer
  ) 
{
  return TRUE;                                                
}
