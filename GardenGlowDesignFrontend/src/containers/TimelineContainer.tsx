import React from 'react' ;
import {useState} from 'react';
import {Timeline} from '@xzdarcy/react-timeline-editor' ;
import {mockData, mockEffect} from './mockData';

import {Box, IconButton} from "@mui/material";
import {
    FastForward,
    FastRewind,
    Pause,
    PlayArrow,
    SkipNext,
    SkipPrevious
} from "@mui/icons-material";

const TimelineEditor: React.FC = () => {
    const [data, setData] = useState(mockData);

    return (
        <Timeline
            style={{width: 'auto'}}
            onChange={setData}
            editorData={data}
            effects={mockEffect}
            hideCursor={false}
            autoScroll={true}
            getActionRender={(action, row) => {
                return (
                    <div className={`effect0-text`}>{`effect: ${(
                        action.end - action.start
                    ).toFixed(2)}s ${action.movable === false ? '（Fixed)）' : ''} ${
                        action.flexible === false ? '（Set Duration）' : ''
                    }`}</div>
            )
            }}
        />
    )
}

export const TimelineContainer: React.FC = () => {
    return (
        <Box>
            <TimelineEditor/>
            <Box mt={2} display="flex" gap={2} justifyContent="center">
                <IconButton aria-label="Skip Previous" sx={{color: '#fff'}}>
                    <SkipPrevious/>
                </IconButton>
                <IconButton aria-label="Rewind" sx={{color: '#fff'}}>
                    <FastRewind/>
                </IconButton>
                <IconButton aria-label="Pause" sx={{color: '#fff'}}>
                    <Pause/>
                </IconButton>
                <IconButton aria-label="Play" sx={{color: '#fff'}}>
                    <PlayArrow/>
                </IconButton>
                <IconButton aria-label="Fast Forward" sx={{color: '#fff'}}>
                    <FastForward/>
                </IconButton>
                <IconButton aria-label="Skip Next" sx={{color: '#fff'}}>
                    <SkipNext/>
                </IconButton>
            </Box>
        </Box>
    )
}