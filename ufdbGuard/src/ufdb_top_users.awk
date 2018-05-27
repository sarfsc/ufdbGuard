#
# ufdb_topusers.awk  -  awk script to parse ufdbguardd logfiles
#
# just a proof of concept
#
BEGIN 	{ 
      totalblockcount = 0;
      totalpasscount = 0;
      printf "top users\n";
      printf "------------------------------------------------------------------------------------------\n";
   }
END 	{ 
      # awk has a bug: user "administrator" and others are magically not counted...
      for (u in blockcount) 
         printf "%-12s  %8d URLs blocked  %8d URLs passed\n", u, blockcount[u], passcount[u];
      printf "%-12s  %8d URLs blocked  %8d URLs passed\n", "all", totalblockcount, totalpasscount;
   }
$4 == "BLOCK"   {
      user=$5 "";
      blockcount[user]++;
      totalblockcount++;
   }
$4 == "PASS"   {
      user=$5 "";
      passcount[user]++;
      totalpasscount++;
   }
