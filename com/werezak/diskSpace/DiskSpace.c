/* diskSpace.c, Copyright (c)2000-2005 by Richard Werezak, All Rights Reserved

    08MAR00: (REW) Coded
 THU06JUL00: (REW) Removed use of _tcsdup() in favour of home grown
                   CopyString().  I think there's a inconsistency in
                   the way Sun and Microsoft handle UNICODE string endings.
 TUE08JAN02: (REW) Added getDiskSpace()
 FRI15JUL05: (REW) revised

This DLL provide a very thin JNI wrapper for the class.

Some notes on JNI:

 a) Test the incomming jstring for NULL.  If it's NULL do not call GetString..()
    or ReleaseString...()
 b) Incomming jstrings are const.  Copy them if you need them to be non-const.
 c) Use jlong for HANDLEs.  Do not modify them in Java. Declare them final in the
    Java code, if possible.

Note: - JNI exported function names start with lowercase.  Others with uppercase.
      - This DLL is Unicode-ready.
*/

#define _UNICODE 1
#define UNICODE 1
//#define _DEBUG 1

#ifdef _DEBUG
#define DEBUG0(Z1)       _tprintf(_T(Z1));
#define DEBUG1(Z1,Z2)    _tprintf(_T(Z1),(Z2));
#define DEBUG2(Z1,Z2,Z3) _tprintf(_T(Z1),(Z2),(Z3));
#else
#define DEBUG0(Z1)
#define DEBUG1(Z1,Z2)   
#define DEBUG2(Z1,Z2,Z3)
#endif

#include <jni.h>
#include "DiskSpace.h"
#include <stdio.h>
#include <tchar.h>

#include <io.h>
#pragma warning ( disable : 4115 )
#include <windows.h>
#pragma warning ( default : 4115 )

#ifdef _UNICODE
#pragma message ("_UNICODE defined")
#define _NewString(Z1,Z2)    NewString((Z1),(Z2),_tcslen((Z2)))
#define _GetStringChars      GetStringChars         
#define _GetStringLength     GetStringLength        
#define _ReleaseStringChars  ReleaseStringChars     
#else // UTF-8 versions
#pragma message ("_UNICODE not defined")
#define _NewString           NewStringUTF           
#define _GetStringChars      GetStringUTFChars      
#define _GetStringLength     GetStringUTFLength     
#define _ReleaseStringChars  ReleaseStringUTFChars  
#endif // _UNICODE

#pragma warning ( disable : 4100 )

#if defined(__GNUC__)
#define INT64 long long
#elif defined(_MSC_VER)
#define INT64 _int64
#elif !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L)
#endif

/*$***************************************************************************

                             D l l M a i n

------------------------------------------------------------------------------

*****************************************************************************/

BOOL WINAPI DllMain (HANDLE h, DWORD dwReason, LPVOID lp)
{
   if (dwReason==DLL_PROCESS_ATTACH) {
      DEBUG0("DLL_PROCESS_ATTACH\n");
   } else if (dwReason==DLL_PROCESS_DETACH) {
      DEBUG0("DLL_PROCESS_DETACH\n");
   } else if (dwReason==DLL_THREAD_ATTACH) {
      DEBUG0("DLL_THREAD_ATTACH\n");
   } else if (dwReason==DLL_THREAD_DETACH) {
      DEBUG0("DLL_THREAD_DETACH\n");
   } else {
      DEBUG0("DllMain() unknown dwReason\n");
   }

   return TRUE;

}// DllMain()

/*$***************************************************************************

                           C o p y S t r i n g

------------------------------------------------------------------------------

Copy a Java String to a LPTSTR.

The following won't work:

  pString = _tcsdup ((*env)->_GetStringChars (env, jstring, 0));

I think there's a inconsistency in the way Sun and Microsoft
handle UNICODE string endings.  Works for odd-lengthed strings only.
So we do it the hard way ...

*****************************************************************************/

LPTSTR CopyString (JNIEnv *env, jstring string)
{
   LPTSTR lp;
   LPCTSTR pString;
   int len;
   int i;

   if (!string) return NULL;

   // The following won't work:
   //
   //   pFileName = _tcsdup ((*env)->_GetStringChars (env, fileName, 0));
   //
   // I think there's a inconsistency in the way Sun and Microsoft
   // handle UNICODE string endings.  Works for odd-lengthed strings only.
   // So we do it the hard way ...

   len = (*env)->_GetStringLength (env, string);

   lp = malloc ((len+2)*sizeof(TCHAR));

   if (!lp) return NULL;

   pString = (*env)->_GetStringChars (env, string, 0);

   for (i=0; i<len; ++i)
       lp[i] = pString[i];

   lp[i] = 0x0000;

   // Clean up.

   (*env)->_ReleaseStringChars (env, string, pString);

   return lp;

}/* CopyString() */

/*$***************************************************************************

                             THROW Macros

------------------------------------------------------------------------------

*****************************************************************************/

void Throw (JNIEnv * env, LPSTR sz)
{
   jclass classExc;

   // Strings used in ThrowNew() must not be TCHAR

   (*env)->ExceptionClear (env);

   classExc = (*env)->FindClass (env, "java/io/IOException");

   if (classExc!=0) (*env)->ThrowNew (env, classExc, sz);

}// Throw()

/*$***************************************************************************

                         g e t D i s k S p a c e

------------------------------------------------------------------------------

Java Declaration:

   public static native long getDiskSpace (String dirName);

*****************************************************************************/

JNIEXPORT jlong JNICALL Java_com_werezak_diskSpace_DiskSpace_getDiskSpace (
   JNIEnv *env, jclass class, jstring dirName)
{
   char sz[100];
   DWORD err = 0;
   unsigned INT64 i64FreeBytesAvailable = 0;// bytes available to caller
   unsigned INT64 i64TotalNumberOfBytes;    // bytes on disk
   unsigned INT64 i64TotalNumberOfFreeBytes;// free bytes on disk
   unsigned INT64 i64FreeMBAvailable;

   LPTSTR DirectoryName = CopyString (env, dirName);

//   _tcscpy (DirectoryName, _TEXT("D:\\"));

   if (!GetDiskFreeSpaceEx(
      (LPTSTR) DirectoryName,
      (PULARGE_INTEGER) &i64FreeBytesAvailable,    
      (PULARGE_INTEGER) &i64TotalNumberOfBytes,    
      (PULARGE_INTEGER) &i64TotalNumberOfFreeBytes)) {

      err = GetLastError();
//      _ftprintf(stderr,_T("GetDiskFreeSpace(%s) failed. Error %u\n"), DirectoryName,err);

      sprintf (sz, "GetDiskFreeSpace(%S) failed. Error #%d", DirectoryName, err);
      Throw (env, sz); goto finally;

return ((jlong)0);
   }

   free (DirectoryName);

//   _ftprintf(stderr,_T("There is %I64uMB available on %s.\n"), i64FreeBytesAvailable/1024/1024, DirectoryName);

   i64FreeMBAvailable = i64FreeBytesAvailable/1024/1024;

//   return ((jlong)i64FreeMBAvailable);
finally:
   return ((jlong)i64FreeBytesAvailable);

}// getDiskSpace()

#pragma warning ( default : 4100 )
