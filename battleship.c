#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <curses.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif


#define Horizontal  0
#define Vertical    1 

#define WATER   "~"
#define BOAT    "O"

#define KICK    "X"
#define MISS    "M"

#define MAXIMUM_ONE 20 //nr max de 1 pe tabla

struct point {
int x,y;
};

//windows
void create_beginning_window(int argc, char const *argv[],int computer_map[12][12], int user_map[12][12]);
void create_menu_window(int argc, char const *argv[],int computer_map[12][12], int user_map[12][12]);
void create_new_game_window(int computer_map[12][12], int user_map[12][12],int argc, char const *argv[]);
void create_resumed_game_window(int computer_map[12][12], int user_map[12][12],int argc, char const *argv[]);
void create_hooray_window(int computer_map[12][12], int user_map[12][12],int argc, char const *argv[]);
void create_how_to_play_game_window(int computer_map[12][12], int user_map[12][12],int argc, char const *argv[]);

//actual game
void play(int player_map[12][12],int pc_map[12][12], WINDOW *win,int argc, char const *argv[],int count_player,int count_pc);
int pc_Plays(int player_map[12][12], WINDOW *win); //1 daca a nimerit, 0 daca nu
int player_Plays(int pc_map[12][12], WINDOW *win,int ch); //1 daca a nimerit, 0 daca nu
int kick(int mat[12][12]); //just ONE kick; returneaza 1 daca a nimerit sau 0 daca nu
struct point get_random_kick_coordinates();
void destroy_in_advance( WINDOW *win, int computer_map[12][12], int user_map[12][12]);

//counters
int count_user_map(int mat[12][12]);
int count_comp_map(int mat[12][12]); //cate puncte pe afla pe mapa calculatorului

//create maps
void generate_Map(int mat[12][12]);
void bordare_matrice(int mat[12][12]);
void citire_mapa(int mat[12][12], int argc, char const *argv[]);
struct point get_random_boat_coord(int size, int direction);
void add_boat(int mat[12][12], struct point p, int size, int direction);
void generate_boats(int mat[12][12], int size, int nr_boats);

//validare
int check_valid_boat(int mat[12][12], struct point p, int size, int direction);

//afisare mape
void afisare(int computer_map[12][12], int user_map[12][12], WINDOW *win);

//culori
void colors(WINDOW *win);

int main(int argc, char const *argv[])
{
    srand( time(NULL) );

    initscr(); 
    cbreak(); 
    noecho(); 
    keypad(stdscr, TRUE); 

    int computer_map[12][12], user_map[12][12];

    bordare_matrice( computer_map );
    generate_Map( computer_map );

    if( argc > 1 )
        citire_mapa( user_map , argc , argv );
    else
    {
        printf("[Eroare]: Nu s-au dat argumente de comanda.\n\n");
        return 1;
    }
    
    create_beginning_window( argc , argv , computer_map , user_map );

    return 0;
}

void create_beginning_window(int argc, char const *argv[],int computer_map[12][12], int user_map[12][12])
{
    int h, w;
    getmaxyx(stdscr, h, w);
    WINDOW * win = newwin(h, w, 0, 0);
    colors(win);
    refresh();
    box(win,0,0);
    
    wattron(win,A_BOLD);
    mvwprintw(win, 1, w/2 - 8, "BATTLESHIP");
    mvwprintw(win, 2, w/2 - 5, "MENU");
    wattroff(win,A_BOLD);
    
    wrefresh(win);
    
    create_menu_window(argc,argv,computer_map,user_map);
    wrefresh(win);
    
    getch();
    endwin();
}

