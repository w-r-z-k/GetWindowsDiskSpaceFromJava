// CheckDiskSpace.java

// MON07AUG06: (REW) coded

import java.io.*;
import com.werezak.diskSpace.DiskSpace;

public class CheckDiskSpace {

/*$***************************************************************************

                               m a i n

------------------------------------------------------------------------------

*****************************************************************************/

public static void main (String args[])
   throws IOException {

   if (args.length<1) {
      System.out.println("Usage: java CheckDiskSpace directory-path");
      System.exit(1);
   }

   String sPath = args[0];

   File path = new File (sPath);

   if (!path.exists()) {
      System.err.println("Unable to find folder '"+path+"'.");
      System.exit(1);
   }

   long freeSpace = DiskSpace.getDiskSpace(sPath);

   System.err.println("There are "+freeSpace+" bytes free on '"+path+"'");

}// main()

}// class CheckDiskSpace
