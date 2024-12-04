const numRows = 26/2;
const numCols = 116/2 ;

const pumpkinColor = "#fca63d"; // Orange for the pumpkin
const stemColor = "#012e01"; // Dark green for the stem
const faceColor = "#000000"; // Black for eyes and smile
const pumpkinHeight = Math.floor(numRows / 3);
const centerRow = Math.floor(numRows / 2);
const centerCol = Math.floor(numCols / 2);

const ghostColor = "#ffffff"; // White color for the ghost
const ghostFaceColor = "#000000"; // Black for the ghost face
const ghostCenterRow = Math.floor(numRows / 2); // Center row of the ghost
const ghostCenterCol = Math.floor(numCols / 2); // Center column of the ghost

export const rainbowColors = [
  '#e81416', // Red
  '#e88000', // Orange
  '#faeb36', // Yellow
  '#79c314', // Green
  '#487de7', // Blue
  '#4b369d', // Indigo
  '#70369d', // Violet
];

export const pumpkinShape = [
  // Stem (centered at the top of the pumpkin)
  { row: centerRow - pumpkinHeight, col: centerCol - 1, color: stemColor },
  { row: centerRow - pumpkinHeight, col: centerCol, color: stemColor },
  { row: centerRow - pumpkinHeight, col: centerCol + 1, color: stemColor },

  // Top of pumpkin
  {
    row: centerRow - pumpkinHeight + 1,
    col: centerCol - 2,
    color: pumpkinColor,
  },
  {
    row: centerRow - pumpkinHeight + 1,
    col: centerCol - 1,
    color: pumpkinColor,
  },
  { row: centerRow - pumpkinHeight + 1, col: centerCol, color: pumpkinColor },
  {
    row: centerRow - pumpkinHeight + 1,
    col: centerCol + 1,
    color: pumpkinColor,
  },
  {
    row: centerRow - pumpkinHeight + 1,
    col: centerCol + 2,
    color: pumpkinColor,
  },

  // Second row (wider than the top) & Eyes
  {
    row: centerRow - pumpkinHeight + 2,
    col: centerCol - 3,
    color: pumpkinColor,
  },
  {
    row: centerRow - pumpkinHeight + 2,
    col: centerCol - 2,
    color: pumpkinColor,
  },
  { row: centerRow - pumpkinHeight + 2, col: centerCol - 1, color: faceColor },
  { row: centerRow - pumpkinHeight + 2, col: centerCol, color: pumpkinColor },
  { row: centerRow - pumpkinHeight + 2, col: centerCol + 1, color: faceColor },
  {
    row: centerRow - pumpkinHeight + 2,
    col: centerCol + 2,
    color: pumpkinColor,
  },
  {
    row: centerRow - pumpkinHeight + 2,
    col: centerCol + 3,
    color: pumpkinColor,
  },

  // Middle rows
  {
    row: centerRow - pumpkinHeight + 3,
    col: centerCol - 4,
    color: pumpkinColor,
  },
  {
    row: centerRow - pumpkinHeight + 3,
    col: centerCol - 3,
    color: pumpkinColor,
  },
  {
    row: centerRow - pumpkinHeight + 3,
    col: centerCol - 2,
    color: pumpkinColor,
  },
  {
    row: centerRow - pumpkinHeight + 3,
    col: centerCol - 1,
    color: pumpkinColor,
  },
  { row: centerRow - pumpkinHeight + 3, col: centerCol, color: pumpkinColor },
  {
    row: centerRow - pumpkinHeight + 3,
    col: centerCol + 1,
    color: pumpkinColor,
  },
  {
    row: centerRow - pumpkinHeight + 3,
    col: centerCol + 2,
    color: pumpkinColor,
  },
  {
    row: centerRow - pumpkinHeight + 3,
    col: centerCol + 3,
    color: pumpkinColor,
  },
  {
    row: centerRow - pumpkinHeight + 3,
    col: centerCol + 4,
    color: pumpkinColor,
  },
  {
    row: centerRow - pumpkinHeight + 4,
    col: centerCol - 4,
    color: pumpkinColor,
  },
  {
    row: centerRow - pumpkinHeight + 4,
    col: centerCol - 3,
    color: pumpkinColor,
  },
  { row: centerRow - pumpkinHeight + 4, col: centerCol - 2, color: faceColor },
  {
    row: centerRow - pumpkinHeight + 4,
    col: centerCol - 1,
    color: pumpkinColor,
  },
  { row: centerRow - pumpkinHeight + 4, col: centerCol, color: pumpkinColor },
  {
    row: centerRow - pumpkinHeight + 4,
    col: centerCol + 1,
    color: pumpkinColor,
  },
  { row: centerRow - pumpkinHeight + 4, col: centerCol + 2, color: faceColor },
  {
    row: centerRow - pumpkinHeight + 4,
    col: centerCol + 3,
    color: pumpkinColor,
  },
  {
    row: centerRow - pumpkinHeight + 4,
    col: centerCol + 4,
    color: pumpkinColor,
  },

  // Smile (bottom half of mouth)
  {
    row: centerRow - pumpkinHeight + 5,
    col: centerCol - 3,
    color: pumpkinColor,
  },
  {
    row: centerRow - pumpkinHeight + 5,
    col: centerCol - 2,
    color: pumpkinColor,
  },
  { row: centerRow - pumpkinHeight + 5, col: centerCol - 1, color: faceColor },
  { row: centerRow - pumpkinHeight + 5, col: centerCol, color: faceColor },
  { row: centerRow - pumpkinHeight + 5, col: centerCol + 1, color: faceColor },
  {
    row: centerRow - pumpkinHeight + 5,
    col: centerCol + 2,
    color: pumpkinColor,
  },
  {
    row: centerRow - pumpkinHeight + 5,
    col: centerCol + 3,
    color: pumpkinColor,
  },

  // Bottom row to round the shape
  {
    row: centerRow - pumpkinHeight + 6,
    col: centerCol - 2,
    color: pumpkinColor,
  },
  {
    row: centerRow - pumpkinHeight + 6,
    col: centerCol - 1,
    color: pumpkinColor,
  },
  { row: centerRow - pumpkinHeight + 6, col: centerCol, color: pumpkinColor },
  {
    row: centerRow - pumpkinHeight + 6,
    col: centerCol + 1,
    color: pumpkinColor,
  },
  {
    row: centerRow - pumpkinHeight + 6,
    col: centerCol + 2,
    color: pumpkinColor,
  },
];

