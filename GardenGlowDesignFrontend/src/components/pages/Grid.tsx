import React, { useState, useEffect } from 'react';
import './LEDGrid.css';

import { updateRainbow, updateRipple, updateGhostRainbow, updateGhostRipple, updatePumpkinRainbow, updatePumpkinRipple, updatePumpkinGhostRainbow, updatePumpkinGhostRipple, updateSnowflake, updateSnowman, updateChristmasTree, updateCandyCane } from '../../utils/effects';
import { pumpkinShape, ghostShape,snowflakeShape, christmasColors,christmasColors2, snowmanShape, christmasTreeShape, candyCaneShape} from '../../utils/shapes';
import { EffectType } from '../../types/index';

const LEDGrid: React.FC = () => {
  const numRows = 26/2; 
  const numCols = 116/2;

  const [ledGrid, setLedGrid] = useState<number[][]>(
    Array(numRows)
      .fill(0)
      .map(() => Array(numCols).fill(0))
  );

  const rainbowColors = [
    '#e81416', // Red
    '#e88000', // Orange
    '#faeb36', // Yellow
    '#79c314', // Green
    '#487de7', // Blue
    '#4b369d', // Indigo
    '#70369d', // Violet
  ];

  const [effectData, setEffectData] = useState<any[]>([]);
  const [, setColorOffset] = useState(0);
  const [isRunning, setIsRunning] = useState(false);
  const [effectType, setEffectType] = useState<EffectType>('rainbow');
  const [pumpkinPosition, setPumpkinPosition] = useState(-1);
  const [ghostPosition, setGhostPosition] = useState(-1);
  const [snowflakePositions, setSnowflakePositions] = useState([{col:-1, row: -1},{col: -1, row: -1},{col:-1, row: -1},{col:-1, row: -1},{col:-1, row: -1},{col:-1, row: -1}]);
  const [snowmanPosition, setSnowmanPosition] = useState(-1);
  const [christmasTreePosition, setChristmasTreePosition] = useState(-1);
  const [candyCanePositions, setCandyCanePositions] = useState([{col:-1, row: -1},{col: -1, row: -1},{col:-1, row: -1},{col:-1, row: -1},{col:-1, row: -1},{col:-1, row: -1}]);

  // Fetch the effects data from JSON file
  useEffect(() => {
    const fetchEffectData = async () => {
      try {
        const response = await fetch('/exampleShows/christmas-y.json'); // Make sure the file is in the public folder
        if (!response.ok) {
          throw new Error('Failed to fetch effects.json');
        }
        const data = await response.json();
        setEffectData(data.effects);  // Assuming the JSON has an "effects" array
      } catch (error) {
        console.error('Error fetching effects data:', error);
      }
    };
    fetchEffectData();
  }, []);

  const startEffect = async () => {
    if (effectData.length === 0) {
      console.log("No effects loaded");
      return;
    }

    setIsRunning(true);

    // Loop through each effect and run it for its duration
    for (const effect of effectData) {
      const { effect: type, durationMs } = effect;
      let offset = 0;
      setEffectType(effect.type);
      // Set the correct pumpkin/ghost positions based on effect
      if (effect.type === 'pumpkin-ghost-rainbow' || effect.type === 'pumpkin-ghost-ripple') {
        setPumpkinPosition(-5);
        setGhostPosition(5);
      } else {
        setPumpkinPosition(0);
        setGhostPosition(0);
        setSnowflakePositions([{col:0, row: 0}, {col:10, row: 0}, {col:20, row: 0}, {col:30, row: 0}, {col:40, row: 0},{col:50, row: 0}]);
        setSnowmanPosition(0);
        setChristmasTreePosition(0);
        setCandyCanePositions([{col:0, row: 0}, {col:10, row: 0}, {col:20, row: 0}, {col:30, row: 0}, {col:40, row: 0},{col:50, row: 0}]);
      }
      console.log(effect);
      // Start the interval for this effect
      const interval = setInterval(() => {
        offset += 1;
        setColorOffset(offset);
        // Call the appropriate effect update function
        switch (effect.type) {
          case 'rainbow':
            updateRainbow(offset, numRows, numCols, rainbowColors, setLedGrid);
            break;
          case 'ripple':
            updateRipple(offset, numRows, numCols, rainbowColors, setLedGrid);
            break;
          case 'pumpkin-rainbow':
            updatePumpkinRainbow(offset, numRows, numCols, rainbowColors, setLedGrid, setPumpkinPosition);
            break;
          case 'pumpkin-ripple':
            updatePumpkinRipple(offset, numRows, numCols, rainbowColors, setLedGrid, setPumpkinPosition);
            break;
          case 'ghost-rainbow':
            updateGhostRainbow(offset, numRows, numCols, rainbowColors, setLedGrid, setGhostPosition);
            break;
          case 'ghost-ripple':
            updateGhostRipple(offset, numRows, numCols, rainbowColors, setLedGrid, setGhostPosition);
            break;
          case 'pumpkin-ghost-rainbow':
            updatePumpkinGhostRainbow(offset, numRows, numCols, rainbowColors, setLedGrid, setPumpkinPosition, setGhostPosition);
            break;
          case 'pumpkin-ghost-ripple':
            updatePumpkinGhostRipple(offset, numRows, numCols, rainbowColors, setLedGrid, setPumpkinPosition, setGhostPosition);
            break;
          case 'snowflake':
            updateSnowflake(offset, numRows, numCols, christmasColors, setLedGrid, setSnowflakePositions);
            break;
          case 'snowman':
            updateSnowman(offset, numRows, numCols, christmasColors, setLedGrid, setSnowmanPosition);
            break;
          case 'christmas-tree':
            updateChristmasTree(offset, numRows, numCols, christmasColors2, setLedGrid, setChristmasTreePosition);
            break;
            case 'candy-cane':
              updateCandyCane(offset, numRows, numCols, christmasColors2, setLedGrid, setCandyCanePositions);
              break;
          default:
            console.log(`Unknown effect: ${effect.type}`);
        }
      }, 1000/effect.speed); //controls the speed

      // Wait for the effect duration to complete before stopping the interval and moving to the next effect
      await new Promise<void>((resolve) => {
        setTimeout(() => {
          clearInterval(interval);
          resolve();
        }, durationMs); 
      });
    }

    // After all effects are done, stop the overall running state
    setIsRunning(false);
  };

  return (
    <div>
      <div className="controls">
        <label htmlFor="effect-select">Choose effect: </label>
        <select
          id="effect-select"
          value={effectType}
          onChange={(e) => setEffectType(e.target.value as EffectType)}
        >
          {effectData.map((effect) => (
            <option key={effect.type} value={effect.type}>
              {effect.type.charAt(0).toUpperCase() + effect.type.slice(1)}
            </option>
          ))}
        </select>

        <br />
        <label htmlFor="duration-input">Duration: </label>
        <input type="number" value={effectData[0]?.durationMs || 0} disabled /> {/* Disable the input, duration comes from JSON */}
        <button onClick={startEffect} disabled={isRunning}>
          Start Effects
        </button>
      </div>

      <div className="led-grid">
        {ledGrid.map((row, rowIndex) => (
          <div key={rowIndex} className="led-row">
            {row.map((colorIndex, colIndex) => {
              const pumpkinPart = pumpkinShape.find(
                (part) => rowIndex === part.row && colIndex === (pumpkinPosition + part.col) % numCols
              );

              const ghostPart = ghostShape.find(
                (part) => rowIndex === part.row && colIndex === (ghostPosition + part.col) % numCols
              );

              const snowflakePart = snowflakeShape.find((part) => 
                snowflakePositions.some(
                  (position) =>
                    rowIndex === (position.row + part.row) % numRows && // Match row
                    colIndex === (position.col + part.col) % numCols    // Match column
                )
              );

              const snowmanPart = snowmanShape.find(
                (part) => rowIndex === part.row && colIndex === (snowmanPosition + part.col) % numCols
              );
              
              const christmasTreePart = christmasTreeShape.find(
                (part) => rowIndex === part.row && colIndex === (christmasTreePosition + part.col) % numCols
              );
                    
              const candyCanePart = candyCaneShape.find((part) => 
                candyCanePositions.some(
                  (position) =>
                    rowIndex === (position.row + part.row) % numRows && // Match row
                    colIndex === (position.col + part.col) % numCols    // Match column
                )
              );

              let backgroundColor: string;

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
              }  else if (effectType === 'candy-cane') {
                backgroundColor = candyCanePart ? candyCanePart.color : christmasColors2[colorIndex];
              } 
              else {
                backgroundColor = rainbowColors[colorIndex];
              }

              return (
                <div
                  key={colIndex}
                  className={`led-circle 
                    ${pumpkinPart ? 'pumpkin-cell' : ''} 
                    ${ghostPart ? 'ghost-cell' : ''}
                    ${snowflakePart ? 'snowflake-cell' : ''}
                    ${snowmanPart? 'snowman-cell': ''}
                    ${christmasTreePart? 'christmasTree-cell': ''}
                    ${candyCanePart? 'candy-cane-cell': ''}
                    `}
                  style={{
                    backgroundColor,
                  }}
                ></div>
              );
            })}
          </div>
        ))}
      </div>
    </div>
  );
};

export default LEDGrid;
