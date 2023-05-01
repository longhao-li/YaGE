#pragma once

#include "../Core/Common.h"

#include <cstdint>

namespace YaGE {

struct Color {
    float red;
    float green;
    float blue;
    float alpha;

    /// @brief
    ///   Create an transparent black color. All values are set to 0.
    constexpr Color() noexcept : red(), green(), blue(), alpha() {}

    /// @brief
    ///   Create a color with the given values.
    ///
    /// @param r        The red value.
    /// @param g        The green value.
    /// @param b        The blue value.
    /// @param alpha    Opacity of this color. Default is 1.0.
    constexpr Color(float r, float g, float b, float alpha = 1.0f) noexcept : red(r), green(g), blue(b), alpha(alpha) {}

    /// @brief
    ///   Create a color from RGBA 8 values.
    ///
    /// @param r        The red value.
    /// @param g        The green value.
    /// @param b        The blue value.
    /// @param alpha    Opacity of the RGBA8 color. Default is 255.
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha = 255U) noexcept
        : red(static_cast<float>(r) / 255.0f),
          green(static_cast<float>(g) / 255.0f),
          blue(static_cast<float>(b) / 255.0f),
          alpha(static_cast<float>(alpha) / 255.0f) {}

    YAGE_NODISCARD static constexpr auto Snow() noexcept -> Color {
        return {1.0f, 0.9803921568627451f, 0.9803921568627451f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto GhostWhite() noexcept -> Color {
        return {0.9725490196078431f, 0.9725490196078431f, 1.0f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto WhiteSmoke() noexcept -> Color {
        return {0.9607843137254902f, 0.9607843137254902f, 0.9607843137254902f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Gainsboro() noexcept -> Color {
        return {0.8627450980392157f, 0.8627450980392157f, 0.8627450980392157f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto FloralWhite() noexcept -> Color {
        return {1.0f, 0.9803921568627451f, 0.9411764705882353f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto OldLace() noexcept -> Color {
        return {0.9921568627450981f, 0.9607843137254902f, 0.9019607843137255f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Linen() noexcept -> Color {
        return {0.9803921568627451f, 0.9411764705882353f, 0.9019607843137255f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto AntiqueWhite() noexcept -> Color {
        return {0.9803921568627451f, 0.9215686274509803f, 0.8431372549019608f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto PapayaWhip() noexcept -> Color {
        return {1.0f, 0.9372549019607843f, 0.8352941176470589f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto BlanchedAlmond() noexcept -> Color {
        return {1.0f, 0.9215686274509803f, 0.803921568627451f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Bisque() noexcept -> Color {
        return {1.0f, 0.8941176470588236f, 0.7686274509803922f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto PeachPuff() noexcept -> Color {
        return {1.0f, 0.8549019607843137f, 0.7254901960784313f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto NavajoWhite() noexcept -> Color {
        return {1.0f, 0.8705882352941177f, 0.6784313725490196f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Moccasin() noexcept -> Color {
        return {1.0f, 0.8941176470588236f, 0.7098039215686275f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Cornsilk() noexcept -> Color {
        return {1.0f, 0.9725490196078431f, 0.8627450980392157f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Ivory() noexcept -> Color { return {1.0f, 1.0f, 0.9411764705882353f, 1.0f}; }

    YAGE_NODISCARD static constexpr auto LemonChiffon() noexcept -> Color {
        return {1.0f, 0.9803921568627451f, 0.803921568627451f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Seashell() noexcept -> Color {
        return {1.0f, 0.9607843137254902f, 0.9333333333333333f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Honeydew() noexcept -> Color {
        return {0.9411764705882353f, 1.0f, 0.9411764705882353f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto MintCream() noexcept -> Color {
        return {0.9607843137254902f, 1.0f, 0.9803921568627451f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Azure() noexcept -> Color { return {0.9411764705882353f, 1.0f, 1.0f, 1.0f}; }

    YAGE_NODISCARD static constexpr auto AliceBlue() noexcept -> Color {
        return {0.9411764705882353f, 0.9725490196078431f, 1.0f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Lavender() noexcept -> Color {
        return {0.9019607843137255f, 0.9019607843137255f, 0.9803921568627451f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto LavenderBlush() noexcept -> Color {
        return {1.0f, 0.9411764705882353f, 0.9607843137254902f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto MistyRose() noexcept -> Color {
        return {1.0f, 0.8941176470588236f, 0.8823529411764706f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto White() noexcept -> Color { return {1.0f, 1.0f, 1.0f, 1.0f}; }

    YAGE_NODISCARD static constexpr auto Black() noexcept -> Color { return {0.0f, 0.0f, 0.0f, 1.0f}; }

    YAGE_NODISCARD static constexpr auto DarkSlateGray() noexcept -> Color {
        return {0.1843137254901961f, 0.30980392156862746f, 0.30980392156862746f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto DimGray() noexcept -> Color {
        return {0.4117647058823529f, 0.4117647058823529f, 0.4117647058823529f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto SlateGray() noexcept -> Color {
        return {0.4392156862745098f, 0.5019607843137255f, 0.5647058823529412f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto LightSlateGray() noexcept -> Color {
        return {0.4666666666666667f, 0.5333333333333333f, 0.6f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Gray() noexcept -> Color {
        return {0.5019607843137255f, 0.5019607843137255f, 0.5019607843137255f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto LightGray() noexcept -> Color {
        return {0.8274509803921568f, 0.8274509803921568f, 0.8274509803921568f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto MidnightBlue() noexcept -> Color {
        return {0.09803921568627451f, 0.09803921568627451f, 0.4392156862745098f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Navy() noexcept -> Color { return {0.0f, 0.0f, 0.5019607843137255f, 1.0f}; }

    YAGE_NODISCARD static constexpr auto CornflowerBlue() noexcept -> Color {
        return {0.39215686274509803f, 0.5843137254901961f, 0.9294117647058824f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto DarkSlateBlue() noexcept -> Color {
        return {0.2823529411764706f, 0.23921568627450981f, 0.5450980392156862f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto SlateBlue() noexcept -> Color {
        return {0.41568627450980394f, 0.35294117647058826f, 0.803921568627451f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto MediumSlateBlue() noexcept -> Color {
        return {0.4823529411764706f, 0.40784313725490196f, 0.9333333333333333f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto LightSlateBlue() noexcept -> Color {
        return {0.5176470588235295f, 0.4392156862745098f, 1.0f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto MediumBlue() noexcept -> Color {
        return {0.0f, 0.0f, 0.803921568627451f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto RoyalBlue() noexcept -> Color {
        return {0.2549019607843137f, 0.4117647058823529f, 0.8823529411764706f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Blue() noexcept -> Color { return {0.0f, 0.0f, 1.0f, 1.0f}; }

    YAGE_NODISCARD static constexpr auto DodgerBlue() noexcept -> Color {
        return {0.11764705882352941f, 0.5647058823529412f, 1.0f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto DeepSkyBlue() noexcept -> Color {
        return {0.0f, 0.7490196078431373f, 1.0f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto SkyBlue() noexcept -> Color {
        return {0.5294117647058824f, 0.807843137254902f, 0.9215686274509803f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto LightSkyBlue() noexcept -> Color {
        return {0.5294117647058824f, 0.807843137254902f, 0.9803921568627451f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto SteelBlue() noexcept -> Color {
        return {0.27450980392156865f, 0.5098039215686274f, 0.7058823529411765f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto LightSteelBlue() noexcept -> Color {
        return {0.6901960784313725f, 0.7686274509803922f, 0.8705882352941177f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto LightBlue() noexcept -> Color {
        return {0.6784313725490196f, 0.8470588235294118f, 0.9019607843137255f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto PowderBlue() noexcept -> Color {
        return {0.6901960784313725f, 0.8784313725490196f, 0.9019607843137255f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto PaleTurquoise() noexcept -> Color {
        return {0.6862745098039216f, 0.9333333333333333f, 0.9333333333333333f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto DarkTurquoise() noexcept -> Color {
        return {0.0f, 0.807843137254902f, 0.8196078431372549f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto MediumTurquoise() noexcept -> Color {
        return {0.2823529411764706f, 0.8196078431372549f, 0.8f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Turquoise() noexcept -> Color {
        return {0.25098039215686274f, 0.8784313725490196f, 0.8156862745098039f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Cyan() noexcept -> Color { return {0.0f, 1.0f, 1.0f, 1.0f}; }

    YAGE_NODISCARD static constexpr auto LightCyan() noexcept -> Color {
        return {0.8784313725490196f, 1.0f, 1.0f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto CadetBlue() noexcept -> Color {
        return {0.37254901960784315f, 0.6196078431372549f, 0.6274509803921569f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto MediumAquamarine() noexcept -> Color {
        return {0.4f, 0.803921568627451f, 0.6666666666666666f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Aquamarine() noexcept -> Color {
        return {0.4980392156862745f, 1.0f, 0.8313725490196079f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto DarkGreen() noexcept -> Color {
        return {0.0f, 0.39215686274509803f, 0.0f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto DarkOliveGreen() noexcept -> Color {
        return {0.3333333333333333f, 0.4196078431372549f, 0.1843137254901961f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto DarkSeaGreen() noexcept -> Color {
        return {0.5607843137254902f, 0.7372549019607844f, 0.5607843137254902f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto SeaGreen() noexcept -> Color {
        return {0.1803921568627451f, 0.5450980392156862f, 0.3411764705882353f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto MediumSeaGreen() noexcept -> Color {
        return {0.23529411764705882f, 0.7019607843137254f, 0.44313725490196076f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto LightSeaGreen() noexcept -> Color {
        return {0.12549019607843137f, 0.6980392156862745f, 0.6666666666666666f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto PaleGreen() noexcept -> Color {
        return {0.596078431372549f, 0.984313725490196f, 0.596078431372549f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto SpringGreen() noexcept -> Color {
        return {0.0f, 1.0f, 0.4980392156862745f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto LawnGreen() noexcept -> Color {
        return {0.48627450980392156f, 0.9882352941176471f, 0.0f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Chartreuse() noexcept -> Color {
        return {0.4980392156862745f, 1.0f, 0.0f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto MediumSpringGreen() noexcept -> Color {
        return {0.0f, 0.9803921568627451f, 0.6039215686274509f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto GreenYellow() noexcept -> Color {
        return {0.6784313725490196f, 1.0f, 0.1843137254901961f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto LimeGreen() noexcept -> Color {
        return {0.19607843137254902f, 0.803921568627451f, 0.19607843137254902f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto YellowGreen() noexcept -> Color {
        return {0.6039215686274509f, 0.803921568627451f, 0.19607843137254902f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto ForestGreen() noexcept -> Color {
        return {0.13333333333333333f, 0.5450980392156862f, 0.13333333333333333f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto OliveDrab() noexcept -> Color {
        return {0.4196078431372549f, 0.5568627450980392f, 0.13725490196078433f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto DarkKhaki() noexcept -> Color {
        return {0.7411764705882353f, 0.7176470588235294f, 0.4196078431372549f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Khaki() noexcept -> Color {
        return {0.9411764705882353f, 0.9019607843137255f, 0.5490196078431373f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto PaleGoldenrod() noexcept -> Color {
        return {0.9333333333333333f, 0.9098039215686274f, 0.6666666666666666f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto LightGoldenrodYellow() noexcept -> Color {
        return {0.9803921568627451f, 0.9803921568627451f, 0.8235294117647058f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto LightYellow() noexcept -> Color {
        return {1.0f, 1.0f, 0.8784313725490196f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Yellow() noexcept -> Color { return {1.0f, 1.0f, 0.0f, 1.0f}; }

    YAGE_NODISCARD static constexpr auto Gold() noexcept -> Color { return {1.0f, 0.8431372549019608f, 0.0f, 1.0f}; }

    YAGE_NODISCARD static constexpr auto LightGoldenrod() noexcept -> Color {
        return {0.9333333333333333f, 0.8666666666666667f, 0.5098039215686274f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Goldenrod() noexcept -> Color {
        return {0.8549019607843137f, 0.6470588235294118f, 0.12549019607843137f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto DarkGoldenrod() noexcept -> Color {
        return {0.7215686274509804f, 0.5254901960784314f, 0.043137254901960784f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto RosyBrown() noexcept -> Color {
        return {0.7372549019607844f, 0.5607843137254902f, 0.5607843137254902f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto IndianRed() noexcept -> Color {
        return {0.803921568627451f, 0.3607843137254902f, 0.3607843137254902f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto SaddleBrown() noexcept -> Color {
        return {0.5450980392156862f, 0.27058823529411763f, 0.07450980392156863f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Sienna() noexcept -> Color {
        return {0.6274509803921569f, 0.3215686274509804f, 0.17647058823529413f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Peru() noexcept -> Color {
        return {0.803921568627451f, 0.5215686274509804f, 0.24705882352941178f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Burlywood() noexcept -> Color {
        return {0.8705882352941177f, 0.7215686274509804f, 0.5294117647058824f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Beige() noexcept -> Color {
        return {0.9607843137254902f, 0.9607843137254902f, 0.8627450980392157f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Wheat() noexcept -> Color {
        return {0.9607843137254902f, 0.8705882352941177f, 0.7019607843137254f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto SandyBrown() noexcept -> Color {
        return {0.9568627450980393f, 0.6431372549019608f, 0.3764705882352941f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Tan() noexcept -> Color {
        return {0.8235294117647058f, 0.7058823529411765f, 0.5490196078431373f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Chocolate() noexcept -> Color {
        return {0.8235294117647058f, 0.4117647058823529f, 0.11764705882352941f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Firebrick() noexcept -> Color {
        return {0.6980392156862745f, 0.13333333333333333f, 0.13333333333333333f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Brown() noexcept -> Color {
        return {0.6470588235294118f, 0.16470588235294117f, 0.16470588235294117f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto DarkSalmon() noexcept -> Color {
        return {0.9137254901960784f, 0.5882352941176471f, 0.47843137254901963f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Salmon() noexcept -> Color {
        return {0.9803921568627451f, 0.5019607843137255f, 0.4470588235294118f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto LightSalmon() noexcept -> Color {
        return {1.0f, 0.6274509803921569f, 0.47843137254901963f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Orange() noexcept -> Color { return {1.0f, 0.6470588235294118f, 0.0f, 1.0f}; }

    YAGE_NODISCARD static constexpr auto DarkOrange() noexcept -> Color {
        return {1.0f, 0.5490196078431373f, 0.0f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Coral() noexcept -> Color {
        return {1.0f, 0.4980392156862745f, 0.3137254901960784f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto LightCoral() noexcept -> Color {
        return {0.9411764705882353f, 0.5019607843137255f, 0.5019607843137255f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Tomato() noexcept -> Color {
        return {1.0f, 0.38823529411764707f, 0.2784313725490196f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto OrangeRed() noexcept -> Color {
        return {1.0f, 0.27058823529411763f, 0.0f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Red() noexcept -> Color { return {1.0f, 0.0f, 0.0f, 1.0f}; }

    YAGE_NODISCARD static constexpr auto HotPink() noexcept -> Color {
        return {1.0f, 0.4117647058823529f, 0.7058823529411765f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto DeepPink() noexcept -> Color {
        return {1.0f, 0.0784313725490196f, 0.5764705882352941f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Pink() noexcept -> Color {
        return {1.0f, 0.7529411764705882f, 0.796078431372549f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto LightPink() noexcept -> Color {
        return {1.0f, 0.7137254901960784f, 0.7568627450980392f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto PaleVioletRed() noexcept -> Color {
        return {0.8588235294117647f, 0.4392156862745098f, 0.5764705882352941f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Maroon() noexcept -> Color {
        return {0.6901960784313725f, 0.18823529411764706f, 0.3764705882352941f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto MediumVioletRed() noexcept -> Color {
        return {0.7803921568627451f, 0.08235294117647059f, 0.5215686274509804f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto VioletRed() noexcept -> Color {
        return {0.8156862745098039f, 0.12549019607843137f, 0.5647058823529412f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Magenta() noexcept -> Color { return {1.0f, 0.0f, 1.0f, 1.0f}; }

    YAGE_NODISCARD static constexpr auto Violet() noexcept -> Color {
        return {0.9333333333333333f, 0.5098039215686274f, 0.9333333333333333f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Plum() noexcept -> Color {
        return {0.8666666666666667f, 0.6274509803921569f, 0.8666666666666667f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Orchid() noexcept -> Color {
        return {0.8549019607843137f, 0.4392156862745098f, 0.8392156862745098f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto MediumOrchid() noexcept -> Color {
        return {0.7294117647058823f, 0.3333333333333333f, 0.8274509803921568f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto DarkOrchid() noexcept -> Color {
        return {0.6f, 0.19607843137254902f, 0.8f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto DarkViolet() noexcept -> Color {
        return {0.5803921568627451f, 0.0f, 0.8274509803921568f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto BlueViolet() noexcept -> Color {
        return {0.5411764705882353f, 0.16862745098039217f, 0.8862745098039215f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Purple() noexcept -> Color {
        return {0.6274509803921569f, 0.12549019607843137f, 0.9411764705882353f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto MediumPurple() noexcept -> Color {
        return {0.5764705882352941f, 0.4392156862745098f, 0.8588235294117647f, 1.0f};
    }

    YAGE_NODISCARD static constexpr auto Thistle() noexcept -> Color {
        return {0.8470588235294118f, 0.7490196078431373f, 0.8470588235294118f, 1.0f};
    }
};

} // namespace YaGE
