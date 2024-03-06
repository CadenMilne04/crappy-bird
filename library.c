#include <fcntl.h> //open()
#include <sys/mman.h> //mmap()
#include <sys/ioctl.h> //ioctl()
#include <linux/fb.h> //FBIOGET_*
#include <termios.h> //termios
#include <sys/select.h> //select()
#include <time.h> //nanosleep(), timespec
#include <sys/syscall.h>

#include "graphics.h" //my interface
#include "iso_font.h" //the apple font from the project

//Global Variables ========
int fd;
color_t* file_contents;

void init_graphics(){
  //1. Open the graphics device (framebuffer)
  fd = open("/dev/fb0", O_RDWR); //file_descriptor

  //3.Calculating the length of the area to map
 
  //Getting fixed screen information
  struct fb_fix_screeninfo fix_info;
  int r2 = ioctl(fd, FBIOGET_FSCREENINFO, &fix_info);

  //Getting variable screen information
  struct fb_var_screeninfo var_info;
  int r1 = ioctl(fd, FBIOGET_VSCREENINFO, &var_info);

  size_t x = fix_info.line_length; //x resolution * 2, it includes bit depth
  size_t y = var_info.yres_virtual; //y resolution
  size_t len = x * y; //total size of file to map in bytes
  
  //2. Memory Mapping
  file_contents = mmap(0, len, PROT_WRITE, MAP_SHARED, fd, 0);

  //4. Use ioctl system call to disable keypress echo
  //Get old settings
  struct termios old_settings;
  ioctl(0, TCGETS, &old_settings);
  
  //Set new settings
  struct termios new_settings = old_settings;
  new_settings.c_lflag &= ~(ICANON | ECHO);
  ioctl(0, TCSETS, &new_settings);

}

void exit_graphics(){

  //Fix Terminal Settings
  //Get old settings
  struct termios old_settings;
  ioctl(0, TCGETS, &old_settings);
  
  //Set new settings
  struct termios new_settings = old_settings;
  new_settings.c_lflag |= (ICANON | ECHO);
  ioctl(0, TCSETS, &new_settings);

  //Close the file decriptor
  close(fd);
}

char getkey(){
  fd_set fds; //set of file descriptors

  FD_ZERO(&fds); //initialize the set of file descriptors
  FD_SET(0, &fds); //add the STDIN file number to the set
  
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  //Check if an input is exists on STDIN
  int res = select(0 + 1, &fds, NULL, NULL, &tv);

  if (res){
    //Read which key from the STDIN 
    char key[1];
    read(0, key, 1);

    return *key;
  }
  else{
    return ' ';
  }
}

void sleep_ms(long ms){
  //miliseconds => nanoseconds (ms * 1,000,000 = ns)
  long ns = ms * 1000000;
  
  struct timespec ts; //create timespec struct
  ts.tv_sec = 0;
  ts.tv_nsec = ns; //set timespec nanoseconds to our value

  nanosleep(&ts, NULL); 
}

void draw_pixel(int x, int y, color_t color){
  //Calculate the address of the pixel.
  //Stored in row-major order
  int offset = (640 * y) + x;
  // void* address = file_contents[offset];
  file_contents[offset] = color;
}

void draw_rect(int x1, int y1, int width, int height, color_t c){
  int i;
  for(i = 0; i < width; i++){
    int j;
    for(j = 0; j < height; j++){
      draw_pixel(x1 + i, y1 + j ,c);
    }
  }
}


void draw_text(int x, int y, const char *text, color_t c){
  int x1 = x;

  //Loop through each letter
  while(*text != '\0'){
    int char_code = (int) *text;
    int start_location = char_code * 16;
    //check each integer byte. if it is a 1, then draw a pixel.
    int i = 0;
    for(i = 0; i < 16; i++){
      int current_position = start_location + i;
      unsigned char current_byte = iso_font[current_position];
      int j = 0;
      for(j = 0; j < 8; j++){
        
        if(current_byte & 0x01){
          draw_pixel(x1 + j, y + i, c);
        }

        current_byte = current_byte >> 1;
      }
    }

    x1 = x1 + 8;
    text = text + 1;
  }
}

//CODE TAKEN FROM: https://www.geeksforgeeks.org/mid-point-circle-drawing-algorithm/
void draw_circle(int x, int y, int r, color_t c){
  int x1 = r, y1 = 0;

  draw_pixel(x1 + x, y1 + y, c);

  if ( r > 0 ){
      draw_pixel(x1 + x, -y1 + y, c);
      draw_pixel(y1 + x, x1 + y, c);
      draw_pixel(-y1 + x, x1 + y, c);
  }

  int P = 1 - r;
  while(x1 > y1){
    y1++;

    if(P <= 0){
      P = P + 2*y1 + 1;
    }
    else{
      x1--;
      P = P + 2*y1 - 2*x1 +1;
    }

    if(x1 < y1){
      break;
    }

    draw_pixel(x1 + x, y1 + y, c);
    draw_pixel(-x1 + x, y1 + y, c);
    draw_pixel(x1 + x, -y1 + y, c);
    draw_pixel(-x1 + x, -y1 + y, c);

    if(x1 != y1){
      draw_pixel(y1 + x, x1 + y, c);
      draw_pixel(-y1 + x, x1 + y, c);
      draw_pixel(y1 + x, -x1 + y, c);
      draw_pixel(-y1 + x, -x1 + y, c);
    }
  }
}

void clear_screen(){
  char ansi_code[] = "\x1B[2J";

  //write to STDOUT
  write(1, ansi_code, 5);
}
