 /*-------------------------------------------
 Mannan Sethia (2020): 'Matrix Code effect'
 --------------------------------------------
                (\__/) ||
                (•ㅅ•) ||
                /   づ

 It is recommended to view this cpp file in Visual Studio.
 Apologies for the all the comments obliterating readability: I'm getting in as much detail as possible so I don't forget how any of this works!
 */






// Because UNICODE is used by Windows headers, but _UNICODE is used by standard C++ headers, both need to be defined. _UNICODE is already defined so we define UNICODE here. 
#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>        // The <windows.h header is needed for access to the Win32 API, which is what the entire program is built upon!
#include <gdiplus.h>        // <gdiplus.h> and <gdiplusenums.h> are needed for usage of GDI, which is a Windows standard library for the window to be painted.
#include <gdiplusenums.h>   // ^^
#include <uxtheme.h>        // <uxtheme.h> allows for the window theme to be set. By setting this to 0, window themes are disabled, which reverts the window to the lovely 'classic' look.

int soundPlayed = 0;        // To ensure a sound plays only once, a global boolean is set to check if the sound has been played already.

// This is the 2-dimensional array in which all the characters are stored. There are 51 columns, with 64 characters in each column. 
// Integers are stored in the array, which represent characters. The character map can be found after main(). The array is stored globally which is *easily* simplest way of doing so.
// Setting an array to {} means the entire array is zero-initialised.
int MATRIX[51][64] = {};


// Not entirely sure what these two statements do, but they're needed for the program to run! I think it's a predefinition / initialisation of functions that needs to be done before main() is called. 
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void draw(HDC hdc);
void newFrame(HDC hdc);
LPCWSTR returnChar(int integerToMap);







// This is the main function of the program, declared as an 'int WINAPI' to recognise it is a Windows function. 
// Most important parameter is the first, hInstance, which declares the handle of the window (handle is similar to a process)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) 
{
    // Initialize GDI+ (the graphics library) 
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    // Store the window class, and registers it with the API. 
    const wchar_t CLASS_NAME[] = L"Sample Window Class";
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

   
    // Create the window. Stored as HWND (handle to window)
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"O   U   T   R   U   N",       // Window title
        WS_OVERLAPPED | WS_SYSMENU,     // Window style
        CW_USEDEFAULT, CW_USEDEFAULT,   // Position (x, y)
        1063, 870,                      // Size (x, y)
        NULL,                           // Parent window    
        NULL,                           // Menu
        hInstance,                      // Instance (window) handle
        NULL                            // Additional application data
    );

    // Exception handles the CreateWindow function
    if (hwnd == NULL)
    {
        return 0;
    }

    SetWindowTheme(hwnd, L" ", L" ");                       // This function comes from <uxtheme.h>; sets the window style to blank to revert back to classic Windows as described earlier.

    ShowWindow(hwnd, nCmdShow);                             // Shows the window!

    // Starts timer. The timer sends a message every 80 milliseconds to the window, which then updates the frame at said interval
    int timer = -1;
    SetTimer(hwnd, timer = 1, 80, (TIMERPROC) NULL);

    // This is the while loop in which the program operates. From here it can respond to all different types of inputs, messages and more.
    // The loop gets a message from Windows, 'translates' it into a case from machine code, and then sends it to the WindowProc switch statement which deals with every different type of message. 
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Once the while loop has been terminated by a 'CloseWindow' message, it closes GDI, then returns 0 to exit the program. 
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
}

// WindowProc is the function that deals with all the different types of messages received from the while loop in wWinMain.
// It has a switch statement which execs code based on the message.
// hwnd is of course handle to window, uMsg is the message sent by the while loop, and wParam & lParam are any additional parameters sent by the program. 
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // uMsg is the message sent. It's the subject of the switch statement here. Order isn't relevant for the statement. 
    switch (uMsg)
    {
    // WM_DESTROY is of course the message sent when the program is closed. 
    case WM_DESTROY:
        {
        PostQuitMessage(0);     // returns 0, which terminates the while loop and therefore the program itself.
        return 0;               // also returns zero to function
        }
    // WM_PAINT is sent when the window is first made.
    case WM_PAINT:
        {
        PAINTSTRUCT ps; 
        HDC hdc = BeginPaint(hwnd, &ps);    // These intialise the 'painter' tool.
        draw(hdc);                          // The hdc object is sent to a function which does the painting (defined below)
        EndPaint(hwnd, &ps);                // Ends the paint tool. 
        }
    // WM_TIMER is what a lot of the program is based on. It is sent every 80 milliseconds as defined earlier and in it the next frame in the animation is drawn. 
    case WM_TIMER:
        {
        InvalidateRect(hwnd, NULL, NULL);   // InvalidateRect 'clears' the screen, and lets the program redraw the window to display the next frame.
        PAINTSTRUCT ps; 
        HDC hdc = BeginPaint(hwnd, &ps);    // Intialising the painter tool like in WM_PAINT
        newFrame(hdc);                      // Again, passes the paint object to the function which executes the drawing
        EndPaint(hwnd, &ps);                // Ends paint. 
        }
    // When InvalidateRect is passed, it clears the entire screen and then repaints it. This causes lots of flickering, so it's manually handled here as 'do nothing'. 
    case WM_ERASEBKGND:
        {
        return TRUE;
        }

    return 0;                               // 0 returned when switch is finished

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);       // Sent message back to while loop for the next one to be sent back.
}







