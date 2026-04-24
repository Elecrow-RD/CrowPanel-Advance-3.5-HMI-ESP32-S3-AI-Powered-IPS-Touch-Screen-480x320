// ChatDialog.cpp
#include "ChatDialog.h"
#include "img/elecrow_logo.h"
#include "img/ai_logo.h"
#include "img/user_logo.h"
#include "img/mic_logo.h"
#include "img/spk_logo.h"

ChatDialog::ChatDialog() {}

void ChatDialog::showMicOrSpk(int status) {
    currentMicSpkStatus = status;
    int x = (tft.width() - 20) / 2;  // 居中显示
    int y = 8;
    int w = 20;
    int h = 20;
    
    if (status == 1) {
        tft.pushImage(x, y, w, h, MIC_LOGO);
    } else if (status == 2) {
        tft.pushImage(x, y, w, h, SPK_LOGO);
    } else {
        tft.fillRect(x, y, w, h, TFT_WHITE);
    }
}

void ChatDialog::begin() {
    tft.init();
    tft.setSwapBytes(true);
    tft.setRotation(0);  // 设置为竖屏模式
    tft.fillScreen(TFT_BLACK);
    tft.setTextFont(0);
    drawStaticElements();
}

void ChatDialog::addMessage(String text, bool isAI) {
    if (messageCount >= MAX_MESSAGES) {
        for (int i = 0; i < MAX_MESSAGES - 1; i++) {
            messages[i] = messages[i + 1];
        }
        messageCount = MAX_MESSAGES - 1;
    }

    messages[messageCount].text = text;
    messages[messageCount].isAI = isAI;
    messageCount++;

    recalculateMessageLayout();
    scrollPosition = max(0, totalContentHeight - (DIALOG_HEIGHT - DIALOG_INNER_PADDING*2));
    drawAllMessages();
}

void ChatDialog::appendToLastMessage(String text) {
    if (messageCount == 0 || !messages[messageCount - 1].isAI) {
        addMessage(text, true);
        return;
    }

    Message* lastMsg = &messages[messageCount - 1];
    lastMsg->text += text;
    
    recalculateMessageLayout();
    scrollPosition = max(0, totalContentHeight - (DIALOG_HEIGHT - DIALOG_INNER_PADDING*2));
    drawAllMessages();
}

void ChatDialog::drawStaticElements() {
    tft.fillScreen(TFT_WHITE);
    int dialogX = (tft.width() - DIALOG_WIDTH) / 2;  // 水平居中
    
    // 绘制对话框
    tft.drawRoundRect(dialogX, 
                     DIALOG_MARGIN_TOP,
                     DIALOG_WIDTH, 
                     DIALOG_HEIGHT,
                     10, 
                     TFT_DARKGREY);
    
    // 绘制标题
    tft.setTextSize(2);
    tft.setTextColor(TFT_BLUE, TFT_WHITE);
    tft.setCursor(dialogX + 5, DIALOG_MARGIN_TOP - 20);
    tft.print("DEEPSEEK");
    
    // 右侧LOGO（保持原始尺寸）
    tft.pushImage(tft.width() - 130, DIALOG_MARGIN_TOP - 25, 100, 20, ELECROW_LOGO);
    
    // 状态图标
    showMicOrSpk(currentMicSpkStatus);
}

void ChatDialog::recalculateMessageLayout() {
    totalContentHeight = 0;
    int currentY = DIALOG_INNER_PADDING;
    
    for (int i = 0; i < messageCount; i++) {
        int textWidth = BUBBLE_CONTENT_WIDTH - TEXT_MARGIN*2;
        int textHeight = calculateTextHeight(messages[i].text, textWidth);
        int bubbleHeight = textHeight + TEXT_MARGIN * 2;
        messages[i].height = max(bubbleHeight, AVATAR_SIZE);
        messages[i].y = currentY;
        currentY += messages[i].height + BUBBLE_MARGIN;
        totalContentHeight = currentY + DIALOG_INNER_PADDING;
    }
}

