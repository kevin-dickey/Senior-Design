import React from 'react';
import { Box } from '@mui/material';
import { TransformWrapper, TransformComponent } from 'react-zoom-pan-pinch';
import { GridLayout } from '../components/serialization/Layout';
import { Show } from '../components/serialization/Show';

interface GridContainerProps {
  show: Show;
}

const GridContainer: React.FC<GridContainerProps> = ({ show }) => {
  const grid = show.layouts[0] as GridLayout;

  return (

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
              {Array.from({ length: grid.height }).map((_, rowIndex) => (
                <Box key={rowIndex} display="flex" gap={0.3}>
                  {Array.from({ length: grid.width }).map((_, colIndex) => (
                    <Box
                      key={colIndex}
                      sx={{
                        width: 10,
                        height: 10,
                        bgcolor: '#222',
                        borderRadius: '50%',
                      }}
                    />
                  ))}
                </Box>
              ))}
            </Box>
          </TransformComponent>
        )}
      </TransformWrapper>
  );
};

export default GridContainer;