// This function returns a character for an integer provided, effectively mapping numbers from 0 - 83 to a character.
// Working with arrays of Unicode characters was an acute pain in several places in the body, so it was happily ditched for this much simpler character map.
// 'LPCWSTR' is a Windows API definition of a Unicode string (full name: Long Pointer to Constant Wide String (ouch)
LPCWSTR returnChar(int integerToMap)
{
    LPCWSTR KATAchar = L"";     // Defining the variable so it can be returned later. Stores the character.

    // This switch statement links each integer to a character. (Note: switch statements are like condensed if statements.)
    // The reason the 'katakana' alphabet was used was because it is the most "vectorised" of any alphabet, and so looked the best.
    switch (integerToMap)
    {
    case 0:     KATAchar = L"ア";    break;       case 1:     KATAchar = L"イ";    break;
    case 2:     KATAchar = L"ウ";    break;       case 3:     KATAchar = L"エ";    break;
    case 4:     KATAchar = L"オ";    break;       case 5:     KATAchar = L"カ";    break;
    case 6:     KATAchar = L"ガ";    break;       case 7:     KATAchar = L"キ";    break;
    case 8:     KATAchar = L"ギ";    break;       case 9:     KATAchar = L"ク";    break;
    case 10:    KATAchar = L"グ";    break;       case 11:    KATAchar = L"ケ";    break;
    case 12:    KATAchar = L"ゲ";    break;       case 13:    KATAchar = L"コ";    break;
    case 14:    KATAchar = L"ゴ";    break;       case 15:    KATAchar = L"サ";    break;
    case 16:    KATAchar = L"ザ";    break;       case 17:    KATAchar = L"シ";    break;
    case 18:    KATAchar = L"ジ";    break;       case 19:    KATAchar = L"ス";    break;
    case 20:    KATAchar = L"ズ";    break;       case 21:    KATAchar = L"セ";    break;
    case 22:    KATAchar = L"ゼ";    break;       case 23:    KATAchar = L"ソ";    break;
    case 24:    KATAchar = L"ゾ";    break;       case 25:    KATAchar = L"タ";    break;
    case 26:    KATAchar = L"ダ";    break;       case 27:    KATAchar = L"チ";    break;
    case 28:    KATAchar = L"ヂ";    break;       case 29:    KATAchar = L"ツ";    break;
    case 30:    KATAchar = L"ヅ";    break;       case 31:    KATAchar = L"テ";    break;
    case 32:    KATAchar = L"デ";    break;       case 33:    KATAchar = L"ト";    break;
    case 34:    KATAchar = L"ド";    break;       case 35:    KATAchar = L"ナ";    break;
    case 36:    KATAchar = L"ニ";    break;       case 37:    KATAchar = L"ヌ";    break;
    case 38:    KATAchar = L"ネ";    break;       case 39:    KATAchar = L"ノ";    break;
    case 40:    KATAchar = L"ハ";    break;       case 41:    KATAchar = L"バ";    break;
    case 42:    KATAchar = L"パ";    break;       case 43:    KATAchar = L"ヒ";    break;
    case 44:    KATAchar = L"ビ";    break;       case 45:    KATAchar = L"ピ";    break;
    case 46:    KATAchar = L"フ";    break;       case 47:    KATAchar = L"ブ";    break;
    case 48:    KATAchar = L"プ";    break;       case 49:    KATAchar = L"ヘ";    break;
    case 50:    KATAchar = L"ベ";    break;       case 51:    KATAchar = L"ペ";    break;
    case 52:    KATAchar = L"ホ";    break;       case 53:    KATAchar = L"ボ";    break;
    case 54:    KATAchar = L"ポ";    break;       case 55:    KATAchar = L"マ";    break;
    case 56:    KATAchar = L"ミ";    break;       case 57:    KATAchar = L"ム";    break;
    case 58:    KATAchar = L"メ";    break;       case 59:    KATAchar = L"モ";    break;
    case 60:    KATAchar = L"ャ";    break;       case 61:    KATAchar = L"ヤ";    break;
    case 62:    KATAchar = L"ユ";    break;       case 63:    KATAchar = L"ヨ";    break;
    case 64:    KATAchar = L"ラ";    break;       case 65:    KATAchar = L"リ";    break;
    case 66:    KATAchar = L"ル";    break;       case 67:    KATAchar = L"レ";    break;
    case 68:    KATAchar = L"ロ";    break;       case 69:    KATAchar = L"ヮ";    break;
    case 70:    KATAchar = L"ワ";    break;       case 71:    KATAchar = L"ヰ";    break;
    case 72:    KATAchar = L"ヱ";    break;       case 73:    KATAchar = L"ヲ";    break;
    case 74:    KATAchar = L"ン";    break;       case 75:    KATAchar = L"ヴ";    break;
    case 76:    KATAchar = L"ヷ";    break;       case 77:    KATAchar = L"ヸ";    break;
    case 78:    KATAchar = L"ヹ";    break;       case 79:    KATAchar = L"ヺ";    break;
    case 80:    KATAchar = L"ー";    break;       case 81:    KATAchar = L"ヽ";    break;
    case 82:    KATAchar = L"ヾ";    break;       case 83:    KATAchar = L"ヿ";    break;
    }
    return KATAchar;         // Returning the mapped character
}



