//---------------------------------------------
//               Header Files
//---------------------------------------------
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <graphics.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

//---------------------------------------------
//            Constants/Globals
//---------------------------------------------
//Keys
const int KEY_ENTER=13;
const int KEY_SPACEBAR=32;
const int KEY_ESC=27;
const int KEY_BACKSPACE=8;
//True, False Values
const int YES=1;
const int NO=0;
//Options
const int MENU=300;
const int PLAY=301;
const int HOWTOPLAY=302;
const int HIGHSCORES=303;
const int RESUME=304;
const int RESTART=305;
const int QUIT=306;
//Building details
const int WIDTH_OF_BUIILDING=25;
const int NO_OF_BUILDINGS=20;
//Screen Resolution
const int SCREEN_WIDTH=500;
const int SCREEN_HEIGHT=400;
//Image pointers
void *current=NULL;
void *home_screen=NULL;
void *hscore_screen=NULL;
void *how_to_play=NULL;
void *sky=NULL;
void *congo=NULL;
void *oops=NULL;
void *new_score=NULL;
void *enter_name=NULL;
//Color codes for buildings
const int total_colors=7;
enum {red, green, blue, yellow, orange, purple, grey};
//Extra Colors
const int CELTICS = COLOR(0, 97, 28);
const int GOLDGREEN = COLOR(170, 221, 0);
const int SURF = COLOR(99, 209, 244);
const int ORANGE = COLOR(255, 102, 0);
const int BRIGHTRED = COLOR(255, 0, 0);
int i=0, page=0, mouse_x=0, mouse_y=0, flag=0;

//---------------------------------------------
//             Function Prototypes
//---------------------------------------------
void read_all_images();
int display_menu();
int display_instructions();
int highscore_menu();
void initialise_gamescreen();
void draw_city();
int check_city_destroyed();
void display_crash(int x, int y);
int calc_score(int h, int b);
void game_over();
int pause(void *p);
int run_game();
void free_memory();
void free_gamescreen();

//---------------------------------------------
//                  CLASSES
//---------------------------------------------
class button{
      int x, y, width, height;
      public:
             void draw(int x1, int y1, char a[], int bgcolor, int txtcolor);
             void highlight();
             int is_clicked(int mouse_x, int mouse_y);
};

button play, howtoplay, highscores, quit; //Main Menu Buttons
button back; //"How To Play?" and "Highscores" Buttons
button resume; //Pause Menu Button

class building{
      int left, top, height, width, color, no_of_blocks;
      void *buildblock;
      public:
             void filldetails(int left_coord, int top_coord, int c, int h);
             void drawbuilding();
             int getwidth();
             int gettop();
             int getheight();
             void updatebuilding();
             void destroybuilding();
}buildset[NO_OF_BUILDINGS];

class player{
      int score, score_x, score_y;
      char scorestring[15];
      char name[21];
      public:
             void initialise();
             void update_score(int a);
             void display_score();
             int get_score();
             void change_score(int x);
             char* get_name(); 
             void change_name(char a[]);
}playa;

class helicopter{
      int heli_x, heli_y, collision_status;
      int speed, info_x, info_y, altitude;
      char info_string[30];
      public:
             void initialise();
             void change_speed(int);
             void draw_heli();
             void draw_speedometer();
             void move_heli();
             int check_collision();
             void collide();
             void destroy_heli();
             int getheli_x();
             int getheli_y();
}alpha;

class bomb{
      int bomb_x, bomb_y, bombs, color;
      int status; //Collision Status
      public:
             void initialise();
             void draw_bomb();
             int collide();
             void drop_bomb();
             void move_bomb();
             int getbombs();
             int getbomb_x();
             int getbomb_y();
             void destroy_bomb();
}thebomb;

class highscore_datafile{
      ifstream fin;
      ofstream fout;
      int eof, is_highscore;
      player p[10];
      public:
             int initialise();
             void display_highscores(int x, int y);
             void save_highscore(int scr, void *cur);
             void new_highscore(int y);
}file;

//---------------------------------------------
//               MAIN Function
//---------------------------------------------
main()
{
      initwindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TownBreakdown", 370, 150, true);
      srand(time(NULL));
      int option, menu_option;
      option=file.initialise(); //Initialise highscores
      if(option==QUIT)
      {
                      goto END;
      }
      setactivepage(0);
      readimagefile("data/credits.gif", 0, 0, 500, 400);
      delay(3000);
      readimagefile("data/intro.gif", 0, 0, 500, 400);
      delay(1000);
      read_all_images(); //Read external images and backgrounds
      A:
      option=display_menu();
      B:
      switch(option)
      {
                    case PLAY: {
                         C:
                         initialise_gamescreen();
                         menu_option=run_game();
                         free_gamescreen();
                         if(menu_option==QUIT)
                         break;
                         else if(menu_option==MENU)
                              goto A;
                              else if(menu_option==RESTART)
                                   goto C;
                         }
                    case HOWTOPLAY: {
                         menu_option=display_instructions();
                         if(menu_option==MENU)
                         goto A;
                         else if(menu_option==PLAY)
                              {
                                                   option=PLAY;
                                                   goto B;
                              }
                              else
                              break;
                         }
                    case HIGHSCORES: {
                         menu_option=highscore_menu();
                         if(menu_option==MENU)
                         goto A;
                         else if(menu_option==PLAY)
                              {
                                                   option=PLAY;
                                                   goto B;
                              }
                              else
                              break;
                         }
                    case QUIT: {
                         break;
                         }
      }
      free_memory();
      END:
      closegraph();
}