void create_menu_window(int argc, char const *argv[],int computer_map[12][12], int user_map[12][12])
{
    WINDOW *w;
    char list[4][10] = { "New Game", "Resume", "Quit", "How To" };
    char item[10];
    int ch, i = 0;
    
    int h, wdh;
    getmaxyx(stdscr, h, wdh);
    initscr();
    w = newwin(h-4, wdh, 4, 0);
    colors(w);
    box( w, 0, 0 );
    refresh();     
    
    for( i=0; i<4; i++ ) 
    {
        if( i == 0 ) 
            wattron( w, A_STANDOUT );
        else
            wattroff( w, A_STANDOUT );
        sprintf(item, "%s",  list[i]);
        mvwprintw( w, i+4, 2, "%s", item );
    } 
    //am creat lista din meniu cu highlight pe primul elem

    mvwprintw(w, h-9, 1, "Press q to quit game. You can resume later.");
    mvwprintw(w, h-8, 1, "Double click on the computer map to destroy a ship.");
    mvwprintw(w, h-7, 1, "        M = Miss. X = Kick.");
    mvwprintw(w, h-6, 1, "Press 'd' in game mode for destroy in advance option.");
  
    wrefresh( w );
    
    i = 0;
    
    noecho();
    keypad( w, TRUE );
    curs_set( 0 ); 
    
    do
    { 
        ch = wgetch(w);
        sprintf(item, "%s",  list[i]); 
        mvwprintw( w, i+4, 2, "%s", item ); 
        
        switch( ch ) 
        {
            case KEY_UP:
                i--;
                i = ( i<0 ) ? 3 : i;
                break;
            case KEY_DOWN:
                i++;
                i = ( i>3 ) ? 0 : i;
                break;
        }
        
        wattron( w, A_STANDOUT );         
        sprintf(item, "%s",  list[i]);
        mvwprintw( w, i+4, 2, "%s", item);
        wattroff( w, A_STANDOUT );

        if( i == 0 && ch == 10 )
        {
            bordare_matrice(computer_map);
            generate_Map(computer_map);
            citire_mapa(user_map,argc,argv);
            create_new_game_window(computer_map, user_map,argc,argv);
            break;
        }
        else if ( i == 1 && ch == 10 )
        {
            create_resumed_game_window(computer_map,user_map,argc,argv);
            break;
        }
        else if ( i == 2 && ch == 10 )
        {
            endwin();
            exit(0);
            break;
        }
        else if( i == 3 && ch == 10 )
        {
            create_how_to_play_game_window(computer_map,user_map,argc,argv);
            break;
        }
    } while (ch != 'q');
    
    if( ch == 'q')
    {
        endwin();
        exit(0);
    } 
}

void create_new_game_window(int computer_map[12][12], int user_map[12][12],int argc, char const *argv[])
{
    int h, w;
    int ch;
    getmaxyx(stdscr, h, w);

    WINDOW * window = newwin(h, w, 0, 0);
    colors(window);
    refresh();
    box(window,0,0);

    wattron(window,A_BOLD);
    mvwprintw(window, 1, w/2 - 8, "BATTLESHIP");
    wattroff(window,A_BOLD);

    do
    {
        mvwprintw(window, 2, w/2 - 7, "New Game");
        mvwprintw(window, 4, 1, "Press q to quit game.");
        mvwprintw(window, 5, 1, "Double press any key to start.");
        
        wrefresh(window);
        
        afisare(computer_map,user_map,window);
        play(user_map,computer_map,window,argc,argv,0,0);
        
        wrefresh(window);
        ch = wgetch(window);
    
    } while (ch != 'q');
    
    if( ch == 'q')
    {
        endwin();
        create_beginning_window(argc,argv,computer_map,user_map);
        create_menu_window(argc,argv,computer_map,user_map);
    }    
}

void create_resumed_game_window(int computer_map[12][12], int user_map[12][12],int argc, char const *argv[])
{
    int h, w;
    int ch;
    getmaxyx(stdscr, h, w);
    WINDOW * window = newwin(h, w, 0, 0);
    colors(window);
    refresh();
    box(window,0,0);

    wattron(window,A_BOLD);
    mvwprintw(window, 1, w/2 - 8, "BATTLESHIP");
    wattroff(window,A_BOLD);

    int count_player = count_comp_map(computer_map);
    int count_pc = count_user_map(user_map);

    mvwprintw(window, 4, 1, "%d %d",count_player,count_pc);

    do
    {
        mvwprintw(window, 2, w/2 - 10, "Resumed Game");
        mvwprintw(window, 4, 1, "Press q to quit game. You can resume later.");
        mvwprintw(window, 5, 1, "Double press any key to start.");
        wrefresh(window);
        
        afisare(computer_map,user_map,window);

        if( count_pc < MAXIMUM_ONE && count_player < MAXIMUM_ONE )
            play(user_map,computer_map,window,argc,argv, count_player, count_pc);
        else
        {
            if ( count_pc == MAXIMUM_ONE )
            {
                create_hooray_window(computer_map,user_map,argc,argv);
            }
            else if( count_player == MAXIMUM_ONE )
            {
                create_hooray_window(computer_map,user_map,argc,argv);
            }  
        }
        
        wrefresh(window);
        ch = wgetch(window);

    } while (ch != 'q');

    ch = wgetch(window);
    
    if( ch == 'q')
    {
        endwin();
        create_beginning_window( argc,argv,computer_map,user_map);
        create_menu_window(argc,argv,computer_map,user_map);
    } 
}