void ChatDialog::drawAllMessages() {
    drawStaticElements();
    
    int dialogX = (tft.width() - DIALOG_WIDTH) / 2;
    tft.setClipRect(dialogX, 
                   DIALOG_MARGIN_TOP + DIALOG_INNER_PADDING,
                   DIALOG_WIDTH, 
                   DIALOG_HEIGHT - DIALOG_INNER_PADDING*2);
    
    for (int i = 0; i < messageCount; i++) {
        int bubbleY = DIALOG_MARGIN_TOP + messages[i].y - scrollPosition;
        int messageBottom = bubbleY + messages[i].height;
        
        if (messageBottom > (DIALOG_MARGIN_TOP + DIALOG_INNER_PADDING) && 
            bubbleY < (DIALOG_MARGIN_TOP + DIALOG_HEIGHT - DIALOG_INNER_PADDING)) {
            drawSingleMessage(i);
        }
    }
    tft.clearClipRect();
}

void ChatDialog::drawSingleMessage(int index) {
    Message msg = messages[index];
    int dialogX = (tft.width() - DIALOG_WIDTH) / 2;
    int bubbleContentWidth = BUBBLE_CONTENT_WIDTH;
    int bubbleWidth = bubbleContentWidth + ARROW_WIDTH;
    
    int avatarX = msg.isAI ? (dialogX + AVATAR_MARGIN) 
                         : (dialogX + DIALOG_WIDTH - AVATAR_SIZE - AVATAR_MARGIN);
    int baseY = DIALOG_MARGIN_TOP + DIALOG_INNER_PADDING + msg.y - scrollPosition;
    
    int bubbleX, arrowX, arrowY;
    if(msg.isAI) {
        bubbleX = avatarX + AVATAR_SIZE + AVATAR_MARGIN - ARROW_WIDTH;
        arrowX = bubbleX + ARROW_WIDTH;
        arrowY = baseY + AVATAR_SIZE/2 - ARROW_WIDTH/2;
    } else {
        bubbleX = avatarX - bubbleWidth;
        arrowX = bubbleX + bubbleContentWidth;
        arrowY = baseY + AVATAR_SIZE/2 - ARROW_WIDTH/2;
    }

    // 绘制头像（保持25x25）
    tft.pushImage(avatarX, baseY, AVATAR_SIZE, AVATAR_SIZE, msg.isAI ? AI_LOGO : USER_LOGO);

    // 绘制气泡
    tft.fillRoundRect(
        msg.isAI ? (bubbleX + ARROW_WIDTH) : bubbleX,
        baseY,
        bubbleContentWidth,
        msg.height,
        BUBBLE_RADIUS,
        msg.isAI ? 0xE71C : 0xBDF7
    );

    // 绘制箭头
    if(msg.isAI) {
        tft.fillTriangle(
            bubbleX + ARROW_WIDTH, arrowY,
            bubbleX + ARROW_WIDTH, arrowY + ARROW_WIDTH,
            bubbleX, arrowY + ARROW_WIDTH/2,
            0xE71C
        );
    } else {
        tft.fillTriangle(
            arrowX, arrowY,
            arrowX, arrowY + ARROW_WIDTH,
            arrowX + ARROW_WIDTH, arrowY + ARROW_WIDTH/2,
            0xBDF7
        );
    }

    // 绘制完整文本
    drawWrappedText(
        msg.text, 
        msg.isAI ? (bubbleX + ARROW_WIDTH + TEXT_MARGIN) : (bubbleX + TEXT_MARGIN),
        baseY + TEXT_MARGIN,
        BUBBLE_CONTENT_WIDTH - TEXT_MARGIN*2,
        msg.isAI ? TFT_BLACK : TFT_NAVY
    );
}