//---------------------------------------------
//             "BUTTON" Definitions
//---------------------------------------------
void button::draw(int x1, int y1, char a[], int bgcolor, int txtcolor)
{
     //Set Parameters
     settextstyle(COMPLEX_FONT,HORIZ_DIR,2);
     x=x1;
     y=y1;
     height=textheight(a);
     width=textwidth(a);
     //Draw Box
     int points[]={x,y,x+width+10,y,x+width+10,y+height+10,x,y+height+10,x,y};
     setfillstyle(SOLID_FILL, bgcolor);
     fillpoly(5,points);
     setcolor(txtcolor);
     rectangle(x,y,x+width+10,y+height+10);
     //Write Text
     setbkcolor(bgcolor);
     outtextxy(x+5,y+5,a);
     setfillstyle(EMPTY_FILL, WHITE);
     settextstyle(COMPLEX_FONT, HORIZ_DIR, 1);
     setbkcolor(BLACK);
     setcolor(WHITE);
}

void button::highlight()
{
     setcolor(YELLOW);
     setlinestyle(SOLID_LINE,0,3);
     rectangle(x-2,y-2,x+width+12,y+height+12);
     setlinestyle(SOLID_LINE,0,1);
     setcolor(WHITE);
}

int button::is_clicked(int mouse_x, int mouse_y)
{
    if(mouse_x>x&&mouse_x<x+width+10&&mouse_y>y&&mouse_y<y+height+10)
    return YES;
    else
    return NO;
}

//----------------------------------------------
//            "BUILDING" Definitions
//----------------------------------------------
int build_var=0;
void building::filldetails(int left_coord, int top_coord, int c, int h)
{
     height=h;
     width=WIDTH_OF_BUIILDING;
     left=left_coord;
     top=top_coord;
     color=c;
     if(height==50)
     no_of_blocks=2;
     else if(height==100)
          no_of_blocks=4;
          else if(height==150)
               no_of_blocks=6;
     switch(color)
     {
                  case red: {
                       buildblock=malloc(imagesize(0, 0, 25, 25));
                       if(buildblock==NULL)
                       exit(0);
                       readimagefile("data/build_block_red.gif", 0, 0, 25, 25);
                       getimage(0, 0, 25, 25, buildblock);
                       break;
                       }
                  case green: {
                       buildblock=malloc(imagesize(0, 0, 25, 25));
                       if(buildblock==NULL)
                       exit(0);
                       readimagefile("data/build_block_green.gif", 0, 0, 25, 25);
                       getimage(0, 0, 25, 25, buildblock);
                       break;
                       }
                  case blue: {
                       buildblock=malloc(imagesize(0, 0, 25, 25));
                       if(buildblock==NULL)
                       exit(0);
                       readimagefile("data/build_block_blue.gif", 0, 0, 25, 25);
                       getimage(0, 0, 25, 25, buildblock);
                       break;
                       }
                  case yellow: {
                       buildblock=malloc(imagesize(0, 0, 25, 25));
                       if(buildblock==NULL)
                       exit(0);
                       readimagefile("data/build_block_yellow.gif", 0, 0, 25, 25);
                       getimage(0, 0, 25, 25, buildblock);
                       break;
                       }
                  case orange: {
                       buildblock=malloc(imagesize(0, 0, 25, 25));
                       if(buildblock==NULL)
                       exit(0);
                       readimagefile("data/build_block_orange.gif", 0, 0, 25, 25);
                       getimage(0, 0, 25, 25, buildblock);
                       break;
                       }
                  case purple: {
                       buildblock=malloc(imagesize(0, 0, 25, 25));
                       if(buildblock==NULL)
                       exit(0);
                       readimagefile("data/build_block_purple.gif", 0, 0, 25, 25);
                       getimage(0, 0, 25, 25, buildblock);
                       break;
                       }
                  case grey: {
                       buildblock=malloc(imagesize(0, 0, 25, 25));
                       if(buildblock==NULL)
                       exit(0);
                       readimagefile("data/build_block_grey.gif", 0, 0, 25, 25);
                       getimage(0, 0, 25, 25, buildblock);
                       break;
                       }
     }
}

void building::drawbuilding()
{
     for(build_var=0;build_var<no_of_blocks;build_var++)
     {
                                 putimage(left, (top+(build_var*25)), buildblock, COPY_PUT); 
     }
}
             
int building::getwidth()
{ 
    return width; 
}
             
int building::gettop()
{
    return top; 
}
             
int building::getheight()
{ 
    return height; 
}
             
void building::updatebuilding()
{ 
     if(top<350)
     top+=25;
     if(height>0)
     {
                 height-=25;
                 no_of_blocks--;
     }
     else
     {
         height=0;
         no_of_blocks=0;
     }
}

void building::destroybuilding()
{
     free(buildblock);
     buildblock=NULL;
}

//----------------------------------------------
//            "PLAYER" Definitions
//----------------------------------------------
void player::initialise()
{
     score=0;
     score_x=25;
     score_y=0;
     strcpy(name,"");
}
             
void player::update_score(int a)
{
     score+=a;
}
             
void player::display_score()
{
     sprintf(scorestring, "Score: %i", score);
     settextstyle(COMPLEX_FONT, HORIZ_DIR, 2);
     outtextxy(score_x, score_y, scorestring);
     settextstyle(COMPLEX_FONT, HORIZ_DIR, 1);
}

void player::change_score(int x)
{
     score=x;
}

int player::get_score()
{
    return score;
}

char* player::get_name()
{
      return name;
}