export const ghostShape = [
  { row: ghostCenterRow - 3, col: ghostCenterCol, color: ghostColor }, // Top of the head
  { row: ghostCenterRow - 3, col: ghostCenterCol + 1, color: ghostColor },
  { row: ghostCenterRow - 3, col: ghostCenterCol - 1, color: ghostColor },
  { row: ghostCenterRow - 2, col: ghostCenterCol + 2, color: ghostColor },
  { row: ghostCenterRow - 2, col: ghostCenterCol - 2, color: ghostColor },
  { row: ghostCenterRow - 2, col: ghostCenterCol, color: ghostColor },
  { row: ghostCenterRow - 1, col: ghostCenterCol - 1, color: ghostColor },
  { row: ghostCenterRow - 1, col: ghostCenterCol + 1, color: ghostColor },
  { row: ghostCenterRow, col: ghostCenterCol - 2, color: ghostFaceColor }, //mouth
  { row: ghostCenterRow, col: ghostCenterCol + 2, color: ghostFaceColor }, //mouth
  { row: ghostCenterRow, col: ghostCenterCol - 1, color: ghostColor },
  { row: ghostCenterRow + 1, col: ghostCenterCol, color: ghostFaceColor }, //mouth
  { row: ghostCenterRow - 2, col: ghostCenterCol + 1, color: ghostFaceColor }, //right eye
  { row: ghostCenterRow - 2, col: ghostCenterCol - 1, color: ghostFaceColor }, //left eye
  { row: ghostCenterRow - 1, col: ghostCenterCol, color: ghostColor },
  { row: ghostCenterRow, col: ghostCenterCol, color: ghostColor },
  { row: ghostCenterRow, col: ghostCenterCol + 1, color: ghostColor },
  { row: ghostCenterRow - 1, col: ghostCenterCol - 2, color: ghostColor },
  { row: ghostCenterRow - 1, col: ghostCenterCol + 2, color: ghostColor },
  { row: ghostCenterRow + 1, col: ghostCenterCol + 2, color: ghostColor },
  { row: ghostCenterRow + 2, col: ghostCenterCol + 2, color: ghostColor },
  { row: ghostCenterRow + 3, col: ghostCenterCol + 2, color: ghostColor },
  { row: ghostCenterRow + 1, col: ghostCenterCol - 2, color: ghostColor },
  { row: ghostCenterRow + 2, col: ghostCenterCol - 2, color: ghostColor },
  { row: ghostCenterRow + 3, col: ghostCenterCol - 2, color: ghostColor },
  { row: ghostCenterRow + 1, col: ghostCenterCol - 1, color: ghostFaceColor }, //mouth
  { row: ghostCenterRow + 1, col: ghostCenterCol + 1, color: ghostFaceColor }, //mouth
  { row: ghostCenterRow + 2, col: ghostCenterCol - 1, color: ghostColor },
  { row: ghostCenterRow + 2, col: ghostCenterCol + 1, color: ghostColor },
  { row: ghostCenterRow + 3, col: ghostCenterCol, color: ghostColor },
  { row: ghostCenterRow + 2, col: ghostCenterCol, color: ghostColor },
  { row: ghostCenterRow + 3, col: ghostCenterCol - 1, color: ghostColor },
  { row: ghostCenterRow + 3, col: ghostCenterCol + 1, color: ghostColor },
];

