/* DiskSpace.java, Copyright (c)2000-2005 by Richard Werezak, All Rights Reserved

THU26FEB04: (REW) switched to gcc
FRI15JUL05: (REW) revised
*/

package com.werezak.diskSpace;

import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.ArrayList;

public class DiskSpace {

   // JNI Calls to Win32 API

   static {
      System.loadLibrary ("DiskSpace");
   }

   public static native long getDiskSpace (String dirName)
      throws IOException;

   private DiskSpace() {}

}// class DiskSpace()
