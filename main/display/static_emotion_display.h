#pragma once

#include "lcd_display.h"
#include "static_emotions/static_emotions.h"

// 定义宏来控制是否显示文本
#define ENABLE_TEXT_DISPLAY 0  // 设置为0关闭文本显示，1开启文本显示

/**
 * @brief 静态图片表情显示类
 * 继承SpiLcdDisplay，添加静态图片表情支持
 */
class StaticEmotionDisplay : public SpiLcdDisplay {
public:
    /**
     * @brief 构造函数，参数与SpiLcdDisplay相同
     */
    StaticEmotionDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel,
                         int width, int height, int offset_x, int offset_y, bool mirror_x,
                         bool mirror_y, bool swap_xy, DisplayFonts fonts);

    virtual ~StaticEmotionDisplay() = default;

    // 重写表情设置方法
    virtual void SetEmotion(const char* emotion) override;

    // 重写聊天消息设置方法
    virtual void SetChatMessage(const char* role, const char* content) override;

    // 重写图标设置方法
    virtual void SetIcon(const char* icon) override;

private:
    void SetupImageContainer();

    lv_obj_t* emotion_image_;  ///< 静态图片表情组件

    // 表情映射
    struct EmotionMap {
        const char* name;
        const lv_image_dsc_t* image;
    };

    static const EmotionMap emotion_maps_[];
};