//Snow Flake
const snowflakeColor = "#f0f0f0"; // White for the snowflake
export const christmasColors = ["#014f16", "#911300"];
export const christmasColors2 = ["#d6facf", "#ff6e6e"];
export const snowflakeShape = [
  { row: 3, col: 3, color: snowflakeColor },
  { row: 3, col: 6, color: snowflakeColor },
  { row: 3, col: 9, color: snowflakeColor },
  { row: 4, col: 4, color: snowflakeColor },
  { row: 4, col: 6, color: snowflakeColor },
  { row: 4, col: 8, color: snowflakeColor },
  { row: 5, col: 5, color: snowflakeColor },
  { row: 5, col: 6, color: snowflakeColor },
  { row: 5, col: 7, color: snowflakeColor },
  { row: 6, col: 3, color: snowflakeColor },
  { row: 6, col: 4, color: snowflakeColor },
  { row: 6, col: 5, color: snowflakeColor },
  { row: 6, col: 6, color: snowflakeColor },
  { row: 6, col: 7, color: snowflakeColor },
  { row: 6, col: 8, color: snowflakeColor },
  { row: 6, col: 9, color: snowflakeColor },
  { row: 7, col: 5, color: snowflakeColor },
  { row: 7, col: 6, color: snowflakeColor },
  { row: 7, col: 7, color: snowflakeColor },
  { row: 8, col: 4, color: snowflakeColor },
  { row: 8, col: 6, color: snowflakeColor },
  { row: 8, col: 8, color: snowflakeColor },
  { row: 9, col: 3, color: snowflakeColor },
  { row: 9, col: 6, color: snowflakeColor },
  { row: 9, col: 9, color: snowflakeColor },
];

const snowmanColor = "#f0f0f0";
const snowmanArmsColor = "#785200";
const snowmanNoseColor = "#ffb005";
const snowmanFaceColor = "#000000";
const snowmanButtonColor = "#000000";

