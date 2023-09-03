/*
 *--------------------------------------
 * Program Name: TINET Client (Calculator)
 * Author: TKB Studios
 * License: Apache License 2.0
 * Description: Allows the user to communicate with the TINET servers
 *--------------------------------------
 */

/* When contributing, please add your username in the list here */
/*
 *--------------Contributors--------------
 * TIny_Hacker
 * ACagliano (Anthony Cagliano)
 * Powerbyte7
 *--------------------------------------
 */

#include <string.h>
#include <sys/lcd.h>
#include <sys/timers.h>
#include <keypadc.h>
#include <graphx.h>
#include <fileioc.h>
#include <stdio.h>
#include <ti/getcsc.h>
#include <compression.h>
#include <srldrvce.h>
#include <tice.h>
#include <debug.h>
#include <stdbool.h>
#include <ti/info.h>
#include <stdint.h>

#include "gfx/gfx.h"

#include "ui/shapes.h"

#include "tinet.h"

#define MAX_MESSAGES 15
#define MAX_LINE_LENGTH (GFX_LCD_WIDTH - 40)

const system_info_t *calcSystemInfo;

bool inside_RTC_chat = false;

/* READ BUFFERS */
size_t read_flen;
char in_buffer;

/* USER */
char username;

/* DEFINE FUNCTIONS */
void GFXspritesInit();
void GFXsettings();
void NoKeyFileGFX();
void KeyFileAvailableGFX();
void FreeMemory();
void quitProgram();
void readSRL();
void getCurrentTime();
void printServerPing();
void dashboardScreen();
void mailNotVerifiedScreen();
bool startsWith(const char *str, const char *prefix);
void displayIP(const char *ipAddress);
void howToUseScreen();
void alreadyConnectedScreen();
void userNotFoundScreen();
void calcIDneedsUpdateScreen();
void TINETChatScreen();
void accountInfoScreen(const char *accountInfo);
void updateCaseBox(bool isUppercase);
void ESP8266login();
bool kb_Update();


uint8_t previous_kb_Data[8];
uint8_t debounce_delay = 10;

/* DEFINE SPRITES */
gfx_sprite_t *globe_sprite;
gfx_sprite_t *key_sprite;
gfx_sprite_t *bridge_sprite;
gfx_sprite_t *keyboard_sprite;

/* DEFINE EMOJIS */
gfx_sprite_t *blush_sprite;
gfx_sprite_t *cry_sprite;
gfx_sprite_t *dark_sunglasses_sprite;
gfx_sprite_t *dizzy_face_sprite;
gfx_sprite_t *eyeglasses_sprite;
gfx_sprite_t *eyes_sprite;
gfx_sprite_t *flushed_sprite;
gfx_sprite_t *frowning2_sprite;
gfx_sprite_t *grimacing_sprite;
gfx_sprite_t *grin_sprite;
gfx_sprite_t *grinning_sprite;
gfx_sprite_t *heart_eyes_sprite;
gfx_sprite_t *hushed_sprite;
gfx_sprite_t *innocent_sprite;
gfx_sprite_t *joy_sprite;
gfx_sprite_t *kissing_sprite;
gfx_sprite_t *kissing_heart_sprite;
gfx_sprite_t *no_mouth_sprite;
gfx_sprite_t *open_mouth_sprite;
gfx_sprite_t *pensive_sprite;
gfx_sprite_t *poop_sprite;
gfx_sprite_t *rage_sprite;
gfx_sprite_t *rofl_sprite;
gfx_sprite_t *sleeping_sprite;
gfx_sprite_t *slight_smile_sprite;
gfx_sprite_t *smiley_sprite;
gfx_sprite_t *smirk_sprite;
gfx_sprite_t *sob_sprite;
gfx_sprite_t *stuck_out_tongue_sprite;
gfx_sprite_t *stuck_out_tongue_closed_eyes_sprite;
gfx_sprite_t *stuck_out_tongue_winking_eye_sprite;
gfx_sprite_t *sunglasses_sprite;
gfx_sprite_t *sweat_sprite;
gfx_sprite_t *wink_sprite;
gfx_sprite_t *yum_sprite;
gfx_sprite_t *zipper_mouth_sprite;

