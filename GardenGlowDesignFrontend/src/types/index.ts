export enum EffectType {
  rainbow = "rainbow",
  ripple = "ripple",
  pumpkinRainbow = "pumpkinRainbow",
  pumpkinRipple = "pumpkinRipple",
  ghostRainbow = "ghostRainbow",
  ghostRipple = "ghostRipple",
  pumpkinGhostRainbow = "pumpkinGhostRainbow",
  pumpkinGhostRipple = "pumpkinGhostRipple",
  snowflake = "snowflake",
  snowman = "snowman",
  christmasTree = "christmasTree",
  candyCane = "candyCane",
}
 

export const effectConfig = {
  rainbow: {
    fields: [{ name: "speed", label: "Rainbow Speed", type: "number" }],
  },
  ripple: {
    fields: [{ name: "speed", label: "Ripple Speed", type: "number" }],
  },
  "pumpkin-rainbow": {
    fields: [
      { name: "speed", label: "Pumpkin Rainbow Speed", type: "number" },
      { name: "color", label: "Primary Color", type: "text" },
    ],
  },
  snowflake: {
    fields: [{ name: "flakeSize", label: "Flake Size", type: "number" }],
  },
  snowman: {
    fields: [{ name: "height", label: "Snowman Height", type: "number" }],
  },
  christmasTree: {
    fields: [
      { name: "height", label: "Tree Height", type: "number" },
      { name: "lights", label: "Number of Lights", type: "number" },
    ],
  },
  candyCane: {
    fields: [{ name: "stripes", label: "Stripe Count", type: "number" }],
  },
} as const;
