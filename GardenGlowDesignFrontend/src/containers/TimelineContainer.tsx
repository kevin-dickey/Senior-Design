import React from 'react';
import {Box, IconButton} from "@mui/material";
import {
    FastForward,
    FastRewind,
    Pause,
    PlayArrow,
    SkipNext,
    SkipPrevious
} from "@mui/icons-material";
import {
    Timeline,
    TimelineAction,
    TimelineEffect,
    TimelineRow
} from '@xzdarcy/react-timeline-editor';

import './TimelineContainer.css';
import {Effect} from "../components/serialization/Effect";


interface CustomTimelineAction extends TimelineAction {
    effectId: string;
    data: {
        name: string;
    }
}

interface CustomTimelineRow extends TimelineRow {
    actions: CustomTimelineAction[];
}

const createTimelineEffects = (): Record<string, TimelineEffect> => {
    const effectTypes = [
        {id: 'RippleEffect', name: 'Ripple Effect'},
        {id: 'RainbowEffect', name: 'Rainbow Effect'},
    ]

    const timelineEffects: Record<string, TimelineEffect> = {};
    effectTypes.forEach((effectType) => {
        timelineEffects[effectType.id] = {
            id: effectType.id,
            name: effectType.name
        }
    });

    return timelineEffects;
}

const convertEffectsToTimelineRows = (effects: Effect[]): CustomTimelineRow[] => {
    const timelineRows: CustomTimelineRow[] = [];

    effects.forEach((effect: Effect) => {
        const timelineAction: CustomTimelineAction = {
            id: `${effect.id}`,
            start: effect.startTimeMs / 1000,
            end: (effect.startTimeMs + effect.durationMs) / 1000,
            // TODO: Add shared effect ID between timeline and effect entity
            effectId: 'RainbowEffect',
            data: {
                name: effect.name
            }
        };

        // TODO: Add Layer prop
        let row = timelineRows.find((row) => row.id === `${effect.layer}`);
        if (!row) {
            row = {
                id: `${effect.layer}`,
                actions: []
            };
            timelineRows.push(row);
        }

        row.actions.push(timelineAction);
    });

    return timelineRows;
}

interface TimelineEditorProps {
    timelineEffects: Record<string, TimelineEffect>;
    timelineRows: TimelineRow[];
    onChangeTimelineRows: (editorData: TimelineRow[]) => boolean | void;
    onClickActionOnly: (e: React.MouseEvent<HTMLElement, MouseEvent>, param: {
        action: TimelineAction;
        row: TimelineRow;
        time: number;
    }) => void;
    style?: React.CSSProperties;
}

const TimelineEditor: React.FC<TimelineEditorProps> = (
    {timelineEffects, timelineRows, onChangeTimelineRows, onClickActionOnly, style}
) => {
    const CustomScale = (props: { scale: number }) => {
        const {scale} = props;
        const min = parseInt(scale / 60 + '');
        const second = (scale % 60 + '').padStart(2, '0');
        return <>{`${min}:${second}`}</>
    }

    return (
        <Timeline
            style={{width: 'auto', ...style}}
            onChange={onChangeTimelineRows}
            onClickActionOnly={onClickActionOnly}
            editorData={timelineRows}
            effects={timelineEffects}
            scale={1}
            // TODO: This is a hack to make the scale render nicely. Need interactive resizing!
            scaleWidth={50}
            gridSnap={true}
            scaleSplitCount={5}
            hideCursor={false}
            autoScroll={true}
            getScaleRender={(scale: number) => <CustomScale scale={scale}/>}
            getActionRender={(action: TimelineAction) => {
                const customAction = action as CustomTimelineAction;
                return (
                    <div className={`effect${action.id}-text`}>
                        {`${customAction.data.name}: ${(action.end - action.start).toFixed(2)}s`}
                    </div>
                )
            }}
        />
    )
}

export interface TimelineContainerProps {
    effects: Effect[];
    onChangeEffects: (effects: Effect[]) => void;
    onChangeSelectedEffectId: (effectId: number) => void;
}

export const TimelineContainer: React.FC<TimelineContainerProps> = (props) => {
    const timelineEffects = createTimelineEffects();
    const timelineRows = convertEffectsToTimelineRows(props.effects);

    const updateEffectTiming = (effects: Effect[], action: TimelineAction) => {
        const effectId = parseInt(action.id)
        const effectIdx = effects.findIndex((effect) => effect.id === effectId);
        if (effectIdx !== -1) {
            effects[effectIdx].startTimeMs = Math.floor(action.start * 1000);
            effects[effectIdx].durationMs = Math.floor((action.end - action.start) * 1000);
        }
        return effects;
    }

    const onChangeTimelineRows = (editorData: TimelineRow[]) => {
        // TODO: Convert timeline data back to effects and lift state up.
        let updatedEffects = [...props.effects];

        editorData.forEach((row) => {
            row.actions.forEach((action: TimelineAction) => {
                updatedEffects = updateEffectTiming(updatedEffects, action);
                console.log(action);
            });
        });

        props.onChangeEffects(updatedEffects);
        return true;
    }

    const handleClickActionOnly = (e: React.MouseEvent<HTMLElement, MouseEvent>, param: {
        action: TimelineAction;
        row: TimelineRow;
        time: number;
    }) => {
        // Update the selected effect Id
        const effectId = parseInt(param.action.id);
        props.onChangeSelectedEffectId(effectId);
    }

    return (
        <Box className='TimelineContainer'>
            <TimelineEditor
                timelineEffects={timelineEffects}
                timelineRows={timelineRows}
                onChangeTimelineRows={onChangeTimelineRows}
                onClickActionOnly={handleClickActionOnly}
            />
            <Box
                sx={{
                    display: 'flex',
                    justifyContent: 'center',
                    alignItems: 'center',
                    mt: 2
                }}
            >
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