void load_sprites()
{
    globe_sprite = gfx_MallocSprite(globe_width, globe_height);
    key_sprite = gfx_MallocSprite(key_width, key_height);
    bridge_sprite = gfx_MallocSprite(bridge_width, bridge_height);
    keyboard_sprite = gfx_MallocSprite(keyboard_width, keyboard_height);
    blush_sprite = gfx_MallocSprite(blush_width, blush_height);
    cry_sprite = gfx_MallocSprite(cry_width, cry_height);
    dark_sunglasses_sprite = gfx_MallocSprite(dark_sunglasses_width, dark_sunglasses_height);
    dizzy_face_sprite = gfx_MallocSprite(dizzy_face_width, dizzy_face_height);
    eyeglasses_sprite = gfx_MallocSprite(eyeglasses_width, eyeglasses_height);
    eyes_sprite = gfx_MallocSprite(eyes_width, eyes_height);
    flushed_sprite = gfx_MallocSprite(flushed_width, flushed_height);
    frowning2_sprite = gfx_MallocSprite(frowning2_width, frowning2_height);
    grimacing_sprite = gfx_MallocSprite(grimacing_width, grimacing_height);
    grin_sprite = gfx_MallocSprite(grin_width, grin_height);
    grinning_sprite = gfx_MallocSprite(grinning_width, grinning_height);
    heart_eyes_sprite = gfx_MallocSprite(heart_eyes_width, heart_eyes_height);
    hushed_sprite = gfx_MallocSprite(hushed_width, hushed_height);
    innocent_sprite = gfx_MallocSprite(innocent_width, innocent_height);
    joy_sprite = gfx_MallocSprite(joy_width, joy_height);
    kissing_sprite = gfx_MallocSprite(kissing_width, kissing_height);
    kissing_heart_sprite = gfx_MallocSprite(kissing_heart_width, kissing_heart_height);
    no_mouth_sprite = gfx_MallocSprite(no_mouth_width, no_mouth_height);
    open_mouth_sprite = gfx_MallocSprite(open_mouth_width, open_mouth_height);
    pensive_sprite = gfx_MallocSprite(pensive_width, pensive_height);
    poop_sprite = gfx_MallocSprite(poop_width, poop_height);
    rage_sprite = gfx_MallocSprite(rage_width, rage_height);
    rofl_sprite = gfx_MallocSprite(rofl_width, rofl_height);
    sleeping_sprite = gfx_MallocSprite(sleeping_width, sleeping_height);
    slight_smile_sprite = gfx_MallocSprite(slight_smile_width, slight_smile_height);
    smiley_sprite = gfx_MallocSprite(smiley_width, smiley_height);
    smirk_sprite = gfx_MallocSprite(smirk_width, smirk_height);
    sob_sprite = gfx_MallocSprite(sob_width, sob_height);
    stuck_out_tongue_sprite = gfx_MallocSprite(stuck_out_tongue_width, stuck_out_tongue_height);
    stuck_out_tongue_closed_eyes_sprite = gfx_MallocSprite(stuck_out_tongue_closed_eyes_width, stuck_out_tongue_closed_eyes_height);
    stuck_out_tongue_winking_eye_sprite = gfx_MallocSprite(stuck_out_tongue_winking_eye_width, stuck_out_tongue_winking_eye_height);
    sunglasses_sprite = gfx_MallocSprite(sunglasses_width, sunglasses_height);
    sweat_sprite = gfx_MallocSprite(sweat_width, sweat_height);
    wink_sprite = gfx_MallocSprite(wink_width, wink_height);
    yum_sprite = gfx_MallocSprite(yum_width, yum_height);
    zipper_mouth_sprite = gfx_MallocSprite(zipper_mouth_width, zipper_mouth_height);
}

