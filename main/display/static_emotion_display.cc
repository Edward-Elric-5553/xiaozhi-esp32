#include "static_emotion_display.h"
#include <esp_log.h>
#include <cstring>

static const char* TAG = "StaticEmotionDisplay";

// 表情映射数组
const StaticEmotionDisplay::EmotionMap StaticEmotionDisplay::emotion_maps_[] = {
    {"neutral", &neutral_360},
    {"happy", &happy_360},
    {"sad", &sad_360},
    {"angry", &angry_360},
    {"surprised", &surprised_360},
    {"thinking", &thinking_360},
    {"laughing", &happy_360},
    {"funny", &happy_360},
    {"loving", &happy_360},
    {"embarrassed", &happy_360},
    {"confident", &happy_360},
    {"delicious", &happy_360},
    {"crying", &sad_360},
    {"sleepy", &sad_360},
    {"silly", &happy_360},
    {"shocked", &surprised_360},
    {"winking", &happy_360},
    {"relaxed", &neutral_360},
    {"confused", &thinking_360},
    {"idle", &neutral_360},
    {nullptr, nullptr}  // 结束标记
};

StaticEmotionDisplay::StaticEmotionDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel,
                                           int width, int height, int offset_x, int offset_y, bool mirror_x,
                                           bool mirror_y, bool swap_xy, DisplayFonts fonts)
    : SpiLcdDisplay(panel_io, panel, width, height, offset_x, offset_y, mirror_x, mirror_y, swap_xy, fonts),
      emotion_image_(nullptr) {
    SetupImageContainer();
}

void StaticEmotionDisplay::SetupImageContainer() {
    DisplayLockGuard lock(this);

    // 清理现有组件
    if (emotion_label_) {
        lv_obj_del(emotion_label_);
        emotion_label_ = nullptr;
    }
    if (chat_message_label_) {
        lv_obj_del(chat_message_label_);
        chat_message_label_ = nullptr;
    }
    if (content_) {
        lv_obj_del(content_);
    }

    // 创建主容器
    content_ = lv_obj_create(container_);
    lv_obj_set_scrollbar_mode(content_, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(content_, width_, height_);
    lv_obj_set_style_bg_opa(content_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(content_, 0, 0);
    lv_obj_set_style_pad_all(content_, 0, 0);
    lv_obj_center(content_);

    // 创建静态图片表情组件
    emotion_image_ = lv_img_create(content_);
    lv_obj_set_size(emotion_image_, width_, height_);
    lv_obj_set_style_border_width(emotion_image_, 0, 0);
    lv_obj_set_style_bg_opa(emotion_image_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(emotion_image_, 0, 0);
    lv_obj_center(emotion_image_);
    lv_img_set_src(emotion_image_, &neutral_360);  // 默认中性表情

#if ENABLE_TEXT_DISPLAY
    // 创建聊天消息标签
    chat_message_label_ = lv_label_create(content_);
    lv_label_set_text(chat_message_label_, "");
    lv_obj_set_width(chat_message_label_, width_ * 0.9);
    lv_label_set_long_mode(chat_message_label_, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_align(chat_message_label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(chat_message_label_, lv_color_white(), 0);
    lv_obj_set_style_border_width(chat_message_label_, 0, 0);
    lv_obj_set_style_bg_opa(chat_message_label_, LV_OPA_70, 0);
    lv_obj_set_style_bg_color(chat_message_label_, lv_color_black(), 0);
    lv_obj_set_style_pad_ver(chat_message_label_, 5, 0);
    lv_obj_align(chat_message_label_, LV_ALIGN_BOTTOM_MID, 0, 0);
#else
    chat_message_label_ = nullptr;
#endif

    LcdDisplay::SetTheme("dark");
}

void StaticEmotionDisplay::SetEmotion(const char* emotion) {
    if (!emotion || !emotion_image_) {
        return;
    }

    DisplayLockGuard lock(this);

    for (const auto& map : emotion_maps_) {
        if (map.name && strcmp(map.name, emotion) == 0) {
            lv_img_set_src(emotion_image_, map.image);
            ESP_LOGI(TAG, "设置表情: %s", emotion);
            return;
        }
    }

    lv_img_set_src(emotion_image_, &neutral_360);
    ESP_LOGI(TAG, "未知表情'%s'，使用默认", emotion);
}

void StaticEmotionDisplay::SetChatMessage(const char* role, const char* content) {
#if ENABLE_TEXT_DISPLAY
    if (!chat_message_label_) {
        return;
    }

    DisplayLockGuard lock(this);

    if (content && strlen(content) > 0) {
        lv_label_set_text(chat_message_label_, content);
        lv_obj_clear_flag(chat_message_label_, LV_OBJ_FLAG_HIDDEN);
        ESP_LOGI(TAG, "设置聊天消息: %s", content);
    } else {
        lv_obj_add_flag(chat_message_label_, LV_OBJ_FLAG_HIDDEN);
    }
#else
    // 文本显示被禁用，直接返回
    ESP_LOGD(TAG, "文本显示已禁用，忽略聊天消息设置");
#endif
}

void StaticEmotionDisplay::SetIcon(const char* icon) {
    if (!icon) {
        return;
    }

    std::string icon_message = "图标: ";
    
    if (strcmp(icon, "wifi_connected") == 0) {
        icon_message += "WiFi已连接";
        SetEmotion("happy");
    } else if (strcmp(icon, "wifi_disconnected") == 0) {
        icon_message += "WiFi断开";
        SetEmotion("sad");
    } else if (strcmp(icon, "microphone_muted") == 0) {
        icon_message += "麦克风静音";
        SetEmotion("neutral");
    } else if (strcmp(icon, "microphone_unmuted") == 0) {
        icon_message += "麦克风开启";
        SetEmotion("happy");
    } else if (strcmp(icon, "battery_low") == 0) {
        icon_message += "电量不足";
        SetEmotion("sad");
    } else {
        icon_message += "系统状态";
        SetEmotion("neutral");
    }

    SetChatMessage("system", icon_message.c_str());
    ESP_LOGI(TAG, "设置图标: %s", icon);
}