void player::change_name(char a[])
{
     strcpy(name, a);
}
//----------------------------------------------
//            "HELICOPTER" Definitions
//----------------------------------------------
int heli_var=0, heli_var2=0;
void helicopter::initialise()
{
     heli_x=-30;
     heli_y=21;
     speed=5;
     altitude=329-heli_y;
     info_x=175;
     info_y=0;
     collision_status=NO;
}
             
void helicopter::collide()
{
     collision_status=YES;
}
             
void helicopter::destroy_heli()
{
     heli_x=-40;
     for(heli_var=0;heli_var<2;heli_var++)
     {
      setactivepage(heli_var);
      draw_heli();
     }
}
             
int helicopter::getheli_x()
{ 
    return heli_x;
}
             
int helicopter::getheli_y()
{ 
    return heli_y; 
}

void helicopter::draw_heli()
{
     setcolor(BLACK);
     setfillstyle(SOLID_FILL, GREEN);
     //Draw End Extension
     int tailjoint[]={heli_x, heli_y+14, heli_x+11, heli_y+14, heli_x+11, heli_y+16, heli_x, heli_y+16, heli_x, heli_y+14};
     fillpoly(5, tailjoint);
     //Draw TailWing
     int tail_pts[]={heli_x, heli_y+10, heli_x, heli_y+20, heli_x+4, heli_y+15, heli_x, heli_y+10};
     fillpoly(4, tail_pts);
     putpixel(heli_x+2, heli_y+15, YELLOW);
     putpixel(heli_x+1, heli_y+15, YELLOW);
     //Draw BaseLegs
     line(heli_x+18, heli_y+20, heli_x+16, heli_y+22);
     line(heli_x+22, heli_y+20, heli_x+24, heli_y+22);
     line(heli_x+16, heli_y+22, heli_x+24, heli_y+22);
     //Draw LeftBlade and RightBlade
     int lwing_pts[]={heli_x+19, heli_y+7, heli_x+9, heli_y+5, heli_x+9, heli_y+2, heli_x+19, heli_y+7};
     int rwing_pts[]={heli_x+21, heli_y+7, heli_x+31, heli_y+5, heli_x+31, heli_y+2, heli_x+21, heli_y+7};
     fillpoly(4, lwing_pts);
     fillpoly(4, rwing_pts);
     //Draw Body
     ellipse(heli_x+20, heli_y+15, 0, 360, 10, 5);
     fillellipse(heli_x+20, heli_y+15, 10, 5);
     //Draw BladeHolder
     int wingjoint[]={heli_x+19, heli_y+7, heli_x+21, heli_y+7, heli_x+21, heli_y+10, heli_x+19, heli_y+10, heli_x+19, heli_y+7};
     setfillstyle(SOLID_FILL, YELLOW);
     fillpoly(5, wingjoint);
     //Draw Bomb Launching Pad
     putpixel((heli_x+15), (heli_y+19), YELLOW);
     //Draw Cabin
     line(heli_x+20, heli_y+10, heli_x+20, heli_y+15);
     line(heli_x+20, heli_y+15, heli_x+29, heli_y+15);
     int window_pts[]={heli_x+20, heli_y+10, heli_x+20, heli_y+15, heli_x+29, heli_y+15, heli_x+20, heli_y+10};
     setcolor(SURF);
     setfillstyle(SOLID_FILL, SURF);
     fillpoly(4, window_pts);
     setfillstyle(EMPTY_FILL, WHITE);
     setcolor(WHITE);
}

void helicopter::draw_speedometer()
{
     sprintf(info_string, "Altitude: %i Speed: %i%%", (altitude), (speed*10));
     settextstyle(COMPLEX_FONT, HORIZ_DIR, 2);
     outtextxy(info_x, info_y, info_string);
     settextstyle(COMPLEX_FONT, HORIZ_DIR, 1);
}

void helicopter::change_speed(int x)
{
     if(x==KEY_RIGHT)
     {
                               if(speed<10)
                               speed++;
     }
     else if(x==KEY_LEFT)
          {
                         if(speed>1)
                         speed--;
          }
}

void helicopter::move_heli()
{
     if(heli_x<=SCREEN_WIDTH)
     heli_x+=speed;
     else if(heli_x>SCREEN_WIDTH)
          {
                        if(heli_y<320)
                        { 
                                      heli_y+=30;
                                      altitude-=30;
                                      heli_x=-30;
                        }
          }
     if(speed>0&&speed<5)
     {
                         heli_y++;
                         altitude--;
     }
}

int helicopter::check_collision()
{
    if(heli_y+22>350)
    collision_status=YES;
    else if(heli_x+30<SCREEN_WIDTH)
         {
                              for(heli_var2=0;heli_var2<=30;heli_var2++)
                              {
                               heli_var=(heli_x+heli_var2)/buildset[0].getwidth();
                               if(heli_var>=0)
                               {
                                              if(heli_y+22>buildset[heli_var].gettop())
                                              {
                                                                                       collision_status=YES;
                                              }
                               }
                              }
         }
         return collision_status;
}