void decompress_sprites()
{
    zx0_Decompress(globe_sprite, globe_compressed);
    zx0_Decompress(key_sprite, key_compressed);
    zx0_Decompress(bridge_sprite, bridge_compressed);
    zx0_Decompress(keyboard_sprite, keyboard_compressed);
    zx0_Decompress(blush_sprite, blush_compressed);
    zx0_Decompress(cry_sprite, cry_compressed);
    zx0_Decompress(dark_sunglasses_sprite, dark_sunglasses_compressed);
    zx0_Decompress(dizzy_face_sprite, dizzy_face_compressed);
    zx0_Decompress(eyeglasses_sprite, eyeglasses_compressed);
    zx0_Decompress(eyes_sprite, eyes_compressed);
    zx0_Decompress(flushed_sprite, flushed_compressed);
    zx0_Decompress(frowning2_sprite, frowning2_compressed);
    zx0_Decompress(grimacing_sprite, grimacing_compressed);
    zx0_Decompress(grin_sprite, grin_compressed);
    zx0_Decompress(grinning_sprite, grinning_compressed);
    zx0_Decompress(heart_eyes_sprite, heart_eyes_compressed);
    zx0_Decompress(hushed_sprite, hushed_compressed);
    zx0_Decompress(innocent_sprite, innocent_compressed);
    zx0_Decompress(joy_sprite, joy_compressed);
    zx0_Decompress(kissing_sprite, kissing_compressed);
    zx0_Decompress(kissing_heart_sprite, kissing_heart_compressed);
    zx0_Decompress(no_mouth_sprite, no_mouth_compressed);
    zx0_Decompress(open_mouth_sprite, open_mouth_compressed);
    zx0_Decompress(pensive_sprite, pensive_compressed);
    zx0_Decompress(poop_sprite, poop_compressed);
    zx0_Decompress(rage_sprite, rage_compressed);
    zx0_Decompress(rofl_sprite, rofl_compressed);
    zx0_Decompress(sleeping_sprite, sleeping_compressed);
    zx0_Decompress(slight_smile_sprite, slight_smile_compressed);
    zx0_Decompress(smiley_sprite, smiley_compressed);
    zx0_Decompress(smirk_sprite, smirk_compressed);
    zx0_Decompress(sob_sprite, sob_compressed);
    zx0_Decompress(stuck_out_tongue_sprite, stuck_out_tongue_compressed);
    zx0_Decompress(stuck_out_tongue_closed_eyes_sprite, stuck_out_tongue_closed_eyes_compressed);
    zx0_Decompress(stuck_out_tongue_winking_eye_sprite, stuck_out_tongue_winking_eye_compressed);
    zx0_Decompress(sunglasses_sprite, sunglasses_compressed);
    zx0_Decompress(sweat_sprite, sweat_compressed);
    zx0_Decompress(wink_sprite, wink_compressed);
    zx0_Decompress(yum_sprite, yum_compressed);
    zx0_Decompress(zipper_mouth_sprite, zipper_mouth_compressed);
}

void release_sprites()
{
    free(globe_sprite);
    free(key_sprite);
    free(bridge_sprite);
    free(keyboard_sprite);
    free(blush_sprite);
    free(cry_sprite);
    free(dark_sunglasses_sprite);
    free(dizzy_face_sprite);
    free(eyeglasses_sprite);
    free(eyes_sprite);
    free(flushed_sprite);
    free(frowning2_sprite);
    free(grimacing_sprite);
    free(grin_sprite);
    free(grinning_sprite);
    free(heart_eyes_sprite);
    free(hushed_sprite);
    free(innocent_sprite);
    free(joy_sprite);
    free(kissing_sprite);
    free(kissing_heart_sprite);
    free(no_mouth_sprite);
    free(open_mouth_sprite);
    free(pensive_sprite);
    free(poop_sprite);
    free(rage_sprite);
    free(rofl_sprite);
    free(sleeping_sprite);
    free(slight_smile_sprite);
    free(smiley_sprite);
    free(smirk_sprite);
    free(sob_sprite);
    free(stuck_out_tongue_sprite);
    free(stuck_out_tongue_closed_eyes_sprite);
    free(stuck_out_tongue_winking_eye_sprite);
    free(sunglasses_sprite);
    free(sweat_sprite);
    free(wink_sprite);
    free(yum_sprite);
    free(zipper_mouth_sprite);
}

/* Updates kb_Data and keeps track of previous keypresses, returns true if changes were detected */
bool kb_Update()
{
    // Update previous input state
    for (uint8_t i = 0; i <= 7; i++)
    {
        previous_kb_Data[i] = kb_Data[i];
    }

    kb_Scan();

    // Determine whether input has changed
    for (uint8_t i = 0; i <= 7; i++)
    {
        if (previous_kb_Data[i] != kb_Data[i])
        {
            return true;
        }
    }

    return false;
}

/* DEFINE BUTTONS */
typedef struct
{
    int x, y;
    int width, height;
    const char *label;
    void (*action)();
} Button;

void BucketsButtonPressed()
{
    printf("Buckets btn press\n");
    msleep(500);
}

Button dashboardButtons[] = {
    {50, 100, 120, 30, "TINET Chat", TINETChatScreen}
};
int numDashboardButtons = sizeof(dashboardButtons) / sizeof(dashboardButtons[0]);

