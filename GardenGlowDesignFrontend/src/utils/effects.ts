import { Dispatch, SetStateAction } from "react";

function updateChristmasBackground(
  offset: number,
  numRows: number,
  numCols: number,
  christmasColors: string[],
  setLedGrid: React.Dispatch<React.SetStateAction<number[][]>>
) {
  setLedGrid(() =>
    Array(numRows)
      .fill(0)
      .map((_, rowIndex) =>
        Array(numCols)
          .fill(0)
          .map(
            (_, colIndex) =>
              (rowIndex + colIndex + offset) % christmasColors.length
          )
      )
  );
}

export const updateRainbow = (
  offset: number,
  numRows: number,
  numCols: number,
  colors: string[],
  setLedGrid: React.Dispatch<React.SetStateAction<number[][]>>
) => {
  setLedGrid(() =>
    Array(numRows)
      .fill(0)
      .map((_, rowIndex) =>
        Array(numCols)
          .fill(0)
          .map((_, colIndex) => (rowIndex + colIndex + offset) % colors.length)
      )
  );
};

export const updateRipple = (
  offset: number,
  numRows: number,
  numCols: number,
  colors: string[],
  setLedGrid: React.Dispatch<React.SetStateAction<number[][]>>
) => {
  setLedGrid(() => {
    return Array(numRows)
      .fill(0)
      .map((_, rowIndex) =>
        Array(numCols)
          .fill(0)
          .map((_, colIndex) => {
            const distance = Math.sqrt(
              Math.pow(rowIndex - numRows / 2, 2) +
                Math.pow(colIndex - numCols / 2, 2)
            );
            return Math.floor((distance + offset) % colors.length);
          })
      );
  });
};

export const updateGhostRainbow = (
  offset: number,
  numRows: number,
  numCols: number,
  colors: string[],
  setLedGrid: React.Dispatch<React.SetStateAction<number[][]>>,
  setGhostPosition: React.Dispatch<React.SetStateAction<number>>
) => {
  updateRainbow(offset, numRows, numCols, colors, setLedGrid);
  setGhostPosition((prev: number) => (prev + 1) % numCols);
};

export const updateGhostRipple = (
  offset: number,
  numRows: number,
  numCols: number,
  colors: string[],
  setLedGrid: React.Dispatch<React.SetStateAction<number[][]>>,
  setGhostPosition: React.Dispatch<React.SetStateAction<number>>
) => {
  updateRipple(offset, numRows, numCols, colors, setLedGrid);
  setGhostPosition((prev: number) => (prev + 1) % numCols);
};

export const updatePumpkinRainbow = (
  offset: number,
  numRows: number,
  numCols: number,
  colors: string[],
  setLedGrid: React.Dispatch<React.SetStateAction<number[][]>>,
  setPumpkinPosition: React.Dispatch<React.SetStateAction<number>>
) => {
  updateRainbow(offset, numRows, numCols, colors, setLedGrid);
  setPumpkinPosition((prev: number) => (prev + 1) % numCols);
};

export const updatePumpkinRipple = (
  offset: number,
  numRows: number,
  numCols: number,
  colors: string[],
  setLedGrid: React.Dispatch<React.SetStateAction<number[][]>>,
  setPumpkinPosition: React.Dispatch<React.SetStateAction<number>>
) => {
  updateRipple(offset, numRows, numCols, colors, setLedGrid);
  setPumpkinPosition((prev: number) => (prev + 1) % numCols);
};

export const updatePumpkinGhostRainbow = (
  offset: number,
  numRows: number,
  numCols: number,
  colors: string[],
  setLedGrid: React.Dispatch<React.SetStateAction<number[][]>>,
  setPumpkinPosition: React.Dispatch<React.SetStateAction<number>>,
  setGhostPosition: React.Dispatch<React.SetStateAction<number>>
) => {
  updateRainbow(offset, numRows, numCols, colors, setLedGrid); // Apply rainbow effect
  setPumpkinPosition((prev: number) => (prev + 1) % numCols); // Move pumpkin horizontally
  setGhostPosition((prev: number) => (prev + 1) % numCols); // Move ghost horizontally
};

export const updatePumpkinGhostRipple = (
  offset: number,
  numRows: number,
  numCols: number,
  colors: string[],
  setLedGrid: React.Dispatch<React.SetStateAction<number[][]>>,
  setPumpkinPosition: React.Dispatch<React.SetStateAction<number>>,
  setGhostPosition: React.Dispatch<React.SetStateAction<number>>
) => {
  updateRipple(offset, numRows, numCols, colors, setLedGrid); // Apply rainbow effect
  setPumpkinPosition((prev: number) => (prev + 1) % numCols); // Move pumpkin horizontally
  setGhostPosition((prev: number) => (prev + 1) % numCols); // Move ghost horizontally
};

export const updateSnowflake = (
  offset: number,
  numRows: number,
  numCols: number,
  christmasColors: string[],
  setLedGrid: React.Dispatch<React.SetStateAction<number[][]>>,
  setSnowflakePositions: React.Dispatch<React.SetStateAction<any[]>>
) => {
  // Update background or any other global update here
  updateChristmasBackground(
    offset,
    numRows,
    numCols,
    christmasColors,
    setLedGrid
  );

  setSnowflakePositions((prevPositions) =>
    prevPositions.map((position) => ({
      row: (position.row + 1) % numRows, // Move down the rows
      col: position.col,                // Keep column constant
    }))
  );

};

export const updateSnowman = (
  offset: number,
  numRows: number,
  numCols: number,
  christmasColors: string[],
  setLedGrid: React.Dispatch<React.SetStateAction<number[][]>>,
  setSnowmanPosition: React.Dispatch<React.SetStateAction<number>>
) => {
  updateChristmasBackground(
    offset,
    numRows,
    numCols,
    christmasColors,
    setLedGrid
  );
  setSnowmanPosition((prev: number) => (prev + 1) % numCols); 
};

export const updateChristmasTree = (
  offset: number,
  numRows: number,
  numCols: number,
  christmasColors2: string[],
  setLedGrid: React.Dispatch<React.SetStateAction<number[][]>>,
  setChristmasTreePosition: React.Dispatch<React.SetStateAction<number>>
) => {
  updateChristmasBackground(
    offset,
    numRows,
    numCols,
    christmasColors2,
    setLedGrid
  );
  setChristmasTreePosition((prev: number) => (prev + 1) % numCols); 
};

export const updateCandyCane = (
  offset: number,
  numRows: number,
  numCols: number,
  christmasColors2: string[],
  setLedGrid: React.Dispatch<React.SetStateAction<number[][]>>,
  setCandyCanePositions: React.Dispatch<React.SetStateAction<any[]>>
) => {
  // Update background or any other global update here
  updateChristmasBackground(
    offset,
    numRows,
    numCols,
    christmasColors2,
    setLedGrid
  );

  setCandyCanePositions((prevPositions) =>
    prevPositions.map((position) => ({
      row: (position.row + 1) % numRows, // Move down the rows
      col: position.col,                // Keep column constant
    }))
  );
};