//----------------------------------------------
//              "BOMB" Definitions
//----------------------------------------------
int bomb_var=0, bomb_var2=0;
void bomb::draw_bomb()
{
     int pts[]={bomb_x-3, bomb_y-4, bomb_x+2, bomb_y-4, bomb_x+2, bomb_y-9, bomb_x-3, bomb_y-9, bomb_x-3, bomb_y-4};
     setcolor(color);
     fillellipse(bomb_x, bomb_y-2, 3, 3);
     fillpoly(5, pts);
     setcolor(BRIGHTRED);
     line(bomb_x-3, bomb_y-3, bomb_x+2, bomb_y-3);
     setcolor(WHITE);
     //Flames
     putpixel(bomb_x-1, bomb_y-10, YELLOW);
     putpixel(bomb_x-2, bomb_y-11, YELLOW);
     putpixel(bomb_x-3, bomb_y-12, ORANGE);
     putpixel(bomb_x-4, bomb_y-13, ORANGE);
     putpixel(bomb_x, bomb_y-10, YELLOW);
     putpixel(bomb_x+1, bomb_y-11, YELLOW);
     putpixel(bomb_x+2, bomb_y-12, ORANGE);
     putpixel(bomb_x+3, bomb_y-13, ORANGE);
     putpixel(bomb_x-1, bomb_y-11, YELLOW);
     putpixel(bomb_x, bomb_y-11, YELLOW);
     putpixel(bomb_x-1, bomb_y-12, YELLOW);
     putpixel(bomb_x, bomb_y-12, YELLOW);
     putpixel(bomb_x-2, bomb_y-13, ORANGE);
     putpixel(bomb_x+1, bomb_y-13, ORANGE);
     putpixel(bomb_x-3, bomb_y-14, ORANGE);
     putpixel(bomb_x-1, bomb_y-14, ORANGE);
     putpixel(bomb_x, bomb_y-14, ORANGE);
     putpixel(bomb_x+2, bomb_y-14, ORANGE);
}

int bomb::collide()
{
    if(bomb_y<348)
    {
                    for(bomb_var2=-3;bomb_var2<=2;bomb_var2++)
                    {
                     bomb_var=(bomb_x+bomb_var2)/buildset[0].getwidth();
                     if(bomb_var>=0)
                     {
                      if(bomb_y>buildset[bomb_var].gettop()&&buildset[bomb_var].getheight()!=0)
                      {
                                                                                                buildset[bomb_var].updatebuilding();
                                                                                                status=YES;
                                                                                                break;     
                      }
                     }
                    }
                    return status;
    }
    else if(bomb_y>348)
         {
                       destroy_bomb();
                       return status;
         }
         else
         return status;
}

void bomb::drop_bomb()
{
     if((alpha.getheli_x()+15)>0&&(alpha.getheli_x()+15)<SCREEN_WIDTH)
     {
                                                             bomb_x=(alpha.getheli_x())+15;
                                                             bomb_y=(alpha.getheli_y())+15;
                                                             bombs=1;
     }
}

void bomb::initialise()
{
     bomb_x=-10;
     bomb_y=-10;
     bombs=0;
     color=BLACK;
     status=NO;
}
             
void bomb::move_bomb()
{
     if(bomb_y<350)
     bomb_y+=14;
}

int bomb::getbombs()
{ 
    return bombs; 
}
             
int bomb::getbomb_x()
{
    return bomb_x;
}
             
int bomb::getbomb_y()
{
    return bomb_y;
}

void bomb::destroy_bomb()
{
     bomb_x=-10;
     bomb_y=-10;
     bombs=0;
     status=NO;
}

//----------------------------------------------
//       "Highscore Datafile" Definitions
//----------------------------------------------
int hdf_var=0, hdf_var2=0;
int highscore_datafile::initialise()
{
     for(hdf_var=0;hdf_var<10;hdf_var++)
     p[hdf_var].initialise();
     A:
     fin.open("highscores.dat", ios::binary);
     if(fin)
     {
            hdf_var=0;
            fin.seekg(0, ios::end);
            eof=fin.tellg()/sizeof(player);
            fin.clear();
            fin.seekg(0, ios::beg);
            while(hdf_var!=eof)
            {
                           fin.read((char *)&p[hdf_var], sizeof(p[hdf_var])); //Read scores in Descending Order
                           hdf_var++;
            }
            fin.close(); //File Exists, No need to create new file.
            hdf_var=0;
     }
     else
     {
         fin.close();
         fout.open("highscores.dat", ios::binary); //Create new file.
         eof=0;
         fout.close();
         setactivepage(0);
         outtextxy(180, 160, "Unexpected Error!!");
         outtextxy(170, 178, "Some files were not found!!");
         outtextxy(180, 196, "Please RESTART the game.");
         outtextxy(180, 214, "Press any key to exit...");
         getch();
         hdf_var=QUIT;
     }
     is_highscore=NO;
     return hdf_var;
}
void highscore_datafile::new_highscore(int y)
{
     if(is_highscore==YES)
     {
                          putimage(0, y, new_score, COPY_PUT);
     }    
}

void highscore_datafile::display_highscores(int x, int y)
{
     player p;
     fin.open("highscores.dat", ios::binary);
     hdf_var=0;
     char string[100];
     while(hdf_var!=eof)
     {
                    fin.read((char *)&p, sizeof(p));
                    hdf_var++;
                    sprintf(string, "%i. %i", hdf_var, p.get_score());
                    strcat(string, " (");
                    strcat(string, p.get_name());
                    strcat(string, ")");
                    outtextxy(x, (y+(hdf_var-1)*24), string);
     }
     if(hdf_var==0)
     outtextxy(x+200, y, "(empty)");
     fin.close();
}