void loginButtonPressed()
{
    enum TINETLoginStatus loggedIn = tinet_login();
    if (loggedIn == TINET_LOGIN_SUCCESS)
    {
        username = tinetGetUsername();
        dashboardScreen();
    }
}

Button mainMenuButtons[] = {
    // Button properties: X (top left corner), Y (top left corner), width, height, label, function
    {20, 160, 120, 30, "Login", loginButtonPressed},
    {20, 200, 120, 30, "How to Use", howToUseScreen}};

int numMainMenuButtons = sizeof(mainMenuButtons) / sizeof(mainMenuButtons[0]);

void drawButtons(Button *buttons, int numButtons, int selectedButton)
{
    for (int i = 0; i < numButtons; i++)
    {
        if (i == selectedButton)
        {
            shapes_RoundRectangleFill(255, 10, buttons[i].width, buttons[i].height, buttons[i].x, buttons[i].y);
            shapes_RoundRectangleFill(18, 10, buttons[i].width - 2, buttons[i].height - 2, buttons[i].x + 1, buttons[i].y + 1);
        }
        else
        {
            shapes_RoundRectangleFill(18, 10, buttons[i].width, buttons[i].height, buttons[i].x, buttons[i].y);
        }
        gfx_PrintStringXY(buttons[i].label, buttons[i].x + 10, buttons[i].y + 10);
    }
    msleep(250);
}

/* DEFINE CHAT */
void printWrappedText(const char *text, int x, int y);
void displayMessages();
void addMessage(const char *message, int posY);

typedef struct
{
    char recipient[19];
    int timestamp;
    char message[64];
    int posY;
} ChatMessage;

ChatMessage messageList[MAX_MESSAGES];
int messageCount = 0;

/* DEFINE DATETIME */
typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} DateTime;

/* DEFINE UI */
#define TITLE_X_POS 5
#define TITLE_Y_POS 5

int main(void)
{
    const system_info_t *calcSystemInfo = os_GetSystemInfo();

    if (calcSystemInfo->hardwareType2 != 9)
    {
        printf("%02X", calcSystemInfo->hardwareType2);
        printf("\n");
        printf("not TI-84+CE");
        msleep(500);
        return 1;
    }

    // Initialize graphics and settings
    gfx_Begin();
    GFXsettings();

    // initialize TINET
    enum TINETInitStatus tinet_initialized = tinet_init();
    if (tinet_initialized == TINET_INIT_FAIL)
    {
        printf("TINET INIT\nFAIL\n");
        msleep(1000);
        return 0;
    }

    // load sprites
    load_sprites();
    decompress_sprites();

    // Display main menu
    gfx_ZeroScreen();
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY("TINET", (GFX_LCD_WIDTH - gfx_GetStringWidth("TINET")) / 2, 5);
    gfx_SetTextFGColor(224);
    gfx_PrintStringXY("Press [clear] to quit.", (GFX_LCD_WIDTH - gfx_GetStringWidth("Press [clear] to quit.")) / 2, 35);
    gfx_SetTextFGColor(255);
    gfx_SetTextScale(1, 1);

    /* CALC ID DISPLAY BOTTOM RIGHT */
    char calcidStr[sizeof(calcSystemInfo->calcid) * 2 + 1];
    for (unsigned int i = 0; i < sizeof(calcSystemInfo->calcid); i++)
    {
        sprintf(calcidStr + i * 2, "%02X", calcSystemInfo->calcid[i]);
    }

    gfx_PrintStringXY(calcidStr, 320 - gfx_GetStringWidth(calcidStr), 232);

    int selectedButton = 0;
    drawButtons(mainMenuButtons, numMainMenuButtons, selectedButton);

    do
    {
        kb_Update();

        tinet_handle_srl();

        if (kb_Data[7] == kb_Down && previous_kb_Data[7] != kb_Down)
        {
            selectedButton = (selectedButton + 1) % numMainMenuButtons;
            drawButtons(mainMenuButtons, numMainMenuButtons, selectedButton);
        }
        else if (kb_Data[7] == kb_Up && previous_kb_Data[7] != kb_Up)
        {
            selectedButton = (selectedButton - 1 + numMainMenuButtons) % numMainMenuButtons;
            drawButtons(mainMenuButtons, numMainMenuButtons, selectedButton);
        }
        else if (kb_Data[6] == kb_Enter && previous_kb_Data[6] != kb_Enter)
        {
            mainMenuButtons[selectedButton].action();
        }
    } while (kb_Data[6] != kb_Clear);

    quitProgram();

    return 0;
}

