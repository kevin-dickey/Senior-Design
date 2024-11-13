import setGhostPosition from "../components/pages/Grid";
import setPumpkinPosition from "../components/pages/Grid";

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
                Math.pow(rowIndex - numRows / 2, 2) + Math.pow(colIndex - numCols / 2, 2)
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
  }

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
    updateRainbow(offset, numRows, numCols, colors, setLedGrid);  // Apply rainbow effect
    setPumpkinPosition((prev: number) => (prev + 1) % numCols); // Move pumpkin horizontally
    setGhostPosition((prev: number) => (prev + 1) % numCols);   // Move ghost horizontally
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
    updateRipple(offset, numRows, numCols, colors, setLedGrid);  // Apply rainbow effect
    setPumpkinPosition((prev: number) => (prev + 1) % numCols ); // Move pumpkin horizontally
    setGhostPosition((prev: number) => (prev + 1) % numCols);   // Move ghost horizontally
  };