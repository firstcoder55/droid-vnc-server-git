/*
droid VNC server  - a vnc server for android
Copyright (C) 2011 Jose Pereira <onaips@gmail.com>

Other contributors:
  Oleksandr Andrushchenko <andr2000@gmail.com>

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

#include "framebuffer.h"
#include "gui.h"

int fbfd = -1;
unsigned int *fbmmap;

char framebuffer_device[256] = "/dev/graphics/fb0";

struct fb_var_screeninfo scrinfo;
struct fb_fix_screeninfo fscrinfo;

void FB_setDevice(char *s)
{
  strcpy(framebuffer_device,s);
}

void update_fb_info(void)
{  
  if (ioctl(fbfd, FBIOGET_VSCREENINFO, &scrinfo) != 0) {
    L("ioctl error\n");
    sendMsgToGui("~SHOW|Framebuffer ioctl error, please try out other display grab method\n");
    exit(EXIT_FAILURE);
  }
}

inline int roundUpToPageSize(int x) {
  return (x + (PAGE_SIZE-1)) & ~(PAGE_SIZE-1);
}

int initFB(void)
{  
  //L("--Initializing framebuffer access method--\n");
  L("\n--Initializing framebuffer access method--\n");//tina add

  fbmmap = MAP_FAILED;

  if ((fbfd = open(framebuffer_device, O_RDWR)) == -1) { 
    L("Cannot open fb device %s\n", framebuffer_device);
    sendMsgToGui("~SHOW|Cannot open fb device, please try out other display grab method\n");
    return -1;
  }

  update_fb_info();


  if (ioctl(fbfd, FBIOGET_FSCREENINFO, &fscrinfo) != 0) {
    L("ioctl error\n");
    return -1;
  }

  L("line_lenght=%d xres=%d, yres=%d, xresv=%d, yresv=%d, xoffs=%d, yoffs=%d, bpp=%d\n",
    (int)fscrinfo.line_length,(int)scrinfo.xres, (int)scrinfo.yres,
    (int)scrinfo.xres_virtual, (int)scrinfo.yres_virtual,
    (int)scrinfo.xoffset, (int)scrinfo.yoffset,
    (int)scrinfo.bits_per_pixel);

  size_t size = scrinfo.yres_virtual;
  if (size < scrinfo.yres * 2) {
    L("Using Droid workaround\n");
    size = scrinfo.yres * 2;
  }

  if ((scrinfo.bits_per_pixel == 24)) {
    scrinfo.bits_per_pixel = 32;
    L("24-bit XRGB display detected\n");
  }

  size = scrinfo.yres_virtual;//tina add
  //if size = scrinfo.yres * 2; mmap will fail, don't know why

  size_t fbSize = roundUpToPageSize(fscrinfo.line_length * size);

  fbmmap = mmap(NULL, fbSize , PROT_READ|PROT_WRITE ,  MAP_SHARED , fbfd, 0);

  if (fbmmap == MAP_FAILED) { 
    L("mmap failed\n");
    sendMsgToGui("~SHOW|Framebuffer mmap failed, please try out other display grab method\n");
    return -1;
  } 

  screenformat.bitsPerPixel = scrinfo.bits_per_pixel;
  screenformat.size = scrinfo.xres * scrinfo.yres * scrinfo.bits_per_pixel / CHAR_BIT;
  screenformat.width = scrinfo.xres;
  screenformat.height = scrinfo.yres;
  screenformat.redShift = scrinfo.red.offset;
  screenformat.redMax = scrinfo.red.length;
  screenformat.greenShift = scrinfo.green.offset;
  screenformat.greenMax = scrinfo.green.length;
  screenformat.blueShift = scrinfo.blue.offset;
  screenformat.blueMax = scrinfo.blue.length;
  screenformat.alphaShift = scrinfo.transp.offset;
  screenformat.alphaMax = scrinfo.transp.length;

  return 1;
} 

void closeFB(void) 
{
  if(fbfd != -1)
  close(fbfd);
} 

struct fb_var_screeninfo FB_getscrinfo(void)
{
  return scrinfo;
}

unsigned int *readBufferFB(void)
{
  update_fb_info();
  return fbmmap;
}
