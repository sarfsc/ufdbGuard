#
# ufdb_analyse_user.awk  -  awk script to parse ufdbguardd logfiles
#
# just a proof of concept
#
BEGIN 	{ 
      printf "analysis for user %s\n", repuser; 
      nu_repuser = 0;
      nu_blocked = 0;
      nu_passed = 0;
      otherusers = 0;
      printf "------------------------------------------------------------------------------------------\n";
   }
END 	{ 
      printf "------------------------------------------------------------------------------------------\n";
      total = nu_repuser + otherusers;
      printf "total number of processed URLs: %d\n", total;
      perc = ((nu_repuser * 100.0) / (total + 0.00000001));
      printf "user \"%s\": %d URLs  (%.1f %%)  %d URLs blocked and %d URLs passed\n", 
             repuser, nu_repuser, perc, nu_blocked, nu_passed;
      printf "all other users: %d URLs\n", otherusers;
   }
NF == 10  &&  ($4 == "BLOCK"  ||  $4 == "PASS")	{
   if ($5 == repuser)
   {
      nu_repuser++;
      if ($4 == "BLOCK") nu_blocked++;
      if ($4 == "PASS") nu_passed++; 
      if ((printblocked  &&  $4 == "BLOCK")  ||  (printpassed  && $4 == "PASS"))
      {
	 printf "%s %s %-5s %-12s %s\n", $1, $2, $4, $8, $9; 
      }
   }
   else
      otherusers++;
}