void create_how_to_play_game_window(int computer_map[12][12], int user_map[12][12],int argc, char const *argv[])
{
    int h, w;
    int ch;
    getmaxyx(stdscr, h, w);
    WINDOW * window = newwin(h, w, 0, 0);
    colors(window);
    refresh();
    box(window,0,0);

    wattron(window,A_BOLD);
    mvwprintw(window, 1, w/2 - 8, "BATTLESHIP");
    wattroff(window,A_BOLD);

    do
    {
        mvwprintw(window, 2, w/2 - 8, "How To Play");
        mvwprintw(window, 4, 1, "Press q to return to menu.");
        mvwprintw(window, 6, 1, "Double press any key to start playing.");
        mvwprintw(window, 8, 1, "If you just entered the game, Resume works like New Game option.");
        mvwprintw(window, 10, 1, "When it's your turn, click on PC's map to choose what you kick.");
        mvwprintw(window, 12, 1, "You can quit the game if it's your turn. Resume later.");
        mvwprintw(window, 14, 1, "You will get 1 point if you kick a boat/ a part of a boat.");
        mvwprintw(window, 16, 1, "20 points = win.");
        mvwprintw(window, 18, 1, "Press 'd' at any time in the game. It will choose 10 random positions on both maps.");
        mvwprintw(window, 20, 1, "Note: Only click on PC map. Otherwise PC will choose for you.");

        wrefresh(window);
        ch = wgetch(window);

    } while (ch != 'q');

    ch = wgetch(window);
    
    if( ch == 'q')
    {
        endwin();
        create_beginning_window( argc,argv,computer_map,user_map);
        create_menu_window(argc,argv,computer_map,user_map);
    } 
}

void create_hooray_window(int computer_map[12][12], int user_map[12][12],int argc, char const *argv[])
{
    int h, w;
    int ch;
    getmaxyx(stdscr, h, w);

    WINDOW * window = newwin(h, w, 0, 0);
    colors(window);
    refresh();
    box(window,0,0);

    wattron(window,A_BOLD);
    mvwprintw(window, 1, w/2 - 8, "BATTLESHIP");
    wattroff(window,A_BOLD);
    int count_player = count_comp_map(computer_map);
    int count_pc = count_user_map(user_map);
    do
    {
        mvwprintw(window, h-3, 1, "Press q to return to menu.");
        if ( count_pc == MAXIMUM_ONE )
        {
            wattron( window ,A_BOLD );
            mvwprintw(window , h / 2 , w / 2 -8, "YOU LOST" );
            wattroff(window,A_BOLD);
        }
        else if( count_player == MAXIMUM_ONE )
        {
            wattron( window , A_BOLD );
            mvwprintw( window , h / 2 , w / 2 - 7 ,"YOU WON!");
            mvwprintw( window , h / 2 , w / 2 - 14 ,"Just defeated 10 ships! Hooray!");
            wattroff( window , A_BOLD );        
        }  
        
        wrefresh(window);
        ch = wgetch(window);
    
    } while ( ch != 'q');
    
    if( ch == 'q' )
    {
        endwin();
        create_beginning_window(argc,argv,computer_map,user_map);
        create_menu_window(argc,argv,computer_map,user_map);
    }    
}