int temp1=0, max=0, pos=0;
char temp2[21];
void highscore_datafile::save_highscore(int scr, void *cur)
{
     max=pos=temp1=0;
     //Check for a highscore
     if(eof<10)
     {
      eof++;
      is_highscore=YES;
      p[eof-1].change_score(scr);
     }
     else if(eof==10)
          {
                     if(scr>p[eof-1].get_score())
                     {
                                     is_highscore=YES;
                                     p[eof-1].change_score(scr);
                     }
          }
     //Get user's name if its a highscore
     if(is_highscore==YES)
     {
     hdf_var=0;
     temp2[0]='\0';
     setactivepage(1);
     cleardevice();
     putimage(0, 0, cur, COPY_PUT);
     new_highscore(300);
     putimage(0, 350, enter_name, COPY_PUT);
     setvisualpage(1);
     getimage(0,0,500,400,cur);
     A:
     i=0;
     while(!kbhit())
     {
                    setactivepage(i);
                    setvisualpage(1-i);
                    cleardevice();
                    putimage(0,0,cur,COPY_PUT);
                    outtextxy(180, 375, temp2);
                    i=1-i;
                    if(ismouseclick(WM_LBUTTONDOWN))
                    clearmouseclick(WM_LBUTTONDOWN);
                    delay(50);
     }
     hdf_var2=getch();
     if((hdf_var2>=97&&hdf_var2<=122)||(hdf_var2>=48&&hdf_var2<=57)||(hdf_var2>=65&&hdf_var2<=90)||hdf_var2==32) //Either from A-Z or a-z or 0-9 or <space>
     {
      if(hdf_var<20)
      {
       temp2[hdf_var]=char(hdf_var2);
       hdf_var++;
       temp2[hdf_var]='\0';
      }
      else
      {
       setactivepage(i);
       setcolor(YELLOW);
       outtextxy(8, 375, "             Max. 20 characters!!              ");
       setcolor(WHITE);
       delay(2000);
      }
      goto A;
     }
     else if(hdf_var2==KEY_BACKSPACE)
          {
                                     if(hdf_var>0)
                                     {
                                      hdf_var--;
                                      temp2[hdf_var]='\0';
                                     }
                                     else
                                     {
                                      setactivepage(i);
                                      setcolor(YELLOW);
                                      outtextxy(8, 375, "              Unable to delete!!               ");
                                      setcolor(WHITE);
                                      delay(2000);
                                     }
                                     goto A;
          }
          else if(hdf_var2==KEY_ENTER)
               {
                                      goto DONE;
               }
               else
               {
                   goto A;
               }
     DONE:
     p[eof-1].change_name(temp2);
     }
     //Sort in Descending Order (Selection Sort)
     for(hdf_var=0;hdf_var<eof;hdf_var++)
     {
      max=p[hdf_var].get_score();
      pos=hdf_var;
      for(hdf_var2=hdf_var;hdf_var2<eof;hdf_var2++)
      {
       if(p[hdf_var2].get_score()>max)
       {
        max=p[hdf_var2].get_score();
        pos=hdf_var2;
       }
      }
      //Swap scores
      temp1=p[hdf_var].get_score();
      p[hdf_var].change_score(p[pos].get_score());
      p[pos].change_score(temp1);
      //Swap names
      strcpy(temp2, p[hdf_var].get_name());
      p[hdf_var].change_name(p[pos].get_name());
      p[pos].change_name(temp2);
     }
     //Print in Descending Order
     fout.open("highscores.dat", ios::binary|ios::trunc);
     for(hdf_var=0;hdf_var<eof;hdf_var++)
     {
                          fout.write((char *)&p[hdf_var], sizeof(p[hdf_var]));
     }
     fout.close();
}

//---------------------------------------------
//            Function Definitions
//---------------------------------------------
int size=0;
void read_all_images()
{
     setactivepage(0);
     readimagefile("data/loading.gif", 310, 350, 500, 400);
     setactivepage(1);
     setvisualpage(0);
     size=imagesize(0, 0, 500, 400);
     home_screen=malloc(size);
     if(home_screen==NULL)
     exit(0);
     readimagefile("data/home_screen.gif", 0, 0, 500, 400);
     getimage(0, 0, 500, 400, home_screen);
     hscore_screen=malloc(size);
     if(hscore_screen==NULL)
     exit(0);
     readimagefile("data/highscore_screen.gif", 0, 0, 500, 400);
     getimage(0, 0, 500, 400, hscore_screen);
     how_to_play=malloc(size);
     if(how_to_play==NULL)
     exit(0);
     readimagefile("data/howto.gif", 0, 0, 500, 400);
     getimage(0, 0, 500, 400, how_to_play);
     cleardevice();
     setactivepage(1);
}

int  display_menu()
{
     A:
     page=0;
     clearmouseclick(WM_LBUTTONUP);
     while(!ismouseclick(WM_LBUTTONUP))
     {
                    //Double Buffering
                    setactivepage(page);
                    setvisualpage(1 - page);
                    cleardevice();
                    //Draw Background
                    putimage(0, 0, home_screen, COPY_PUT);
                    //Menu
                    play.draw(219, 125, "PLAY!", CELTICS, WHITE);
                    howtoplay.draw(175, 170, "HOW TO PLAY?", CELTICS, WHITE);
                    highscores.draw(187, 215, "HIGHSCORES", CELTICS, WHITE);
                    quit.draw(222, 260, "QUIT", CELTICS, WHITE);
                    //Credits
                    outtextxy(0, 380, "    Designed and Programmed by Sahil Lamba    ");
                    //Reset Page
                    mouse_x=mousex();
                    mouse_y=mousey();
                    delay(100);
                    page = 1 - page;
                    if(kbhit())
                    getch();
                    if(play.is_clicked(mouse_x, mouse_y))
                    play.highlight();
                    else if(howtoplay.is_clicked(mouse_x, mouse_y))
                         howtoplay.highlight();
                         else if(highscores.is_clicked(mouse_x, mouse_y))
                              highscores.highlight();
                              else if(quit.is_clicked(mouse_x, mouse_y))
                                   quit.highlight();
     }
     getmouseclick(WM_LBUTTONUP, mouse_x, mouse_y);
     if(play.is_clicked(mouse_x, mouse_y))
     return PLAY;
     else if(howtoplay.is_clicked(mouse_x, mouse_y))
          return HOWTOPLAY;
          else if(highscores.is_clicked(mouse_x, mouse_y))
               return HIGHSCORES;
               else if(quit.is_clicked(mouse_x, mouse_y))
                    return QUIT;
                    else
                    {
                        clearmouseclick(WM_LBUTTONUP);
                        goto A;
                    }
}

