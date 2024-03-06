//color_t typedef =======================================
#pragma pack(push, 2)

typedef struct color_t{
  int blue : 5;
  int green : 6;
  int red : 5;
} color_t;

#pragma pack(pop)

//RGB Conversion Macro =======================================

void create_pixel(int r, int g, int b, color_t *pixel){
  color_t new_pixel;
  new_pixel.red = r;
  new_pixel.green = g;
  new_pixel.blue = b;
  *pixel = new_pixel;
}

//"public" functions =======================================

//open, ioctl, mmap
void init_graphics();

//ioctl
void exit_graphics();

//select, read
char getkey();

//nanosleep
void sleep_ms(long ms);

//write
void clear_screen();

void draw_pixel(int x, int y, color_t color);

void draw_rect(int x1, int y1, int width, int height, color_t c);

void draw_text(int x, int y, const char *text, color_t c);

void draw_circle(int x, int y, int r, color_t c);