void play(int player_map[12][12],int pc_map[12][12], WINDOW *win,int argc, char const *argv[],int count_player,int count_pc)
{
    int i = 0; //par pentru jucator, impar pt calculator    
    int h, w;
    int ch;
    ch = wgetch(win);
    getmaxyx(stdscr, h, w);
    
    do
    {
        
        mvwprintw(win,h-4,1," RANDOM PLAYER:           YOU: ");
        mvwprintw(win,h-3,1,"      %d                  %d",count_pc,count_player); 
        mvwprintw(win,3,w-20," Please wait. ");
        
        if( ch == 'd')
        {
            destroy_in_advance(win,pc_map,player_map);
            count_player = count_comp_map(pc_map);
            count_pc = count_user_map(player_map);
            afisare( pc_map, player_map, win );
            wrefresh(win);
            if(count_player == MAXIMUM_ONE)
            {
                break;   
            }
            if(count_player == MAXIMUM_ONE)
            {
                break;   
            }
        }

        if( i % 2 == 0 ) //player turn
        {
            sleep(1);
            mvwprintw(win,3,w-20,"* Your turn. *");
            ch = wgetch(win);
            
            if( player_Plays(pc_map,win,ch) == 1 )
            {
                count_player++;
                if(count_player == MAXIMUM_ONE)
                {
                    break;   
                }
                mvwprintw(win,3,w-20," Please wait. ");
                afisare( pc_map, player_map, win );
                
                wrefresh( win ); 
                continue;
            }   

            if( ch != KEY_MOUSE )
            {
                if( kick(pc_map) == 1 )
                {
                    count_player++;
                    if(count_player == MAXIMUM_ONE)
                    {
                        break;   
                    }
                    mvwprintw(win,3,w-20," Please wait. ");
                    afisare( pc_map, player_map, win );
                    
                    wrefresh( win ); 
                    continue;
                }      
            }   
        }
        
        if( i % 2 != 0 ) //pc turn
        {
            mvwprintw(win,3,w-20," Please wait. ");
            
            sleep(2);
            
            if( pc_Plays(player_map,win) == 1 )
            {
                count_pc++;
                    
                if ( count_pc == MAXIMUM_ONE )
                {
                    break;
                }
                sleep(1);
                mvwprintw(win,3,w-20," Please wait. ");
                afisare( pc_map, player_map, win );
                
                wrefresh( win ); 
                continue;
            }
        }

        mvwprintw(win,3,w-20," Please wait. ");
        afisare( pc_map, player_map, win );
        
        wrefresh( win );  
        i++;
    }while( ch != 'q' );

    curs_set( 0 );

    if ( count_pc == MAXIMUM_ONE )
    {
        mvwprintw(win,h-5,1,"        RANDOM PLAYER:            YOU: ");
        mvwprintw(win,h-4,1,"Points:      %d                    %d",count_pc,count_player);        
        sleep(1);
        create_hooray_window(pc_map,player_map,argc,argv);
    }
    else if( count_player == MAXIMUM_ONE )
    {
        mvwprintw(win,h-5,1,"        RANDOM PLAYER:            YOU: ");
        mvwprintw(win,h-4,1,"Points:      %d                    %d",count_pc,count_player);
        sleep(1);
        create_hooray_window(pc_map,player_map,argc,argv);    
    }    

    ch = wgetch(win);      

    if( ch == 'q' )
    {
        endwin();
        create_beginning_window(argc,argv,pc_map,player_map);
        create_menu_window(argc,argv,pc_map,player_map);
        
    }  
    else
    {
        ch = wgetch(win); 
    }   
}

int player_Plays(int pc_map[12][12], WINDOW *win,int ch)
{
    int ok=0;

    MEVENT event;
    keypad(win, TRUE);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

    if (ch == KEY_MOUSE)
    {
        assert(getmouse(&event) == OK);
        int x = event.x;
        int y = event.y-8;

        if( x <= 10 && y <= 10)
        {
            if (pc_map[y][x] == 1)
            {
                pc_map[y][x] = -1;
                ok = 1;
            }
            else if(pc_map[y][x] == 0)
            {
                pc_map[y][x] = -2;
                ok = 0;
            } 
        } 
        else
        {
            ok = kick(pc_map);
        }
                  
    }

    wrefresh(win);

    return ok;
}

int pc_Plays(int player_map[12][12], WINDOW *win)
{
    int ok = kick(player_map);

    refresh();
    
    return ok;
}