int display_instructions()
{
     A:
     page=0;
     clearmouseclick(WM_LBUTTONUP);
     while(!ismouseclick(WM_LBUTTONUP))
     {
                    //Double Buffering
                    setactivepage(page);
                    setvisualpage(1 - page);
                    cleardevice();
                    //Draw Background
                    putimage(0, 0, how_to_play, COPY_PUT);
                    //Buttons
                    back.draw(0, 369, "Back", CELTICS, WHITE);
                    play.draw(441, 369, "PLAY", CELTICS, WHITE);
                    //Reset Page
                    mouse_x=mousex();
                    mouse_y=mousey();
                    delay(100);
                    page = 1 - page;
                    if(kbhit())
                    getch();
                    if(back.is_clicked(mouse_x, mouse_y))
                    back.highlight();
                    else if(play.is_clicked(mouse_x, mouse_y))
                         play.highlight();
    }
    getmouseclick(WM_LBUTTONUP, mouse_x, mouse_y);
    if(play.is_clicked(mouse_x, mouse_y))
    return PLAY;
    else if(back.is_clicked(mouse_x, mouse_y))
         return MENU;
         else
         {
             clearmouseclick(WM_LBUTTONUP);
             goto A;
         }
}

int highscore_menu()
{
    A:
    page=0;
    clearmouseclick(WM_LBUTTONUP);
    while(!ismouseclick(WM_LBUTTONUP))
    {
                    //Double Buffering
                    setactivepage(page);
                    setvisualpage(1 - page);
                    cleardevice();
                    //Draw Background
                    putimage(0, 0, hscore_screen, COPY_PUT);
                    //Buttons
                    back.draw(0, 369, "Back", CELTICS, WHITE);
                    play.draw(441, 369, "PLAY", CELTICS, WHITE);
                    //Credits
                    setcolor(WHITE);
                    file.display_highscores(10, 80);
                    //Reset Page
                    mouse_x=mousex();
                    mouse_y=mousey();
                    delay(100);
                    page = 1 - page;
                    if(kbhit())
                    getch();
                    if(back.is_clicked(mouse_x, mouse_y))
                    back.highlight();
                    else if(play.is_clicked(mouse_x, mouse_y))
                         play.highlight();
    }
    getmouseclick(WM_LBUTTONUP, mouse_x, mouse_y);
    if(play.is_clicked(mouse_x, mouse_y))
    return PLAY;
    else if(back.is_clicked(mouse_x, mouse_y))
         return MENU;
         else
         {
             clearmouseclick(WM_LBUTTONUP);
             goto A;
         }
}

int t=rand()%3, c=rand()%total_colors, h=350-(200+(t*50));
void initialise_gamescreen()
{
     setactivepage(0);
     cleardevice();
     readimagefile("data/loading.gif", 310, 350, 500, 400);
     setvisualpage(0);
     setactivepage(1);
     //Read images
     //Choose random BG and clouds
     i=(rand()%3)+1;
     char str[25];
     sprintf(str, "data/sky%i.jpg", i);
     readimagefile(str, 0, 0, 500, 400);
     if(i!=3)
     {
             int j, k=0;
             while(k<5)
             {
              i=(rand()%420)+1;
              j=(rand()%170)+1;
              readimagefile("data/cloud.gif", i, j, (i+80), (j+25));
              k++;
             }
     }
     sky=malloc(imagesize(0, 0, 500, 400));
     if(sky==NULL)
     exit(0);
     getimage(0, 0, 500, 400, sky);
     congo=malloc(imagesize(0, 0, 500, 100));
     if(congo==NULL)
     exit(0);
     readimagefile("data/congo.gif", 0, 0, 500, 100);
     getimage(0, 0, 500, 100, congo);
     oops=malloc(imagesize(0, 0, 500, 100));
     if(oops==NULL)
     exit(0);
     readimagefile("data/oops.gif", 0, 0, 500, 100);
     getimage(0, 0, 500, 100, oops);
     new_score=malloc(imagesize(0, 0, 500, 50));
     if(new_score==NULL)
     exit(0);
     readimagefile("data/new_score.gif", 0, 0, 500, 50);
     getimage(0, 0, 500, 50, new_score);
     enter_name=malloc(imagesize(0, 0, 500, 50));
     if(enter_name==NULL)
     exit(0);
     readimagefile("data/enter_your_name.gif", 0, 0, 500, 50);
     getimage(0, 0, 500, 50, enter_name);
     //Initialise Buildings
     temp1=c;
     for(i=0;i<NO_OF_BUILDINGS;i++)
     {
                      while(c==temp1) //System to prevent two adjacent buildings from having the same color.
                      { c=rand()%total_colors; }
                      temp1=c;
                      buildset[i].filldetails((i*WIDTH_OF_BUIILDING), (200+(t*50)), c, h);
                      t=rand()%3;
                      c=rand()%total_colors;
                      h=350-(200+(t*50));
     }
     //Initialise Player details
     playa.initialise();
     //Initialise Helicopter position
     alpha.initialise();
     //Initialise Bomb
     thebomb.initialise();
     //Initialise Highscores
     file.initialise();
     cleardevice();
}

