#include <iostream>
#include <SDL2/SDL.h>
// make
// build/debug/play

// https://www.youtube.com/watch?v=5jUHXVwZm0A&t=0ss
//Graphics

const int WINDOW_WIDTH = 512;
const int WINDOW_HEIGHT = 512;
const float PI = 3.1415926; 
SDL_Window* g_main_window;
SDL_Renderer* g_main_renderer;

namespace Colours{
    const SDL_Colour BLACK = {0,0,0,SDL_ALPHA_OPAQUE};
}

static bool Init(){

    if(SDL_Init(SDL_INIT_EVERYTHING) > 0){
        std::cout << "SDL init error: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }
    g_main_window = SDL_CreateWindow(
        "Boids simulation",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL
    );

    if(g_main_window == nullptr){
        std::cout << "Unable to create the main window. ERROR: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return EXIT_FAILURE;
    }
    g_main_renderer = SDL_CreateRenderer(g_main_window, -1, SDL_RENDERER_PRESENTVSYNC);

    return true;
}

void Shutdown(){
    if(g_main_window != nullptr){
        SDL_DestroyWindow(g_main_window);
        g_main_window = nullptr;
    }
    if(g_main_renderer!= nullptr){
        SDL_DestroyRenderer(g_main_renderer);
        g_main_renderer= nullptr;
    }
    SDL_Quit();
}

static void CLearScreen(SDL_Renderer * renderer){
    SDL_SetRenderDrawColor(renderer, Colours::BLACK.r, Colours::BLACK.g, Colours::BLACK.b, Colours::BLACK.a);
    SDL_RenderClear(renderer);
}   

class Boid{
    public:
        float x;
        float y;
        float angle;
        float speed;
        int size;
        int id;

        Boid(){x = WINDOW_WIDTH/2; y = WINDOW_HEIGHT/2;}

        Boid(float x_start, float y_start, float angle_start, int index){
            x = x_start;
            y = y_start;
            angle = angle_start;
            speed = 2;
            size = 5;
            id = index;
        }

        void draw(SDL_Renderer * renderer){
            //Setting the actual draw color.
            SDL_SetRenderDrawColor(g_main_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

            float shape = PI/8;
            SDL_RenderDrawLine(g_main_renderer, x , y, x - size*cos(angle + shape), y + size*sin(angle + shape));
            SDL_RenderDrawLine(g_main_renderer, x , y, x - size*cos(angle -shape), y + size*sin(angle - shape));
            SDL_RenderDrawLine(g_main_renderer, x - size*cos(angle + shape) , y + size*sin(angle + shape), x - size*cos(angle - shape), y + size*sin(angle - shape));
        }

        void move(){
            //std::cout << "angle: " << angle <<  " x = "<< cos(angle) << " y = " << sin(angle)  << std::endl;
            x = x + (int) speed*cos(angle);
            y = y - (int) speed*sin(angle);   
        }

         void collisionWalls(){
            if(x > WINDOW_WIDTH || x < 0){
                float current_x = speed*cos(angle);
                float current_y = speed*sin(angle);
                 angle = atan2(current_y,-current_x);
             }
             if(y > WINDOW_HEIGHT || y < 0){
                 float current_x = speed*cos(angle);
                 float current_y = speed*sin(angle);
                 angle = atan2(-current_y,current_x);
            }
         }    

};

int main(){

    if(Init() == false){Shutdown();}
    //Drawing loop
    SDL_Event event;
    bool running = true;

    int size = 100;
    Boid boids[size];
    for(int i = 0; i < size; i++){
        int num = rand() % (2*31415926);
        float angle = (float) num/10000000;
        Boid boid(200,200, angle, i);
        boids[i] = boid;
    }

    while (running){
        CLearScreen(g_main_renderer);
        
        for(int i = 0; i < size; i++){
            boids[i].draw(g_main_renderer);
            boids[i].move();
            boids[i].collisionWalls();
            }
        
        if(SDL_PollEvent(&event)){
            switch (event.type)
            {
            case SDL_KEYDOWN:{
                running = event.key.keysym.scancode != SDL_SCANCODE_ESCAPE;
                break;
            }
            case SDL_QUIT:{
                running = false;
                break;
            }
            default:
            break;
            }
        } 
        SDL_RenderPresent(g_main_renderer);       
    }

    Shutdown();
    return EXIT_SUCCESS;
}