struct point get_random_kick_coordinates()
{
    struct point p;
    p.x = rand()%10;
    p.y = rand()%10;
    return p;
}

int kick(int mat[12][12])
{
    struct point p;
    int lovita = 0;
    
    while( 1 )
    {
        p = get_random_kick_coordinates();
        if(mat[p.x][p.y] != -1 && mat[p.x][p.y] != -2)
            break;
    }
    
    if( mat[p.x][p.y] == 1 )
    {
        mat[p.x][p.y] = -1;
        lovita++ ;
    }        
    else
        mat[p.x][p.y] = -2;
    
    return lovita;  
}

void destroy_in_advance( WINDOW *win, int computer_map[12][12], int user_map[12][12])
{
    int ok_player = 0;
    int ok_pc = 0;
    int player_points = count_comp_map(computer_map);
    int computer_points = count_user_map(user_map);
    for( int i = 0; i < 10; i++)
    {
        ok_player = kick(computer_map);
        ok_pc = kick(user_map);
        if( ok_player == 1 )
        {
            player_points++;
        }
        if ( ok_pc == 1 )
        {
            computer_points++;
        }
        if ( player_points == MAXIMUM_ONE )
        {
            break;
        }
        if ( computer_points == MAXIMUM_ONE )
        {
            break;
        }
    }
}

int count_user_map(int mat[12][12])
{
    int k = 0;
    
    for(int i = 0; i < 11; i++)
        for (int j = 0; j < 11; j++)
        {
            if( mat[i][j] == -1)
                k++;
        }
    
    return k;    
}

int count_comp_map(int mat[12][12])
{
    int k = 0;
    for(int i = 1; i < 11; i++)
        for (int j = 1; j < 11; j++)
        {
            if( mat[i][j] == -1)
                k++;
        }

    return k;    
}

void generate_Map(int mat[12][12])
{
    generate_boats(mat,4,1);

    generate_boats(mat,3,2);
    
    generate_boats(mat,2,3);
    
    generate_boats(mat,1,4);
}

void bordare_matrice(int mat[12][12]) //o  folosesc ptc la randomizarea mapei putea da pe afara
{
    for( int i = 1; i <= 10; i++)
        for( int j = 1; j <= 10; j++)
            mat[i][j] = 0;
    
    for( int i = 0; i <= 11; i++)
    {
        mat[0][i] = -100;
        mat[i][0] = -100;
        mat[11][i] = -100;
        mat[i][11] = -100;
    }
}

void citire_mapa(int mat[12][12], int argc, char const *argv[])
{
    FILE *pf;
    
    if(argc == 2)
    {
        pf = fopen (argv[1], "r");
    }
    else if(argc > 2)
    {
        int r = rand() % argc + 1;
        
        if ( r != argc )
        {
            pf = fopen (argv[r], "r");
        }
        else
        {
            pf = fopen (argv[1], "r");
        }    
        if( pf == NULL )
        {
            printf("[Eroare]: Fisierul %s nu poate fi deschis.",argv[r]);
            exit(1);
        }
    }
    
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
            fscanf(pf, "%d", &mat[i][j]);
    }
    
    fclose (pf); 
}

struct point get_random_boat_coord(int size, int direction) 
{
    //calculul cu 10 - size il fac ptc nu vreau sa se puna barca peste margini
    struct point p;
    if( direction == Horizontal )
    {
        p.x = rand() % ( 10 - size ) + 1; 
        p.y = rand() % 10 + 1;        
    }
    else
    {
        p.x = rand() % 10 + 1;
        p.y = rand() % ( 10 - size ) + 1;
    }  
    return p;
}

void add_boat(int mat[12][12], struct point p, int size, int direction) 
{
    if (direction == Horizontal) 
    {
        for ( int i = 0; i < size; i++ ) 
        {
            mat[p.x+i][p.y] = 1;
        }
    }
    else 
    {
        for ( int i = 0; i < size; i++ ) 
        {
            mat[p.x][p.y+i] = 1;
        }
    }
}

