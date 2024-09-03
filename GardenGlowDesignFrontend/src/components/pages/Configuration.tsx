import React, { useState, useRef, useEffect } from 'react';
import { Box, Button, Slider, Drawer, Divider, IconButton } from '@mui/material';
import { ExpandLess, ExpandMore, Pause, PlayArrow, FastForward, FastRewind, SkipNext, SkipPrevious } from '@mui/icons-material';
import { TransformWrapper, TransformComponent } from "react-zoom-pan-pinch";

const App: React.FC = () => {
    const [isShapesOpen, setIsShapesOpen] = useState(true);
    const [isEffectsOpen, setIsEffectsOpen] = useState(true);
    const [isColorsOpen, setIsColorsOpen] = useState(true);

    const toggleShapes = () => setIsShapesOpen(!isShapesOpen);
    const toggleEffects = () => setIsEffectsOpen(!isEffectsOpen);
    const toggleColors = () => setIsColorsOpen(!isColorsOpen);

    return (
        <Box display="flex" height="100vh" bgcolor="#181818" color="#ffffff">
            {/* Sidebar */}
            <Drawer
                variant="permanent"
                anchor="left"
                sx={{
                    width: '15%',
                    height: '100vh',
                    '& .MuiDrawer-paper': {
                        width: '15%',
                        bgcolor: '#2a2a2a',
                        overflow: 'auto',
                    },
                }}
            >
                <Box>
                    <Button fullWidth onClick={toggleShapes} sx={{ color: '#fff', justifyContent: 'flex-start' }}>
                        Shapes {isShapesOpen ? <ExpandLess /> : <ExpandMore />}
                    </Button>
                    {isShapesOpen && <Box sx={{ bgcolor: '#3a3a3a', p: 2 }}>Shapes content</Box>}
                </Box>
                <Divider sx={{ bgcolor: '#444' }} />
                <Box>
                    <Button fullWidth onClick={toggleEffects} sx={{ color: '#fff', justifyContent: 'flex-start' }}>
                        Effects {isEffectsOpen ? <ExpandLess /> : <ExpandMore />}
                    </Button>
                    {isEffectsOpen && <Box sx={{ bgcolor: '#3a3a3a', p: 2 }}>Effects content</Box>}
                </Box>
                <Divider sx={{ bgcolor: '#444' }} />
                <Box>
                    <Button fullWidth onClick={toggleColors} sx={{ color: '#fff', justifyContent: 'flex-start' }}>
                        Colors {isColorsOpen ? <ExpandLess /> : <ExpandMore />}
                    </Button>
                    {isColorsOpen && (
                        <Box sx={{ bgcolor: '#3a3a3a', p: 2 }}>
                            <input type="color" value="#9731f2" />
                        </Box>
                    )}
                </Box>
            </Drawer>

            {/* Grid Container */}
            <Box
                sx={{
                    width: '85%',
                    height: '85%',
                    position:'absolute',
                    top: 0,
                    right: 0,
                    overflow: 'hidden',
                }}
            >
                <TransformWrapper
                    initialScale={1}
                    wheel={{ step: 0.5 }}
                    minScale={.5}
                    maxScale={5}
                >
                    {({ zoomIn, zoomOut, resetTransform }) => (
                        <TransformComponent wrapperStyle={{ flex: 1 }}>
                            <Box flexDirection="column">
                                {/* Generate a grid of dots to represent LEDs */}
                                {[...Array(50)].map((_, rowIndex) => (
                                    <Box key={rowIndex} display="flex" gap={0.5}>
                                        {[...Array(50)].map((_, colIndex) => (
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

            {/* Timeline Container */}
            <Box
                position="absolute"
                bottom={0}
                right={0}
                width="80%"
                height="10%"
                bgcolor="#2a2a2a"
                p={2}
                zIndex={1}
            >
                {/**add better time indicator */}
                <Slider
                    defaultValue={0}
                    aria-label="Time Slider"
                    valueLabelDisplay="auto"
                    min={0}
                    max={100}
                    sx={{ width: '100%', color: '#fff' }}
                />
                <Box mt={2} display="flex" gap={2} justifyContent="center">
                    <IconButton aria-label="Skip Previous" sx={{ color: '#fff' }}>
                        <SkipPrevious />
                    </IconButton>
                    <IconButton aria-label="Rewind" sx={{ color: '#fff' }}>
                        <FastRewind />
                    </IconButton>
                    <IconButton aria-label="Pause" sx={{ color: '#fff' }}>
                        <Pause />
                    </IconButton>
                    <IconButton aria-label="Play" sx={{ color: '#fff' }}>
                        <PlayArrow />
                    </IconButton>
                    <IconButton aria-label="Fast Forward" sx={{ color: '#fff' }}>
                        <FastForward />
                    </IconButton>
                    <IconButton aria-label="Skip Next" sx={{ color: '#fff' }}>
                        <SkipNext />
                    </IconButton>
                </Box>
            </Box>
        </Box>
    );
};

export default App;