// The draw function for WM_PAINT, responsible for the initial startup effects
void draw(HDC hdc) {
    Gdiplus::Graphics gf(hdc);                  // The paint class is sent to the GDI library which now handles it.

    Gdiplus::Bitmap bmp(L"SAD\\accept.png");    // The method for storing a bitmap image. This image is the 'Windows 95' image which is first displayed.
    gf.DrawImage(&bmp, -269, 0);                // Draws the image: the address of the bitmap, and x, y coordinates to draw it.

    // If a sound has not already been played before, play a random sound. 
    if (soundPlayed == 0)
    {
        LPCWSTR soundfilename = L"";            // Declaring the sound file name variable so it can be used outside the switch statement
        srand(GetTickCount64());                // Sets the seed of the RNG as the current time in milliseconds
        int fileno = (rand() % 5);              // Selects a random number from 0 - 4

        // Uses that number to select one of 5 different sounds
        switch (fileno)
        {
        case 0:     soundfilename = L"timemachine\\begin31.wav";     break;
        case 1:     soundfilename = L"timemachine\\begin95.wav";     break;
        case 2:     soundfilename = L"timemachine\\beginME.wav";     break;
        case 3:     soundfilename = L"timemachine\\begiNT4.wav";     break;
        case 4:     soundfilename = L"timemachine\\begiNT5.wav";     break;
        }

        PlaySound(soundfilename, NULL, SND_SYNC);   // Plays the sound file. SND_SYNC means that the next line of code is not executed until the sound has finished playing.
        Sleep(500);                                 // Little bit of extra delay
        soundPlayed++;                              // Sets soundPlayed to true so that it is not played again.
    }
}