export const snowmanShape = [
  { row: 1, col: 7, color: snowmanColor },
  { row: 1, col: 8, color: snowmanColor },
  { row: 1, col: 9, color: snowmanColor },
  { row: 2, col: 6, color: snowmanColor },
  { row: 2, col: 7, color: snowmanFaceColor },
  { row: 2, col: 8, color: snowmanColor },
  { row: 2, col: 9, color: snowmanFaceColor },
  { row: 2, col: 10, color: snowmanColor },
  { row: 3, col: 5, color: snowmanColor },
  { row: 3, col: 6, color: snowmanColor },
  { row: 3, col: 7, color: snowmanColor },
  { row: 3, col: 8, color: snowmanNoseColor },
  { row: 3, col: 9, color: snowmanColor },
  { row: 3, col: 10, color: snowmanColor },
  { row: 3, col: 11, color: snowmanColor },
  { row: 4, col: 5, color: snowmanColor },
  { row: 4, col: 6, color: snowmanFaceColor },
  { row: 4, col: 7, color: snowmanColor },
  { row: 4, col: 8, color: snowmanColor },
  { row: 4, col: 9, color: snowmanColor },
  { row: 4, col: 10, color: snowmanFaceColor },
  { row: 4, col: 11, color: snowmanColor },
  { row: 5, col: 5, color: snowmanColor },
  { row: 5, col: 6, color: snowmanColor },
  { row: 5, col: 7, color: snowmanFaceColor },
  { row: 5, col: 8, color: snowmanFaceColor },
  { row: 5, col: 9, color: snowmanFaceColor },
  { row: 5, col: 10, color: snowmanColor },
  { row: 5, col: 11, color: snowmanColor },
  { row: 6, col: 6, color: snowmanColor },
  { row: 6, col: 7, color: snowmanColor },
  { row: 6, col: 8, color: snowmanColor },
  { row: 6, col: 9, color: snowmanColor },
  { row: 6, col: 10, color: snowmanColor },
  { row: 7, col: 7, color: snowmanColor },
  { row: 7, col: 8, color: snowmanButtonColor },
  { row: 7, col: 9, color: snowmanColor },
  { row: 8, col: 5, color: snowmanColor },
  { row: 8, col: 6, color: snowmanColor },
  { row: 8, col: 7, color: snowmanColor },
  { row: 8, col: 8, color: snowmanColor },
  { row: 8, col: 9, color: snowmanColor },
  { row: 8, col: 10, color: snowmanColor },
  { row: 8, col: 11, color: snowmanColor },
  { row: 9, col: 1, color: snowmanArmsColor },
  { row: 9, col: 2, color: snowmanArmsColor },
  { row: 9, col: 3, color: snowmanArmsColor },
  { row: 9, col: 4, color: snowmanColor },
  { row: 9, col: 5, color: snowmanColor },
  { row: 9, col: 6, color: snowmanColor },
  { row: 9, col: 7, color: snowmanColor },
  { row: 9, col: 8, color: snowmanButtonColor },
  { row: 9, col: 9, color: snowmanColor },
  { row: 9, col: 10, color: snowmanColor },
  { row: 9, col: 11, color: snowmanColor },
  { row: 9, col: 12, color: snowmanColor },
  { row: 9, col: 13, color: snowmanArmsColor },
  { row: 9, col: 14, color: snowmanArmsColor },
  { row: 9, col: 15, color: snowmanArmsColor },
  { row: 10, col: 4, color: snowmanColor },
  { row: 10, col: 5, color: snowmanColor },
  { row: 10, col: 6, color: snowmanColor },
  { row: 10, col: 7, color: snowmanColor },
  { row: 10, col: 8, color: snowmanColor },
  { row: 10, col: 9, color: snowmanColor },
  { row: 10, col: 10, color: snowmanColor },
  { row: 10, col: 11, color: snowmanColor },
  { row: 10, col: 12, color: snowmanColor },
  { row: 11, col: 4, color: snowmanColor },
  { row: 11, col: 5, color: snowmanColor },
  { row: 11, col: 6, color: snowmanColor },
  { row: 11, col: 7, color: snowmanColor },
  { row: 11, col: 8, color: snowmanButtonColor },
  { row: 11, col: 9, color: snowmanColor },
  { row: 11, col: 10, color: snowmanColor },
  { row: 11, col: 11, color: snowmanColor },
  { row: 11, col: 12, color: snowmanColor },
  { row: 12, col: 5, color: snowmanColor },
  { row: 12, col: 6, color: snowmanColor },
  { row: 12, col: 7, color: snowmanColor },
  { row: 12, col: 8, color: snowmanColor },
  { row: 12, col: 9, color: snowmanColor },
  { row: 12, col: 10, color: snowmanColor },
  { row: 12, col: 11, color: snowmanColor },
  { row: 13, col: 6, color: snowmanColor },
  { row: 13, col: 7, color: snowmanColor },
  { row: 13, col: 8, color: snowmanColor },
  { row: 13, col: 9, color: snowmanColor },
  { row: 13, col: 10, color: snowmanColor },
];

const treeColor = "#169100";
const woodColor = "#593d01";
const starColor = "#fcff36";

