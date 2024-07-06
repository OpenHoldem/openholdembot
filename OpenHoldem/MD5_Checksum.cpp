//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//                  CPOL:   http://www.codeproject.com/info/cpol10.aspx
//
//******************************************************************************
//
// Purpose: calculating a MD5 checksum of OpenHoldem for the logs
//   to verify that we have an "official" file and sort out some PEBKACs.
//
//******************************************************************************
//
// Original sources: http://www.codeproject.com/Articles/32202/MD5-File-Creation-and-Verification
//                   http://www.rohitab.com/discuss/topic/21740-md5-hashing/
// Original authors: Shaheer Abdulrahiman
//                   "Napalm"
// Slight simplifications (and all the bugs): TheHighFish
//
//******************************************************************************

#include "stdafx.h"
#include "MD5_Checksum.h"

#include <wincrypt.h>

HCRYPTPROV hCryptProv;
typedef struct {
	unsigned char digest[16];
#ifdef _WIN64
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=270&t=19676
  // http://stackoverflow.com/questions/8672887/is-there-a-define-for-64-bit-in-visual-studio-2010
	unsigned long long hHash;
#else
  unsigned long hHash;
#endif
} MD5Context;


BOOL CryptStartup()
{
	if(CryptAcquireContext(&hCryptProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET) == 0){
		if(GetLastError() == NTE_EXISTS){
			if(CryptAcquireContext(&hCryptProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0) == 0)
				return FALSE;
		}else return FALSE;
	}
	return TRUE;
}

void CryptCleanup()
{
	if(hCryptProv) CryptReleaseContext(hCryptProv, 0);
	hCryptProv = NULL;
}

void inline MD5Init(MD5Context *ctx)
{
	CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &ctx->hHash);
}


void inline MD5Update(MD5Context *ctx, unsigned char const *buf, unsigned len)
{
	CryptHashData(ctx->hHash, buf, len, 0);
}

void inline MD5Final(MD5Context *ctx)
{
	DWORD dwCount = 16;
	CryptGetHashParam(ctx->hHash, HP_HASHVAL, ctx->digest, &dwCount, 0);
	if(ctx->hHash) CryptDestroyHash(ctx->hHash);
	ctx->hHash = 0;
}

// Returning an error-constant instead of error-popups
//   * because "it works"
//   * because error-messages might be unwanted
const char *k_error_md5_creation_failed = " *** MD5 failed ***";

CString CalculateMD5(const CString FileName)
{
    int i, j;
    MD5Context md5Hash;
    unsigned char bBuffer[4096];
    unsigned char b;
    char c;
	CString MD5;
    
    if(!CryptStartup())
    {
        return k_error_md5_creation_failed;
    }
    
	FILE *stream;
	errno_t err = fopen_s(&stream, FileName, "rb");
    if(err != 0)
	{
        CryptCleanup();
        return k_error_md5_creation_failed;
    }
    
    memset(&md5Hash, 0, sizeof(MD5Context));
    MD5Init(&md5Hash);
    while(!feof(stream)){
        unsigned int nCount = fread(bBuffer, sizeof(unsigned char), 
                                    4096, stream);
        MD5Update(&md5Hash, bBuffer, nCount);
    }
    MD5Final(&md5Hash);
    
    fclose(stream);
    //printf("\nChecksum of '%s' is: ", argv[1]);
    char *Value = new char[1024];int k = 0;
    for(i = 0; i < 16; i++)
	{
        b = md5Hash.digest[i];
        for(j = 4; j >= 0; j -= 4)
		{
            c = ((char)(b >> j) & 0x0F);
            if(c < 10) c += '0';
            else c = ('a' + (c - 10));
            //printf("%c", c);
			Value[k] = c;
			k++;
        }
    }
    Value[k] = '\0';
    CryptCleanup();
	
    MD5 = CString(Value);
	
    delete Value;
    return MD5;
}