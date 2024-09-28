import React, { useState } from 'react';
import './LEDGrid.css'; // Include your CSS styles here

const LEDGrid: React.FC = () => {
  const numRows = 10;
  const numCols = 20;

  // Create the initial state for the grid
  const [ledGrid, setLedGrid] = useState<number[][]>(
    Array(numRows)
      .fill(0)
      .map(() => Array(numCols).fill(0))
  );

  const colors = [
    '#FF0000', // Red
    '#FF7F00', // Orange
    '#FFFF00', // Yellow
    '#00FF00', // Green
    '#0000FF', // Blue
    '#4B0082', // Indigo
    '#9400D3', // Violet
  ];

  const [, setColorOffset] = useState(0);
  const [isRunning, setIsRunning] = useState(false); // Track if the animation is running
  const [duration, setDuration] = useState(0); // User-defined duration
  const [effectType, setEffectType] = useState<'rainbow' | 'ripple'>('rainbow'); // Store the chosen effect

  // Update the grid for the rainbow effect
  const updateRainbow = (offset: number) => {
    setLedGrid(() => {
      return Array(numRows)
        .fill(0)
        .map((_, rowIndex) =>
          Array(numCols)
            .fill(0)
            .map((_, colIndex) => (rowIndex + colIndex + offset) % colors.length)
        );
    });
  };

  // Update the grid for the ripple effect
  const updateRipple = (offset: number) => {
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
              const colorIndex = Math.floor((distance + offset) % colors.length);
              return colorIndex;
            })
        );
    });
  };

  // Start the selected effect
  const startEffect = () => {
    let offset = 0;
    setIsRunning(true); // Start the animation

    const interval = setInterval(() => {
      offset += 1;
      setColorOffset(offset);

      // Run the selected effect
      if (effectType === 'rainbow') {
        updateRainbow(offset);
      } else {
        updateRipple(offset);
      }
    }, 200); // Update every 200ms for smooth transition

    // Stop the animation after user-specified duration
    setTimeout(() => {
      clearInterval(interval);
      setIsRunning(false); // Reset the animation state
    }, duration * 1000); // Convert duration from seconds to milliseconds
  };

  // Handle input change for duration
  const handleInputChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setDuration(Number(event.target.value));
  };

  // Handle effect selection
  const handleEffectChange = (event: React.ChangeEvent<HTMLSelectElement>) => {
    setEffectType(event.target.value as 'rainbow' | 'ripple');
  };

  return (
    <div>
      <div className="controls">
        <label htmlFor="effect-select">Choose effect: </label>
        <select id="effect-select" value={effectType} onChange={handleEffectChange}>
          <option value="rainbow">Rainbow</option>
          <option value="ripple">Ripple</option>
        </select>

        <br />
        <label htmlFor="duration-input">Enter duration (in seconds): </label>
        <input
          type="number"
          id="duration-input"
          value={duration}
          onChange={handleInputChange}
          disabled={isRunning} // Disable input while the animation is running
        />
        <button onClick={startEffect} disabled={isRunning || duration <= 0}>
          Start {effectType.charAt(0).toUpperCase() + effectType.slice(1)}
        </button>
      </div>
      <div className="led-grid">
        {ledGrid.map((row, rowIndex) => (
          <div key={rowIndex} className="led-row">
            {row.map((colorIndex, colIndex) => (
              <div
                key={colIndex}
                className="led-circle"
                style={{ backgroundColor: colors[colorIndex] }}
              ></div>
            ))}
          </div>
        ))}
      </div>
    </div>
  );
};

export default LEDGrid;
