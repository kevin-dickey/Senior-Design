
const numRows =  10; 
const numCols = 20;


const pumpkinColor = '#fca63d'; // Orange for the pumpkin
  const stemColor = '#012e01'; // Dark green for the stem
  const pumpkinBorderColor = '#8a3900'; // Brown for pumpkin border
  const faceColor = '#000000'; // Black for eyes and smile
  const pumpkinHeight = Math.floor(numRows / 3);
  const pumpkinWidth = Math.floor(numCols / 4);
  const centerRow = Math.floor(numRows / 2);
  const centerCol = Math.floor(numCols / 2);

  const ghostColor = '#ffffff'; // White color for the ghost
  const ghostFaceColor = '#000000'// Black for the ghost face
  const ghostBorderColor = '#cccccc'; // Light grey for the ghost's border
  const ghostHeight = Math.floor(numRows / 3); // Height of the ghost
  const ghostWidth = Math.floor(numCols / 4); // Width of the ghost
  const ghostCenterRow = Math.floor(numRows / 2); // Center row of the ghost
  const ghostCenterCol = Math.floor(numCols / 2); // Center column of the ghost  
  const ghostColors = ['#ffffff', '#000000'];  // White and Black

export const pumpkinShape = [
    // Stem (centered at the top of the pumpkin)
    { row: centerRow - pumpkinHeight, col: centerCol - 1, color: stemColor },
    { row: centerRow - pumpkinHeight, col: centerCol, color: stemColor },
    { row: centerRow - pumpkinHeight, col: centerCol + 1, color: stemColor },

    // Top of pumpkin
    { row: centerRow - pumpkinHeight + 1, col: centerCol - 2, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 1, col: centerCol - 1, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 1, col: centerCol, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 1, col: centerCol + 1, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 1, col: centerCol + 2, color: pumpkinColor },

    // Second row (wider than the top) & Eyes
    { row: centerRow - pumpkinHeight + 2, col: centerCol - 3, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 2, col: centerCol - 2, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 2, col: centerCol - 1, color: faceColor },
    { row: centerRow - pumpkinHeight + 2, col: centerCol, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 2, col: centerCol + 1, color: faceColor },
    { row: centerRow - pumpkinHeight + 2, col: centerCol + 2, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 2, col: centerCol + 3, color: pumpkinColor },

    // Middle rows
    { row: centerRow - pumpkinHeight + 3, col: centerCol - 4, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 3, col: centerCol - 3, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 3, col: centerCol - 2, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 3, col: centerCol - 1, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 3, col: centerCol, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 3, col: centerCol + 1, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 3, col: centerCol + 2, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 3, col: centerCol + 3, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 3, col: centerCol + 4, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 4, col: centerCol - 4, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 4, col: centerCol - 3, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 4, col: centerCol - 2, color: faceColor },
    { row: centerRow - pumpkinHeight + 4, col: centerCol - 1, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 4, col: centerCol, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 4, col: centerCol + 1, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 4, col: centerCol + 2, color: faceColor },
    { row: centerRow - pumpkinHeight + 4, col: centerCol + 3, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 4, col: centerCol + 4, color: pumpkinColor },

    // Smile (bottom half of mouth)
    { row: centerRow - pumpkinHeight + 5, col: centerCol - 3, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 5, col: centerCol - 2, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 5, col: centerCol - 1, color: faceColor },
    { row: centerRow - pumpkinHeight + 5, col: centerCol, color: faceColor },
    { row: centerRow - pumpkinHeight + 5, col: centerCol + 1, color: faceColor },
    { row: centerRow - pumpkinHeight + 5, col: centerCol + 2, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 5, col: centerCol + 3, color: pumpkinColor },


    // Bottom row to round the shape
    { row: centerRow - pumpkinHeight + 6, col: centerCol - 2, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 6, col: centerCol - 1, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 6, col: centerCol, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 6, col: centerCol + 1, color: pumpkinColor },
    { row: centerRow - pumpkinHeight + 6, col: centerCol + 2, color: pumpkinColor }
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
    { row: ghostCenterRow, col: ghostCenterCol + 2, color: ghostFaceColor },//mouth
    { row: ghostCenterRow, col: ghostCenterCol - 1, color: ghostColor },
    { row: ghostCenterRow + 1, col: ghostCenterCol, color: ghostFaceColor }, //mouth
    { row: ghostCenterRow - 2, col: ghostCenterCol + 1, color: ghostFaceColor },//right eye
    { row: ghostCenterRow - 2, col: ghostCenterCol - 1, color: ghostFaceColor },//left eye
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
