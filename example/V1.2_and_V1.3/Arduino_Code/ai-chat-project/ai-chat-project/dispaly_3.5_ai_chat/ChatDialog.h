// ChatDialog.h
#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <Arduino.h>
#include "LGFX_Setup.h"

#define MAX_MESSAGES 20  // 保持原始容量

class ChatDialog {
public:
    ChatDialog();
    void begin();
    void addMessage(String text, bool isAI);
    void appendToLastMessage(String text);
    void showMicOrSpk(int status);

private:
    struct Message {
        String text;
        bool isAI;
        int y;
        int height;
    };

    LGFX tft;
    Message messages[MAX_MESSAGES];

    int currentMicSpkStatus = 0;
    int messageCount = 0;
    int scrollPosition = 0;
    int totalContentHeight = 0;

    void drawStaticElements();
    void drawSingleMessage(int index);
    void recalculateMessageLayout();
    void drawAllMessages();
    void drawWrappedText(String text, int x, int y, int maxWidth, uint16_t color);
    int calculateTextHeight(String text, int maxWidth);

    // 保持原始小屏参数，仅调整布局位置
    static const int DIALOG_WIDTH = 440;
    static const int DIALOG_HEIGHT = 280;
    static const int DIALOG_MARGIN_TOP = 30;
    static const int BUBBLE_RADIUS = 4;
    static const int BUBBLE_MARGIN = 4;
    static const int TEXT_MARGIN = 5;
    static const int AVATAR_SIZE = 25;
    static const int AVATAR_MARGIN = 15; 
    static const int BUBBLE_CONTENT_WIDTH = 250;
    static const int TEXT_SIZE = 1;
    static const int CHAR_WIDTH = 6;
    static const int LINE_HEIGHT = 10;
    static const int DIALOG_INNER_PADDING = 5;
    static const int ARROW_WIDTH = 5;
};

#endif