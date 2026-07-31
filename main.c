#include <stdio.h>
#include <windows.h>
#include <conio.h> // برای توابع _kbhit و _getch

// تابع برای تغییر وضعیت کلیدهای اختصاصی کیبورد
void SetKeyboardLED(int led_choice) {
    BYTE keyState[256];
    GetKeyboardState(keyState);

    int vk_code = 0;
    if (led_choice == 1) vk_code = VK_CAPITAL;
    else if (led_choice == 2) vk_code = VK_NUMLOCK;
    else if (led_choice == 3) vk_code = VK_SCROLL;
    else {
        printf("Invalid LED number (1-3).\n");
        return;
    }

    keybd_event(vk_code, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(vk_code, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    
    printf("LED %d toggled.\n", led_choice);
}

int main() {
    int choice;

    printf("=== DOS Shell Multi-LED Controller ===\n");
    printf("Select an LED (1-3) or press [ESC] to exit:\n");
    printf("  [1] Caps Lock\n");
    printf("  [2] Num Lock\n");
    printf("  [3] Scroll Lock\n\n");

    while (1) {
        printf("C:\\> ");
        
        // بررسی هوشمند ورودی عدد یا کلید اسکیپ
        // در اینجا از scanf استفاده می‌کنیم، اما می‌توانید کلید Esc را هم تست کنید
        if (scanf("%d", &choice) != 1) {
            // اگر کاربر کاراکتری غیر از عدد وارد کرد، پاکسازی می‌کنیم
            while(getchar() != '\n');
            continue;
        }

        if (choice >= 1 && choice <= 3) {
            SetKeyboardLED(choice);
        }
        else {
            printf("Bad command or file name\n");
        }
        
        printf("\nPress ESC to exit, or enter another number:\n");
        
        // حلقه داخلی برای انتظار جهت دریافت عدد بعدی یا کلید ESC
        while (1) {
            if (_kbhit()) {
                int ch = _getch();
                if (ch == 27) { // کد اسکی کلید ESC برابر با 27 است
                    printf("\nExiting...\n");
                    return 0;
                }
                else if (ch >= '1' && ch <= '3') {
                    choice = ch - '0';
                    printf("%d\n", choice);
                    SetKeyboardLED(choice);
                    break; // برگشت به حلقه اصلی برای ادامه کار
                }
            }
        }
    }

    return 0;
}