int p[]={0, 0, 500, 0, 500, 20, 0, 20, 0, 0};
void draw_city()
{
     //Draw Background
     putimage(0, 0, sky, COPY_PUT);
     //Draw Info Panel
     setfillstyle(SOLID_FILL, BLACK);
     setcolor(BLACK);
     fillpoly(5, p);
     setfillstyle(EMPTY_FILL, WHITE);
     setcolor(WHITE);
     //Draw Buildings
     for(i=0;i<NO_OF_BUILDINGS;i++)
     buildset[i].drawbuilding();
}

int check_city_destroyed()
{
    for(i=0;i<NO_OF_BUILDINGS;i++)
    {
                     if(buildset[i].getheight()!=0)
                     {
                                                   flag++;
                                                   break;
                     }
    }
    if(flag==0)
    return YES;
    else
    return NO;
}

void display_crash(int x, int y)
{
     int corners[]={x-5,y,x,y-10,x+5,y,x+10,y-7,x+7,y+5,x+10,y+10,x+3,y+7,x,y+18,x-3,y+7,x-10,y+10,x-5,y+3,x-10,y-5,x-5,y};
     setcolor(BRIGHTRED);
     drawpoly(13,corners);
     setfillstyle(SOLID_FILL, YELLOW);
     fillpoly(13,corners);
     setfillstyle(EMPTY_FILL, WHITE);
     setcolor(WHITE);
}

int levelbomb, levelbuilding, diff;
int calc_score(int h, int b)
{
    levelbomb = (h/30)+1;
    levelbuilding = (b/30)+1;
    diff = levelbuilding-levelbomb;
    if(diff<0)
    diff=-diff;
    return (10*diff);//Points that will be added to the score
}

void game_over()
{
     playa.display_score();
     if(check_city_destroyed()==YES)
     {
                                    putimage(0, 50, congo, COPY_PUT);
     }
     else if(alpha.check_collision()==YES)
          {
                                          putimage(0, 50, oops, COPY_PUT);
          }
}

int bgpts_pause[]={189, 151, 311, 151, 311, 245, 189, 245, 189, 151};
int pause(void *p)
{
    A:
    page=0;
    while(!ismouseclick(WM_LBUTTONUP))
    {
                                      //Double Buffering
                                      setactivepage(page);
                                      setvisualpage(1-page);
                                      cleardevice();
                                      //Drawing
                                      putimage(0,0,p,COPY_PUT);
                                      setcolor(YELLOW);
                                      setfillstyle(SOLID_FILL, CELTICS);
                                      fillpoly(5, bgpts_pause);
                                      outtextxy(195, 155, "PAUSE MENU");
                                      resume.draw(209, 174, "Resume", GOLDGREEN, BLACK);
                                      quit.draw(220, 209, "Quit", GOLDGREEN, BLACK);
                                      setbkcolor(RED);
                                      playa.display_score();
                                      setbkcolor(BLACK);
                                      //Reset Page
                                      mouse_x=mousex();
                                      mouse_y=mousey();
                                      page=1-page;
                                      delay(100);
                                      if(kbhit())
                                      getch();
                                      if(quit.is_clicked(mouse_x, mouse_y))
                                      quit.highlight();
                                      else if(resume.is_clicked(mouse_x, mouse_y))
                                           resume.highlight();
    }
    setcolor(WHITE);
    setfillstyle(EMPTY_FILL, WHITE);
    setbkcolor(BLACK);
    getmouseclick(WM_LBUTTONUP, mouse_x, mouse_y);
    if(quit.is_clicked(mouse_x, mouse_y))
    return QUIT;
    else if(resume.is_clicked(mouse_x, mouse_y))
         return RESUME;
         else
         {
             clearmouseclick(WM_LBUTTONUP);
             goto A;
         }
}

