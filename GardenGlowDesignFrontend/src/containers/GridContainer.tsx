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
    <Box
      sx={{
        width: '85%',
        height: '85%',
        position: 'absolute',
        top: 0,
        right: 0,
        overflow: 'hidden',
      }}
    >
      <TransformWrapper
        initialScale={1}
        wheel={{ step: 0.5 }}
        minScale={0.5}
        maxScale={5}
      >
        {({ zoomIn, zoomOut, resetTransform }) => (
          <TransformComponent wrapperStyle={{ flex: 1 }}>
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
    </Box>
  );
};

export default GridContainer;