// The draw function for WM_TIMER, responsible for animation
void newFrame(HDC hdc) {
    // These create a new 'surface' on which to paint. Later, the surface is stored as a bitmap and the bitmap itself is painted to the window.
    // Painting to the window directly causes lots of artifacts and glitches due to the number of actions taken sequentially.
    HDC bufdc = CreateCompatibleDC(hdc);
    HBITMAP hbm = CreateCompatibleBitmap(hdc, 1066, 855);
    SelectObject(bufdc, hbm);

    Gdiplus::Graphics       gf(bufdc);                              // Transfers the 'surface' to the GDI+ library

    Gdiplus::Bitmap         bmp(L"SAD\\simple_times.png");                // Draws the bitmap
    gf.DrawImage(&bmp, 0, 0);

    Gdiplus::FontFamily     fontFamily(L"Consolas");                // Font type
    Gdiplus::Font           font(&fontFamily, 17);                  // Font size
    gf.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);   // Enable anti-aliasing


    // This is the for loop which iterates over the 2D array of character mappings.
    // For each iteration it works with the columnNumber, doing things like adding the next character, resetting the column, printing and more.
    // There are 51 columns so it loops over 51 times. 
    int columnNo = 0;
    int seedvariation = 0;
    for (columnNo = 0; columnNo < 51; columnNo++)
    {
        // Each iteration uses the current time in milliseconds as a seed for the RNG. 
        // However because the loop can run many times per millisecond, the seed ends up being the same for many iterations.
        // This is why seedvariation is used to ensure that for each iteration the seed is completely unique. 
        srand(GetTickCount64() + seedvariation); // (Set the seed for RNG)
        seedvariation = seedvariation + 10000;   // Change seed variation


        // Every column is not meant to have an animation at the same time. Therefore, if the column is empty and ready to be intialised, this only allows a small % chance of it actually happening.
        if (MATRIX[columnNo][0] == 0) {     // Check if column is empty by testing for 0 at 1st position in column (if 1st is 0, then whole thing must be zero)
            int boolinit = rand() % 55;     // 1 in 55 chance of column animation actually initialising
            if (boolinit != 0) {            // Checking for that 1 in 55 chance: if yes, then advance with code to add chars etc. 
                continue;                   // If not, then skip to next iteration
            }
        }


        // This loop iterates over the column to find the next 0; i.e, the position of the new character to be added (printed in white).
        // It also adds a random new number from 0 to 83 into that position, which is then mapped to a character.
        int i = 0;                      // The character number within the column
        int positionNEWchar = 0;        // The position of the new character, declared outside loop so it can be used later
        int check = 0;                  // Boolean to terminate search once the next 0 has been found
        int addcharacter = rand() % 84; // The random character to add to the column

        while (check == 0) {                    // Checking for boolean 
            i++;                                // Character number
            if (MATRIX[columnNo][i - 1] == 0)   // If this character i is 0, then set positionNEWchar, add character, terminate loop.
            {
                positionNEWchar = i - 1;
                MATRIX[columnNo][i - 1] = addcharacter;
                check++;
            }
        }


        // For extra animation, there is a small chance that an existing character already in the column also changes. 
        int boolchange = rand() % 3;                                            // 1 in 4 chance of changed character
        if (boolchange == 0) {                                                  // If TRUE:
            int positionchangedchar = positionNEWchar - ((rand() % 23) + 1);    // Select which of the visible 24 characters in the column changes
            int newchar = rand() % 84;                                          // Select the new char to change to,
            MATRIX[columnNo][positionchangedchar] = newchar;                    // Change the char
        }
        // (I think this section also causes the columns to glitch slightly when displayed, like suddenly popping out of nowhere ... BUT it looks pretty cool so call it an intended feature)
        

        // This section prints the first character of the column that has just been added. For visual effect, this is done in white. 
        Gdiplus::SolidBrush     wbrush(Gdiplus::Color(255, 255, 255, 255));                         // Set colour to white
        int position = positionNEWchar * 24;                                                        // Store the y-position of where the character is going to be stored (24 points after the last)
        Gdiplus::PointF         point((columnNo * 20), position);                                   // Set the x and y positions
        gf.DrawString(returnChar(MATRIX[columnNo][positionNEWchar]), -1, &font, point, &wbrush);    // Draw the integer-mapped character at the positio, with the font, brush, and size. 


        // This prints the trail of characters behind the first. 
        i = 1;
        for (i = 1; i < 24; i++) {                                                                          // The character number behind the first
            position = (positionNEWchar - i) * 24;                                                          // Storing the y-position of the character
            Gdiplus::PointF         point((columnNo * 20), position);                                       // Setting the x,y co-ordinates
            int alpha = 255 - ((i - 1) * 11);                                                               // Calculate transparency of character (further away = more transparent)
            Gdiplus::SolidBrush     bbrush(Gdiplus::Color(alpha, 0, 0, 0));                                 // Set ARGB colour
            gf.DrawString(returnChar(MATRIX[columnNo][positionNEWchar - i]), -1, &font, point, &bbrush);    // Print character using all of the above, mapping the integer in the array to the character.
        }


        // If the trail has reached the very end of the screen, reset the column, which lets it be initialised later. 
        i = 0;
        if (MATRIX[columnNo][63] != 0) {    // If character 64 is not zero (i.e if trail has reached end)
            for (i = 0; i < 64; i++) {      // Iterate over each character in column
                MATRIX[columnNo][i] = 0;    // Set to zero.
            }
        }
    }

    // BitBlt gets the surface upon which we just drew and prints it to the screen, by translating it into a bitmap and rendering.
    BitBlt(hdc,          // The device handle
           0, 0,         // x, y coordinates of the bitmap print onto the window 
           1066, 855,    // Size of bitmap (same size as window)
           bufdc,        // The surface upon which we just drew
           0, 0,         // x, y coordinates of window (i think)
           SRCCOPY);     // Print style (in this case copy directly to window and nothing else.)

    // Delete the surfaces to free memory.
    DeleteObject(hbm);
    DeleteDC(bufdc);
}
