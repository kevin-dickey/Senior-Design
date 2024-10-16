const char *show_json = R"(
{
  "name": "Basic Show File",
  "duration": 10000,
  "effects": [
    {
      "id": 1,
      "name": "Save me!",
      "type": "rainbow",
      "startTimeMs": 10,
      "durationMs": 900,
      "origin": {
        "x": 5,
        "y": 5
      },
      "size": {
        "x": 10,
        "y": 10
      },
      "colors": [
        {
          "r": 255,
          "g": 0,
          "b": 0
        },
        {
          "r": 0,
          "g": 255,
          "b": 0
        },
        {
          "r": 0,
          "g": 0,
          "b": 255
        }
      ],
      "speed": 10
    },
    {
      "id": 2,
      "name": "Crazy Train",
      "type": "rainbow",
      "origin": {
        "x": 5,
        "y": 5
      },
      "size": {
        "x": 10,
        "y": 10
      },
      "startTimeMs": 1000,
      "durationMs": 1000
    }
  ],
  "layouts": [
    {
      "shape": 1,
      "width": 16,
      "height": 16
    }
  ],
  "sensors": [
    {
      "id": 1,
      "type": 1,
      "location": {
        "x": 16,
        "y": 16
      },
      "pin": 34 
    },
    {
      "id": 2,
      "type": 1,
      "location": {
        "x": 0,
        "y": 16 
      },
      "pin": 35
    },
{
      "id": 3,
      "type": 1,
      "location": {
        "x": 0,
        "y": 0 
      },
      "pin": 32
    },
{
      "id": 4,
      "type": 1,
      "location": {
        "x": 16,
        "y": 0
      },
      "pin": 33
    }
  ]
})";