export const christmasTreeShape = [
  { row: 1, col: 9, color: starColor },
  { row: 2, col: 8, color: starColor },
  { row: 2, col: 9, color: starColor },
  { row: 2, col: 10, color: starColor },
  { row: 3, col: 9, color: treeColor },
  { row: 4, col: 8, color: treeColor },
  { row: 4, col: 9, color: treeColor },
  { row: 4, col: 10, color: treeColor },
  { row: 5, col: 7, color: treeColor },
  { row: 5, col: 8, color: treeColor },
  { row: 5, col: 9, color: treeColor },
  { row: 5, col: 10, color: treeColor },
  { row: 5, col: 11, color: treeColor },
  { row: 6, col: 7, color: treeColor },
  { row: 6, col: 8, color: treeColor },
  { row: 6, col: 9, color: treeColor },
  { row: 6, col: 10, color: treeColor },
  { row: 6, col: 11, color: treeColor },
  { row: 7, col: 6, color: treeColor },
  { row: 7, col: 7, color: treeColor },
  { row: 7, col: 8, color: treeColor },
  { row: 7, col: 9, color: treeColor },
  { row: 7, col: 10, color: treeColor },
  { row: 7, col: 11, color: treeColor },
  { row: 7, col: 12, color: treeColor },
  { row: 8, col: 6, color: treeColor },
  { row: 8, col: 7, color: treeColor },
  { row: 8, col: 8, color: treeColor },
  { row: 8, col: 9, color: treeColor },
  { row: 8, col: 10, color: treeColor },
  { row: 8, col: 11, color: treeColor },
  { row: 8, col: 12, color: treeColor },
  { row: 9, col: 5, color: treeColor },
  { row: 9, col: 6, color: treeColor },
  { row: 9, col: 7, color: treeColor },
  { row: 9, col: 8, color: treeColor },
  { row: 9, col: 9, color: treeColor },
  { row: 9, col: 10, color: treeColor },
  { row: 9, col: 11, color: treeColor },
  { row: 9, col: 12, color: treeColor },
  { row: 9, col: 13, color: treeColor },
  { row: 10, col: 5, color: treeColor },
  { row: 10, col: 6, color: treeColor },
  { row: 10, col: 7, color: treeColor },
  { row: 10, col: 8, color: treeColor },
  { row: 10, col: 9, color: treeColor },
  { row: 10, col: 10, color: treeColor },
  { row: 10, col: 11, color: treeColor },
  { row: 10, col: 12, color: treeColor },
  { row: 10, col: 13, color: treeColor },
  { row: 11, col: 8, color: woodColor },
  { row: 11, col: 9, color: woodColor },
  { row: 11, col: 10, color: woodColor },
  { row: 12, col: 8, color: woodColor },
  { row: 12, col: 9, color: woodColor },
  { row: 12, col: 10, color: woodColor },
  { row: 13, col: 8, color: woodColor },
  { row: 13, col: 9, color: woodColor },
  { row: 13, col: 10, color: woodColor },
];

const candyCaneRedColor = "#ab0000";

export const candyCaneShape = [
  { row: 2, col: 3, color: candyCaneRedColor },
  { row: 2, col: 4, color: candyCaneRedColor },
  { row: 2, col: 5, color: candyCaneRedColor },
  { row: 2, col: 6, color: candyCaneRedColor },
  { row: 3, col: 3, color: candyCaneRedColor },
  { row: 3, col: 4, color: candyCaneRedColor },
  { row: 3, col: 5, color: candyCaneRedColor },
  { row: 3, col: 6, color: candyCaneRedColor },
  { row: 4, col: 2, color: candyCaneRedColor },
  { row: 4, col: 3, color: candyCaneRedColor },
  { row: 4, col: 6, color: candyCaneRedColor },
  { row: 4, col: 7, color: candyCaneRedColor },
  { row: 5, col: 2, color: candyCaneRedColor },
  { row: 5, col: 3, color: candyCaneRedColor },
  { row: 5, col: 6, color: candyCaneRedColor },
  { row: 5, col: 7, color: candyCaneRedColor },
  { row: 6, col: 6, color: candyCaneRedColor },
  { row: 6, col: 7, color: candyCaneRedColor },
  { row: 7, col: 6, color: candyCaneRedColor },
  { row: 7, col: 7, color: candyCaneRedColor },
  { row: 8, col: 6, color: candyCaneRedColor },
  { row: 8, col: 7, color: candyCaneRedColor },
  { row: 9, col: 6, color: candyCaneRedColor },
  { row: 9, col: 7, color: candyCaneRedColor },
  { row: 10, col: 6, color: candyCaneRedColor },
  { row: 10, col: 7, color: candyCaneRedColor },
];