int input=0, crashsite_x=-200, crashsite_y=0, temp3=0, ans=0;
int run_game()
{
     A:
     crashsite_x=-50;
     page=0, flag=NO;
     while(!kbhit())
     {
                    //Double Buffering
                    setactivepage(page);
                    setvisualpage(1 - page);
                    cleardevice();
                    //Drawing
                    draw_city();
                    if(alpha.check_collision()==YES)
                    {
                                                    crashsite_x=alpha.getheli_x();
                                                    crashsite_y=alpha.getheli_y();
                                                    alpha.destroy_heli();
                                                    alpha.collide();
                                                    break;
                    }
                    else if(check_city_destroyed()==YES)
                         {
                                                        alpha.destroy_heli();
                                                        break;
                         }
                    if(thebomb.getbombs()==1)
                    {
                                             thebomb.move_bomb();
                                             i=thebomb.getbomb_x()/buildset[0].getwidth();
                                             temp1=buildset[i].gettop();
                                             flag=thebomb.collide();
                                             if(flag==YES)
                                             {
                                                          display_crash(thebomb.getbomb_x()-1, thebomb.getbomb_y());
                                                          display_crash(thebomb.getbomb_x()+5, thebomb.getbomb_y()-3);
                                                          MessageBeep(MB_ICONERROR); //Audible Bell
                                                          delay(10);
                                                          thebomb.destroy_bomb();
                                                          playa.update_score(calc_score(temp1, temp3));
                                             }
                                             thebomb.draw_bomb();
                    }
                    playa.display_score();
                    alpha.move_heli();
                    alpha.draw_heli();
                    alpha.draw_speedometer();
                    delay(90);
                    //Reset Page
                    page = 1 - page;
                    if(ismouseclick(WM_LBUTTONUP))
                    {
                     clearmouseclick(WM_LBUTTONUP);
                    }
     }
     if(alpha.check_collision()==YES)
     {
                                     goto HELI_COLLIDED;
     }
     else if(check_city_destroyed()==YES)
          {
                                         crashsite_x=-100;
                                         crashsite_y=50;
                                         goto CITY_DESTROYED;
          }
     input=getch();
     if(input==KEY_SPACEBAR)
     {
                            input=0;
                            if(thebomb.getbombs()==0)
                            {
                                                     temp3=alpha.getheli_y();
                                                     thebomb.drop_bomb();
                                                     goto A;
                            }
                            else
                            goto A;
     }
     else if(input==KEY_ESC)
          {
                            //Get current screen state
                            current=malloc(imagesize(0, 0, 500, 400));
                            if(current==NULL)
                            exit(0);
                            getimage(0, 0, 500, 400, current);
                            ans=pause(current);
                            free(current);
                            if(ans==QUIT)
                            {
                                         return QUIT;
                            }
                            else if(ans==MENU)
                                 {
                                              return MENU;
                                 }
                                 else if(ans==RESTART)
                                      {
                                                      return RESTART;
                                      }
                                      else
                                      goto A;
          }
          else if(input==KEY_RIGHT)
               {
                                   alpha.change_speed(KEY_RIGHT);
                                   goto A;
               }
               else if(input==KEY_LEFT)
                    {
                                       alpha.change_speed(KEY_LEFT);
                                       goto A;
                    }
                    else
                    goto A;
     HELI_COLLIDED:
                   cleardevice();
                   draw_city();
                   display_crash(crashsite_x, crashsite_y);
                   display_crash(crashsite_x+30, crashsite_y);
                   display_crash(crashsite_x+15, crashsite_y+15);
                   display_crash(crashsite_x, crashsite_y+30);
                   display_crash(crashsite_x+30, crashsite_y+30);
                   game_over();
                   //Get current screen state
                   current=malloc(imagesize(0, 0, 500, 400));
                   if(current==NULL)
                   exit(0);
                   getimage(0, 0, 500, 400, current);
                   file.save_highscore(playa.get_score(),current);
                   free(current);
                   current=NULL;
     HELI_COLLIDED_SAVED_SCORE:
                   page=0;
                   while(!kbhit())
                   {
                                  setactivepage(page);
                                  setvisualpage(1-page);
                                  cleardevice();
                                  draw_city();
                                  display_crash(crashsite_x, crashsite_y);
                                  display_crash(crashsite_x+30, crashsite_y);
                                  display_crash(crashsite_x+15, crashsite_y+15);
                                  display_crash(crashsite_x, crashsite_y+30);
                                  display_crash(crashsite_x+30, crashsite_y+30);
                                  game_over();
                                  file.new_highscore(350);
                                  if(ismouseclick(WM_LBUTTONUP))
                                  clearmouseclick(WM_LBUTTONUP);
                                  page=1-page;
                                  delay(100);
                   }
                   input=getch();
                   if(input==KEY_ESC)
                   {
                                     //Get current screen state
                                     current=malloc(imagesize(0, 0, 500, 400));
                                     if(current==NULL)
                                     exit(0);
                                     getimage(0, 0, 500, 400,current);
                                     ans=pause(current);
                                     free(current);
                                     current=NULL;
                                     if(ans==QUIT)
                                     {
                                                  return QUIT;
                                     }
                                     else if(ans==MENU)
                                          {
                                                       return MENU;
                                          }
                                          else if(ans==RESTART)
                                               {
                                                               return RESTART;
                                               }
                                               else
                                               goto HELI_COLLIDED_SAVED_SCORE;
                   }
                   else
                   goto HELI_COLLIDED_SAVED_SCORE;
     CITY_DESTROYED:
                    cleardevice();
                    draw_city();
                    game_over();
                    //Get current screen state
                    current=malloc(imagesize(0, 0, 500, 400));
                    if(current==NULL)
                    exit(0);
                    getimage(0, 0, 500, 400, current);
                    file.save_highscore(playa.get_score(),current);
                    free(current);
                    current=NULL;
     CITY_DESTROYED_SAVED_SCORE:
                    page=0;
                    while(!kbhit())
                    {
                                   setactivepage(page);
                                   setvisualpage(1-page);
                                   cleardevice();
                                   draw_city();
                                   game_over();
                                   file.new_highscore(350);
                                   if(ismouseclick(WM_LBUTTONUP))
                                   clearmouseclick(WM_LBUTTONUP);
                                   page=1-page;
                                   delay(100);
                    }
                    input=getch();
                    if(input==KEY_ESC)
                    {
                                      //Get current screen state
                                      current=malloc(imagesize(0,0,500,400));
                                      if(current==NULL)
                                      exit(0);
                                      getimage(0,0,500,400,current);
                                      ans=pause(current);
                                      free(current);
                                      current=NULL;
                                      if(ans==QUIT)
                                      {
                                                   return QUIT;
                                      }
                                      else if(ans==MENU)
                                           {
                                                        return MENU;
                                           }
                                           else if(ans==RESTART)
                                                {
                                                                return RESTART;
                                                }
                                                else
                                                goto CITY_DESTROYED_SAVED_SCORE;
                    }
                    else
                    goto CITY_DESTROYED_SAVED_SCORE;
}

void free_memory()
{
     free(home_screen);
     home_screen=NULL;
     free(hscore_screen);
     hscore_screen=NULL;
     free(how_to_play);
     how_to_play=NULL;
}

void free_gamescreen()
{
     free(sky);
     sky=NULL;
     for(i=0;i<NO_OF_BUILDINGS;i++)
     buildset[i].destroybuilding();
     free(congo);
     congo=NULL;
     free(oops);
     oops=NULL;
     free(new_score);
     new_score=NULL;
     free(enter_name);
     enter_name=NULL;
}
