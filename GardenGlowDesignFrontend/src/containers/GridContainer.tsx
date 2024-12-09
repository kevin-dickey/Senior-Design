import React, { useRef } from 'react';
import { Box } from '@mui/material';
import PlayArrowIcon from '@mui/icons-material/PlayArrow';
import StopIcon from '@mui/icons-material/Stop';
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
  const [, setEffectData] = useState<any[]>([]);
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
  const stoppedRef = useRef(false);
  const intervalsRef = useRef<Set<NodeJS.Timeout>>(new Set()); 
  const [isRunning, setIsRunning] = useState(false); 
  
  // Fetch the effects data from JSON file
  useEffect(() => {
    const fetchEffectData = async () => {
      setGrid(show.layouts[0] as GridLayout);
      setEffectData(show.effects);
    };
    fetchEffectData();
  }, [show.layouts, show.effects]);

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

    stoppedRef.current = false; 
    setIsRunning(true); 

    // Loop through each effect and run it for its duration
    for (const effect of show.effects) {
      if (stoppedRef.current) break;

      let offset = 0;
      setEffectType(effect.type);
      // Set the correct pumpkin/ghost positions based on effect
      if (effect.type === EffectType.pumpkinGhostRainbow || effect.type === EffectType.pumpkinGhostRipple) {
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
            case EffectType.rainbow:
              updateRainbow(offset, grid.height, grid.width, rainbowColors, setLedGrid);
              break;
            case EffectType.ripple:
              updateRipple(offset, grid.height, grid.width, rainbowColors, setLedGrid);
              break;
            case EffectType.pumpkinRainbow:
              updatePumpkinRainbow(offset, grid.height, grid.width, rainbowColors, setLedGrid, setPumpkinPosition);
              break;
            case EffectType.pumpkinRipple:
              updatePumpkinRipple(offset, grid.height, grid.width, rainbowColors, setLedGrid, setPumpkinPosition);
              break;
            case EffectType.ghostRainbow:
              updateGhostRainbow(offset, grid.height, grid.width, rainbowColors, setLedGrid, setGhostPosition);
              break;
            case EffectType.ghostRipple:
              updateGhostRipple(offset, grid.height, grid.width, rainbowColors, setLedGrid, setGhostPosition);
              break;
            case EffectType.pumpkinGhostRainbow:
              updatePumpkinGhostRainbow(offset, grid.height, grid.width, rainbowColors, setLedGrid, setPumpkinPosition, setGhostPosition);
              break;
            case EffectType.pumpkinGhostRipple:
              updatePumpkinGhostRipple(offset, grid.height, grid.width, rainbowColors, setLedGrid, setPumpkinPosition, setGhostPosition);
              break;
            case EffectType.snowflake:
              updateSnowflake(offset, grid.height, grid.width, christmasColors, setLedGrid, setSnowflakePositions);
              break;
            case EffectType.snowman:
              updateSnowman(offset, grid.height, grid.width, christmasColors, setLedGrid, setSnowmanPosition);
              break;
            case EffectType.christmasTree:
              updateChristmasTree(offset, grid.height, grid.width, christmasColors2, setLedGrid, setChristmasTreePosition);
              break;
            case EffectType.candyCane:
              updateCandyCane(offset, grid.height, grid.width, christmasColors2, setLedGrid, setCandyCanePositions);
              break;
            default:
              console.log(`Unknown effect: ${effect.type}`);
          }
        }
      }, 1000 / effect.speed); //controls the speed
      
      intervalsRef.current.add(interval);
      
      // Wait for the effect duration to complete before stopping the interval and moving to the next effect
      await new Promise<void>((resolve) => {
        const timeout = setTimeout(() => {
          clearInterval(interval); // Clear interval after duration
          intervalsRef.current.delete(interval);
          resolve();
        }, effect.durationMs);

        intervalsRef.current.add(timeout); // Track the timeout
      
      });
    }
    setIsRunning(false);
  };

  const stopEffect = () => {
    stoppedRef.current = true;
    intervalsRef.current.forEach((id) => clearInterval(id)); // Clear all intervals
    intervalsRef.current.clear(); // Reset the interval tracking
    setIsRunning(false); 
    console.log("Effects stopped");
  };


  return (
    <div className="MuiBox-root css-sxr8rl">

      <TransformWrapper
        initialScale={1}
        wheel={{ step: 0.5 }}
        centerOnInit={true}
        minScale={0.1}
        maxScale={50}
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
                      if (effectType === EffectType.pumpkinRainbow || effectType === EffectType.pumpkinRipple) {
                        backgroundColor = pumpkinPart ? pumpkinPart.color : rainbowColors[colorIndex];
                      } else if (effectType === EffectType.ghostRainbow || effectType === EffectType.ghostRipple) {
                        backgroundColor = ghostPart ? ghostPart.color : rainbowColors[colorIndex];
                      } else if (effectType === EffectType.pumpkinGhostRainbow || effectType === EffectType.pumpkinGhostRipple) {
                        backgroundColor = pumpkinPart ? pumpkinPart.color : (ghostPart ? ghostPart.color : rainbowColors[colorIndex]);
                      } else if (effectType === EffectType.snowflake) {
                        backgroundColor = snowflakePart ? snowflakePart.color : christmasColors[colorIndex];
                      } else if (effectType === EffectType.snowman) {
                        backgroundColor = snowmanPart ? snowmanPart.color : christmasColors[colorIndex];
                      } else if (effectType === EffectType.christmasTree) {
                        backgroundColor = christmasTreePart ? christmasTreePart.color : christmasColors2[colorIndex];
                      } else if (effectType === EffectType.candyCane) {
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
      <Box
        sx={{
          display: 'flex',
          justifyContent: 'center',
          alignItems: 'center',
          mt: 2,
          padding:'5px'
        }}
        style={{backgroundColor: '#2a2a2a' }}
      >
        <button onClick={startEffect} disabled={isRunning} style={{backgroundColor: '#2a2a2a', color:'white', padding:'0px', marginBottom: '0px', width: 'fit-content' }}>
          <PlayArrowIcon />
        </button>
        <button onClick={stopEffect} disabled={!isRunning} style={{ backgroundColor: '#2a2a2a', color:'white', padding:'0px', marginBottom: '0px', width: 'fit-content' }}>
          <StopIcon />
        </button>
      </Box>
    </div>
  );
};

export default GridContainer;