int ChatDialog::calculateTextHeight(String text, int maxWidth) {
    tft.setTextSize(TEXT_SIZE);
    int charWidth = CHAR_WIDTH;
    int spaceWidth = charWidth;
    int lines = 1;
    int currentLineWidth = 0;
    int currentWordWidth = 0;

    for (unsigned int i = 0; i < text.length(); i++) {
        char c = text[i];
        
        if (c == ' ' || c == '\n') {
            if (currentWordWidth > 0) {
                if (currentLineWidth + currentWordWidth > maxWidth) {
                    lines++;
                    currentLineWidth = currentWordWidth;
                } else {
                    currentLineWidth += currentWordWidth;
                }
                currentWordWidth = 0;
            }
            
            if (c == '\n') {
                lines++;
                currentLineWidth = 0;
            } else if (currentLineWidth + spaceWidth > maxWidth) {
                lines++;
                currentLineWidth = spaceWidth;
            } else {
                currentLineWidth += spaceWidth;
            }
        } else {
            currentWordWidth += charWidth;
            if (currentWordWidth > maxWidth) {
                lines++;
                currentLineWidth = currentWordWidth - charWidth;
                currentWordWidth = charWidth;
            }
        }
    }

    if (currentWordWidth > 0) {
        if (currentLineWidth + currentWordWidth > maxWidth) lines++;
    }

    return lines * LINE_HEIGHT;
}

void ChatDialog::drawWrappedText(String text, int x, int y, int maxWidth, uint16_t color) {
    tft.setTextColor(color);
    tft.setTextSize(TEXT_SIZE);
    
    int charWidth = CHAR_WIDTH;
    int spaceWidth = charWidth;
    int cursorX = x;
    int cursorY = y;
    int currentLineWidth = 0;
    String currentWord = "";
    int currentWordWidth = 0;

    for (unsigned int i = 0; i < text.length(); i++) {
        char c = text[i];
        
        if (c == ' ' || c == '\n') {
            if (currentWord.length() > 0) {
                if (currentLineWidth + currentWordWidth > maxWidth) {
                    cursorY += LINE_HEIGHT;
                    cursorX = x;
                    currentLineWidth = 0;
                }
                tft.setCursor(cursorX, cursorY);
                tft.print(currentWord);
                cursorX += currentWordWidth;
                currentLineWidth += currentWordWidth;
                currentWord = "";
                currentWordWidth = 0;
            }
            
            if (c == '\n') {
                cursorY += LINE_HEIGHT;
                cursorX = x;
                currentLineWidth = 0;
            } else {
                if (currentLineWidth + spaceWidth > maxWidth) {
                    cursorY += LINE_HEIGHT;
                    cursorX = x;
                    currentLineWidth = 0;
                }
                tft.drawChar(' ', cursorX, cursorY);
                cursorX += spaceWidth;
                currentLineWidth += spaceWidth;
            }
        } else {
            currentWord += c;
            currentWordWidth += charWidth;
            
            if (currentWordWidth > maxWidth) {
                int availableWidth = maxWidth - currentLineWidth;
                int charsToDraw = availableWidth / charWidth;
                
                if (charsToDraw > 0) {
                    tft.setCursor(cursorX, cursorY);
                    tft.print(currentWord.substring(0, charsToDraw));
                    cursorX += charsToDraw * charWidth;
                    currentLineWidth += charsToDraw * charWidth;
                }
                
                cursorY += LINE_HEIGHT;
                cursorX = x;
                currentWord = currentWord.substring(charsToDraw);
                currentWordWidth = currentWord.length() * charWidth;
                currentLineWidth = 0;
            }
        }
    }

    if (currentWord.length() > 0) {
        if (currentLineWidth + currentWordWidth > maxWidth) {
            cursorY += LINE_HEIGHT;
            cursorX = x;
        }
        tft.setCursor(cursorX, cursorY);
        tft.print(currentWord);
    }
}