void accountInfoScreen(const char *accountInfo)
{
    gfx_ZeroScreen();
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY("TINET Account Info", ((GFX_LCD_WIDTH - gfx_GetStringWidth("TINET Account Info")) / 2), 5);
    gfx_SetTextFGColor(224);
    gfx_PrintStringXY("Press [clear] to quit.", ((GFX_LCD_WIDTH - gfx_GetStringWidth("Press [clear] to quit.")) / 2), 35);
    gfx_SetTextFGColor(255);
    gfx_SetTextScale(1, 1);

    char *infoTokens[8];
    char *token = strtok((char *)accountInfo, ";");
    int i = 0;
    while (token != NULL && i < 8)
    {
        infoTokens[i++] = token;
        token = strtok(NULL, ";");
    }

    int y = 65;
    gfx_PrintStringXY("Account Info:", 1, y);
    y += 20;
    gfx_PrintStringXY("MB Used Total: ", 1, y);
    gfx_PrintString(infoTokens[1]);
    y += 15;
    gfx_PrintStringXY("Total Requests: ", 1, y);
    gfx_PrintString(infoTokens[3]);
    y += 15;
    gfx_PrintStringXY("Plan: ", 1, y);
    gfx_PrintString(infoTokens[4]);
    y += 15;
    gfx_PrintStringXY("Time Online (seconds): ", 1, y);
    gfx_PrintString(infoTokens[5]);
    y += 15;
    gfx_PrintStringXY("Last Login (epoch time): ", 1, y);
    gfx_PrintString(infoTokens[6]);
    y += 15;
    gfx_PrintStringXY("Profile Public: ", 1, y);
    gfx_PrintString(infoTokens[7]);
    do
    {
        kb_Update();

        tinet_handle_srl();
    
    } while (kb_Data[6] != kb_Clear);
}

void dashboardScreen()
{
    gfx_ZeroScreen();
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY("TINET Dashboard", ((GFX_LCD_WIDTH - gfx_GetStringWidth("TINET Dashboard")) / 2), 5);
    gfx_SetTextFGColor(224);
    gfx_SetTextScale(1, 1);
    gfx_PrintStringXY("Press [clear] to quit.", ((GFX_LCD_WIDTH - gfx_GetStringWidth("Press [clear] to quit.")) / 2), 35);
    gfx_SetTextFGColor(255);

    int centerX = (GFX_LCD_WIDTH - gfx_GetStringWidth("Logged in as ")) / 2;

    gfx_PrintStringXY("Logged in as ", centerX - gfx_GetStringWidth("Logged in as "), 45);

    gfx_PrintStringXY(username, centerX, 45);

    int selectedButton = 0;
    drawButtons(dashboardButtons, numDashboardButtons, selectedButton);

    do
    {
        kb_Update();
        
        tinet_handle_srl();

        if (kb_Data[6] == kb_Clear)
        {
            break;
        }

        if (kb_Data[7] == kb_Down && previous_kb_Data[7] != kb_Down)
        {
            selectedButton = (selectedButton + 1) % numDashboardButtons;
            drawButtons(dashboardButtons, numDashboardButtons, selectedButton);
        }
        else if (kb_Data[7] == kb_Up && previous_kb_Data[7] != kb_Up)
        {
            selectedButton = (selectedButton - 1 + numDashboardButtons) % numDashboardButtons;
            drawButtons(dashboardButtons, numDashboardButtons, selectedButton);
        }
        else if (kb_Data[6] == kb_Enter && previous_kb_Data[6] != kb_Enter)
        {
            dashboardButtons[selectedButton].action();
        }
    } while (1);
}

void GFXsettings()
{
    /* GFX SETTINGS */
    gfx_FillScreen(0x00);
    gfx_SetTextFGColor(255);
    gfx_SetTextBGColor(0);
    gfx_SetTextTransparentColor(0);
}

void KeyFileAvailableGFX()
{
    gfx_PrintStringXY("Keyfile detected!", ((GFX_LCD_WIDTH - gfx_GetStringWidth("Keyfile detected!")) / 2), 95);

    char display_str[64];
    snprintf(display_str, sizeof(display_str), "Username: %s", username);
    gfx_PrintStringXY(display_str, ((GFX_LCD_WIDTH - gfx_GetStringWidth(display_str)) / 2), 135);

    gfx_PrintStringXY("Press [enter] to connect!", ((GFX_LCD_WIDTH - gfx_GetStringWidth("Press [enter] to connect!")) / 2), 65);
}

