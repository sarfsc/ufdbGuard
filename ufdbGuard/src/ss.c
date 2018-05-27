/*
 * UFDBaddSafeSearch - modify a URL for a search which requires SafeSearch
 *
 * return UFDB_API_OK for unmodified URLs and UFDB_API_MODIFIED_FOR_SAFESEARCH
 *
 * parameters: domain - the domainname
 *             strippedURL - the stripped URL including the domainname
 *             originalURL - the unmodified user-supplied URL
 *	       The originalURL must be of type char[UFDB_MAX_URL_LENGTH]
 *	       and may be modified to force SafeSearch.
 */
int UFDBaddSafeSearch(
   char * domain,
   char * strippedURL,
   char * originalURL  )
{
   char * slash;

   originalURL[UFDB_MAX_URL_LENGTH-28] = '\0';

   slash = strchr( strippedURL, '/' );
   if (slash == NULL)
      strippedURL = "";
   else
      strippedURL = slash;

#if 0
   printf( "   SS: %s %s\n", domain, strippedURL );
#endif

   if (strstr( domain, "similar-images.googlelabs." ) != NULL  &&	/* Google images */
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&safe=active&safeui=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "images.google." ) != NULL  &&			/* Google images */
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&safe=active&safeui=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if ((domain[0] <= '9' && domain[0] >= '0')  &&			/* google-related sites like www.google-tr.info */
       strstr( strippedURL, "cx=partner" ) != NULL  &&
       strstr( strippedURL, "/cse" ) != NULL  &&
       strstr( strippedURL, "q=" ) != NULL)
   {
      strcat( originalURL, "&safe=active" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   if ((domain[0] <= '9' && domain[0] >= '0')     &&			/* google.com, google.de, google.ws etc. */
       strncmp( strippedURL, "/search", 7 ) == 0  &&
       (strncmp( domain, "74.125.", 7 ) == 0  ||
        strncmp( domain, "173.194.", 8 ) == 0))
   {
      strcat( originalURL, "&safe=active" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if ((strstr( domain, "google." ) != NULL  ||				/* SAFESEARCH: google.* */
        strcmp( domain, "ajax.googleapis.com" ) == 0  ||
        strstr( domain, "googleusercontent.com" ) != NULL)  &&		/* Google */
       strstr( strippedURL, "q=" ) != NULL  &&
       (strstr( strippedURL, "/search" ) != NULL ||
        strstr( strippedURL, "/uds/afs" ) != NULL ||
        strstr( strippedURL, "/uds/gwebsearch" ) != NULL ||
        strstr( strippedURL, "/uds/gvideosearch" ) != NULL ||
        strstr( strippedURL, "/uds/gimagesearch" ) != NULL ||
        strstr( strippedURL, "/uds/gblogsearch" ) != NULL ||
        strstr( strippedURL, "/videosearch" ) != NULL ||
        strstr( strippedURL, "/blogsearch" ) != NULL ||
        strstr( strippedURL, "/gwebsearch" ) != NULL ||
        strstr( strippedURL, "/groups" ) != NULL ||
        strstr( strippedURL, "/cse" ) != NULL ||
        strstr( strippedURL, "/products" ) != NULL ||
        strstr( strippedURL, "/images" ) != NULL ||
        strstr( strippedURL, "/custom" ) != NULL) )
   {
      char * safe;
      /* search for 'safe=off' and replace by 'safe=active' */
      safe = strstr( originalURL, "&safe=off" );
      if (safe != NULL)
      {
         safe += 6;
	 *safe++ = 'a';		/* 'o' */
	 *safe++ = 'c';		/* 'f' */
	 *safe++ = 't';		/* 'f' */
	 (void) memmove( safe+3, safe, strlen(safe)+1 );
	 *safe++ = 'i';
	 *safe++ = 'v';
	 *safe   = 'e';
      }
      strcat( originalURL, "&safe=active&safeui=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "webmenu.com" ) != NULL  &&		/* SAFESEARCH: webmenu.com */
       (strstr( strippedURL, "q_or=") != NULL  ||
        strstr( strippedURL, "q_and=") != NULL  ||
        strstr( strippedURL, "ss=") != NULL  ||
        strstr( strippedURL, "keyword=") != NULL  ||
	strstr( strippedURL, "query=") != NULL) )
   {
      char * p;
      /* TODO: fix problem of cookie override; a user can set preferences to turn the filter OFF
       * in the user preferences.
       */
      while ((p = strstr( originalURL, "&ss=n" )) != NULL)
         *(p+4) = 'y';
      strcat( originalURL, "&ss=y" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "blekko.com" ) != NULL &&		/* SAFESEARCH: blekko.com */
       strncmp( strippedURL, "/ws/", 4 ) == 0)
   {
      if (strchr( strippedURL, '?' ) == NULL)
	 strcat( originalURL, "?safesearch=2" );
      else
	 strcat( originalURL, "&safesearch=2" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "izito." ) != NULL  &&			/* SAFESEARCH: izito.* */
       (strstr( strippedURL, "query=" ) != NULL  ||
        strstr( strippedURL, "q=" ) != NULL))
   {
      strcat( originalURL, "&ss=y" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "zapmeta." ) != NULL  &&			/* SAFESEARCH: zapmeta.* */
       strstr( strippedURL, "vid=" ) != NULL  &&
       strstr( strippedURL, "q=" ) != NULL)
   {
      strcat( originalURL, "&ss=y" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "bing.com" ) != NULL  &&			/* SAFESEARCH: bing.com */
       strstr( strippedURL, "q=" ) != NULL)    			/* bing */
   {
      strcat( originalURL, "&ADLT=STRICT&filt=all" );
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "bing.co.uk" ) != NULL  &&		/* SAFESEARCH: bing.co.uk */
       strstr( strippedURL, "q=" ) != NULL)    			/* bing */
   {
      strcat( originalURL, "&ADLT=STRICT&filt=all" );
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strcmp( domain, "api.bing.net" ) == 0  &&		/* Safesearch: bing API */
       strncmp( strippedURL, "/json.aspx", 10 ) == 0  &&	/* called by searchgby.com */
       strstr( strippedURL, "query=" ) != NULL)
   {
      strcat( originalURL, "&Adult=Strict" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strcmp( domain, "search.searchcompletion.com" ) == 0  &&		/* SAFESEARCH: searchcompletion.com */
       strncmp( strippedURL, "/localsearchresults.aspx", 10 ) == 0  &&		/* search.searchcompletion.com/LocalSearchResults.aspx */
       strstr( strippedURL, "q=" ) != NULL)
   {
      strcat( originalURL, "&safe=on" );			/* TODO: fix this */
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "trovator.com" ) != NULL  &&		/* searchcompletion.com trovator.com */
       strstr( strippedURL, "q=" ) != NULL)
   {
      strcat( originalURL, "&fil=si" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, ".yauba.com" ) != NULL  &&		/* SAFESEARCH: yauba.com */
       strstr( strippedURL, "query=") != NULL)
   {
      strcat( originalURL, "&ss=y" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "forestle.org" ) != NULL  &&		/* SAFESEARCH: forestle.org */
       (strstr( strippedURL, "settings") != NULL  ||
        strstr( strippedURL, "q=") != NULL))
   {
      strcat( originalURL, "&adultfilter=noadult" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "zombol.com" ) != NULL  &&		/* SAFESEARCH: zombol.com */
       strstr( strippedURL, "/results") != NULL  &&
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&safe=active" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "kalooga.com" ) != NULL  &&		/* SAFESEARCH: kalooga.com */
       strstr( strippedURL, "search") != NULL  &&
       strstr( strippedURL, "query=") != NULL)
   {
      strcat( originalURL, "&filter=default" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "muuler.com" ) != NULL  &&		/* SAFESEARCH: muuler.com */
       strstr( strippedURL, "/result") != NULL  &&
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&safe=active" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "foozir.com" ) != NULL  &&		/* SAFESEARCH: foozir.com */
       strstr( strippedURL, "/result") != NULL  &&
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&safe=active" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "moons.it" ) != NULL  &&			/* SAFESEARCH: moons.it */
       strstr( strippedURL, "/ricerca") != NULL  &&
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&safe=active" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "wotbox.com" ) != NULL  &&		/* SAFESEARCH: wotbox.com */
       (strstr( strippedURL, "q=") != NULL  ||
        strstr( strippedURL, "op0=") != NULL) )
   {
      strcat( originalURL, "&a=true" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "ripple.org" ) != NULL  &&		/* SAFESEARCH: ripple.org */
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&safe=active" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "ant.com" ) != NULL  &&			/* SAFESEARCH: ant.com */
       strstr( strippedURL, "antq=") != NULL)
   {
      strcat( originalURL, "&safe=1" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if ((strstr( domain, "duck.co" ) != NULL ||			/* SAFESEARCH: duck.co */
        strstr( domain, "duckduckgo.com" ) != NULL)  &&         /* SAFESEARCH: duckduckgo.com  */
       strncmp( strippedURL, "/d.js", 5 ) == 0  &&
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&p=1" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "duckduckgo.org" ) != NULL  &&		/* SAFESEARCH: duckduckgo.org */
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&kp=1" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "zoower.com" ) != NULL  &&		/* SAFESEARCH: zoower.com */
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&safe=active" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if ((strstr( domain, "qbyrd.com" ) != NULL  ||  		/* SAFESEARCH: qbyrd.com */
        strstr( domain, "search-results.com" ) != NULL)  &&	/* SAFESEARCH: search-results.com */
       strstr( strippedURL, "q=") != NULL)
   {
      char * adt;
      adt = strstr( originalURL, "adt=1" );
      if (adt != NULL)
         *(adt+4) = '0';
      else
	 strcat( originalURL, "&adt=0" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "easysearch.org.uk" ) != NULL  &&	/* SAFESEARCH: easysearch.org.uk */
       strstr( strippedURL, "search") != NULL)
   {
      strcat( originalURL, "&safe=on" );
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "dly.net" ) != NULL  &&			/* SAFESEARCH: dly.net */
       (strstr( strippedURL, "/search?") != NULL  ||  strstr( strippedURL, "/custom?") != NULL)  &&
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&safe=active" );
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "ask.com" ) != NULL  &&
       strchr( strippedURL, '?' ) != NULL)    			/* SAFESEARCH: ask.com */
   {
      strcat( originalURL, "&adt=0" );
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strncmp( domain, "api.search.yahoo.", 17 ) == 0  &&	/* Safesearch: API yahoo.* */
       strstr( strippedURL, "query=" ) != NULL)
   {
      strcat( originalURL, "&adult_ok=0" );				
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if ((strstr( domain, ".terra." ) != NULL  &&  		/* SAFESEARCH: terra.* */
        strstr( domain, "busca" ) != NULL)  &&
       (strstr( strippedURL, "query=" ) != NULL  ||  
        strstr( strippedURL, "source=" ) != NULL) )	  /* .ar .br .cl .co .ec .es */
   {
      strcat( originalURL, "&npl=%26safe%3dhigh" );				
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strcmp( domain, "search.alot.com" ) == 0  &&		/* SAFESEARCH: alot.com */
       strstr( strippedURL, "q=" ) != NULL)
   {
      strcat( originalURL, "&f=1" );				
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "searchalot.com" ) != NULL  &&		/* SAFESEARCH: searchalot.com */
       strstr( strippedURL, "q=" ) != NULL)
   {
      strcat( originalURL, "&safesearch=high" );				
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "alltheinternet.com" ) != NULL  &&	/* SAFESEARCH: alltheinternet.com */
       strstr( strippedURL, "q=" ) != NULL)
   {
      strcat( originalURL, "&safesearch=high" );				
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "search.yahoo." ) != NULL  &&		/* SAFESEARCH: yahoo.* */
       strstr( strippedURL, "p=" ) != NULL)
   {
      strcat( originalURL, "&vm=r" );				
      /* TODO: investigate http://www.yahoo.com/r/sx/ *-http://search.yahoo.com/search */
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "excite." ) != NULL  &&			/* SAFESEARCH: excite.* */
       strstr( strippedURL, "search" ) != NULL  &&
       strchr( strippedURL, '?' ) != NULL)  			/* Excite */
   {
      strcat( originalURL, "&familyfilter=1&splash=filtered" );
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strncmp( domain, "search.msn.", 11 ) == 0)		/* SAFESEARCH: msn.* */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&adlt=strict" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strncmp( domain, "search.live.", 12 ) == 0  &&		/* SAFESEARCH: live.* */
       strstr( strippedURL, "q=" ) != NULL)
   {
      strcat( originalURL, "&adlt=strict" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strcmp( domain, "api.search.live.net" ) == 0  &&		/* Safesearch: live API */
       strstr( strippedURL, "sources=" ) != NULL)
   {
      strcat( originalURL, "&adlt=strict" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "yauba.co.in" ) != NULL  &&		/* SAFESEARCH: yauba.co.in */
       strstr( strippedURL, "query=" ) != NULL)
   {
      strcat( originalURL, "&ss=y" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "blinkx.com" ) != NULL  &&		/* SAFESEARCH: blinkx.com */
       strchr( strippedURL, '?' ) != NULL)
   {
      strcat( originalURL, "&safefilter=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strncmp( domain, "busca.ya.", 9 ) == 0  &&		/* SAFESEARCH: ya.* */
       strstr( strippedURL, "buscar=" ) != NULL)
   {
      strcat( originalURL, "&filtrofamiliar=Activado&safe=active" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strncmp( domain, "search.lycos.", 13 ) == 0)		/* SAFESEARCH: lycos.* */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&contentFilter=strict&family=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strncmp( domain, "suche.lycos.", 12 ) == 0)		/* Lycos .de .at  .ch */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&family=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strcmp( domain, "buscador.lycos.es" ) == 0)		/* Lycos .es */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&family=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strcmp( domain, "vachercher.lycos.fr" ) == 0)		/* Lycos .fr */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&family=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strcmp( domain, "cerca.lycos.it" ) == 0)			/* Lycos .it */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&family=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "webpile.it" ) != NULL  &&		/* webpile.it */
       strncmp( strippedURL, "search", 6 ) == 0)
   {
      strcat( originalURL, "&filtro=4" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strcmp( domain, "alltheweb.com" ) == 0)			/* SAFESEARCH: alltheweb.com */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&copt_offensive=on&nooc=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "dogpile.com" ) != NULL  ||		/* SAFESEARCH: dogpile.com */
       strstr( domain, "dogpile.co.uk" ) != NULL)		/* SAFESEARCH: dogpile.co.uk  */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&adultfilter=heavy" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strcmp( domain, "a9.com" ) == 0)				/* SAFESEARCH: a9.com */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&qsafe=high" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strcmp( domain, "hotbot.com" ) == 0)			/* SAFESEARCH: hotbot.com */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&adf=on&family=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "infospace.com" ) != NULL)		/* SAFESEARCH: infospace.com */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&familyfilter=1" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "metacrawler.com" ) != NULL)		/* SAFESEARCH: metacrawler.com */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&familyfilter=1" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "metaspy.com" ) != NULL)			/* SAFESEARCH: metaspy.com */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&familyfilter=1" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "webfetch.com" ) != NULL  ||		/* SAFESEARCH: webfetch.com */
       strstr( domain, "webfetch.co.uk" ) != NULL)		/* SAFESEARCH: webfetch.co.uk */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&familyfilter=1" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "webcrawler.com" ) != NULL)		/* SAFESEARCH: webcrawler.com */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&familyfilter=1" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "buscamundo.com" ) != NULL  &&		/* SAFESEARCH: buscamundo.com */
       strstr( strippedURL, "qu=") != NULL)
   {
      strcat( originalURL, "&filter=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strncmp( domain, "search", 6 ) == 0  &&
       strstr( domain, "foxnews.com" ) != NULL)			/* Safesearch: foxnews.com */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&familyfilter=1" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "altavista.com" ) != NULL  &&		/* SAFESEARCH: altavista.com */
       (strstr( strippedURL, "q=" ) != NULL  ||  strstr( strippedURL, "aqa=" ) != NULL))
   {
      /* Altavista redirects to Yahoo */
      char * dis;
      dis = strstr( strippedURL, "dis=1" );
      if (dis != NULL)						/* replace dis=1 by dis=0 */
         *(dis+4) = '0';
      else
	 strcat( originalURL, "&dis=0" );				/* OR add "dis=0" */
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }

   return UFDB_API_OK;
}