void generate_boats(int mat[12][12], int size, int nr_boats) 
{
    int count = 0;
    while( 1 ) 
    {
        int direction = rand() % 2;
        struct point p;
        p = get_random_boat_coord(size, direction);
        if (check_valid_boat(mat,p,size,direction)) 
        {
            add_boat(mat,p,size,direction);
            count++;
        }

        if(count == nr_boats)
            break;
    }
}

int check_valid_boat(int mat[12][12], struct point p, int size, int direction) 
{
    if ( direction == Horizontal ) 
    {
        //verific sa nu se intretaie + extremitatile barcii
        if( p.x > 1 )
        {
            for( int i = -1; i <= size; i++ )  
            {
                if ( mat[p.x+i][p.y] != 0 )
                    return 0;
            }

            for( int i = -1; i <= size; i++ ) 
            {
                if ( mat[p.x+i][p.y-1] != 0 )
                    return 0;
                if ( mat[p.x+i][p.y+1] != 0 )
                    return 0;
            }

        return 1;
        }    

        else
        {
            for( int i = 0; i <= size; i++ )  
            {
                if ( mat[p.x+i][p.y] != 0 )
                    return 0;
            }

            for( int i = 0; i <= size; i++ ) 
            {
                if ( mat[p.x+i][p.y-1] != 0 )
                    return 0;
                if ( mat[p.x+i][p.y+1] != 0 )
                    return 0;
            }

            return 1;
        }    
        //verific deasupra si sub barca     
    } 
    else 
    {
        if( p.y > 1 )
        {
            //verific sa nu se intretaie + extremitatile barcii
            for( int i = -1; i <= size; i++ )  
            {
                if ( mat[p.x][p.y+i] != 0 )
                    return 0;
            }
            //verific lateralele
            for( int i = -1; i <= size; i++ ) 
            {
                if ( mat[p.x-1][p.y+i] != 0 )
                    return 0;
                if ( mat[p.x+1][p.y+i] != 0 )
                    return 0;
            }
            return 1;
        }
        else
        {
            //verific sa nu se intretaie + extremitatile barcii
            for( int i = 0; i <= size; i++ )  
            {
                if ( mat[p.x][p.y+i] != 0 )
                    return 0;
            }
            //verific lateralele
            for( int i = 0; i <= size; i++ ) 
            {
                if ( mat[p.x-1][p.y+i] != 0 )
                    return 0;
                if ( mat[p.x+1][p.y+i] != 0 )
                    return 0;
            }

            return 1;
        }   
    }   
}

void afisare(int computer_map[12][12], int user_map[12][12], WINDOW *win)
{
    int w;
    w = getmaxx(win);
    
    mvwprintw(win,7,3,"PC Map:");
    
    for( int i = 1; i <= 10; i++)
    {
        
        for( int j = 1; j <= 10; j++)
        {
            wattron(win,A_BOLD);
            
            mvwprintw(win,i+8,j,"~");
            
            wattroff(win,A_BOLD);

            if( computer_map[i][j] == -1 )
                mvwprintw(win,i+8,j,KICK);
            
            if( computer_map[i][j] == -2 )
                mvwprintw(win,i+8,j,MISS);
        }    
        //mvwprintw(win,i+8,11,"\n");  
    }

    mvwprintw(win,7,w-24,"Player Map:");

    for( int i = 0; i <= 9; i++)
    {
        
        for( int j = 0; j <= 9; j++)
        {
            if( user_map[i][j] == 1 )
                mvwprintw(win,i+9,w-j-15,BOAT);
            else if( user_map[i][j] == 0 )
            {
                wattron(win,A_BOLD);

                mvwprintw(win,i+9,w-j-15,WATER); 
                
                wattroff(win,A_BOLD);
            }
            else if( user_map[i][j] == -1 )
                mvwprintw(win,i+9,w-j-15,KICK);
            
            else if( user_map[i][j] == -2 )
                mvwprintw(win,i+9,w-j-15,MISS);   
        }  
        //mvwprintw(win,i+9,w-2,"\n");  
    } 
}

void colors(WINDOW *win)
{
    start_color();
    init_color(COLOR_MAGENTA,100,0,100);
	init_pair(1, COLOR_CYAN, COLOR_MAGENTA);
    wbkgd(win, COLOR_PAIR(1));
}