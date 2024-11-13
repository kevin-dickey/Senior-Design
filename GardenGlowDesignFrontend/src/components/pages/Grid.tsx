import React, { useState, useEffect } from 'react';
import './LEDGrid.css';

import { updateRainbow, updateRipple, updateGhostRainbow, updateGhostRipple, updatePumpkinRainbow, updatePumpkinRipple, updatePumpkinGhostRainbow, updatePumpkinGhostRipple } from '../../utils/effects';
import { pumpkinShape, ghostShape } from '../../utils/shapes';
import { EffectType } from '../../types/index';

const LEDGrid: React.FC = () => {
  const numRows = 10;
  const numCols = 20;

  const [ledGrid, setLedGrid] = useState<number[][]>(
    Array(numRows)
      .fill(0)
      .map(() => Array(numCols).fill(0))
  );

  const colors = [
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

  // Fetch the effects data from JSON file
  useEffect(() => {
    const fetchEffectData = async () => {
      try {
        const response = await fetch('/effects.json'); // Make sure the file is in the public folder
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
      const { effect: effectName, duration } = effect;
      let offset = 0;
      setEffectType(effectName);
      // Set the correct pumpkin/ghost positions based on effect
      if (effectName === 'pumpkin-ghost-rainbow' || effectName === 'pumpkin-ghost-ripple') {
        setPumpkinPosition(-5);
        setGhostPosition(5);
      } else {
        setPumpkinPosition(0);
        setGhostPosition(0);
      }

      // Start the interval for this effect
      const interval = setInterval(() => {
        offset += 1;
        setColorOffset(offset);
        // Call the appropriate effect update function
        switch (effectName) {
          case 'rainbow':
            updateRainbow(offset, numRows, numCols, colors, setLedGrid);
            break;
          case 'ripple':
            updateRipple(offset, numRows, numCols, colors, setLedGrid);
            break;
          case 'pumpkin-rainbow':
            updatePumpkinRainbow(offset, numRows, numCols, colors, setLedGrid, setPumpkinPosition);
            break;
          case 'pumpkin-ripple':
            updatePumpkinRipple(offset, numRows, numCols, colors, setLedGrid, setPumpkinPosition);
            break;
          case 'ghost-rainbow':
            updateGhostRainbow(offset, numRows, numCols, colors, setLedGrid, setGhostPosition);
            break;
          case 'ghost-ripple':
            updateGhostRipple(offset, numRows, numCols, colors, setLedGrid, setGhostPosition);
            break;
          case 'pumpkin-ghost-rainbow':
            updatePumpkinGhostRainbow(offset, numRows, numCols, colors, setLedGrid, setPumpkinPosition, setGhostPosition);
            break;
          case 'pumpkin-ghost-ripple':
            updatePumpkinGhostRipple(offset, numRows, numCols, colors, setLedGrid, setPumpkinPosition, setGhostPosition);
            break;
          default:
            console.log(`Unknown effect: ${effectName}`);
        }
      }, 200);

      // Wait for the effect duration to complete before stopping the interval and moving to the next effect
      await new Promise<void>((resolve) => {
        setTimeout(() => {
          clearInterval(interval);
          resolve();
        }, duration * 1000); // Duration in seconds
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
            <option key={effect.effect} value={effect.effect}>
              {effect.effect.charAt(0).toUpperCase() + effect.effect.slice(1)}
            </option>
          ))}
        </select>

        <br />
        <label htmlFor="duration-input">Duration: </label>
        <input type="number" value={effectData[0]?.duration || 0} disabled /> {/* Disable the input, duration comes from JSON */}
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

              let backgroundColor: string;

              console.log(effectType);
              // Handle pumpkin or ghost effect
              if (effectType === 'pumpkin-rainbow' || effectType === 'pumpkin-ripple') {
                backgroundColor = pumpkinPart ? pumpkinPart.color : colors[colorIndex];
              } else if (effectType === 'ghost-rainbow' || effectType === 'ghost-ripple') {
                backgroundColor = ghostPart ? ghostPart.color : colors[colorIndex];
              } else if (effectType === 'pumpkin-ghost-rainbow' || effectType === 'pumpkin-ghost-ripple') {
                backgroundColor = pumpkinPart ? pumpkinPart.color : (ghostPart ? ghostPart.color : colors[colorIndex]);
              } else {
                backgroundColor = colors[colorIndex];
              }

              return (
                <div
                  key={colIndex}
                  className={`led-circle ${pumpkinPart ? 'pumpkin-cell' : ''} ${ghostPart ? 'ghost-cell' : ''}`}
                  style={{
                    backgroundColor,
                    borderColor: pumpkinPart || ghostPart ? '#8a3900' : 'transparent',
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