void NoKeyFileGFX()
{
    gfx_PrintStringXY("Please first add your keyfile!!", ((GFX_LCD_WIDTH - gfx_GetStringWidth("Please first add your keyfile!!")) / 2), 90);
    gfx_PrintStringXY("https://tinet.tkbstudios.com/login", ((GFX_LCD_WIDTH - gfx_GetStringWidth("https://tinet.tkbstudios.com/login")) / 2), 100);
}

void readSRL()
{
    in_buffer = tinet_handle_srl();

    if (startsWith(in_buffer, "ACCOUNT_INFO:"))
    {
        printf("got acc inf");
        accountInfoScreen(in_buffer + strlen("ACCOUNT_INFO:"));
    }


    if (startsWith(in_buffer, "RTC_CHAT:"))
    {
        char *messageContent = strstr(in_buffer, ":");
        
        if (messageContent)
        {
            messageContent = strstr(messageContent + 1, ":");
            if (messageContent)
            {
                messageContent++;
                messageContent++;
                addMessage(messageContent, 200 + messageCount * 15);
                displayMessages();
            }
        }
    }
}

void quitProgram()
{
    gfx_End();
    usb_Cleanup();
    // GFXspritesFree();
    exit(0);
}

bool startsWith(const char *str, const char *prefix)
{
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

void displayIP(const char *ipAddress)
{
    printf("Received IP address: %s\n", ipAddress);
}

void howToUseScreen()
{
    free(globe_sprite);
    free(bridge_sprite);
    free(key_sprite);

    gfx_ZeroScreen();
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY("How To TINET", ((GFX_LCD_WIDTH - gfx_GetStringWidth("How To TINET")) / 2), 5);
    gfx_SetTextFGColor(224);
    gfx_PrintStringXY("Press [clear] to quit.", (GFX_LCD_WIDTH - gfx_GetStringWidth("Press [clear] to quit.")) / 2, 35);
    gfx_SetTextFGColor(255);
    gfx_SetTextScale(1, 1);

    gfx_PrintStringXY("https://tinet.tkbstudios.com/", (GFX_LCD_WIDTH - gfx_GetStringWidth("https://tinet.tkbstudios.com/")) / 2, GFX_LCD_HEIGHT / 2);

    do
    {
        kb_Update();

        tinet_handle_srl();

        if (kb_Data[6] == kb_Clear)
        {
            break;
        }
    } while (1);
}

void alreadyConnectedScreen()
{
    gfx_ZeroScreen();
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY("Already Connected", ((GFX_LCD_WIDTH - gfx_GetStringWidth("Already Connected")) / 2), 5);
    gfx_SetTextFGColor(224);
    gfx_PrintStringXY("you're already connected to TINET", (GFX_LCD_WIDTH - gfx_GetStringWidth("you're already connected to TINET")) / 2, 35);
    gfx_SetTextFGColor(255);
    gfx_PrintStringXY("Is it not you?", (GFX_LCD_WIDTH - gfx_GetStringWidth("Is it not you?")) / 2, 65);
    gfx_SetTextScale(1, 1);
    gfx_PrintStringXY("Reset your calc key", (GFX_LCD_WIDTH - gfx_GetStringWidth("Reset your calc key")) / 2, 65);
    gfx_PrintStringXY("And log out from everywhere", (GFX_LCD_WIDTH - gfx_GetStringWidth("And log out from everywhere")) / 2, 80);
    gfx_PrintStringXY("on https://tinet.tkbstudios.com/dashboard", (GFX_LCD_WIDTH - gfx_GetStringWidth("https://tinet.tkbstudios.com/dashboard")) / 2, GFX_LCD_HEIGHT / 2);
    do
    {
        kb_Update();
        if (kb_Data[6] == kb_Clear)
        {
            break;
        }
    } while (1);
}

void userNotFoundScreen()
{
    gfx_ZeroScreen();
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY("TINET USER NOT FOUND", ((GFX_LCD_WIDTH - gfx_GetStringWidth("TINET USER NOT FOUND")) / 2), 5);
    gfx_SetTextFGColor(224);
    gfx_PrintStringXY("Your user doesn't exist", (GFX_LCD_WIDTH - gfx_GetStringWidth("Your user doesn't exist")) / 2, 35);
    do
    {
        kb_Update();

        tinet_handle_srl();

        if (kb_Data[6] == kb_Clear)
        {
            break;
        }
    } while (1);
}

void calcIDneedsUpdateScreen()
{
    gfx_ZeroScreen();
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY("ID UPDATE", ((GFX_LCD_WIDTH - gfx_GetStringWidth("ID UPDATE")) / 2), 5);
    gfx_SetTextFGColor(224);
    gfx_PrintStringXY("calc ID update", (GFX_LCD_WIDTH - gfx_GetStringWidth("calc ID update")) / 2, 35);
    gfx_SetTextScale(1, 1);
    gfx_PrintStringXY("update it on https://tinet.tkbstudios.com/dashboard", (GFX_LCD_WIDTH - gfx_GetStringWidth("update it on https://tinet.tkbstudios.com/dashboard")) / 2, 50);

    if (calcSystemInfo != NULL)
    {
        gfx_PrintStringXY("calcid: ", 10, 70);

        char calcidStr[sizeof(calcSystemInfo->calcid) * 2 + 1];
        for (unsigned int i = 0; i < sizeof(calcSystemInfo->calcid); i++)
        {
            sprintf(calcidStr + i * 2, "%02X", calcSystemInfo->calcid[i]);
        }
        gfx_PrintStringXY(calcidStr, 10 + gfx_GetStringWidth("calcid: "), 70);
    }
    else
    {
        gfx_SetTextScale(2, 2);
        gfx_PrintStringXY("Failed to get system info!", (GFX_LCD_WIDTH - gfx_GetStringWidth("Failed to get system info!")) / 2, GFX_LCD_HEIGHT / 2);
    }
    do
    {
        kb_Update();

        tinet_handle_srl();

        if (kb_Data[6] == kb_Clear)
        {
            break;
        }
    } while (1);
}

void TINETChatScreen()
{
    const char *uppercasechars = "\0\0\0\0\0\0\0\0\0\0\"WRMH\0\0?[VQLG\0\0:ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";
    const char *lowercasechars = "\0\0\0\0\0\0\0\0\0\0\"wrmh\0\0?[vqlg\0\0:zupkfc\0 ytojeb\0\0xsnida\0\0\0\0\0\0\0\0";
    uint8_t key, i = 0;
    int textX = 10;
    inside_RTC_chat = true;
    bool need_to_send = true;
    bool uppercase = false;
    bool emojiMethod = false;

    typedef struct
    {
        const char *sequence;
        gfx_sprite_t *sprite;
    } EmojiSpriteEntry;

    EmojiSpriteEntry emojiSpriteTable[] = {
        {":kb:", keyboard_sprite}, // pointer to emoji
    };

    /* DRAW SCREEN */
    gfx_ZeroScreen();
    gfx_SetTextScale(1, 1);
    gfx_SetColor(25);
    gfx_FillRectangle(0, 0, GFX_LCD_WIDTH, 20);
    gfx_PrintStringXY("TINET Chat", TITLE_X_POS, TITLE_Y_POS);
    updateCaseBox(uppercase);

    key = os_GetCSC();
    while (key != sk_Clear)
    {
        char buffer[128] = {0};
        i = 0;

        tinet_handle_srl();

        gfx_SetColor(25);
        gfx_FillRectangle(0, 190, 320, 50);

        char output_buffer[48] = "RTC_CHAT:global:";

        do
        {
            key = os_GetCSC();
            if (!emojiMethod)
            {
                const char *charSet = (uppercase) ? uppercasechars : lowercasechars;
                char typedChar = charSet[key];

                if (typedChar && key != 0)
                {
                    gfx_SetTextScale(2, 2);
                    gfx_PrintStringXY(&typedChar, textX, 220);
                    gfx_SetTextScale(1, 1);
                    textX += gfx_GetStringWidth(&typedChar) * 2;
                    buffer[i++] = typedChar;
                }
            }
            else
            {
                for (size_t j = 0; j < sizeof(emojiSpriteTable) / sizeof(emojiSpriteTable[0]); j++)
                {
                    if (strncmp(emojiSpriteTable[j].sequence, buffer + i, strlen(emojiSpriteTable[j].sequence)) == 0)
                    {
                        gfx_Sprite(emojiSpriteTable[j].sprite, textX, 210);
                        textX += emojiSpriteTable[j].sprite->width;
                        i += strlen(emojiSpriteTable[j].sequence);
                        break;
                    }
                }
            }

            if (key == sk_Del && i > 0)
            {
                i--;
                char removedChar = buffer[i];
                textX -= gfx_GetStringWidth(&removedChar) * 2;
                buffer[i] = '\0';
                gfx_SetColor(25);
                gfx_FillRectangle(0, 210, 320, 30);
                gfx_SetTextScale(2, 2);
                gfx_PrintStringXY(buffer, 10, 220);
                gfx_SetTextScale(1, 1);
            }
            
            tinet_handle_srl();

            if (key == sk_Clear)
            {
                break;
            }

            if (key == sk_Enter)
            {
                need_to_send = true;
                break;
            }

            if (key == sk_Alpha)
            {
                uppercase = !uppercase;
                updateCaseBox(uppercase);
            }

            if (key == sk_Mode)
            {
                emojiMethod = !emojiMethod;
                gfx_FillRectangle(GFX_LCD_WIDTH - keyboard_width - 5, GFX_LCD_HEIGHT - keyboard_height - 5, keyboard_width, keyboard_height);
                if (emojiMethod == true)
                {
                    gfx_Sprite(keyboard_sprite, GFX_LCD_WIDTH - keyboard_width - 5, GFX_LCD_HEIGHT - keyboard_height - 5);
                }
                else
                {
                    gfx_Sprite(keyboard_sprite, GFX_LCD_WIDTH - keyboard_width - 5, GFX_LCD_HEIGHT - keyboard_height - 5);
                }
            }
        } while (1);

        if (strcmp(buffer, "") != 0 && need_to_send == true)
        {
            strcat(output_buffer, buffer);

            tinet_write_serial(output_buffer);
            msleep(100);
            gfx_SetColor(25);
            gfx_FillRectangle(0, 210, 320, 30);
            textX = 20;
            need_to_send = false;
        }
    }

    inside_RTC_chat = false;
    free(keyboard_sprite);
}

/*
void displayMessages()
{
    gfx_SetTextScale(1, 1);
    int yOffset = 60;
    int maxTextWidth = 290;
    int lineHeight = 10;

    for (int i = 0; i < messageCount; i++)
    {
        const char *message = messageList[i].message;
        int messageLength = strlen(message);
        int xPos = 20;
        int j = 0;

        while (j < messageLength)
        {
            int substringWidth = gfx_GetStringWidth(&message[j]);

            if (xPos + substringWidth <= maxTextWidth)
            {
                gfx_PrintStringXY(&message[j], xPos, yOffset);
                xPos += substringWidth;
                j++;
            }
            else
            {
                yOffset += lineHeight;
                xPos = 20;
            }
        }
        yOffset += lineHeight;
    }
}
*/

void displayMessages()
{
    gfx_SetTextScale(1, 1);
    gfx_SetTextFGColor(255);
    int yOffset = 25;
    for (int i = 0; i < messageCount; i++)
    {
        gfx_PrintStringXY(messageList[i].message, 10, yOffset);
        yOffset += 10;
    }
}

void addMessage(const char *message, int posY)
{
    if (messageCount >= MAX_MESSAGES)
    {
        for (int i = 0; i < messageCount - 1; i++)
        {
            strncpy(messageList[i].recipient, messageList[i + 1].recipient, sizeof(messageList[i].recipient) - 1);
            strncpy(messageList[i].message, messageList[i + 1].message, sizeof(messageList[i].message) - 1);
            messageList[i].recipient[sizeof(messageList[i].recipient) - 1] = '\0';
            messageList[i].message[sizeof(messageList[i].message) - 1] = '\0';
            messageList[i].posY = messageList[i + 1].posY;
        }
        messageCount--;
    }

    ChatMessage newMessage;
    strncpy(newMessage.message, message, sizeof(newMessage.message) - 1);
    newMessage.recipient[sizeof(newMessage.recipient) - 1] = '\0';
    newMessage.message[sizeof(newMessage.message) - 1] = '\0';
    newMessage.posY = posY;
    messageList[messageCount] = newMessage;
    messageCount++;
    printf("%s", newMessage.recipient);
}

void updateCaseBox(bool isUppercase)
{
    char *boxText = isUppercase ? "UC" : "lc";
    gfx_SetColor(25);
    gfx_SetTextFGColor(255);
    gfx_FillRectangle(GFX_LCD_WIDTH - gfx_GetStringWidth(boxText) - 5, 0, gfx_GetStringWidth(boxText) + 5, 14);
    gfx_PrintStringXY(boxText, GFX_LCD_WIDTH - gfx_GetStringWidth(boxText) - 5, 4);
}
