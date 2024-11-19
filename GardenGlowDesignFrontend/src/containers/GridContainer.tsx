import React from 'react';
import { Box } from '@mui/material';
import { TransformWrapper, TransformComponent } from 'react-zoom-pan-pinch';
import { GridLayout } from '../components/serialization/Layout';
import { Show } from '../components/serialization/Show';
import { useState, useEffect } from 'react';
import { updateRainbow, updateRipple, updateGhostRainbow, updateGhostRipple, updatePumpkinRainbow, updatePumpkinRipple, updatePumpkinGhostRainbow, updatePumpkinGhostRipple, updateSnowflake, updateSnowman, updateChristmasTree, updateCandyCane } from '../utils/effects';
import { pumpkinShape, ghostShape, snowflakeShape, christmasColors, christmasColors2, snowmanShape, christmasTreeShape, candyCaneShape, rainbowColors } from '../utils/shapes';
import { EffectType } from '../types/index';

interface GridContainerProps {
  show: Show;
}

const GridContainer: React.FC<GridContainerProps> = ({ show }) => {
  const [effectData, setEffectData] = useState<any[]>([]);
  const [, setColorOffset] = useState(0);
  const [effectType, setEffectType] = useState<EffectType>();
  const [pumpkinPosition, setPumpkinPosition] = useState(-1);
  const [ghostPosition, setGhostPosition] = useState(-1);
  const [snowflakePositions, setSnowflakePositions] = useState([{ col: -1, row: -1 }, { col: -1, row: -1 }, { col: -1, row: -1 }, { col: -1, row: -1 }, { col: -1, row: -1 }, { col: -1, row: -1 }]);
  const [snowmanPosition, setSnowmanPosition] = useState(-1);
  const [christmasTreePosition, setChristmasTreePosition] = useState(-1);
  const [candyCanePositions, setCandyCanePositions] = useState([{ col: -1, row: -1 }, { col: -1, row: -1 }, { col: -1, row: -1 }, { col: -1, row: -1 }, { col: -1, row: -1 }, { col: -1, row: -1 }]);
  const [grid, setGrid] = useState<GridLayout | null>(null);
  const [ledGrid, setLedGrid] = useState<number[][]>([]);
   // Fetch the effects data from JSON file
   useEffect(() => {
    const fetchEffectData = async () => {
        setGrid(show.layouts[0] as GridLayout);
        setEffectData(show.effects);
    };
    fetchEffectData();
  }, []);

  useEffect(() => {
      const height = grid?.height;
      const width = grid?.width;

      // Initialize LED grid with height and width from grid
      setLedGrid(
        Array(height)
          .fill(0)
          .map(() => Array(width).fill(0))
      );
  }, [grid]);

  const startEffect = async () => {
    if (show.effects.length === 0) {
      console.log("No effects loaded");
      return;
    }


    // Loop through each effect and run it for its duration
    for (const effect of show.effects) {
      let offset = 0;
      setEffectType(effect.type);
      // Set the correct pumpkin/ghost positions based on effect
      if (effect.type === 'pumpkin-ghost-rainbow' || effect.type === 'pumpkin-ghost-ripple') {
        setPumpkinPosition(-5);
        setGhostPosition(5);
      } else {
        setPumpkinPosition(0);
        setGhostPosition(0);
        setSnowflakePositions([{ col: 0, row: 0 }, { col: 10, row: 0 }, { col: 20, row: 0 }, { col: 30, row: 0 }, { col: 40, row: 0 }, { col: 50, row: 0 }]);
        setSnowmanPosition(0);
        setChristmasTreePosition(0);
        setCandyCanePositions([{ col: 0, row: 0 }, { col: 10, row: 0 }, { col: 20, row: 0 }, { col: 30, row: 0 }, { col: 40, row: 0 }, { col: 50, row: 0 }]);
      }
      console.log(effect);
      // Start the interval for this effect
      const interval = setInterval(() => {
        offset += 1;
        setColorOffset(offset);
        // Call the appropriate effect update function
        if (grid) {
          switch (effect.type) {
            case 'rainbow':
              updateRainbow(offset, grid.height, grid.width, rainbowColors, setLedGrid);
              break;
            case 'ripple':
              updateRipple(offset, grid.height, grid.width, rainbowColors, setLedGrid);
              break;
            case 'pumpkin-rainbow':
              updatePumpkinRainbow(offset, grid.height, grid.width, rainbowColors, setLedGrid, setPumpkinPosition);
              break;
            case 'pumpkin-ripple':
              updatePumpkinRipple(offset, grid.height, grid.width, rainbowColors, setLedGrid, setPumpkinPosition);
              break;
            case 'ghost-rainbow':
              updateGhostRainbow(offset, grid.height, grid.width, rainbowColors, setLedGrid, setGhostPosition);
              break;
            case 'ghost-ripple':
              updateGhostRipple(offset, grid.height, grid.width, rainbowColors, setLedGrid, setGhostPosition);
              break;
            case 'pumpkin-ghost-rainbow':
              updatePumpkinGhostRainbow(offset, grid.height, grid.width, rainbowColors, setLedGrid, setPumpkinPosition, setGhostPosition);
              break;
            case 'pumpkin-ghost-ripple':
              updatePumpkinGhostRipple(offset, grid.height, grid.width, rainbowColors, setLedGrid, setPumpkinPosition, setGhostPosition);
              break;
            case 'snowflake':
              updateSnowflake(offset, grid.height, grid.width, christmasColors, setLedGrid, setSnowflakePositions);
              break;
            case 'snowman':
              updateSnowman(offset, grid.height, grid.width, christmasColors, setLedGrid, setSnowmanPosition);
              break;
            case 'christmas-tree':
              updateChristmasTree(offset, grid.height, grid.width, christmasColors2, setLedGrid, setChristmasTreePosition);
              break;
            case 'candy-cane':
              updateCandyCane(offset, grid.height, grid.width, christmasColors2, setLedGrid, setCandyCanePositions);
              break;
            default:
              console.log(`Unknown effect: ${effect.type}`);
          }
        }
      }, 1000 / effect.speed); //controls the speed

      // Wait for the effect duration to complete before stopping the interval and moving to the next effect
      await new Promise<void>((resolve) => {
        setTimeout(() => {
          clearInterval(interval);
          resolve();
        }, effect.durationMs);
      });
    }
  };

  return (
    <div>
      <div className="controls">
        <button onClick={startEffect}>
          Start Effects
        </button>
      </div>

      <TransformWrapper
        initialScale={1}
        wheel={{ step: 0.5 }}
        centerOnInit={true}
        minScale={0.5}
        maxScale={5}
      >
        {({ zoomIn, zoomOut, resetTransform }) => (
          <TransformComponent wrapperClass="react-transform-wrapper">
            <Box flexDirection="column">
            {ledGrid.map((row, rowIndex) => (
              <Box key={rowIndex} display="flex" gap={0.3}>
                {row.map((colorIndex, colIndex) => {
                  let pumpkinPart: { row: number; col: number; color: string } | undefined;
                  let ghostPart: { row: number; col: number; color: string } | undefined;
                  let snowflakePart: { row: number; col: number; color: string } | undefined;
                  let snowmanPart: { row: number; col: number; color: string } | undefined;
                  let christmasTreePart: { row: number; col: number; color: string } | undefined;
                  let candyCanePart: { row: number; col: number; color: string } | undefined;

                  let backgroundColor = '';

                  if (grid) {

                    pumpkinPart = pumpkinShape.find(
                      (part) => rowIndex === part.row && colIndex === (pumpkinPosition + part.col) % grid.width
                    );

                    ghostPart = ghostShape.find(
                      (part) => rowIndex === part.row && colIndex === (ghostPosition + part.col) % grid.width
                    );

                    snowflakePart = snowflakeShape.find((part) =>
                      snowflakePositions.some(
                        (position) =>
                          rowIndex === (position.row + part.row) % grid.height && // Match row
                          colIndex === (position.col + part.col) % grid.width    // Match column
                      )
                    );

                    snowmanPart = snowmanShape.find(
                      (part) => rowIndex === part.row && colIndex === (snowmanPosition + part.col) % grid.width
                    );

                    christmasTreePart = christmasTreeShape.find(
                      (part) => rowIndex === part.row && colIndex === (christmasTreePosition + part.col) % grid.width
                    );

                    candyCanePart = candyCaneShape.find((part) =>
                      candyCanePositions.some(
                        (position) =>
                          rowIndex === (position.row + part.row) % grid.height && // Match row
                          colIndex === (position.col + part.col) % grid.width    // Match column
                      )
                    );

                    // Handle shape effect
                    if (effectType === 'pumpkin-rainbow' || effectType === 'pumpkin-ripple') {
                      backgroundColor = pumpkinPart ? pumpkinPart.color : rainbowColors[colorIndex];
                    } else if (effectType === 'ghost-rainbow' || effectType === 'ghost-ripple') {
                      backgroundColor = ghostPart ? ghostPart.color : rainbowColors[colorIndex];
                    } else if (effectType === 'pumpkin-ghost-rainbow' || effectType === 'pumpkin-ghost-ripple') {
                      backgroundColor = pumpkinPart ? pumpkinPart.color : (ghostPart ? ghostPart.color : rainbowColors[colorIndex]);
                    } else if (effectType === 'snowflake') {
                      backgroundColor = snowflakePart ? snowflakePart.color : christmasColors[colorIndex];
                    } else if (effectType === 'snowman') {
                      backgroundColor = snowmanPart ? snowmanPart.color : christmasColors[colorIndex];
                    } else if (effectType === 'christmas-tree') {
                      backgroundColor = christmasTreePart ? christmasTreePart.color : christmasColors2[colorIndex];
                    } else if (effectType === 'candy-cane') {
                      backgroundColor = candyCanePart ? candyCanePart.color : christmasColors2[colorIndex];
                    }
                    else {
                      backgroundColor = rainbowColors[colorIndex];
                    }
                  }
                  
                  return (
                    <div
                      key={colIndex}
                      className={`led-circle 
                    ${pumpkinPart ? 'pumpkin-cell' : ''} 
                    ${ghostPart ? 'ghost-cell' : ''}
                    ${snowflakePart ? 'snowflake-cell' : ''}
                    ${snowmanPart ? 'snowman-cell' : ''}
                    ${christmasTreePart ? 'christmasTree-cell' : ''}
                    ${candyCanePart ? 'candy-cane-cell' : ''}
                    `}
                      style={{
                        backgroundColor,
                      }}
                    ></div>
                  );
                })}
              </Box>
            ))}
            </Box>
          </TransformComponent>
        )}
      </TransformWrapper>
    </div>
  );
};

export default GridContainer;
