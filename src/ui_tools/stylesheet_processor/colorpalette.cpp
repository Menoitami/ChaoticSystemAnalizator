#include "colorpalette.h"

const QHash<QString, QString> ColorPalette::palette = {

    // Foreground
    {"fg.primary", "#262D38"},
    {"fg.secondary", "#5D6F92"},
    {"fg.disabled", "#CDD2DB"},
    {"fg.caption", "#A7B5D0"},

    // Background
    {"bg.page", "#ECEDF2"},
    {"bg.block", "#F1F1F1"},
    {"bg.disabled", "#ECEDF2"},

    // Accent
    {"accent.default", "#4882EE"},
    {"accent.hover", "#4074D4"},
    {"accent.active", "#3662B3"},
    {"accent.on-accent", "#F1F1F1"},

    // Border
    {"border.default", "#CDD2DB"},
    {"border.disabled", "#8B95A7"},

    // Success
    {"success.default", "#4BB543"},
    {"success.hover", "#3D7C38"},
    {"success.active", "#4BA344"},
    {"success.container-default", "rgba(75, 181, 67, 0.1)"},
    {"success.container-hover", "rgba(75, 181, 67, 0.2)"},
    {"success.conatiner-active", "rgba(75, 181, 67, 0.3)"},
    {"success.on-conatiner", "#4BB543"},

    // Warinig
    {"warning.default", "#FED44D"},
    {"warning.hover", "#EDC648"},
    {"warning.active", "#D4B242"},
    {"warning.container-default", "rgba(254, 212, 77, 0.1)"},
    {"warning.container-hover", "rgba(254, 212, 77, 0.2)"},
    {"warning.conatiner-active", "rgba(254, 212, 77, 0.3)"},
    {"warning.on-conatiner", "#FED44D"},

    // Error
    {"error.default", "#FF4D4F"},
    {"error.hover", "#DE4547"},
    {"error.active", "#B43738"},
    {"error.container-default", "rgba(255, 77, 79, 0.1)"},
    {"error.container-hover", "rgba(255, 77, 79, 0.2)"},
    {"error.conatiner-active", "rgba(255, 77, 79, 0.3)"},
    {"error.on-conatiner", "#FF4D4F"},

    // Tokens
    //// Accent Blue
    {"accent_100", "#142546"},
    {"accent_090", "#22427D"},
    {"accent_080", "#3662B3"},
    {"accent_070", "#4074D4"},
    {"accent_060", "#4882EE"},
    {"accent_050", "#70A0FA"},
    {"accent_040", "#93B8FB"},
    {"accent_030", "#BCCEF2"},
    {"accent_020", "#CDDEFF"},
    {"accent_010", "#E9F1FF"},
    {"accent_005", "#E7EBF5"}, 
    {"accent_000", "#FFFFFF"},

    //// Neutral
    {"neutral_100", "#1A1F27"},
    {"neutral_090", "#262D38"},
    {"neutral_080", "#384357"},
    {"neutral_070", "#50607E"},
    {"neutral_060", "#6A7384"},
    {"neutral_050", "#8B95A7"},
    {"neutral_040", "#AFBBD0"},
    {"neutral_030", "#B4BAC4"},
    {"neutral_020", "#CDD2DB"},
    {"neutral_010", "#E5E5E5"},
    {"neutral_005", "#ECEDF2"},
    {"neutral_005_op50", "#80ECEDF2"},
    {"neutral_000", "#FFFFFF"},

    //// Success
    {"success_100", "#1B3519"},
    {"success_090", "#254E22"},
    {"success_080", "#3D7C38"},
    {"success_070", "#4BA344"},
    {"success_060", "#4BB543"},
    {"success_050", "#5EC257"},
    {"success_040", "#7CC377"},
    {"success_030", "#9ECE9A"},
    {"success_020", "#B1CDAF"},
    {"success_010", "#C4DEC2"},
    {"success_005", "#E1F2E0"},
    {"success_000", "#FFFFFF"},

    //// Warning
    {"warning_100", "#433918"},
    {"warning_090", "#89732A"},
    {"warning_080", "#D4B242"},
    {"warning_070", "#EDC648"},
    {"warning_060", "#FED44D"},
    {"warning_050", "#FFDE70"},
    {"warning_040", "#FFE696"},
    {"warning_030", "#FFEBA9"},
    {"warning_020", "#FFEFBA"},
    {"warning_010", "#FEF2CC"},
    {"warning_005", "#FEF9E8"},
    {"warning_000", "#FFFFFF"},

    //// Error
    {"error_100", "#481818"},
    {"error_090", "#6B2323"},
    {"error_080", "#B43738"},
    {"error_070", "#DE4547"},
    {"error_060", "#FF4D4F"},
    {"error_050", "#FB6466"},
    {"error_040", "#FF8385"},
    {"error_030", "#FF9D9E"},
    {"error_020", "#FFB5B6"},
    {"error_010", "#FFCECF"},
    {"error_005", "#FFE8E8"},
    {"error_000", "#FFFFFF"},

};
