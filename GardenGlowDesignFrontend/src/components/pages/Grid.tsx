import React, { useState } from 'react';
import './LEDGrid.css';

import { updateRainbow, updateRipple ,updateGhostRainbow,updateGhostRipple,updatePumpkinRainbow,updatePumpkinRipple,updatePumpkinGhostRainbow,updatePumpkinGhostRipple } from '../../utils/effects';
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

  //should this move
  const [, setColorOffset] = useState(0);
  
  const [isRunning, setIsRunning] = useState(false);
  const [duration, setDuration] = useState(0);
  const [effectType, setEffectType] = useState<EffectType>('rainbow');
  const [pumpkinPosition, setPumpkinPosition] = useState(-1);
  const [ghostPosition, setGhostPosition] = useState(-1);


  const startEffect = () => {
    let offset = 0;
    setIsRunning(true);
    if(effectType ==='pumpkin-ghost-rainbow' || effectType === 'pumpkin-ghost-ripple'){
      setPumpkinPosition(-5);
      setGhostPosition(5);
    }else{
      setPumpkinPosition(0);
      setGhostPosition(0);
    }
    

    const interval = setInterval(() => {
      offset += 1;
      setColorOffset(offset);

      if (effectType === 'rainbow') {
        updateRainbow(offset, numRows, numCols, colors, setLedGrid);
      } else if (effectType === 'ripple') {
        updateRipple(offset, numRows, numCols, colors, setLedGrid);
      } else if (effectType === 'pumpkin-rainbow') {
        updatePumpkinRainbow(offset, numRows, numCols, colors, setLedGrid,setPumpkinPosition);
      } else if (effectType === 'pumpkin-ripple') {
        updatePumpkinRipple(offset, numRows, numCols, colors, setLedGrid, setPumpkinPosition);
      } else if (effectType === 'ghost-rainbow') {
        updateGhostRainbow(offset, numRows, numCols, colors, setLedGrid, setGhostPosition);
      } else if (effectType === 'ghost-ripple') {
        updateGhostRipple(offset, numRows, numCols, colors, setLedGrid, setGhostPosition);
      }else if (effectType === 'pumpkin-ghost-rainbow') {
        updatePumpkinGhostRainbow(offset, numRows, numCols, colors, setLedGrid, setPumpkinPosition, setGhostPosition);
      }else if (effectType === 'pumpkin-ghost-ripple') {
        updatePumpkinGhostRipple(offset, numRows, numCols, colors, setLedGrid, setPumpkinPosition, setGhostPosition);
      }
    }, 200);

    setTimeout(() => {
      clearInterval(interval);
      setIsRunning(false);
      // setPumpkinPosition(-1);
      // setGhostPosition(-1);
    }, duration * 1000);
  };

  const handleInputChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setDuration(Number(event.target.value));
  };

  const handleEffectChange = (event: React.ChangeEvent<HTMLSelectElement>) => {
    setEffectType(event.target.value as EffectType);
  };

  return (
    <div>
      <div className="controls">
        <label htmlFor="effect-select">Choose effect: </label>
        <select id="effect-select" value={effectType} onChange={handleEffectChange}>
          <option value="rainbow">Rainbow</option>
          <option value="ripple">Ripple</option>
          <option value="pumpkin-rainbow">Pumpkin on a Rainbow</option>
          <option value="pumpkin-ripple">Pumpkin on a Ripple</option>
          <option value="ghost-rainbow">Ghost on a Rainbow</option>
          <option value="ghost-ripple">Ghost on a Ripple</option>
          <option value="pumpkin-ghost-rainbow">Pumpkin/Ghost on a Rainbow</option>
          <option value="pumpkin-ghost-ripple">Pumpkin/Ghost on a Ripple</option>
        </select>

        <br />
        <label htmlFor="duration-input">Enter duration (in seconds): </label>
        <input
          type="number"
          id="duration-input"
          value={duration}
          onChange={handleInputChange}
          disabled={isRunning}
        />
        <button onClick={startEffect} disabled={isRunning || duration <= 0}>
          Start {effectType.charAt(0).toUpperCase() + effectType.slice(1)}
        </button>
      </div>
      <div className="led-grid">
        {ledGrid.map((row, rowIndex) => (
          <div key={rowIndex} className="led-row">
            {row.map((colorIndex, colIndex) => {
              // Check for pumpkin or ghost part
              const pumpkinPart = pumpkinShape.find(
                (part) => rowIndex === part.row && colIndex === (pumpkinPosition + part.col) % numCols
              );

              const ghostPart = ghostShape.find(
                (part) => rowIndex === part.row && colIndex === (ghostPosition + part.col) % numCols
              );

              let backgroundColor: string;

              // Handle pumpkin or ghost effect
              if (effectType === 'pumpkin-rainbow' || effectType === 'pumpkin-ripple') {
                // If pumpkin effect is active and this is a part of the pumpkin, apply pumpkin color
                backgroundColor = pumpkinPart ? pumpkinPart.color : colors[colorIndex];
              } else if (effectType === 'ghost-rainbow' || effectType === 'ghost-ripple') {
                // If ghost effect is active and this is a part of the ghost, apply ghost color
                backgroundColor = ghostPart ? ghostPart.color : colors[colorIndex];
              } else if (effectType === 'pumpkin-ghost-rainbow'|| effectType === 'pumpkin-ghost-ripple') {
                // If it's the "Pumpkin/Ghost on a Rainbow" effect, combine both
                backgroundColor = pumpkinPart ? pumpkinPart.color : (ghostPart ? ghostPart.color : colors[colorIndex]);
              }else {
                // Default rainbow or ripple
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
