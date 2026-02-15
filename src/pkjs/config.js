// Clay Config: see https://github.com/pebble/clay
module.exports = [
  {
    "type": "heading",
    "defaultValue": "Settings"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Theme settings"
      },
      {
        "type": "toggle",
        "messageKey": "AddZero12h",
        "label": "Add leading zero to 12h time",
        "description": "Applies when 12h time selected in watch settings",
        "defaultValue": false
      },
      {
        "type": "toggle",
        "messageKey": "RemoveZero24h",
        "label": "Remove leading zero from 24h time",
        "description": "Applies when 24h time selected in watch settings",
        "defaultValue": false
      },
      {
        "type": "toggle",
        "messageKey": "BTVibeOn",
        "label": "Bluetooth Disconnect Vibe",
        "description": "OFF = Never Vibrate, ON = Vibrate (when quiet time is off)",
        "defaultValue": true
      },
      {
        "type": "color",
        "messageKey": "Back1Color",
        "defaultValue": "0x000000",
        "label": "Background Colour"
      },
      {
        "type": "color",
        "messageKey": "HourColor",
        "defaultValue": "0xFFFFFF",
        "label": "Hour Number Colour"
      }, 
      {
        "type": "color",
        "messageKey": "MinColor",
        "defaultValue": "0xFFFFFF",
        "label": "Minute Number Colour"
      },
      {
        "type": "color",
        "messageKey": "Text1Color",
        "defaultValue": "0xFFFFFF",
        "label": "AM/PM Text Colour"
      }, 
      {
        "type": "color",
        "messageKey": "Text3Color",
        "defaultValue": "0xFFFFFF",
        "label": "Day of the week Text Colour"
      },
      {
        "type": "color",
        "messageKey": "FrameColor1",
        "defaultValue": "0x0055AA",
        "label": "Date Background Colour"
      },
      {
        "type": "color",
        "messageKey": "Text6Color",
        "defaultValue": "0xFFFFFF",
        "label": "Date Number Colour"
      },
      {
        "type": "color",
        "messageKey": "Text5Color",
        "defaultValue": "0xFFFFFF",
        "label": "Month Text Colour"
      }, 
      {
        "type": "color",
        "messageKey": "Text2Color",
        "defaultValue": "0xFFFFFF",
        "label": "Battery Percentage Colour"
      },

          {
        "type": "color",
        "messageKey": "Text4Color",
        "defaultValue": "0xFFFFFF",
        "label": "Quiet Time & BT icon colour"
      }, 
          {
          "type": "submit",
          "defaultValue":"SAVE"
          },
          {
          "type": "heading",
          "defaultValue": "version v2.0",
          "size":6
          },
          {
          "type": "heading",
          "defaultValue": "Made in UK",
          "size":6
          }
       ]
      },
      ];