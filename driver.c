typedef unsigned short color_t;

void clear_screen();
void exit_graphics();
void init_graphics();
char getkey();
void sleep_ms(long ms);

void draw_rect(int x1, int y1, int width, int height, color_t c);

void intToString(int num, char* str) {
    int i = 0;
    int isNegative = 0;

    if (num < 0) {
        isNegative = 1;
        num = -num;
    }
    do {
        str[i++] = num % 10 + '0';
        num /= 10;
    } while (num > 0);

    if (isNegative) {
        str[i++] = '-';
    }

    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }

    str[i] = '\0';
}

//Color Pallette ====================
color_t green;
color_t yellow;
color_t black;


//Game Variables ====================
int gravity;
int in_game;
int high_score;

int main(){
  //Initializes the mapped pixel file from the frame buffer.
  init_graphics();

  high_score = 0;

  create_pixel(0, 63, 0, &green);
  create_pixel(31, 63, 0, &yellow);
  create_pixel(0, 0, 0, &black);

  //start with a clear screen
  clear_screen();
  
    //Main Loop =========================
  int i = 0;
  while (1) {
    //Prepetually Draw the help message.
    char *titlescreen = "Crappy Bird!";
    draw_text(200, 200, titlescreen, yellow);
    
    char *hs_label = "High Score:";
    draw_text(200, 220, hs_label, green);
    
    char hs[4];
    intToString(high_score, hs);
    draw_text(200, 240, &hs, green);
  
    char *fly = "Press 'x' to fly up while in game";
    draw_text(200, 300, fly, green);

    char *start = "Press 's' to start game";
    draw_text(200, 320, start, green);

    char *quit = "Press 'q' to quit program";
    draw_text(200, 340, quit, green);

    char *rules = "Fly in between the green pipes to increase score.";
    draw_text(200, 380, rules, green);

    char *caden = "A Caden Milne Production";
    draw_text(200, 420, caden, yellow);

    char key = getkey();

    //Press 'q' on the keyboard to quit.
    if(key == 'q'){
      clear_screen();
      break;
    }

    if(key == 's'){
      clear_screen();
      in_game = 1;
    }

    //Press 'h' to display the "help" menu.
    if(key == 'h'){
      clear_screen();
          }

    if(in_game){
      int bird_height = 230;

      int mod[10] = {0, -60, 50, -40, 75, -40, 55, -65, 60, -50}; 
      int n = 0;

      int rectangle_left = 520;
      int rectangle_top = 150;
      int rectangle_bottom = 330;
      int rectangle_right = 630;

      int gravity = 4;
      int game_speed = 3;
      int game_score = 0;

      while(1){
        char game_key = getkey();
        if(game_key == 'q'){
          clear_screen();
          in_game = 0;
          break;
        }
        //Scoreboard ======
        char* label = "Score:";
        draw_text(30, 50, label, yellow);
        char score[20];
        intToString(game_score, score);
        draw_text(30, 70, &score, yellow);

        //Collisions ======
        int proximity_top = bird_height - rectangle_top;
        int proximity_bottom = rectangle_bottom - bird_height;
        int proximity_left = rectangle_left - 125;
        int proximity_right = rectangle_right - 125;

        int proximity_frame = 480 - bird_height; 

        if(proximity_left < 10){
          if(proximity_top < 10 || proximity_bottom < 10){
          clear_screen();
          in_game = 0;
          if(game_score > high_score){
              high_score = game_score;
            }
          break;
          } 
        }
        if(proximity_frame < 30){
          clear_screen();
          in_game = 0;
          if(game_score > high_score){
            high_score = game_score;
          }
          break;
        }

        //Bird Controls ===
        if(game_key == 'x'){
          draw_circle(125, bird_height, 20, black);
          bird_height = bird_height - 70;
          draw_circle(125, bird_height, 20, yellow);
        }
        draw_circle(125, bird_height, 20, black);
        bird_height = bird_height + gravity;
        draw_circle(125, bird_height, 20, yellow);

        //Pipes =========
        if(rectangle_left <= game_speed){
          draw_rect(rectangle_left, 0, 110, 150 + mod[n], black);
          draw_rect(rectangle_left, 330 + mod[n], 110, 150 - mod[n], black);
          rectangle_left = 520;
          rectangle_right = 630;
          game_score++;
          if(game_score % 3 == 0){
            game_speed++;
          }
          if(n == 9){
            n = 0;
          }
          n++;
          rectangle_top = 150 + mod[n];
          rectangle_bottom = 330 + mod[n]; 
          draw_rect(rectangle_left, 0, 110, 150 + mod[n], green);
          draw_rect(rectangle_left, 330 + mod[n], 110, 150 - mod[n], green);
        }

        rectangle_left = rectangle_left - game_speed; 
        rectangle_right= rectangle_right - game_speed; 
        draw_rect(rectangle_left + 110, 0, game_speed, 150 + mod[n], black);
        draw_rect(rectangle_left + 110, 330 + mod[n], game_speed, 150 - mod[n], black);
        draw_rect(rectangle_left, 0, 110, 150 + mod[n], green);
        draw_rect(rectangle_left, 330 + mod[n], 110, 150 - mod[n], green);
         
        sleep_ms(1);
      }
      clear_screen();
    }

    sleep_ms(1);
  }

  exit_graphics();
  return 0;
}

