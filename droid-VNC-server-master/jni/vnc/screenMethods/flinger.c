/*
droid vnc server - Android VNC server
Copyright (C) 2011 Jose Pereira <onaips@gmail.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <dlfcn.h>

#include "flinger.h"
#include "common.h"

void *flinger_lib = NULL;

close_fn_type close_flinger = NULL;
readfb_fn_type readfb_flinger = NULL;
getscreenformat_fn_type getscreenformat_flinger = NULL;

int initFlinger(void)
{
  //L("--Loading flinger native lib--\n");
  L("\n--Loading flinger native lib--\n");//tina add
  int i,len;
  char lib_name[64];

  len=ARR_LEN(compiled_sdks);
  //L("**********************");//tina add
  //L("tina: 1 lib_name = %s \n",lib_name);//tina add
  for (i=0;i<len;i++) {
    //sprintf(lib_name, DVNC_LIB_PATH "/libdvnc_flinger_sdk%d.so",compiled_sdks[i]);
	//sprintf(lib_name, DVNC_LIB_PATH "libdvnc_flinger_sdk%d.so",compiled_sdks[i]);//tina add
	 sprintf(lib_name, DVNC_FILES_PATH "libdvnc_flinger_sdk%d.so",compiled_sdks[i]);//tina add

   //L("**********************");//tina add
    //L("tina: 2 lib_name = %s \n",lib_name);//tina add

    if (flinger_lib != NULL)
    {
    	dlclose(flinger_lib);
    	//L("**********************");//tina add
        //L("tina: 1 flinger_lib = %s \n",flinger_lib);//tina add
    }

    L("Loading lib: %s\n",lib_name);
    //lib_name = "";//tina add
    flinger_lib = dlopen(lib_name, RTLD_NOW);

    //*************tina add*******************
  /*  if((access(lib_name,F_OK))!=-1)
    {
    	L("%s exist \n",lib_name);
    }
    else
    {
    	L("%s not exist \n",lib_name);
    }
    if((access(lib_name,R_OK))!=-1)
    {
    	L("%s can read \n",lib_name);
	}
	else
	{
		L("%s can't read \n",lib_name);
	}
    if((access(lib_name,W_OK))!=-1)
	{
    	L("%s can write \n",lib_name);
	}
	else
	{
		L("%s can't write \n",lib_name);
	}
    if((access(lib_name,X_OK))!=-1)
	{
    	L("%s can execute \n",lib_name);
	}
	else
	{
		L("%s can't execute \n",lib_name);
	}*/
    //*************end of tina add************


    if (flinger_lib == NULL){
      L("Couldnt load flinger library %s! Error string: %s\n",lib_name,dlerror());
      continue; 
    }
  
    init_fn_type init_flinger = dlsym(flinger_lib,"init_flinger");
    if(init_flinger == NULL) {
      L("Couldn't load init_flinger! Error string: %s\n",dlerror());
      continue;
    }
  
    close_flinger = dlsym(flinger_lib,"close_flinger");
    if(close_flinger == NULL) {
      L("Couldn't load close_flinger! Error string: %s\n",dlerror());
      continue;
    }

    readfb_flinger = dlsym(flinger_lib,"readfb_flinger");
    if(readfb_flinger == NULL) {
      L("Couldn't load readfb_flinger! Error string: %s\n",dlerror());
      continue;
    }

    getscreenformat_flinger = dlsym(flinger_lib,"getscreenformat_flinger");
    if(getscreenformat_flinger == NULL) {
      L("Couldn't load get_screenformat! Error string: %s\n",dlerror());
      continue;
    }
L("AKI1\n");

    int ret = init_flinger();
    L("AKII12");
    if (ret == -1) {
       L("flinger method not supported by this device!\n");
       continue;
    }
L("AKI2\n");

    screenformat = getScreenFormatFlinger();
    if ( screenformat.width <= 0 ) {
      L("Error: I have received a bad screen size from flinger.\n");
      continue;;
    }
  
    if ( readBufferFlinger() == NULL) {
      L("Error: Could not read surfaceflinger buffer!\n");
      continue;
    }
    return 0;
  }
  return -1;
}

screenFormat getScreenFormatFlinger(void)
{
  screenFormat f;
  if (getscreenformat_flinger)
     f = getscreenformat_flinger();
  return f;
}

void closeFlinger(void)
{
  if (close_flinger)
    close_flinger();
  if (flinger_lib)
    dlclose(flinger_lib);
}

unsigned char *readBufferFlinger(void)
{
  if (readfb_flinger)
    return readfb_flinger();
  return NULL;
}

