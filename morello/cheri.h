#pragma once

#ifndef __CHERI__
#define __cheri_tocap
#define __capability
#endif

#define CAP_PERM_ALL            (0x3ffffULL)

#define CAP_PERM_GLOBAL         (1ULL)       // Global
#define CAP_PERM_EXECUTIVE      (1ULL << 1)  // Executive
#define CAP_PERM_MUTABLE_LOAD   (1ULL << 6)  // LoadMutable
#define CAP_PERM_COMPARTMENT_ID (1ULL << 7)  // CompartmentID
#define CAP_PERM_SYSTEM         (1ULL << 9)  // System
#define CAP_PERM_UNSEAL         (1ULL << 10) // Unseal
#define CAP_PERM_SEAL           (1ULL << 11) // Seal
#define CAP_PERM_STORE_LOCAL    (1ULL << 12) // StoreLocalCap
#define CAP_PERM_STORE_CAP      (1ULL << 13) // StoreCap
#define CAP_PERM_LOAD_CAP       (1ULL << 14) // LoadCap
#define CAP_PERM_EXECUTE        (1ULL << 15) // Execute
#define CAP_PERM_STORE          (1ULL << 16) // Store
#define CAP_PERM_LOAD           (1ULL << 17) // Load
