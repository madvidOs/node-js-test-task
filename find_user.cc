#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "netapi32.lib")

//#include <stdio.h>
#include <assert.h>
#include <windows.h> 
#include <lm.h>
#include <napi.h>


//#include <iostream>		


bool findUser(std::string name) 
{
    LPUSER_INFO_0 pBuf = NULL;
    LPUSER_INFO_0 pTmpBuf;
    DWORD dwLevel = 0;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    DWORD i;
    DWORD dwTotalCount = 0;
    NET_API_STATUS nStatus;
    LPTSTR pszServerName = NULL;
    
    
    bool check = false;//

    // Call the NetUserEnum function, specifying level 0; 
    //   enumerate global user account types only.
    //
    do // begin do
    {
        nStatus = NetUserEnum(
            pszServerName,
            dwLevel,
            FILTER_NORMAL_ACCOUNT, // global users
            (LPBYTE*)&pBuf,
            dwPrefMaxLen,
            &dwEntriesRead,
            &dwTotalEntries,
            NULL
        );        
        
        if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
        {
            if ((pTmpBuf = pBuf) != NULL) {
                
                for (i = 0; (i < dwEntriesRead); i++) {

                    std::wstring wsUserName(pTmpBuf->usri0_name);
                    std::string stringUserName(wsUserName.begin(), wsUserName.end());

                    //std::cout<<name<<std::endl;
                    //std::cout<<stringUserName<<std::endl;

                    if (stringUserName == name) {
                        check = true;
                        break;
                    }

                     //wprintf(L"\t-- %s\n", pTmpBuf->usri0_name);                   
                     //wprintf(L"\t-- %s\n", typeid(pTmpBuf->usri0_name).name());                   

                    pTmpBuf++;                  
                }
            }
        }
            
        //
        // Free the allocated buffer.
        //
        if (pBuf != NULL)
        {
            NetApiBufferFree(pBuf);
            pBuf = NULL;
        }

    }  while (nStatus == ERROR_MORE_DATA && !check); // end do
    
    // Check again for allocated memory.    
    if (pBuf != NULL) {
        NetApiBufferFree(pBuf);
    }
        
    return check;
}


Napi::Boolean findUserWrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of arguments")
          .ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "You need to name user")
          .ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    Napi::String name = info[0].As<Napi::String>();

    Napi::Boolean result = Napi::Boolean::New(env, findUser(name.Utf8Value()));

    return result;
}

Napi::Object init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "findUser"), Napi::Function::New(env, findUserWrapper));
    return exports;
};

NODE_API_MODULE(find_user, init);